#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "mvftdi.h"
#include "mvusblib.h"

mvftdi_ctx_t mvftdi_open(IOUSBDeviceInterface** usbdev) {

	if(usbdev == NULL) {
		printf("%s: WARNING, passing a null USB device.", __func__);
		return NULL;
	}

	mvftdi_ctx_t ftdi = malloc(sizeof(struct mvftdi_ctx));
	memset(ftdi, 0, sizeof(struct mvftdi_ctx));
	ftdi->usbdev = usbdev;
	ftdi->usb_tx_to = 10;
    return ftdi;
}


kern_return_t mvftdi_set_bitmode(mvftdi_ctx_t ftdi, unsigned char bitmask, unsigned char mode) {
	unsigned short usbval;
	if(ftdi == NULL || ftdi->usbdev == NULL) {
		printf("%s: USB device or ftdi context may not be null!\n", __func__);
		return KERN_INVALID_ARGUMENT;
	}
	usbval = bitmask;
	usbval |= (mode << 8);
	int err = mvusblib_control_msg(ftdi->usbdev, MVFTDIDEV_OUT_REQ_T, SIO_SET_BITMODE_REQUEST, usbval, ftdi->index, NULL, ftdi->usb_tx_to);
    
    if(err) {
        
    }
    
	ftdi->bb_mode = mode;
	ftdi->bb = (mode == BITMODE_RESET) ? 0 : 1;
	return KERN_SUCCESS;
}

kern_return_t mvftdi_read_pins(mvftdi_ctx_t ftdi, unsigned char* pins) {

	if(ftdi == NULL || ftdi->usbdev == NULL) {
                printf("%s: USB device or ftdi context may not be null!\n", __func__);
                return KERN_INVALID_ARGUMENT;
    }

	int err = mvusblib_control_msg(ftdi->usbdev, MVFTDIDEV_IN_REQ_T, SIO_READ_PINS_REQUEST, 0, ftdi->index, (const char*)pins, ftdi->usb_tx_to);
    if(err) {
        
    }
	return KERN_SUCCESS;
}

