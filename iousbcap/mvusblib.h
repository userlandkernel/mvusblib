/**
	
    Simple library using IOUSBDeviceFamily and IOKit to talk to USB devices.
	Written by Sem Voigtländer on November 13, 2019

	Licensed under the MIT License

 */

#ifndef MVUSBLIB_H
#define MVUSBLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mach/mach.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <CoreFoundation/CoreFoundation.h>

#include "vendors.h"
#include "products.h"

/**
    Project name: Minerva USB library
    Platform: Darwin (XNU)
    Description: Library for working with USB devices at the lowest level in userland
    
    Use case:
 
    1. Print the currently connected USB devices with mvusblib_printdevices();
    2. Given a vendor ID + product ID, and optionally a locationID look for matching devices and perform a callback with a device interface when found.
    3. In the callback, mark the device as open with mvusblib_opendevice();
    4. Write data to the device at the lowest level with mvusblib_writedevice() or perform a control transfer request with mvusblib_control_msg.
*/

/**
  * @typedef mvusblib_location_t
  * @brief USB Location Identifier
**/
typedef uint32_t mvusblib_location_t;
 
/**
 * @typedef mvusblib_awaitcallback_t
 * @brief Callback for mvusblib_awaitdevice()
*/
typedef int (*mvusblib_awaitcallback_t)(IOUSBDeviceInterface** iface);

/**
 * @function mvusblib_printdevice
 * @brief Prints all USB devices to the console
 * @return Error code or success
*/
kern_return_t mvusblib_printdevices(void);

/**
 * @function mvusblib_awaitdevice
 * @brief Waits for a device to be discovered using IONotifications
 * @param vid USB Vendor Identifier
 * @param pid USB Product Identifier
 * @param locationid USB Location Identifier
 * @param callback Function to call when the device has been detected
 * @return Error code or success
 */
kern_return_t mvusblib_awaitdevice(mvusblib_vendor_t vid, mvusblib_product_t pid, uint32_t locationid, mvusblib_awaitcallback_t callback);

/**
 @function mvusblib_claiminterface
 @brief Claims a USB interface on an IOKit USB Device object
 @param dev IOKit USB Device object
 @param iface USB Interface reference to be updated with the claimed interface
 @return KERN_SUCCESS on success, or error on failure
 */
kern_return_t mvusblib_claiminterface(io_service_t dev, IOUSBDeviceInterface* iface);

/**
 @function mvusblib_opendevice
 @brief Opens the device via the interface
 @param iface Reference to interface reference to call the open operation on
 @return KERN_SUCCESS on success, or error on failure
 */
kern_return_t mvusblib_opendevice(IOUSBDeviceInterface** iface);

/**
 @function mvusblib_closedevice
 @brief Closes the device via the interface
 @param iface Reference to interface reference to call the close operation on
 @return KERN_SUCCESS on success, or error on failure
 */
kern_return_t mvusblib_closedevice(IOUSBDeviceInterface** iface);

/**
 @function mvusblib_writedevice
 @brief Directly performs a write transfer on the device
 @param address Address to write to
 @param length Length of the data to write
 @param data Data to write to the address
 @return KERN_SUCCESS on success, or error on failure
 */
kern_return_t mvusblib_writedevice(IOUSBInterfaceInterface** iface, uint16_t address, uint16_t length, char* data);

/**
 @function mvusblib_control_msg
 @brief Method for sending USB Control Transfer requests
 @param dev USB interface to send the control message to
 @param type USB Control Transfer type
 @param reqno USB Control Request number
 @param value USB Control Request value
 @param index USB Control Transfer index to use
 @param msg USB Control Transfer data to send
 @param timeout USB Control Transfer timeout before the transfer fails
 @return KERN_SUCCESS on success, or error on failure
 */
kern_return_t mvusblib_control_msg(IOUSBDeviceInterface** dev, int type, int reqno, int value, int index, const char *msg, int timeout);

/**
 @typedef mvusblib_rcpnt
 @brief USB Protocol Recipient identifier
*/
typedef enum mvusblib_rcpnt {
	MVUSBLIB_RCPNT_DEV = 0x00,
	MVUSBLIB_RCPNT_IFACE = 0x01,
	MVUSBLIB_RCPNT_EP = 0x02,
	MVUSBLIB_RCPNT_OTHER = 0x03,
} mvusblib_rcpnt_t;

/**
 @typedef mvusblib_endpoint_dir
 @brief USB Protocol Endpoint Direction
 */
typedef enum mvusblib_endpoint_dir {
	MVUSBLIB_EP_IN = 0x80,
	MVUSBLIB_EP_OUT = 0x00,
} mvusblib_ep_t;

/**
 @typedef mvusblib_req_type
 @brief USB Protocol Request Type
 */
typedef enum mvusblib_req_type {
	MVUSBLIB_REQ_T_STANDARD 	= (0x00<<5),
	MVUSBLIB_REQ_T_CLASS 	= (0x01<<5),
	MVUSBLIB_REQ_T_VENDOR	= (0x02<<5),
	MVUSBLIB_REQ_T_RESERVED	= (0x03<<5),
} mvusblib_req_t;

/**
 @typedef mvusblib_usb_class
 @brief USB Protocol Class
 */
typedef enum mvusblib_usb_class {
    USBCLASS_USAGEINFO = 0x00,
    USBCLASS_AUDIO = 0x01,
    USBCLASS_COMM_AND_CDC = 0x02,
    USBCLASS_HID = 0x03,
    USBCLASS_PHYS = 0x05,
    USBCLASS_IMAGE = 0x06,
    USBCLASS_PRINTER = 0x07,
    USBCLASS_MASS_STORAGE = 0x08,
    USBCLASS_HUB = 0x09,
    USBCLASS_CDC_DATA = 0x0A,
    USBCLASS_SMARTCARD = 0x0B,
    USBCLASS_CONTENT_SECURITY = 0x0D,
    USBCLASS_VIDEO = 0x0E,
    USBCLASS_PERSONAL_HEALTHCARE = 0x0F,
    USBCLASS_AUDIOVIDEO_DEVICES = 0x10,
    USBCLASS_BILLBOARD_DEVICE = 0x11,
    USBCLASS_USB_C_BRIDGE = 0x12,
    USBCLASS_DIAG = 0xDC,
    USBCLASS_WIRELESS_CONTROLLER = 0xE0,
    USBCLASS_MISC = 0xEF,
    USBCLASS_APP_SPECIFIC = 0xFE,
    USBCLASS_VENDOR_SPECIFIC = 0xFF
} mvusblib_usbclass_t;

/**
 @function mvusblib_usbclass_string
 @param class USB protocol class
 @brief Converts a USB class enum value to a representing string
 @return String representation of the USB class enum value
 */
const char* mvusblib_usbclass_string(mvusblib_usbclass_t class);

#endif
