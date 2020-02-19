//
//  futuretechdev.h
//  iousbcap
//
//  Created by Sem Voigtländer on 19/02/20.
//  Copyright © 2020 kernelprogrammer. All rights reserved.
//

#ifndef futuretech_dev_h
#define futuretech_dev_h

#include <stdio.h>
#include "mvusblib.h"
typedef enum mvusblib_futuretech_product {
    FUTURETECH_DCSD_STATUS_LED = 0x8a88,
    FUTURETECH_FT32_SERIAL_UART_IC = 0x6001,
} mvusblib_futuretech_product_t;

#endif /* futuretech_dev_h */
