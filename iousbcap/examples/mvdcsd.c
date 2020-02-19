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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <CoreFoundation/CoreFoundation.h>
#include "mvusblib.h"
#include "mvftdi.h"

void usage(void) {
    fprintf(stderr, "%s: mvdcsd [ledcode]\n", __func__);
    exit(1);
}

mvftdi_ctx_t ftdi = NULL;

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
	if(!ftdi) {
		return KERN_FAILURE;
	}
	err = mvftdi_set_bitmode(ftdi, tab[led], BITMODE_CBUS);
	err = mvftdi_read_pins(ftdi, &buf[0]);
	return KERN_SUCCESS;
}

void msleep(unsigned int ms){
	usleep(ms * 1000);
}

int lednumber = 0;

int callbackDCSDDevice(IOUSBDeviceInterface** iface) {
    kern_return_t err = KERN_SUCCESS;
    fprintf(stderr, "%s: Making the serial cable twinkle.\n", __func__);

    err = mvusblib_opendevice(iface);

    if( KERN_SUCCESS != err) {
        fprintf(stderr, "%s: Failed to open USB device: %s.\n", __func__, mach_error_string(err));
        return err;
    }

    fprintf(stderr, "%s: Got a connection to the device.\n", __func__);

    ftdi = mvftdi_open(iface);
    light_up(lednumber);
    return err;
}



int main(int argc, char *argv[]) {

    uint32_t vid, pid, locationid;
    
    if (argc <= 1) {
        usage();
    }
    
    lednumber = (int)atoi(argv[1]);

    vid = USBVENDOR_FUTURE_TECH_DEV_INTL_LTD;
    pid = 0x8a88;
    locationid = 0;

    fprintf(stderr, "%s: Starting mvdcsd device lookup for %#x:%#x @ %#x\n", __func__, vid, pid, locationid);
    kern_return_t err = KERN_FAILURE;
    while( KERN_SUCCESS != err ){

        err = mvusblib_awaitdevice(vid, pid, locationid, (mvusblib_awaitcallback_t)callbackDCSDDevice);
        usleep(100);
    }
    return 0;
}
