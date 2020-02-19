/*
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

// Documentation
/*
    Project name: Minerva USB library
    Platform: Darwin (XNU)
    Description: Library for working with USB devices at the lowest level in userland
    
    Use case:
 
    1. Print the currently connected USB devices with mvusblib_printdevices();
    2. Given a vendor ID + product ID, and optionally a locationID look for matching devices and perform a callback with a device interface when found.
    3. In the callback, mark the device as open with mvusblib_opendevice();
    4. Write data to the device at the lowest level with mvusblib_writedevice() or perform a control transfer request with mvusblib_control_msg.
*/
 
// Types
typedef int (*mvusblib_awaitcallback_t)(IOUSBDeviceInterface** iface); /* Callback for device detection */

// Methods
int mvusblib_printdevices(void); /* Method for printing currently attached devices */
int mvusblib_awaitdevice(int vid, int pid, int locationid, mvusblib_awaitcallback_t callback); /* Method for looking up an attached device and performing a callback when matching a vid / pid and locationid */
int mvusblib_claiminterface(io_service_t dev, IOUSBDeviceInterface* iface); /* Method to claim a device interface for an iokit usb device reference */
int mvusblib_opendevice(IOUSBDeviceInterface** iface); /* Opens / marks a device as connection open */
int mvusblib_closedevice(IOUSBDeviceInterface** iface); /* Closes / marks a device as connection closed */
int mvusblib_writedevice(IOUSBInterfaceInterface** iface, uint16_t address, uint16_t length, char* data); /* Writes data to the device */
int mvusblib_control_msg(IOUSBDeviceInterface** dev, int type, int reqno, int value, int index, const char *msg, int timeout); /* Method for sending USB Control Transfer requests */

// USB protocol recipient types
typedef enum mvusblib_rcpnt {
	MVUSBLIB_RCPNT_DEV = 0x00,
	MVUSBLIB_RCPNT_IFACE = 0x01,
	MVUSBLIB_RCPNT_EP = 0x02,
	MVUSBLIB_RCPNT_OTHER = 0x03,
} mvusblib_rcpnt_t;

// USB protocol endpoint directions
typedef enum mvusblib_endpoint_dir {
	MVUSBLIB_EP_IN = 0x80,
	MVUSBLIB_EP_OUT = 0x00,
} mvusblib_ep_t;

// USB protocol request types
typedef enum mvusblib_req_type {
	MVUSBLIB_REQ_T_STANDARD 	= (0x00<<5),
	MVUSBLIB_REQ_T_CLASS 	= (0x01<<5),
	MVUSBLIB_REQ_T_VENDOR	= (0x02<<5),
	MVUSBLIB_REQ_T_RESERVED	= (0x03<<5),
} mvusblib_req_t;

#endif
