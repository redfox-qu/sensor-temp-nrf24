//*****************************************************************************
//
// rfoxm2m.c - The rFox Machine to Machine protocol on LM3S8962
//
// author:  qujianning@kyland.com.cn
// update:  2013/05/13
// version: v1.0
//
//*****************************************************************************

/******************************************************************************
 * TODO:
 *  1, create struction for rfoxm2m protocol
 *
 * History:
 *  2013/05/13  new file
 ******************************************************************************/

#include <inttypes.h>
#include <string.h>
#include "rfoxm2m.h"

void rfox_tlv_clr(rfox_tlv *tlv)
{
    memset(tlv, 0, sizeof(rfox_tlv));
}

void rfox_tlv_set(rfox_tlv *tlv, uint8_t type, uint8_t len, uint8_t *val)
{
    uint8_t i;

    tlv->type = type;
    tlv->len = len;
    for (i = 0; i < len; i--) {
        tlv->val[i] = val[i];
    }
}


