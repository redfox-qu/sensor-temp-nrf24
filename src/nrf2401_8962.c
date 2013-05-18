//*****************************************************************************
//
// nrf2401_8962.c - The NRF24L01 Project on LM3S8962
//
// author:  qujianning@kyland.com.cn
// update:  2013/05/02
// version: v1.0
//
//*****************************************************************************

/******************************************************************************
TODO:
    1, add the nrf24l01 test function
    2, add the spi test function

History:
    2013/05/09  add the inner tempurator reader function
    2013/05/09  add the ds18b20 test function
    2013/05/09  new file
******************************************************************************/

#include <stdio.h>
#include <inttypes.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "driverlib/systick.h"
#include "thirdparty/rit128x96x4.h"
#include "thirdparty/uartstdio.h"
#include "thirdparty/ustdlib.h"
#include "inc/hw_gpio.h"
#include "nrf24l01.h"
#include "rfoxm2m.h"

#define SYSTICKHZ               100
#define SYSTICKMS               (1000 / SYSTICKHZ)
#define SYSTICKUS               (1000000 / SYSTICKHZ)
#define SYSTICKNS               (1000000000 / SYSTICKHZ)

unsigned long sys_tick = 0;

extern void w1_init(void);
extern uint16_t ds18b20_read_temp(void);

void InitConsole(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioInit(0);
}

void SysTickIntHandler(void)
{
    sys_tick++;
}

void inner_temp(void)
{
    unsigned long ulADC0_Value[1];

    unsigned long ulTemp_ValueC;
    unsigned long ulTemp_ValueF;

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE |
                             ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);

    while(1)
    {
        ADCProcessorTrigger(ADC0_BASE, 3);

        while(!ADCIntStatus(ADC0_BASE, 3, false))
        {
        }

        ADCSequenceDataGet(ADC0_BASE, 3, ulADC0_Value);

        ulTemp_ValueC = ((1475 * 1023) - (2250 * ulADC0_Value[0])) / 10230;
        ulTemp_ValueF = ((ulTemp_ValueC * 9) + 160) / 5;

        UARTprintf("Temperature = %3d*C or %3d*F\r", ulTemp_ValueC,
                   ulTemp_ValueF);

        SysCtlDelay(SysCtlClockGet() / 12);
    }
}


void ds18b20_test(void)
{
    //char str_temp[32];
    uint16_t temp = 0;

    temp = ds18b20_read_temp();
    UARTprintf("temp=%04x, ", temp);

    temp = temp * 1000 / 16;
    UARTprintf("(%d.%d)C\r\n", temp/1000, temp%1000);

    //usprintf(str_temp, "TEMP=%d.%d", temp/1000, temp%1000);
    //RIT128x96x4StringDraw("               ",       0,  16, 15);
    //RIT128x96x4StringDraw(str_temp,                0,  16, 15);
}

void nrf24l01_test(void)
{
    uint8_t loop;
    uint8_t reg;

    UARTprintf("nrf24l01 register:\r\n");
    for (loop = 0; loop < 18; loop++) {
        nrf24l01_r_reg(loop, &reg, 1);
        UARTprintf("reg 0x%02x=%02x\r\n", loop, reg);
    }
    UARTprintf("\r\n");
}


extern uint8_t rx_buf[];
extern uint8_t nrf24_flag;

int main(void)
{
    //uint8_t buffer[TX_PLOAD_WIDTH];
    uint32_t loop = 0;

    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    IntMasterEnable();

    SysTickPeriodSet(SysCtlClockGet() / SYSTICKHZ);
    SysTickEnable();
    SysTickIntEnable();

    //RIT128x96x4Init(1000000);
    //RIT128x96x4StringDraw("NRF24L01",             36,  0, 15);
    //RIT128x96x4StringDraw("--------------",       0,  8, 15);

    InitConsole();

    UARTprintf("DS18B20 Test\r\n");

    //inner_temp();

    w1_init();

    nrf24l01_init();
    nrf24l01_prx();

    while(1)
    {
        uint8_t i;

        //ds18b20_test();
        //nrf24l01_test();
        //nrf24l01_read_data(buffer);

        if (0)
        {   // test nrf24l01 receive
            while (!(nrf24_flag & NRF24_FLAG_RX)) {}

            UARTprintf("read data:");
            for (i = 0; i < TX_PLOAD_WIDTH; i++) {
                if (i % 16 == 0)
                    UARTprintf("\r\n%04x: ", i);
                UARTprintf("%02x ", rx_buf[i]);
            }
            UARTprintf("\r\n");
            nrf24_flag &= ~NRF24_FLAG_RX;
        }

        if (1)
        {
            rfox_tlv temp_tlv;
            uint16_t temp;

            temp = ds18b20_read_temp();
            rfox_tlv_clr(&temp_tlv);
            rfox_tlv_set(&temp_tlv, RFOX_TYPE_TEMP_DS18B20, 2, (uint8_t*)&temp);

            UARTprintf("send ds18b20 tempurator 0x%x", temp);
            nrf24l01_write_data((uint8_t*)&temp_tlv, sizeof(rfox_tlv));
            SysCtlDelay(5000);

            while (!(nrf24_flag & (NRF24_FLAG_TX|NRF24_FLAG_TX_E))) {}
            if (nrf24_flag & NRF24_FLAG_TX) {
                UARTprintf(" ok\r\n");
            } else {
                UARTprintf(" err\r\n");
            }
            nrf24_flag &= ~(NRF24_FLAG_TX|NRF24_FLAG_TX_E);

        }

        //SysCtlDelay(50000000);
        loop++;
    }
}


