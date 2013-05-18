//*****************************************************************************
//
// nrf24l01.c - The NRF24L01 Project on LM3S8962
//
// author:  qujianning@kyland.com.cn
// update:  2013/05/02
// version: v1.0
//
//*****************************************************************************

/******************************************************************************
 * TODO:
 *  1, (ok)add the nrf24l01 test function
 *  2, (ok)add the spi test function
 *  3, complex the ds18b20 and nrf24l01, send the tempurator to STM32 board
 *  4, add six channel to send and receive to another board
 *  4, cpmplex the uCOSII operating system
 *
 * History:
 *  2013/05/13  send and interrupt from nrf24l01 ok!
 *  2013/05/12  receive and interrupt from nrf24l01 ok!
 *  2013/05/09  add the inner tempurator reader function
 *  2013/05/09  add the ds18b20 test function
 *  2013/05/09  new file
 ******************************************************************************/
#include <inttypes.h>
#include <inc/hw_types.h>
#include <inc/hw_ints.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/uart.h>
#include <nrf24l01.h>
#include <lib_spi.h>

uint8_t nrf24_pipe;
uint8_t nrf24_baud;
uint8_t nrf24_flag;
uint8_t tx_addr0[TX_ADDR_WIDTH];    // Define a static TX address
uint8_t tx_addr1[TX_ADDR_WIDTH];    // Define a static TX address
uint8_t tx_addr2[TX_ADDR_WIDTH];    // Define a static TX address
uint8_t tx_addr3[TX_ADDR_WIDTH];    // Define a static TX address
uint8_t tx_addr4[TX_ADDR_WIDTH];    // Define a static TX address
uint8_t tx_addr5[TX_ADDR_WIDTH];    // Define a static TX address

uint8_t rx_buf[TX_PLOAD_WIDTH];
uint8_t tx_buf[TX_PLOAD_WIDTH];

void nrf24l01_init(void)
{
    // SPI使能
    SPI_spi0_master_init();

    // NRF24 PIN管脚外设
    SysCtlPeripheralEnable(PIN_PERIPH);

    // CE管脚输出
    GPIOPinTypeGPIOOutput(PIN_BASE, PIN_CE);

    // CSN管脚输出
    GPIOPinTypeGPIOOutput(PIN_BASE, PIN_CSN);
    GPIOPinWrite(PIN_BASE, PIN_CSN, PIN_CSN);

    // IRQ管脚输入中断
    GPIOPinTypeGPIOInput(PIN_BASE, PIN_IRQ);
    GPIOPadConfigSet(PIN_BASE, PIN_IRQ, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(PIN_BASE, PIN_IRQ, GPIO_FALLING_EDGE);
    GPIOPinIntEnable(PIN_BASE, PIN_IRQ);
    IntEnable(INT_GPIOC);
}

void nrf24l01_irq(void)
{
    long status;
    uint8_t val;

    //读取GPIO中断状态
    status = GPIOPinIntStatus(PIN_BASE, true);
    //清除GPIO中断状态,重要
    GPIOPinIntClear(PIN_BASE, status);

    //如果中断状态有效
    if (status & PIN_IRQ)
    {
        //读取状态寄存器
        nrf24l01_r_reg(STATUS, &val, 1);

        //判断是否接收到数据
        if (val & STATUS_RX_DS) {
            //读取数据至缓冲区
            nrf24l01_read_data(rx_buf);
            nrf24_flag |= NRF24_FLAG_RX;

        //发射达到最大复发次数
        } else if (val & STATUS_MAX_RT) {
            //清除发送缓冲区
            nrf24l01_w_reg(FLUSH_TX, 0, 0);
            //进入接收模式
            nrf24l01_prx();
            nrf24_flag |= NRF24_FLAG_TX_E;

        //发射后收到应答
        } else if (val & STATUS_TX_DS) {
            //清除发送缓冲区
            nrf24l01_w_reg(FLUSH_TX, 0, 0);
            //进入接收模式
            nrf24l01_prx();
            nrf24_flag |= NRF24_FLAG_TX;
        }

        //清除中断
        nrf24l01_w_reg(STATUS, &val, 1);

    }
}

void nrf24l01_ce(uint8_t en)
{
    if (en) {
        GPIOPinWrite(PIN_BASE, PIN_CE, PIN_CE);
    } else {
        GPIOPinWrite(PIN_BASE, PIN_CE, 0);
    }
}

void nrf24l01_csn(uint8_t en)
{
    if (en) {
        GPIOPinWrite(PIN_BASE, PIN_CSN, PIN_CSN);
    } else {
        GPIOPinWrite(PIN_BASE, PIN_CSN, 0);
    }
}

void nrf24l01_r_reg(uint8_t addr, uint8_t *buf, uint8_t size)
{
    nrf24l01_csn(0);
    SPI_spi0_xfer(&addr, 1, buf, size);
    //SysCtlDelay(300);
    nrf24l01_csn(1);
}

void nrf24l01_w_reg(uint8_t addr, uint8_t *buf, uint8_t size)
{
    addr |= 0x20;
    nrf24l01_csn(0);
    SPI_spi0_xfer(&addr, 1, 0, 0);
    if (size) {
        SPI_spi0_xfer(buf, size, 0, 0);
    }
    nrf24l01_csn(1);
}

void nrf24l01_write_data(uint8_t *data, uint8_t size)
{
    uint8_t reg;
    uint8_t loop;

    //NRF 模式控制
    nrf24l01_ce(0);

    //设置状态寄存器初始化
    reg = 0xff;
    nrf24l01_w_reg(STATUS, &reg, 1);
    //清除TX FIFO寄存器
    nrf24l01_w_reg(FLUSH_TX, 0, 0);
    //清除RX FIFO寄存器
    nrf24l01_w_reg(FLUSH_RX, 0, 0);

    //设置为发送模式
    nrf24l01_ptx();

    // 延时1毫秒
    SysCtlDelay(50000);

    for (loop = 0; loop < size; loop++) {
        tx_buf[loop] = data[loop];
    }

    //当接收到的USB虚拟串口数据小于32，把有效数据外的空间用0填满
    if (size < 32) {
        for (loop = size; loop < 32; loop++)
            tx_buf[loop] = 0;
    }

    nrf24l01_ce(0);

    //发送32字节的缓存区数据到NRF24L01
    nrf24l01_w_reg(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);

    //保持10us以上，将数据发送出去
    SysCtlDelay(200);
    nrf24l01_ce(1);
}

void nrf24l01_read_data(uint8_t *data)
{
    uint8_t tx_reg = RD_RX_PLOAD;
    nrf24l01_csn(0);
    SPI_spi0_xfer(&tx_reg, 1, data, TX_PLOAD_WIDTH);
    nrf24l01_csn(1);
}

uint8_t nrf24l01_set_addr(uint8_t addr)
{
    return 0;
}

uint8_t nrf24l01_get_addr(uint8_t addr)
{
    return 0;
}

void nrf24l01_ptx(void)
{
    uint8_t reg;

    nrf24l01_ce(1);

    nrf24l01_ce(0);

    nrf24_pipe = 0;

    //自动重发延时500us+86us,自动重发计数10次
    reg = 0x1a;
    nrf24l01_w_reg(SETUP_RETR, &reg, 1);


    //设置发送地址
    if (nrf24_pipe == 0)
        nrf24l01_w_reg(TX_ADDR, tx_addr0, TX_ADDR_WIDTH);     //数据通道0发送地址,最大5个字节
    else if (nrf24_pipe == 1)
        nrf24l01_w_reg(TX_ADDR, tx_addr1, TX_ADDR_WIDTH);     //数据通道1发送地址,最大5个字节
    else if (nrf24_pipe == 2)
        nrf24l01_w_reg(TX_ADDR, tx_addr2, TX_ADDR_WIDTH);     //数据通道2发送地址,最大5个字节
    else if (nrf24_pipe == 3)
        nrf24l01_w_reg(TX_ADDR, tx_addr3, TX_ADDR_WIDTH);     //数据通道3发送地址,最大5个字节
    else if (nrf24_pipe == 4)
        nrf24l01_w_reg(TX_ADDR, tx_addr4, TX_ADDR_WIDTH);     //数据通道4发送地址,最大5个字节
    else if (nrf24_pipe == 5)
        nrf24l01_w_reg(TX_ADDR, tx_addr5, TX_ADDR_WIDTH);     //数据通道5发送地址,最大5个字节

    //设置接收地址
    if (nrf24_pipe == 0)
        nrf24l01_w_reg(RX_ADDR_P0, tx_addr0, TX_ADDR_WIDTH);  // 将0通道的接收地址设置为0通道的发射地址
    else if(nrf24_pipe == 1)
        nrf24l01_w_reg(RX_ADDR_P0, tx_addr1, TX_ADDR_WIDTH);  // 将0通道的接收地址设置为1通道的发射地址
    else if(nrf24_pipe == 2)
        nrf24l01_w_reg(RX_ADDR_P0, tx_addr2, TX_ADDR_WIDTH);  // 将0通道的接收地址设置为2通道的发射地址
    else if(nrf24_pipe == 3)
        nrf24l01_w_reg(RX_ADDR_P0, tx_addr3, TX_ADDR_WIDTH);  // 将0通道的接收地址设置为3通道的发射地址
    else if(nrf24_pipe == 4)
        nrf24l01_w_reg(RX_ADDR_P0, tx_addr4, TX_ADDR_WIDTH);  // 将0通道的接收地址设置为4通道的发射地址
    else if(nrf24_pipe == 5)
        nrf24l01_w_reg(RX_ADDR_P0, tx_addr5, TX_ADDR_WIDTH);  // 将0通道的接收地址设置为5通道的发射地址

    reg = 0x0e;
    nrf24l01_w_reg(CONFIG, &reg, 1);    /* bit6 接收中断产生时,IRQ引脚产生低电平
                                           bit5 发送中断产生时,IRQ引脚产生低电平
                                           bit4 最大重复发送次数完成时 IRQ引脚产生低电平
                                           bit3 CRC校验允许
                                           bit2 16位CRC
                                           bit1 上电
                                           bit0 发送模式 */

    nrf24l01_ce(1);                     /* 使能发送模式 */
}

void nrf24l01_prx(void)
{
    uint8_t reg;

    //默认速率2Mbps
    nrf24_baud = 0;

    tx_addr0[0] = 0x34;       //通道0发射地址
    tx_addr0[1] = 0x43;
    tx_addr0[2] = 0x10;
    tx_addr0[3] = 0x10;
    tx_addr0[4] = 0x01;

    tx_addr1[0] = 0x01;       //通道1发射地址
    tx_addr1[1] = 0xE1;
    tx_addr1[2] = 0xE2;
    tx_addr1[3] = 0xE3;
    tx_addr1[4] = 0x02;

    tx_addr2[0] = 0x02;       //通道2发射地址
    tx_addr2[1] = 0xE1;
    tx_addr2[2] = 0xE2;
    tx_addr2[3] = 0xE3;
    tx_addr2[4] = 0x02;

    tx_addr3[0] = 0x03;       //通道3发射地址
    tx_addr3[1] = 0xE1;
    tx_addr3[2] = 0xE2;
    tx_addr3[3] = 0xE3;
    tx_addr3[4] = 0x02;

    tx_addr4[0] = 0x04;       //通道4发射地址
    tx_addr4[1] = 0xE1;
    tx_addr4[2] = 0xE2;
    tx_addr4[3] = 0xE3;
    tx_addr4[4] = 0x02;

    tx_addr5[0] = 0x05;       //通道5发射地址
    tx_addr5[1] = 0xE1;
    tx_addr5[2] = 0xE2;
    tx_addr5[3] = 0xE3;
    tx_addr5[4] = 0x02;

    nrf24l01_ce(0);

    nrf24l01_w_reg(RX_ADDR_P0, tx_addr0, TX_ADDR_WIDTH);    //数据通道0接收地址,最大5个字节,此处接收地址和发送地址相同
    nrf24l01_w_reg(RX_ADDR_P1, tx_addr1, TX_ADDR_WIDTH);    //数据通道1接收地址,最大5个字节,此处接收地址和发送地址相同
    nrf24l01_w_reg(RX_ADDR_P2, tx_addr2, 1);    //数据通道2接收地址,5个字节,高字节与TX_ADDRESS1[39:8]相同,低字节同TX_ADDRESS2[0]
    nrf24l01_w_reg(RX_ADDR_P3, tx_addr3, 1);    //数据通道3接收地址,5个字节,高字节与TX_ADDRESS1[39:8]相同,低字节同TX_ADDRESS3[0]
    nrf24l01_w_reg(RX_ADDR_P4, tx_addr4, 1);    //数据通道4接收地址,5个字节,高字节与TX_ADDRESS1[39:8]相同,低字节同TX_ADDRESS4[0]
    nrf24l01_w_reg(RX_ADDR_P5, tx_addr5, 1);    //数据通道5接收地址,5个字节,高字节与TX_ADDRESS1[39:8]相同,低字节同TX_ADDRESS5[0]

    reg = TX_PLOAD_WIDTH;
    nrf24l01_w_reg(RX_PW_P0, &reg, 1);          // 接收数据通道0有效数据宽度32范围1-32
    nrf24l01_w_reg(RX_PW_P1, &reg, 1);          // 接收数据通道1有效数据宽度32范围1-32
    nrf24l01_w_reg(RX_PW_P2, &reg, 1);          // 接收数据通道2有效数据宽度32范围1-32
    nrf24l01_w_reg(RX_PW_P3, &reg, 1);          // 接收数据通道3有效数据宽度32范围1-32
    nrf24l01_w_reg(RX_PW_P4, &reg, 1);          // 接收数据通道4有效数据宽度32范围1-32
    nrf24l01_w_reg(RX_PW_P5, &reg, 1);          // 接收数据通道5有效数据宽度32范围1-32

    reg = 0x3f;
    nrf24l01_w_reg(EN_AA, &reg, 1);             // 使能通道0-5接收自动应答
    nrf24l01_w_reg(EN_RXADDR, &reg, 1);         // 接收通道0-5使能
    reg = 0;
    nrf24l01_w_reg(RF_CH, &reg, 1);             // 选择射频工作频道0,范围0-127

    if (nrf24_baud == 0) {
        reg = 0x0f;
        nrf24l01_w_reg(RF_SETUP, &reg, 1);  // 0db,2MPS射频寄存器,无线速率bit5:bit3,发射功率bit2-bit1
        //                    00:1M BPS         00:-18dB
        //                    01:2M BPS         01:-12dB
        //                    10:250K BPS       10:-6dB
        //                    11:保留           11:0dB
    } else {
        reg = 0x07;
        nrf24l01_w_reg(RF_SETUP, &reg, 1);  // 0db, 1MPS
    }

    reg = 0x0f;
    nrf24l01_w_reg(CONFIG, &reg, 1);        // bit6 接收中断产生时,IRQ引脚产生低电平
    // bit5 发送中断产生时,IRQ引脚产生低电平
    // bit4 最大重复发送次数完成时 IRQ引脚产生低电平
    // bit3 CRC校验允许
    // bit2 16位CRC
    // bit1 上电
    // bit0 接收模式

    nrf24l01_ce(1);                         // 使能接收模式
}

