//*****************************************************************************
//
// rfoxm2m.h - The rFox Machine to Machine protocol on LM3S8962
//
// author:  qujianning@kyland.com.cn
// update:  2013/05/13
// version: v1.0
//
//*****************************************************************************

#ifndef __RFOX_M2M_H__
#define __RFOX_M2M_H__


#define RFOX_TYPE_TEMP_DS18B20      0x01
#define RFOX_TYPE_TEMP_8962         0x02


typedef struct rfox_tlv_st {
    uint8_t type;
    uint8_t len;
    uint8_t val[30];
} rfox_tlv;

void rfox_tlv_clr(rfox_tlv *tlv);
void rfox_tlv_set(rfox_tlv *tlv, uint8_t type, uint8_t len, uint8_t *val);

#endif  //__RFOX_M2M_H__
