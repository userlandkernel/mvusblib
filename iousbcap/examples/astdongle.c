//
//  astdongle.c
//  iousbcap
//
//  Created by Sem Voigtländer on 19/02/20.
//  Copyright © 2020 kernelprogrammer. All rights reserved.
//

#include "astdongle.h"
#include "mvusblib.h"
#include "mvftdi.h"
#include <string.h>
#include <mach/mach.h>
ASTDongle dongle = {
    .pid = 0x1009,
    .vid = 0x05ac,
    .location = 0,
    .uart = {
        .pid = 0x6001,
        .vid = 0x0403,
        .location = 0xfa430000,
        .ftdi = NULL
    }
};

static inline const char* lastPathComponent(char* str){
    size_t len = 0;
    for(len = strlen(str); len && str[len] != '/';len--);
    return str+(len ? len+1 : len);
}

// Taken from GeoSn0w
int light_up(int led) {
    kern_return_t err = KERN_SUCCESS;
    long int tab[5] = {0xF0, 0xF2, 0xF8, 0xF1, 0xFB};
    static int led_status = -1;
    unsigned char buf[1];
    if (!(led ^ led_status)) {
        return KERN_SUCCESS;
    } else {
        led_status = led;
    }
    if(!dongle.uart.ftdi) {
        return KERN_FAILURE;
    }
    err = mvftdi_set_bitmode(dongle.uart.ftdi, tab[led], BITMODE_CBUS);
    err = mvftdi_read_pins(dongle.uart.ftdi, &buf[0]);
    return KERN_SUCCESS;
}

int callbackDongleUARTFound(IOUSBDeviceInterface** iface){
    
    kern_return_t err = KERN_SUCCESS;
    
    dongle.uart.iface = iface;
    
    fprintf(stderr, "%s: Found the AST2 Dongle UART interface.\n", __func__);
    err = mvusblib_opendevice(iface);
    
    if( KERN_SUCCESS != err) {
        fprintf(stderr, "%s: Failed to open USB device: %s.\n", __func__, mach_error_string(err));
        return err;
    }
    
    fprintf(stderr, "%s: Got a connection to the device.\n", __func__);
    
    dongle.uart.ftdi = mvftdi_open(iface);
    
    light_up(3);
    
    return err;
}

int callbackDongleFound(IOUSBDeviceInterface** iface) {
    kern_return_t err = KERN_SUCCESS;
    fprintf(stderr, "%s: Found the AST2 Dongle.\n", __func__);
    
    
    
    UInt32 locationid = 0;
    (*iface)->GetLocationID(iface, &locationid);
    
    printf("Opening AST2 Dongle @ %#x\n", locationid);
    err = mvusblib_opendevice(iface);
    
    if( KERN_SUCCESS != err) {
        fprintf(stderr, "%s: Failed to open USB device: %s.\n", __func__, mach_error_string(err));
        return err;
    }
    
    fprintf(stderr, "%s: Got a connection to the device.\n", __func__);
    
    fprintf(stderr, "%s: Starting astdongle UART device lookup for %#04x:%#04x @ %#x\n", __func__, dongle.uart.vid, dongle.uart.pid, dongle.uart.location);
    while( KERN_SUCCESS != err ){
        
        err = mvusblib_awaitdevice(dongle.uart.vid, dongle.uart.pid, dongle.uart.location, (mvusblib_awaitcallback_t)callbackDongleUARTFound);
        usleep(100);
    }
    
    return err;
}


int main(int argc, char *argv[]) {

    if(argc <= 0) {
        printf("Usage: %s\n",lastPathComponent(argv[0]));
        return 1;
    }
    
    fprintf(stderr, "%s: Starting astdongle device lookup for %#x:%#x @ %#x\n", __func__, dongle.vid, dongle.pid, dongle.location);
    kern_return_t err = KERN_FAILURE;
    while( KERN_SUCCESS != err ){
        
        err = mvusblib_awaitdevice(dongle.vid, dongle.pid, dongle.location, (mvusblib_awaitcallback_t)callbackDongleFound);
        usleep(100);
    }
}
