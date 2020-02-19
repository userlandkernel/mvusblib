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

//#define XCODEBUILD

void usage(void) {
    fprintf(stderr, "%s: iousbcap [vid] [pid] (locationid) (lookupinterval = 2)\n", __func__);
    exit(1);
}

int callbackCaptureDevice(IOUSBDeviceInterface** iface) {
    
    kern_return_t err = KERN_SUCCESS;
    
    fprintf(stderr, "%s: Capturing USB traffic on the device.\n", __func__);
    
    err = mvusblib_opendevice(iface);
    
    if( KERN_SUCCESS != err) {
        fprintf(stderr, "%s: Failed to open USB device: %s.\n", __func__, mach_error_string(err));
        return err;
    }
    
    fprintf(stderr, "%s: Got a connection to the device.\n", __func__);
    
    fprintf(stderr, "%s: Starting transfer.\n", __func__);
    while(1) {
        err = mvusblib_control_msg(iface, 0|0x80, 6, 0x30c, 0x409, "EXAMPLE", 0);
        fprintf(stderr, "%s: ctrl_msg_return: %s\n", __func__, mach_error_string(err));
    }
    return err;
}



int main(int argc, char *argv[]) {
    
    uint32_t vid, pid, locationid = 0;
    
#ifndef XCODEBUILD
    
    if (argc <= 2)
        usage();
    
    vid = (uint32_t)strtol(argv[1], NULL, 16);
    pid = (uint32_t)strtol(argv[2], NULL, 16);
    
    if(argc >= 4) {
        locationid = (uint32_t)strtol(argv[3], NULL, 16);
    }
#else
    vid = 0x5ac;
    pid = 0x12a8;
    locationid = 0;
#endif
    
    
    fprintf(stderr, "%s: Starting iousbcap device lookup for %#x:%#x @ %#x\n", __func__, vid, pid, locationid);
    mvusblib_printdevices();
    kern_return_t err = KERN_FAILURE;
    
    while( KERN_SUCCESS != err ){
        err = mvusblib_awaitdevice(vid, pid, locationid, (mvusblib_awaitcallback_t)callbackCaptureDevice);
        sleep(3);
    }
    return 0;
}
