//
//  products-apple.c
//  iousbcap
//
//  Created by Sem Voigtländer on 19/02/20.
//  Copyright © 2020 kernelprogrammer. All rights reserved.
//

#include "apple.h"

mvusblib_prod_desc_t APPLE_PRODUCTS[UINT16_MAX] = {
    {APPLE_KEYBOARD_M2452, "USB Keyboard [Alps or Logitech, M2452]"},
    {APPLE_KEYBOARD_ALPS, "Keyboard [ALPS]"},
    
    {APPLE_3G_IPOD, "3G iPod"},
    {APPLE_IPOD_2G, "iPod 2G"},
    {APPLE_IPOD_4GEN_GRAYSCALE_40G, "iPod 4.Gen Grayscale 40G"},
    {APPLE_IPOD_PHOTO, "iPod [Photo]"},
    {APPLE_IPOD_MINI_1OR2GEN, "iPod Mini 1.Gen/2.Gen"},
    {APPLE_IPOD_06, "iPod '06'"},
    {APPLE_IPOD_07, "iPod '07'"},
    {APPLE_IPOD_08, "iPod '08'"},
    {APPLE_IPOD_SHUFFLE, "iPod Shuffle"},
    {APPLE_IPOD_SHUFFLE2GEN, "iPod Shuffle 2.Gen"},
    {APPLE_IPOD_SHUFFLE3GEN, "iPod Shuffle 3.Gen"},
    {APPLE_IPOD_SHUFFLE4GEN, "iPod Shuffle 4.Gen"},
    {APPLE_IPOD_TOUCH5GEN_A1421, "iPod Touch 5.Gen [A1421]"},
    
    {APPLE_IPAD2, "iPad 2"},
    {APPLE_IPAD4_MINI1, "iPad 4/Mini1"},
    
    {APPLE_IPHONE, "iPhone"},
    {APPLE_IPHONE3G, "iPhone 3G"},
    {APPLE_IPHONE3GS, "iPhone 3GS"},
    {APPLE_IPHONE4, "iPhone 4"},
    {APPLE_IPHONE4_CDMA, "iPhone 4 (CDMA)"},
    {APPLE_IPHONE4S, "iPhone 4S"},
    
    {APPLE_IPHONE5_5C_5S_6, "iPhone 5 / 5C / 5S / 6"},
    {APPLE_PONGO_OS, "PongoOS device"},
    
    {APPLE_MODEM, "Modem"},
    
    {APPLE_APPLECARE_DONGLE, "Apple Care Dongle"},
    
    {APPLE_BLUETOOTH_HID_KEYBOARD, "Bluetooth HID Keyboard"},
    {APPLE_BLUETOOTH_HID_MOUSE, "Bluetooth HID Mouse"},
    {APPLE_BLUETOOTH_HCI, "Bluetooth HCI"},
    {APPLE_BLUETOOTH_HCI2, "Bluetooth HCI"},
    {APPLE_BLUETOOTH_HCI3, "Bluetooth HCI"},
    {APPLE_BLUETOOTH_HCI4, "Bluetooth HCI"},
    {APPLE_BLUETOOTH_HOST_CONTROLLER, "Bluetooth Host Controller"},
    {APPLE_BLUETOOTH_DFU, "Bluetooth DFU Mode"},
};
