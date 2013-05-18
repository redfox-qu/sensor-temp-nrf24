/*
 [Stellaris] LM3S811使用心得之nRF24L01无线通信 [复制链接]   此帖已结(0)

baixichi

中级技术员 Rank: 3Rank: 3

积分
    256
帖子
    52
本月在线
    0小时0分

专家等级:

结帖率:100%

电梯直达 跳转到指定楼层
1#
baixichi 发表于 2011-11-23 22:18:26 |只看该作者 |倒序浏览 |返回版面
0
本帖最后由 baixichi 于 2011-11-23 22:20 编辑


这是在优酷上边看到的视频，给大家共享一下

http://v.youku.com/v_show/id_XMjU5NjM3NzQw.html

本来想上传一个视频的，结果发现论坛并不支持上传视频，这里就简单的截个图：


这上边是一个高手用两个nrf传输摄像头采集到的数据，然后在LCD电视上显示，具体用的什么处理器就不得而知。。。


现在说说nrf：



文档里的第一段就给nRF做了一个简介。nRF24L01是一款新型单片射频收发器件,工作于2.4 GHz～2.5 GHz ISM频段。与nRF24XX系列完全兼容。

无线数据传输速率：1或2Mbps

SPI接口速率：0到8Mbps

工作电压：1.9到3.6v，建议使用3.3V。IO可以承受5v电压输入。

应用领域：无线鼠标，键盘，游戏机手柄，玩具小车，智能运动设备…


    值得注意的是，电源电压不能超过3.6V，不然很容易烧坏芯片。值得一提的是，该芯片在接收模式下，支持1对1和1对多通信，最多可接收6个信号。


在调试程序的时候，始终觉得SSI挺奇怪的，自己感觉操作起来有点别扭。因之前对器件的操作差不多都是用GPIO口模拟，看过一些资料之后才明白FSS可以自己跳变。说来很遗憾，研究好几天对SPI操作也不是很明白。要不是TI有函数库，配置起来真的很麻烦。还望高手赐教。



由于器件的原因，就只写了一个自发自收的程序，编译器里边查看变量值而验证。如有错的地方，希望指教。下边是源代码：


nrf24.h：

*/

#ifndef __NRF24_H_
#define __NRF24_H_
#include "hw_memmap.h"
#include "hw_types.h"
#include "sysctl.h"
#include "gpio.h"
#include "ssi.h"
#include "hw_ssi.h"
#define INT8U unsigned char
#define INT32U unsigned long
//***********************************************************
//***********************定义引脚
#define     SPI_BASE         SSI0_BASE
#define     SPICLK           GPIO_PA2_SSI0CLK
#define     SPIFSS      GPIO_PA3_SSI0FSS
#define     SPIRX      GPIO_PA4_SSI0RX
#define     SPITX      GPIO_PA5_SSI0TX
#define     GPIO_BASE      GPIO_PORTA_BASE
#define     PINS        0X3C

#define     PIN_CE      0X01
#define     PIN_IRQ      0X02
#define     CE_0             GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_0,0X00)
#define     CE_1             GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_0,0XFF)

//***************************************************************************
//****************************定义寄存器地址
#define CONFIG          0x00  // 配置收发状态，CRC校验模式以及收发状态响应方式
#define EN_AA           0x01  // 自动应答功能设置
#define EN_RXADDR       0x02  // 可用信道设置
#define SETUP_AW        0x03  // 收发地址宽度设置
#define SETUP_RETR      0x04  // 自动重发功能设置
#define RF_CH           0x05  // 工作频率设置
#define RF_SETUP        0x06  // 发射速率、功耗功能设置
#define NRFRegSTATUS    0x07  // 状态寄存器
#define OBSERVE_TX      0x08  // 发送监测功能
#define CD              0x09  // 载波检测
#define RX_ADDR_P0      0x0A  // 频道0接收数据地址
#define RX_ADDR_P1      0x0B  // 频道1接收数据地址
#define RX_ADDR_P2      0x0C  // 频道2接收数据地址
#define RX_ADDR_P3      0x0D  // 频道3接收数据地址
#define RX_ADDR_P4      0x0E  // 频道4接收数据地址
#define RX_ADDR_P5      0x0F  // 频道5接收数据地址
#define TX_ADDR         0x10  // 发送地址寄存器
#define RX_PW_P0        0x11  // 接收频道0接收数据长度
#define RX_PW_P1        0x12  // 接收频道1接收数据长度
#define RX_PW_P2        0x13  // 接收频道2接收数据长度
#define RX_PW_P3        0x14  // 接收频道3接收数据长度
#define RX_PW_P4        0x15  // 接收频道4接收数据长度
#define RX_PW_P5        0x16  // 接收频道5接收数据长度
#define FIFO_STATUS     0x17
#define FEATURE         0x1D  //

//************************************************************
//*********************命令
#define RD_RX_PLOAD_W   0x60    // 读取接收数据长度指令
#define RD_RX_PLOAD     0x61   // 读取接收数据指令
#define WR_TX_PLOAD     0xA0   // 写待发数据指令
#define FLUSH_TX        0xE1  // 冲洗发送 FIFO指令
#define FLUSH_RX        0xE2   // 冲洗接收 FIFO指令
#define REUSE_TX_PL     0xE3   // 定义重复装载数据指令
#define NOP             0xFF   // 保留

//***************nrf FIFO状态
#define     TX_FULL          0x20
#define     TX_EMPTY         0x10
#define     TX_NOT_EMPTY     0x00
#define     RX_FULL          0x02
#define     RX_EMPTY         0x01
#define     RX_NOT_EMPTY     0x00

extern void spi_Init(void);
extern void SET_PTX(void);
extern void SET_PRX(void);
extern void PUT_DATA(INT8U data);
extern INT8U  GET_DATA(void);
extern INT8U   W_REG(INT32U addr,INT32U cmd);
extern INT8U   R_REG(INT8U addr);
extern INT8U   SET_ADDR(INT8U WHICH);
extern INT8U  READ_ADDR(INT8U WHICH);
extern INT8U  RECEIVE_DATA(INT8U *data);
extern INT8U   SEND_DATA(INT8U *data);
extern INT8U   CHK_TF(void);
extern INT8U  CHK_RF(void);
#endif


nrf24.c：


#include "nrf24.h"
extern INT8U addr[5];
extern unsigned long TheSysClock;
//*****************************************************
//******************初始化SPI
void spi_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, PIN_CE);
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE,PIN_IRQ);
    GPIOPinConfigure(SPICLK);
    GPIOPinConfigure(SPIFSS);
    GPIOPinConfigure(SPIRX);
    GPIOPinConfigure(SPITX);
    GPIOPinTypeSSI(GPIO_BASE, PINS);
    SSIConfigSetExpClk(SPI_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_1, SSI_MODE_MASTER, 1000000, 8);
    SSIEnable(SPI_BASE);

}

//**********************************************
//******************从SPI写数据
void PUT_DATA(INT8U data)
{
    while(!(HWREG(0x4000800C) & SSI_SR_TNF)){}
    HWREG(0x40008008) = data;
}

//**********************************************
//******************从SPI读数据
INT8U GET_DATA(void)
{
    INT32U feedbk;
    while(!(HWREG(0x4000800C) & SSI_SR_RNE)){}
    feedbk = HWREG(0x40008008);

    return (INT32U) (feedbk);
}

//**********************************************
//*****************写寄存器
INT8U W_REG(INT32U addr,INT32U cmd)
{
    INT8U stat;
    addr |= 0x20;
    PUT_DATA(addr);
    PUT_DATA(cmd);
    stat = GET_DATA();
    GET_DATA();

    return stat;
}

//**********************************************
//*****************读寄存器
INT8U R_REG(INT8U addr)
{
    INT8U  stat,val;
    PUT_DATA(addr);
    PUT_DATA(0x00);
    stat = GET_DATA();
    val  = GET_DATA();
    return val;
}

//**********************************************
//*****************设置地址
INT8U SET_ADDR(INT8U WHICH)
{
    INT8U stat = 0;
    WHICH|=0X20;
    PUT_DATA(WHICH);
    PUT_DATA(0XE7);
    PUT_DATA(0XE7);
    PUT_DATA(0XE7);
    PUT_DATA(0XE7);
    PUT_DATA(0XE7);

    stat = GET_DATA();
    GET_DATA();
    GET_DATA();
    GET_DATA();
    GET_DATA();
    GET_DATA();
    return stat;
}

//**********************************************
//*****************设置地址
INT8U READ_ADDR(INT8U WHICH)
{
    INT8U stat = 0;
    PUT_DATA(WHICH);
    PUT_DATA(0X00);
    PUT_DATA(0X00);
    PUT_DATA(0X00);
    PUT_DATA(0X00);
    PUT_DATA(0X00);

    stat = GET_DATA();
    addr[4] = GET_DATA();
    addr[3] = GET_DATA();
    addr[2] = GET_DATA();
    addr[1] = GET_DATA();
    addr[0] = GET_DATA();

    return stat;
}


//**********************************************
//******************接收数据
INT8U RECEIVE_DATA(INT8U *data)
{
    INT8U stat, *p;
    p = data;

    while(CHK_RF() == TX_EMPTY){}

    PUT_DATA(RD_RX_PLOAD);
    PUT_DATA(0X00);
    PUT_DATA(0X00);
    PUT_DATA(0X00);
    PUT_DATA(0X00);
    PUT_DATA(0X00);
    PUT_DATA(0X00);
    PUT_DATA(0X00);

     stat  = GET_DATA();
    *( p ) = GET_DATA();
    *(p+1) = GET_DATA();
    *(p+2) = GET_DATA();
    *(p+3) = GET_DATA();
    *(p+4) = GET_DATA();
    *(p+5) = GET_DATA();
    *(p+6) = GET_DATA();

    return stat;

}

//**********************************************
//******************发送数据
INT8U SEND_DATA(INT8U *data)
{
    INT8U stat, *p;
    p = data;

    while(CHK_TF() != TX_EMPTY){}

    PUT_DATA(WR_TX_PLOAD);
    PUT_DATA(*p);
    PUT_DATA(*(p+1));
    PUT_DATA(*(p+2));
    PUT_DATA(*(p+3));
    PUT_DATA(*(p+4));
    PUT_DATA(*(p+5));
    PUT_DATA(*(p+6));

    stat = GET_DATA();
    GET_DATA();
    GET_DATA();
    GET_DATA();
    GET_DATA();
    GET_DATA();
    GET_DATA();
    GET_DATA();

    CE_1;
    SysCtlDelay(TheSysClock/7000);
    CE_0;

    return stat;
}

/*****************************************************
-NRF24L01+ INITIALIZATION AS A PTX
***************/
void SET_PTX(void)
{
    W_REG(CONFIG,0X72);       //NO_CRC -- PTX -- MASK_IRQ -- PWR_UP
    W_REG(EN_AA,0X00);        //ALL_PIPE_NO_AUTO_ACK
    W_REG(EN_RXADDR,0X01);    //ONLY_ENABLE_PIPE0
    W_REG(SETUP_AW,0X11);     //ADDR_WIDTH_5_BYTES
    W_REG(SETUP_RETR,0X00);   //NO_RETRANSMIT
    W_REG(RF_CH,0X00);        //2.4GHZ_CHANNEL
    W_REG(RF_SETUP,0X07);     //1Mbps -- 0dBm
    SET_ADDR(RX_ADDR_P0);     //RX_ADDR_0XE7E7E7E7E7
    SET_ADDR(TX_ADDR);        //TX_ADDR_0XE7E7E7E7E7
    //W_REG(RX_PW_P0,0X01);     //PAYLOAD_LENGTH_1BYTE
    W_REG(FEATURE,0X01);      //WRITE_WITHOUT_ACK
}

/*****************************************************
-NRF24L01+ INITIALIZATION AS A PRX
***************/
void SET_PRX(void)
{
    W_REG(CONFIG,0X73);       //NO_CRC -- PRX -- MASK_IRQ -- PWR_UP
    W_REG(EN_AA,0X00);        //ALL_PIPE_NO_AUTO_ACK
    W_REG(EN_RXADDR,0X01);    //ONLY_ENABLE_PIPE0
    W_REG(SETUP_AW,0X11);     //ADDR_WIDTH_5_BYTES
    W_REG(SETUP_RETR,0X00);   //NO_RETRANSMIT
    W_REG(RF_CH,0X00);        //2.4GHZ_CHANNEL
    W_REG(RF_SETUP,0X07);     //1Mbps -- 0dBm
    SET_ADDR(RX_ADDR_P0);     //RX_ADDR_0XE7E7E7E7E7
    //SET_ADDR(TX_ADDR);        //TX_ADDR_0XE7E7E7E7E7
    W_REG(RX_PW_P0,0X01);     //PAYLOAD_LENGTH_1BYTE
    W_REG(FEATURE,0X01);      //WRITE_WITHOUT_ACK
}

//*****************************************************
//***************检查 TX FIFO

INT8U CHK_TF(void)
{
    INT8U stat;
    stat = R_REG(FIFO_STATUS);
    stat &= 0x30;
    return stat;
}

//*****************************************************
//*************** 检查RX FIFO

INT8U CHK_RF(void)
{
    INT8U stat;
    stat = R_REG(FIFO_STATUS);
    stat &= 0x03;
    return stat;
}

/*
接下来的时间该准备考试了，，还不知道这学期会不会挂掉，好多课都没有去上过。
放假继续和大家讨论学习。
*/
