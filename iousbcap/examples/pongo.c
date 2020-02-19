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

char *cmd = NULL;

void usage(void) {
    fprintf(stderr, "%s: iousbcap [vid] [pid] (locationid) (lookupinterval = 2)\n", __func__);
    exit(1);
}

int callbackCaptureDevice(IOUSBDeviceInterface** iface) {
    
    kern_return_t err = KERN_SUCCESS;
    
    fprintf(stderr, "%s: Sending command to the device.\n", __func__);
    
    err = mvusblib_opendevice(iface);
    
    if( KERN_SUCCESS != err) {
        fprintf(stderr, "%s: Failed to open USB device: %s.\n", __func__, mach_error_string(err));
        return err;
    }
    
    fprintf(stderr, "%s: Got a connection to the device.\n", __func__);
    
	fprintf(stderr, "%s: Starting transfer.\n", __func__);
        err = mvusblib_control_msg(iface, 0|0x80, 0x21, 4, 0, NULL, 10);
        fprintf(stderr, "%s: ctrl_msg_return: %s\n", __func__, mach_error_string(err));
	err = mvusblib_control_msg(iface, 0|0x80, 0x21, 3, 0, cmd, 10);
	fprintf(stderr, "%s: ctrl_msg_return: %s\n", __func__, mach_error_string(err));
    return err;
}


static inline const char* lastPathComponent(char* str){
    size_t len = 0;
    for(len = strlen(str); len && str[len] != '/';len--);
    return str+(len ? len+1 : len);
}


int main(int argc, char *argv[]) {
    
    if(argc <= 1) {
        printf("Usage: %s\n",lastPathComponent(argv[0]));
        return 1;
    }
    
    cmd = strdup(argv[1]);
    cmd = strcat(cmd, "\n");
    
    int vid = USBVENDOR_APPLE_INC;
    int pid = 0x4141;
    
    fprintf(stderr, "%s: Starting iousbcap device lookup for %#x:%#x\n", __func__, vid, pid);
    kern_return_t err = KERN_FAILURE;
    
    while( KERN_SUCCESS != err ){
        err = mvusblib_awaitdevice(vid, pid, 0, (mvusblib_awaitcallback_t)callbackCaptureDevice);
        sleep(3);
    }
    return 0;
}
