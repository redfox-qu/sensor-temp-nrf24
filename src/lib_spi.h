//*****************************************************************************
//
// lib_spi.h - SPI routines for the nrf2401.
//
//*****************************************************************************

#ifndef __LIB_SPI_H__
#define __LIB_SPI_H__

#include "inc/hw_memmap.h"
#include <inttypes.h>

typedef struct spi_driver_st
{
    uint8_t *out_data;                  //
    uint32_t out_count;                 //
    uint32_t out_extra;                 //
    uint8_t *in_data;                   //
    uint32_t in_count;                  //
    uint32_t in_extra;                  //
    uint8_t status;                     //
} SPI_DRIVER;

void SPI_init(void);
int SPI_spi1_read(char *buf, unsigned long *size);
int SPI_spi1_write(char *buf, unsigned long size);
int SPI_spi0_read(unsigned long regaddr, char *buf, unsigned long size);
int SPI_spi0_write(unsigned long regaddr, char *buf, unsigned long size);
void SPI_spi0_xfer(unsigned char *pucDataOut, unsigned long ulOutCount,
            unsigned char *pucDataIn, unsigned long ulInCount);
void SPI_spi0_master_init(void);


#endif // __LIB_SPI_H__

