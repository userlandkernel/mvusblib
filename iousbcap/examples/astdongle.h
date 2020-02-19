//
//  astdongle.h
//  iousbcap
//
//  Created by Sem Voigtländer on 19/02/20.
//  Copyright © 2020 kernelprogrammer. All rights reserved.
//

#ifndef astdongle_h
#define astdongle_h

#include <stdio.h>
#include <stdint.h>
#include "mvftdi.h"
typedef struct ASTDongleUART {
    uint32_t pid;
    uint32_t vid;
    uint32_t location;
    mvftdi_ctx_t ftdi;
    IOUSBDeviceInterface** iface;
} ASTDongleUARTDescriptor;
typedef struct ASTDongle {
    uint32_t pid;
    uint32_t vid;
    uint32_t location;
    ASTDongleUARTDescriptor uart;
    IOUSBDeviceInterface **iface;
} ASTDongleDescriptor;


#endif /* astdongle_h */
