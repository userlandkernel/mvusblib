/*
	This file contains code from libftdi which is licensed under the GNU lesser public license.
	Libftdi is free software and so is this code, however do note that as permitted by GNU LPL this code is redistributed under the MIT License.
*/

#ifndef MVFTDI_H
#define MVFTDI_H
#include "mvusblib.h"
typedef struct mvftdi_ctx {
        IOUSBDeviceInterface** usbdev;
        int index;
        int usb_tx_to;
        unsigned char bb_mode;
        bool bb;
} *mvftdi_ctx_t;

typedef enum mvftdi_chip_type
{
     TYPE_AM	=0,
     TYPE_BM	=1,
     TYPE_2232C	=2,
     TYPE_R	=3,
     TYPE_2232H	=4,
     TYPE_4232H	=5,
     TYPE_232H	=6,
     TYPE_230X	=7,
} mvftdi_chiptype_t;

typedef enum mvftdi_parity_type {
	NONE	=0,
	ODD	=1,
	EVEN	=2,
	MARK	=3,
	SPACE	=4,
} mvftdi_parity_t;

typedef enum mvftdi_stopbits_type {
	STOP_BIT_1	=0,
	STOP_BIT_15	=1,
	STOP_BIT_2	=2
} mvftdi_stopbit_t;

typedef enum mvftdi_bits_type {
	BITS_7=7,
	BITS_8=8
} mvftdi_bits_t;

typedef enum mvftdi_break_type {
	BREAK_OFF=0,
	BREAK_ON=1
} mvftdi_brk_t;

typedef enum mvftdi_mpsse_mode {
     BITMODE_RESET  = 0x00,
     BITMODE_BITBANG= 0x01,
     BITMODE_MPSSE  = 0x02,
     BITMODE_SYNCBB = 0x04,
     BITMODE_MCU    = 0x08,
     /* CPU-style fifo mode gets set via EEPROM */
     BITMODE_OPTO   = 0x10,
     BITMODE_CBUS   = 0x20,
     BITMODE_SYNCFF = 0x40,
     BITMODE_FT1284 = 0x80,
} mvftdi_mpsse_mode_t;

typedef enum mvftdi_interface_class {
     MVFTDI_IFACE_ANY = 0,
     MVFTDI_IFACE_A   = 1,
     MVFTDI_IFACE_B   = 2,
     MVFTDI_IFACE_C   = 3,
     MVFTDI_IFACE_D   = 4
} mvftdi_iface_class_t;

typedef enum mvftdi_module_detach_mode {
     AUTO_DETACH_SIO_MODULE = 0,
     DONT_DETACH_SIO_MODULE = 1
 } mvftdi_mod_detachmode_t;

/* Commands in MPSSE and Host Emulation Mode */
#define SEND_IMMEDIATE 0x87
#define WAIT_ON_HIGH   0x88
#define WAIT_ON_LOW    0x89

/* Commands in Host Emulation Mode */
#define READ_SHORT     0x90
/* Address_Low */
#define READ_EXTENDED  0x91
/* Address High */
/* Address Low  */
#define WRITE_SHORT    0x92
/* Address_Low */
#define WRITE_EXTENDED 0x93
/* Address High */
/* Address Low  */

/* Definitions for flow control */
#define SIO_RESET          0 /* Reset the port */
#define SIO_MODEM_CTRL     1 /* Set the modem control register */
#define SIO_SET_FLOW_CTRL  2 /* Set flow control register */
#define SIO_SET_BAUD_RATE  3 /* Set baud rate */
#define SIO_SET_DATA       4 /* Set the data characteristics of the port */

/* Requests */
#define SIO_RESET_REQUEST             SIO_RESET
#define SIO_SET_BAUDRATE_REQUEST      SIO_SET_BAUD_RATE
#define SIO_SET_DATA_REQUEST          SIO_SET_DATA
#define SIO_SET_FLOW_CTRL_REQUEST     SIO_SET_FLOW_CTRL
#define SIO_SET_MODEM_CTRL_REQUEST    SIO_MODEM_CTRL
#define SIO_POLL_MODEM_STATUS_REQUEST 0x05
#define SIO_SET_EVENT_CHAR_REQUEST    0x06
#define SIO_SET_ERROR_CHAR_REQUEST    0x07
#define SIO_SET_LATENCY_TIMER_REQUEST 0x09
#define SIO_GET_LATENCY_TIMER_REQUEST 0x0A
#define SIO_SET_BITMODE_REQUEST       0x0B
#define SIO_READ_PINS_REQUEST         0x0C
#define SIO_READ_EEPROM_REQUEST       0x90
#define SIO_WRITE_EEPROM_REQUEST      0x91
#define SIO_ERASE_EEPROM_REQUEST      0x92

#define MVFTDIDEV_OUT_REQ_T (MVUSBLIB_REQ_T_VENDOR | MVUSBLIB_RCPNT_DEV | MVUSBLIB_EP_OUT)
#define MVFTDIDEV_IN_REQ_T (MVUSBLIB_REQ_T_VENDOR | MVUSBLIB_RCPNT_DEV | MVUSBLIB_EP_IN)

mvftdi_ctx_t mvftdi_open(IOUSBDeviceInterface** usbdev);
kern_return_t mvftdi_set_bitmode(mvftdi_ctx_t ftdi, unsigned char bitmask, unsigned char mode);
kern_return_t mvftdi_read_pins(mvftdi_ctx_t ftdi, unsigned char* pins);
#endif
