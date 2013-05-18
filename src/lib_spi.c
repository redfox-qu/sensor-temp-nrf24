//*****************************************************************************
//
// lib_spi.c - SPI routines for the nrf2401.
//
//*****************************************************************************

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/ssi.h"
#include "driverlib/interrupt.h"
#include "src/lib_spi.h"


//*****************************************************************************
//
// Init SPI0 hardware to FPGA(MASTER) as Normal
//
//*****************************************************************************

#if 0
//
// this dirver is use the interrupt to driven the ssi hardware
//
static SPI_DRIVER spi0_driver;

//*****************************************************************************
//
// The interrupt handler for the SSI0 interrupt.
//
//*****************************************************************************
void SPI_spi0_int_handler(void)
{
    unsigned long ulStatus, ulCount, ulData;

    //
    // Get the reason for the interrupt.
    //
    ulStatus = SSIIntStatus(SSI0_BASE, true);

    //
    // See if the receive interrupt is being asserted.
    //
    if(ulStatus & (SSI_RXFF | SSI_RXTO))
    {
        //
        // Loop as many times as required to empty the FIFO.
        //
        while(1)
        {
            //
            // Read a byte from the FIFO if possible.  Break out of the loop if
            // the FIFO is empty.
            //
            if(SSIDataGetNonBlocking(SSI0_BASE, &ulData) == 0)
            {
                break;
            }

            //
            // See if this byte needs to be saved.
            //
            if(spi0_driver.in_count)
            {
                //
                // Save this byte.
                //
                *spi0_driver.in_data++ = ulData;

                //
                // Decrement the count of bytes to save.
                //
                spi0_driver.in_count--;
            }
            else
            {
                //
                // Decrement the count of extra bytes to read.
                //
                spi0_driver.in_extra--;
            }
        }

        //
        // See if all data has been transmitted and received.
        //
        if((spi0_driver.in_count + spi0_driver.in_extra + spi0_driver.out_count + spi0_driver.out_extra) == 0)
        {
            //
            // All data has been transmitted and received, so disable the
            // receive interrupt.
            //
            SSIIntDisable(SSI0_BASE, SSI_RXFF | SSI_RXTO);

            //
            // Deassert the SSI chip select.
            //
            GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
        }
    }

    //
    // See if the transmit interrupt is being asserted.
    //
    if(ulStatus & SSI_TXFF)
    {
        //
        // Write up to four bytes into the FIFO.
        //
        for(ulCount = 0; ulCount < 4; ulCount++)
        {
            //
            // See if there is more data to be transmitted.
            //
            if(spi0_driver.out_count)
            {
                //
                // Transmit this byte if possible.  Break out of the loop if
                // the FIFO is full.
                //
                if(SSIDataPutNonBlocking(SSI0_BASE, *spi0_driver.out_data) == 0)
                {
                    break;
                }

                //
                // Decrement the count of bytes to be transmitted.
                //
                spi0_driver.out_data++;
                spi0_driver.out_count--;
            }

            //
            // See if there are more extra bytes to be transmitted.
            //
            else if(spi0_driver.out_extra)
            {
                //
                // Transmit this extra byte if possible.  Break out of the loop
                // if the FIFO is full.
                //
                if(SSIDataPutNonBlocking(SSI0_BASE, 0) == 0)
                {
                    break;
                }

                //
                // Decrement the count of extra bytes to be transmitted.
                //
                spi0_driver.out_extra--;
            }

            //
            // Otherwise, stop transmitting data.
            //
            else
            {
                //
                // Disable the transmit interrupt since all data to be
                // transmitted has been written into the FIFO.
                //
                SSIIntDisable(SSI0_BASE, SSI_TXFF);

                //
                // Break out of the loop since there is no more data to
                // transmit.
                //
                break;
            }
        }
    }
}


//*****************************************************************************
//
// This will start an interrupt driven transfer to the SSI peripheral.
//
//*****************************************************************************
void SPI_spi0_xfer(unsigned char *pucDataOut, unsigned long ulOutCount,
            unsigned char *pucDataIn, unsigned long ulInCount)
{
    uint8_t err;

    OSSemPend(spi0_driver.sem, 0, &err);

    //
    // Save the output data buffer.
    //
    spi0_driver.out_data = pucDataOut;
    spi0_driver.out_count = ulOutCount;

    //
    // Save the input data buffer.
    //
    spi0_driver.in_data = pucDataIn;
    spi0_driver.in_count = ulInCount;

    //
    // Compute the number of extra bytes to send or receive.  These counts make
    // the number of bytes transmitted equal to the number of bytes received;
    // a requirement of the SSI peripheral.
    //
    if(ulInCount > ulOutCount)
    {
        spi0_driver.out_extra = ulInCount - ulOutCount;
        spi0_driver.in_extra = 0;
    }
    else
    {
        spi0_driver.out_extra = 0;
        spi0_driver.in_extra = ulOutCount - ulInCount;
    }

    //
    // Assert the SSI chip select.
    //
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);

    //
    // Enable the transmit and receive interrupts.  This will start the actual
    // transfer.
    //
    SSIIntEnable(SSI0_BASE, SSI_TXFF | SSI_RXFF | SSI_RXTO);

    //
    // Wait until the SSI chip select deasserts, indicating the end of the
    // transfer.
    //
    while(!(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_3) & GPIO_PIN_3))
    {
    }

    OSSemPost(spi0_driver.sem);
}
#endif


//*****************************************************************************
//
// This will use library function driven transfer to the SSI peripheral.
//
//*****************************************************************************
void SPI_spi0_xfer(unsigned char *pucDataOut, unsigned long ulOutCount,
            unsigned char *pucDataIn, unsigned long ulInCount)
{
    unsigned long ulDataTx;
    unsigned long ulDataRx;
    unsigned long ulindex;

    while(SSIDataGetNonBlocking(SSI0_BASE, &ulDataRx))
    {
    }

    for(ulindex = 0; ulindex < ulOutCount; ulindex++)
    {
        //
        // Send the data using the "blocking" put function.  This function
        // will wait until there is room in the send FIFO before returning.
        // This allows you to assure that all the data you send makes it into
        // the send FIFO.
        //
        ulDataTx = *pucDataOut++;
        SSIDataPut(SSI0_BASE, ulDataTx);

        // Read and Drop no use data
        SSIDataGet(SSI0_BASE, &ulDataRx);
    }

    //
    // Wait until SSI0 is done transferring all the data in the transmit FIFO.
    //
    while(!SSIBusy(SSI0_BASE))
    {
    }

    // Read and Drop no use data
    //while(SSIDataGetNonBlocking(SSI0_BASE, &ulDataRx))
    //{
    //}

    for(ulindex = 0; ulindex < ulInCount; ulindex++)
    {
        //
        // Give a clock to slaver and recive data from fifo_rx
        //
        SSIDataPut(SSI0_BASE, 0);

        //
        // Receive the data using the "blocking" Get function. This function
        // will wait until there is data in the receive FIFO before returning.
        //
        SSIDataGet(SSI0_BASE, &ulDataRx);

        //
        // Since we are using 8-bit data, mask off the MSB.
        //
        ulDataRx &= 0x00FF;

        //
        // return the data that SSI0 received.
        //
        *pucDataIn++ = (unsigned char)ulDataRx;
    }
}

void SPI_spi0_master_init(void)
{
    //spi0_driver.sem = OSSemCreate(1);

    // The SSI0 peripheral must be enabled for use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

    // For this example SSI0 is used with PortA[5:2]. GPIO port A needs to be
    // enabled so these pins can be used.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configure the pin muxing for SSI0 functions on port A2, A3, A4, and A5.
    // This step is not necessary if your part does not support pin muxing.
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    //GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);

    // Configure the GPIO settings for the SSI pins.
    // The pins are assigned as follows:
    //      PA5 - SSI0Tx
    //      PA4 - SSI0Rx
    //      PA3 - SSI0Fss
    //      PA2 - SSI0CLK
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | /*GPIO_PIN_3 |*/ GPIO_PIN_2);

    // Use GPIO function to SPIFss Pin
    //GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);
    //GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);

    // Configure and enable the SSI port for SPI master mode.  Use SSI0,
    // system clock supply, idle clock level low and active low clock in
    // freescale SPI mode, master mode, 1MHz SSI frequency, and 8-bit data.
    SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 1000000, 8);

    // Enable the SSI0 module.
    SSIEnable(SSI0_BASE);
}


//*****************************************************************************
//
// Init the SPI Interface
//
//*****************************************************************************
void SPI_init(void)
{
    SPI_spi0_master_init();
}

