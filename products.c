//
//  products.c
//  iousbcap
//
//  Created by Sem Voigtländer on 19/02/20.
//  Copyright © 2020 kernelprogrammer. All rights reserved.
//

#include "products.h"
#include "apple.h"


static inline mvusblib_prod_desc_t* lookup(mvusblib_prod_desc_t desc[UINT16_MAX], uint16_t product) {
    for(int i = 0; i < UINT16_MAX; i++) {
        if(desc[i].product == product && desc[i].name)
            return &desc[i];
    }
    return NULL;
}

void mvusblib_product_string(mvusblib_vendor_t vendor, uint16_t product, char* stringOut) {
   
    extern mvusblib_prod_desc_t APPLE_PRODUCTS[UINT16_MAX];
    extern mvusblib_prod_desc_t FUTURETECH_DEV_PRODUCTS[UINT16_MAX];
    
    if(!stringOut)
        return;
    

    switch(vendor){
            
        case USBVENDOR_APPLE_INC: {
            mvusblib_prod_desc_t* desc = lookup(APPLE_PRODUCTS, product);
            if(!desc)
                goto FAIL;
            strcpy(stringOut, desc->name);
            break;
        }
            
        case USBVENDOR_FUTURE_TECH_DEV_INTL_LTD: {
            mvusblib_prod_desc_t* desc = lookup(FUTURETECH_DEV_PRODUCTS, product);
            if(!desc)
                goto FAIL;
            strcpy(stringOut, desc->name);
            break;
        }
           

        default:
FAIL:
            sprintf(stringOut, "UNKNOWN %#x", product);
            break;
            
    }
}
