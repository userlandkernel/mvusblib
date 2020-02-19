//
//  products.h
//  iousbcap
//
//  Created by Sem Voigtländer on 19/02/20.
//  Copyright © 2020 kernelprogrammer. All rights reserved.
//

#ifndef products_h
#define products_h

#include <stdio.h>
#include "vendors.h"

#define MAX_PRODUCTNAME 1024
typedef char mvusblib_productname_t[MAX_VENDORNAME];

typedef uint16_t mvusblib_product_t;

typedef struct mvusblib_product_descriptor {
    mvusblib_product_t product;
    char* name;
} mvusblib_prod_desc_t;

void mvusblib_product_string(mvusblib_vendor_t vendor, mvusblib_product_t product, mvusblib_productname_t productName);
#endif /* products_h */
