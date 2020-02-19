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

static uint16_t gVendor = 0;
static uint16_t gProduct = 0;
static uint16_t gLocation = 0;
static mvusblib_awaitcallback_t gCallback = NULL;
static IONotificationPortRef gNotifyPort = MACH_PORT_NULL;
static io_iterator_t gRawAddedIter = IO_OBJECT_NULL;
static io_iterator_t gRawRemovedIter = IO_OBJECT_NULL;

int mvusblib_control_msg(IOUSBDeviceInterface** dev, int type, int reqno, int value, int index, const char *msg, int timeout)
{

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

	kern_return_t err = KERN_SUCCESS;

	// send the request
	err = (*dev)->DeviceRequest(dev, &req);

	return err;
}

void mvusblib_freedevice(IOUSBDeviceInterface** iface) {
    
    if(iface){
        (*iface)->Release(iface);
        iface = NULL;
    }
    
}

int mvusblib_opendevice(IOUSBDeviceInterface** iface) {
    
    kern_return_t err = KERN_SUCCESS;
    
    err = (*iface)->USBDeviceOpen(iface);
    
    if (err != KERN_SUCCESS)
    {
        fprintf(stderr, "%s: Unable to open device: %s\n", __func__, mach_error_string(err));
        return err;
    }
    
    return err;
}

int mvusblib_closedevice(IOUSBDeviceInterface** iface) {
    
    kern_return_t err = KERN_SUCCESS;
    
    err = (*iface)->USBDeviceClose(iface);
    
    if (err != KERN_SUCCESS) {
        fprintf(stderr, "%s: Unable to open device: %s\n", __func__, mach_error_string(err));
        return err;
    }
    
    mvusblib_freedevice(iface);
    
    return err;
}

int mvusblib_claiminterface(io_service_t dev, IOUSBDeviceInterface* iface) {

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

	if ( kIOReturnSuccess != err ) {
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

IOReturn ConfigureDevice(IOUSBDeviceInterface **dev)
{
    UInt8 numConfig;
    IOReturn kr;
    IOUSBConfigurationDescriptorPtr configDesc;
    //Get the number of configurations. The sample code always chooses
    //the first configuration (at index 0) but your code may need a
    //different one
    kr = (*dev)->GetNumberOfConfigurations(dev, &numConfig);
    if (!numConfig)
        return -1;
    //Get the configuration descriptor for index 0
    kr = (*dev)->GetConfigurationDescriptorPtr(dev, 0, &configDesc);
    if (kr)
    {
        printf("Couldn’t get configuration descriptor for index %d (err = %08x)\n", 0, kr);
        return -1;
    }
    //Set the device’s configuration. The configuration value is found in
    //the bConfigurationValue field of the configuration descriptor
    kr = (*dev)->SetConfiguration(dev, configDesc->bConfigurationValue);
    if (kr)
    {
        printf("Couldn’t set configuration to value %d (err = %08x)\n", 0,
               kr);
        return -1;
    }
    return kIOReturnSuccess;
}

void RawDeviceAdded(void *refCon, io_iterator_t iterator)
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
            printf("Unable to create a plug-in (%08x)\n", err);
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
            printf("Couldn’t create a device interface (%08x)\n",
                   (int) result);
            continue;
        }
        //Check these values for confirmation
        err = (*dev)->GetDeviceVendor(dev, &vendor);
        err = (*dev)->GetDeviceProduct(dev, &product);
        err = (*dev)->GetLocationID(dev, &location);
        
        if ((vendor != gVendor) || (product != gProduct))
        {
            printf("Found unwanted device (vendor = %#x, product = %#x, location = %#x)\n", vendor, product, location);
            (void) (*dev)->Release(dev);
            dev = NULL;
            continue;
        }
        else if(gLocation) {
            if(gLocation != location) {
                printf("Found unwanted device (vendor = %#x, product = %#x, location = %#x)\n", vendor, product, location);
                (void) (*dev)->Release(dev);
                dev = NULL;
                continue;
            }
        }
        
        printf("Found the device (vendor = %#x, product = %#x, location = %#x)\n", vendor, product, location);
        
        //Open the device to change its state
        err = (*dev)->USBDeviceOpen(dev);
        if (err != kIOReturnSuccess)
        {
            printf("Unable to open device: %08x\n", err);
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


void RawDeviceRemoved(void *refCon, io_iterator_t iterator)
{
    kern_return_t kr;
    io_service_t object;
    while ((object = IOIteratorNext(iterator)))
    {
        kr = IOObjectRelease(object);
        if (kr != kIOReturnSuccess)
        {
            printf("Couldn’t release raw device object: %08x\n", kr);
            continue;
        }
    }
}

int mvusblib_awaitdevice(int vid, int pid, int locationid, mvusblib_awaitcallback_t callback) {

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
    
    // Set matching to our vendor id and product id
   // CFDictionarySetValue(matching, CFSTR(kUSBVendorName), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &vid));
    //CFDictionarySetValue(matching, CFSTR(kUSBProductName), CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &pid));
    
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
    err = IOServiceAddMatchingNotification(gNotifyPort, kIOFirstMatchNotification, matching, RawDeviceAdded, NULL, &gRawAddedIter);

    RawDeviceAdded(NULL, gRawAddedIter);
    
    mach_port_deallocate(mach_task_self(), masterPort);
    masterPort = MACH_PORT_NULL;
    
    // Listen for device remove
    err = IOServiceAddMatchingNotification(gNotifyPort, kIOTerminatedNotification, matching, RawDeviceRemoved, NULL, &gRawRemovedIter);
    
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

		printf("FOUND UNMATCHED DEVICE %#x:%#x @ %#x\n", vidValue, pidValue, locationidValue);
	}

	// At this point we no longer need the iterator
	if(iter) {
		IOObjectRelease(iter);
		iter = IO_OBJECT_NULL;
	}
	return err;
}
