/*
	Simple library using IOUSBDeviceFamily and IOKit to talk to USB devices.
	Written by Sem Voigtländer on November 13, 2019
	Licensed under the MIT License
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mach/mach.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <CoreFoundation/CoreFoundation.h>
#include "mvusblib.h"
#include "apple.h"

static uint16_t gVendor = 0; // Global Temporary Vendor ID
static uint16_t gProduct = 0; // Global Temporary Product ID
static uint16_t gLocation = 0; // Global Temporary Location ID
static mvusblib_awaitcallback_t gCallback = NULL; // Global Temporary Callback

static IONotificationPortRef gNotifyPort = MACH_PORT_NULL; // Global Notification Port
static io_iterator_t gRawAddedIter = IO_OBJECT_NULL; // Global Added Device Iterator
static io_iterator_t gRawRemovedIter = IO_OBJECT_NULL; // Global Removed Device Iterator


int mvusblib_control_msg(IOUSBDeviceInterface** dev, int type, int reqno, int value, int index, const char *msg, int timeout)
{

    kern_return_t err = KERN_SUCCESS;
    
	// Require a device interface argument
	if(!dev) {
		fprintf(stderr, "%s: no interface provided. Argument is mandatory to operate.\n", __func__);
		return KERN_INVALID_ARGUMENT;
	}

	if(!msg) {
		msg = ""; // Make sure the message isn't actually null
	}

	// Set up the control request
	IOUSBDevRequest req = {};
	req.bmRequestType = type;
	req.bRequest = reqno;
	req.wValue = value;
	req.wIndex = index;
	req.pData = (char*)msg;
	req.wLength = strlen(msg);
	req.wLenDone = 0;

	// send the request
	err = (*dev)->DeviceRequest(dev, &req);

	return err;
}


void mvusblib_freedevice(IOUSBDeviceInterface** iface) {
    
    if(iface) {
        (*iface)->Release(iface);
        iface = NULL;
    }
    
}

kern_return_t mvusblib_opendevice(IOUSBDeviceInterface** iface) {
    
    kern_return_t err = KERN_SUCCESS;
    
    err = (*iface)->USBDeviceOpen(iface);
    
    if (err != KERN_SUCCESS)
    {
        fprintf(stderr, "%s: Unable to open device: %s\n", __func__, mach_error_string(err));
        return err;
    }
    
    return err;
}

kern_return_t mvusblib_closedevice(IOUSBDeviceInterface** iface) {
    
    kern_return_t err = KERN_SUCCESS;
    
    err = (*iface)->USBDeviceClose(iface);
    
    if (err != KERN_SUCCESS) {
        fprintf(stderr, "%s: Unable to open device: %s\n", __func__, mach_error_string(err));
        return err;
    }
    
    mvusblib_freedevice(iface);
    
    return err;
}

kern_return_t mvusblib_claiminterface(io_service_t dev, IOUSBDeviceInterface* iface) {

	kern_return_t err = KERN_SUCCESS;
	SInt32 score = 0;
	IOCFPlugInInterface** pluginIface = NULL;
	
    err = IOCreatePlugInInterfaceForService(dev, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &pluginIface, &score);

	if( KERN_SUCCESS != err || !pluginIface ) {
		fprintf(stderr, "%s: Could not create plug-in interface for the device.\n", __func__);
		return err;
	}

	/* Lookup the usbdevice interface from the plugin interface */
	err = (*pluginIface)->QueryInterface(pluginIface, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (LPVOID*)iface);

	if ( KERN_SUCCESS != err ) {
		fprintf(stderr, "%s: Could not lookup usb interface from the plugin interface.\n", __func__);
		return err;
	}

	// We're done with the plugin interface
	(*pluginIface)->Release(pluginIface);
	pluginIface = NULL;

    if(!iface) {
		return KERN_FAILURE;
    }
    
	return err;

};

int mvusblib_writedevice(IOUSBInterfaceInterface** iface, uint16_t address, uint16_t length, char* data) {
    return KERN_NOT_SUPPORTED;
}

/**
 @function mvusblib_configure_device
 @brief Configures a USB device accordingly
 @param dev Reference to a USB interface reference
 @param configIndex Index of the configuration descriptor for the configuration
 @return KERN_SUCCESS on success, KERN_FAILURE on failure
 */
kern_return_t mvusblib_configure_device(IOUSBDeviceInterface **dev, int configIndex)
{
    UInt8 numConfig;
    kern_return_t err = KERN_SUCCESS;
    IOUSBConfigurationDescriptorPtr configDesc;
    
    //Get the number of configurations.
    err = (*dev)->GetNumberOfConfigurations(dev, &numConfig);
    if (!numConfig)
        return KERN_FAILURE;
    
    //Get the configuration descriptor for index 0
    err = (*dev)->GetConfigurationDescriptorPtr(dev, 0, &configDesc);
    if (err)
    {
        printf("Couldn’t get configuration descriptor for index %d (err = %08x)\n", 0, err);
        return KERN_FAILURE;
    }
    //Set the device’s configuration. The configuration value is found in
    //the bConfigurationValue field of the configuration descriptor
    err = (*dev)->SetConfiguration(dev, configDesc->bConfigurationValue);
    if (err)
    {
        printf("Couldn’t set configuration to value %d (err = %08x)\n", 0, err);
        return KERN_FAILURE;
    }
    return KERN_SUCCESS;
}

/**
 @function mvusblib_found_device
 @brief Callback to be called upon device detection by mvusblib_awaitdevice
 @param refCon reference argument (unused at this time)
 @param iterator Iterator object for the USB device
*/
void mvusblib_found_device(void *refCon, io_iterator_t iterator)
{
    kern_return_t err;
    io_service_t usbDevice;
    IOCFPlugInInterface **plugInInterface = NULL;
    IOUSBDeviceInterface **dev = NULL;
    HRESULT result;
    SInt32 score;
    UInt16 vendor;
    UInt16 product;
    UInt32 location;
    while ((usbDevice = IOIteratorNext(iterator)))
    {
        //Create an intermediate plug-in
        err = IOCreatePlugInInterfaceForService(usbDevice,
                                               kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID,
                                               &plugInInterface, &score);
        //Don’t need the device object after intermediate plug-in is created
        err = IOObjectRelease(usbDevice);
        if ((kIOReturnSuccess != err) || !plugInInterface)
        {
            fprintf(stderr, "%s: Unable to create a plug-in (%s)\n", __func__, mach_error_string(err));
            continue;
        }
        //Now create the device interface
        result = (*plugInInterface)->QueryInterface(plugInInterface,
                                                    CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID),
                                                    (LPVOID)&dev);
        //Don’t need the intermediate plug-in after device interface
        //is created
        (*plugInInterface)->Release(plugInInterface);
        if (result || !dev)
        {
            fprintf(stderr, "%s: Couldn’t create a device interface (%d)\n", __func__, (int)result);
            continue;
        }
        //Check these values for confirmation
        err = (*dev)->GetDeviceVendor(dev, &vendor);
        err = (*dev)->GetDeviceProduct(dev, &product);
        err = (*dev)->GetLocationID(dev, &location);
        
        mvusblib_vendorname_t vendorString = {};
        mvusblib_productname_t productString = {};
        
        mvusblib_vendor_string(vendor, vendorString);
        mvusblib_product_string(vendor, product, productString);
        
        if ((vendor != gVendor) || (product != gProduct))
        {
            fprintf(stderr, "%s: Found unwanted device (vendor = %s, product = %s, location = %#x)\n", __func__,vendorString, productString, location);
            (void) (*dev)->Release(dev);
            dev = NULL;
            continue;
        }
        else if(gLocation) {
            if(gLocation != location) {
                fprintf(stderr, "%s: Found unwanted device (vendor = %s, product = %s, location = %#x)\n", __func__, vendorString, productString, location);
                (void) (*dev)->Release(dev);
                dev = NULL;
                continue;
            }
        }
        
        printf("Found the device (vendor = %s, product = %s, location = %#x)\n", vendorString, productString, location);
        
        //Open the device to change its state
        err = (*dev)->USBDeviceOpen(dev);
        if (err != kIOReturnSuccess)
        {
            fprintf(stderr, "%s: Unable to open device: %s\n", __func__, mach_error_string(err));
            (void) (*dev)->Release(dev);
            continue;
        }

        gLocation = 0;
        gProduct = 0;
        gVendor = 0;
        
        gCallback(dev);
        
        //Close this device and release object
        err = (*dev)->USBDeviceClose(dev);
        
    }
}

/**
 @function mvusblib_removed_device
 @brief Callback to be called upon device removal by mvusblib_awaitdevice
 @param refCon reference argument (unused at this time)
 @param iterator Iterator object for the USB device
 */
void mvusblib_removed_device(void *refCon, io_iterator_t iterator)
{
    kern_return_t err = KERN_SUCCESS;
    io_service_t object;
    while ((object = IOIteratorNext(iterator)))
    {
        err = IOObjectRelease(object);
        if (err != KERN_SUCCESS)
        {
            fprintf(stderr, "%s: Couldn’t release raw device object: %s\n", __func__, mach_error_string(err));
            continue;
        }
    }
}

kern_return_t mvusblib_awaitdevice(mvusblib_vendor_t vid, mvusblib_product_t pid, mvusblib_location_t locationid, mvusblib_awaitcallback_t callback) {

	CFMutableDictionaryRef matching = NULL;
	kern_return_t err = KERN_SUCCESS;
    io_master_t masterPort = MACH_PORT_NULL;
    CFRunLoopSourceRef runLoopSource = NULL;
    
    gVendor = vid;
    gProduct = pid;
    gLocation = locationid;
    gCallback = callback;
    
    //Create a master port for communication with the I/O Kit
    err = IOMasterPort(MACH_PORT_NULL, &masterPort);
    if (err || !masterPort) {
        fprintf(stderr, "%s: Couldn’t create a master I/O Kit port (%s)\n", __func__, mach_error_string(err));
        return -1;
    }

    if(!callback) {
        fprintf(stderr, "%s: callback argument is mandatory!\n", __func__);
        return KERN_INVALID_ARGUMENT;
    }
    
    /* Create a matching dictionary for USB device class */
    matching = IOServiceMatching(kIOUSBDeviceClassName);
    if (!matching)
    {
        fprintf(stderr, "%s: Couldn’t create a USB matching dictionary\n", __func__);
        mach_port_deallocate(mach_task_self(), masterPort);
        return KERN_FAILURE;
    }
    
    gNotifyPort = IONotificationPortCreate(masterPort);
    runLoopSource = IONotificationPortGetRunLoopSource(gNotifyPort);
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource,
                       kCFRunLoopDefaultMode);
    
    //Retain additional dictionary references because each call to
    //IOServiceAddMatchingNotification consumes one reference
    matching = (CFMutableDictionaryRef) CFRetain(matching);
    matching = (CFMutableDictionaryRef) CFRetain(matching);
    matching = (CFMutableDictionaryRef) CFRetain(matching);

    // Listen for device add
    err = IOServiceAddMatchingNotification(gNotifyPort, kIOFirstMatchNotification, matching, mvusblib_found_device, NULL, &gRawAddedIter);

    mvusblib_found_device(NULL, gRawAddedIter);
    
    mach_port_deallocate(mach_task_self(), masterPort);
    masterPort = MACH_PORT_NULL;
    
    // Listen for device remove
    err = IOServiceAddMatchingNotification(gNotifyPort, kIOTerminatedNotification, matching, mvusblib_removed_device, NULL, &gRawRemovedIter);
    
    return err;
}

int mvusblib_printdevices(void){
	CFMutableDictionaryRef matching = NULL;
	io_iterator_t iter = 0;
	kern_return_t err = KERN_SUCCESS;
	io_service_t dev = IO_OBJECT_NULL;

	/* Create a matching dictionary for USB device class */
	matching = IOServiceMatching(kIOUSBDeviceClassName);
	if(!matching) {
		fprintf(stderr, "%s: Failed to allocate dictionary.\n", __func__);
		return KERN_FAILURE;
	}

	/* Lookup the USB service in IOKit and get an iterator */
	err = IOServiceGetMatchingServices(kIOMasterPortDefault, matching, &iter);
	if ( KERN_SUCCESS != err || iter == IO_OBJECT_NULL) {
		fprintf(stderr, "%s: Failed to find USB service in IOKit.\n", __func__);
		return KERN_FAILURE;
	}

	/* Start iterating over the devices */
	while ( (dev = IOIteratorNext(iter)) ) {

		CFNumberRef _vid = NULL;
		CFNumberRef _pid = NULL;
		CFNumberRef _locationid = NULL;
		CFMutableDictionaryRef properties = NULL;

		/* Get the properties for the device */
		err = IORegistryEntryCreateCFProperties(dev, &properties, NULL, 0);
		if( KERN_SUCCESS != err ) {
			fprintf(stderr, "%s: Could not get properties for device, skipping this one...\n", __func__);
			continue;
		}

		/* Read the vendor ID from the device */
		if(!CFDictionaryGetValueIfPresent(properties, CFSTR("idVendor"), (void*)&_vid)) {
			fprintf(stderr, "%s: Device doesn't have a vendor ID property, skipping this one...\n", __func__);
			continue;
		}

		/* Read the product ID from the device */
		if(!CFDictionaryGetValueIfPresent(properties, CFSTR("idProduct"), (void*)&_pid)) {
                        fprintf(stderr, "%s: Device doesn't have a product ID property, skipping this one...\n", __func__);
                        continue;
                }

		CFDictionaryGetValueIfPresent(properties, CFSTR("locationID"), (void*)&_locationid);

		SInt32 vidValue = 0;
                SInt32 pidValue = 0;
                SInt32 locationidValue = 0;

                CFNumberGetValue(_vid, kCFNumberSInt32Type, &vidValue);
                CFNumberGetValue(_pid, kCFNumberSInt32Type, &pidValue);

                if(_locationid) {
                        CFNumberGetValue(_locationid, kCFNumberSInt32Type, &locationidValue);
                }

		fprintf(stderr, "FOUND UNMATCHED DEVICE %#x:%#x @ %#x\n", vidValue, pidValue, locationidValue);
	}

	// At this point we no longer need the iterator
	if(iter) {
		IOObjectRelease(iter);
		iter = IO_OBJECT_NULL;
	}
	return err;
}


const char* mvusblib_usbclass_string(mvusblib_usbclass_t class) {
    switch (class) {
        
        case USBCLASS_USAGEINFO:
            return "See iface descriptors";
            
        case USBCLASS_AUDIO:
            return "Audio";
            
        case USBCLASS_COMM_AND_CDC:
            return "Communications and CDC Control";
            
        case USBCLASS_HID:
            return "Human Interface Device";
            
        case USBCLASS_PHYS:
            return "Physical";
            
        case USBCLASS_IMAGE:
            return "Image";
            
        case USBCLASS_PRINTER:
            return "Printer";
            
        case USBCLASS_MASS_STORAGE:
            return "Mass Storage Device";
            
        case USBCLASS_HUB:
            return "USB Hub";
            
        case USBCLASS_CDC_DATA:
            return "CDC Data";
            
        case USBCLASS_SMARTCARD:
            return "Smart Card";
            
        case USBCLASS_CONTENT_SECURITY:
            return "Content Security";
            
        case USBCLASS_VIDEO:
            return "Video";
            
        case USBCLASS_PERSONAL_HEALTHCARE:
            return "Personal Healthcare";
            
        case USBCLASS_AUDIOVIDEO_DEVICES:
            return "Audio/Video Devices";
            
        case USBCLASS_BILLBOARD_DEVICE:
            return "Billboard Device Class";
            
        case USBCLASS_USB_C_BRIDGE:
            return "USB Type-C Bridge Class";
            
        case USBCLASS_DIAG:
            return "Diagnostic Device";
            
        case USBCLASS_WIRELESS_CONTROLLER:
            return "Wireless Controller";
            
        case USBCLASS_MISC:
            return "Miscellaneous";
            
        case USBCLASS_APP_SPECIFIC:
            return "Application Specific";
            
        case USBCLASS_VENDOR_SPECIFIC:
            return "Vendor Specific";
            
        default:
            return "Unknown";
    }
}
