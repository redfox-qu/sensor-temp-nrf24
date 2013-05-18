//*****************************************************************************
//
// ds18b20.c - The NRF24L01 Project on LM3S8962
//
// author:  qujianning@kyland.com.cn
// update:  2013/05/02
// version: v1.0
//
//*****************************************************************************

/******************************************************************************
 * TODO:
 *  1, add multi-chip read function
 *  2, compatible different chip with master frequency
 *
 * History:
 *  2013/05/09  new file
 ******************************************************************************/

#include <inttypes.h>
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <thirdparty/uartstdio.h>


//** 硬件连接定义
/******************************************************************************
    DS18B20 8962
------------------------
1   GND     GND     10
2   VCC     VCC     12
3   CE      PC5     13
******************************************************************************/

#define DS18B20_PIN_DATA        GPIO_PIN_0
#define DS18B20_PIN_BASE        GPIO_PORTB_BASE
#define DS18B20_PIN_PERIPH      SYSCTL_PERIPH_GPIOB

#define DQ_OUT                  HWREG(GPIO_PORTB_BASE + GPIO_O_DIR) |= 1    // (GPIO_PIN_0)
#define DQ_IN                   HWREG(GPIO_PORTB_BASE + GPIO_O_DIR) &= 0    // (~GPIO_PIN_0)
#define DQ_WR_H                 HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + 0x04 /*(GPIO_PIN_0 << 2)*/)) = GPIO_PIN_0
#define DQ_WR_L                 HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + 0x04 /*(GPIO_PIN_0 << 2)*/)) = 0
#define DQ_RD                   HWREG(GPIO_PORTB_BASE + (GPIO_O_DATA + 0x04 /*(GPIO_PIN_0 << 2)*/))
#define DQ_DELAY(n)             SysCtlDelay(n)  /* 1 = (20ns * 3) (50Mhz) */

#ifdef DEBUG
#define DS18_DBG(s...)          UARTprintf(##s)
#else
#define DS18_DBG(s...)
#endif

void w1_init(void)
{
    SysCtlPeripheralEnable(DS18B20_PIN_PERIPH);
    GPIOPinTypeGPIOInput(DS18B20_PIN_BASE, DS18B20_PIN_DATA);
}

uint8_t w1_reset(void)
{
    uint8_t rst_flag = 0;

    DQ_OUT;             //GPIO设为输出

    DQ_WR_H;
    DQ_DELAY(8000);

    DQ_WR_L;
    DQ_DELAY(8000);     //初始化低电平必须大于480us
    DQ_WR_H;
    DQ_DELAY(1000);

    DQ_IN;              //GPIO设为输入
    rst_flag = DQ_RD;
    DQ_DELAY(10000);

    return (rst_flag);

}

void w1_writebyte(uint8_t ucdata)
{
    uint8_t i = 0;

    DQ_OUT;             //GPIO设为输出

    for (i = 8; i > 0; i--) {

        DQ_WR_L;

        if (ucdata & 0x01) {

            DQ_WR_L;
            DQ_DELAY(80);
            DQ_WR_H;
            DQ_DELAY(480);

        } else {

            DQ_WR_L;
            DQ_DELAY(1200);
        }

        DQ_WR_H;
        DQ_DELAY(80);

        ucdata >>= 1;
    }
    DQ_DELAY(32);
}

uint8_t w1_readbyte(void)
{
    uint8_t i = 0;
    uint8_t ucdata = 0;

    for ( i = 8; i > 0; i--) {
        DQ_OUT;         //GPIO设为输出

        DQ_WR_L;
        ucdata >>= 1;

        DQ_IN;          //GPIO设为输入，关键

        DQ_DELAY(208);

        if (0x00 != DQ_RD) {
            ucdata |= 0x80;
        }

        DQ_DELAY(640);
    }

    DQ_DELAY(32);
    return (ucdata);
}


/******************************************************************/
/*                   读取温度                                     */
/******************************************************************/
uint16_t ds18b20_read_temp(void)
{
    uint8_t regbuf[9];
    uint16_t temp, i;

    // 初始化芯片
    w1_reset();

    // 跳过读序号列号的操作
    w1_writebyte(0xcc);

    // 启动温度转换
    w1_writebyte(0x44);

    SysCtlDelay(SysCtlClockGet());

    // 初始化芯片
    w1_reset();

    //跳过读序号列号的操作
    w1_writebyte(0xcc);

    //读取温度寄存器等（共可读9个寄存器） 前两个就是温度
    w1_writebyte(0xbe);

    DS18_DBG("ds18b20 reg: ");
    for (i = 0; i < 9; i++) {
        regbuf[i] = w1_readbyte();
        DS18_DBG("0x%02x ", regbuf[i]);
    }
    DS18_DBG("\r\n");

    if (regbuf[1] & 0x80) {
		temp = ((regbuf[1] << 8) | regbuf[0]);      // 高地位整合
		temp = ((~temp) + 1);                       // 负数取反加1
    } else {
		temp = ((regbuf[1] << 8) | regbuf[0]);      // 高地位整合
    }

    return temp;
}

