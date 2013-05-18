/*
 [Stellaris] LM3S811ʹ���ĵ�֮nRF24L01����ͨ�� [��������]   �����ѽ�(0)

baixichi

�м�����Ա Rank: 3Rank: 3

����
    256
����
    52
��������
    0Сʱ0��

ר�ҵȼ�:

������:100%

����ֱ�� ��ת��ָ��¥��
1#
baixichi ������ 2011-11-23 22:18:26 |ֻ�������� |������� |���ذ���
0
��������� baixichi �� 2011-11-23 22:20 �༭


�������ſ��ϱ߿�������Ƶ������ҹ���һ��

http://v.youku.com/v_show/id_XMjU5NjM3NzQw.html

�������ϴ�һ����Ƶ�ģ����������̳����֧���ϴ���Ƶ������ͼ򵥵Ľظ�ͼ��


���ϱ���һ������������nrf��������ͷ�ɼ��������ݣ�Ȼ����LCD��������ʾ�������õ�ʲô�������Ͳ��ö�֪������


����˵˵nrf��



�ĵ���ĵ�һ�ξ͸�nRF����һ����顣nRF24L01��һ�����͵�Ƭ��Ƶ�շ�����,������2.4 GHz��2.5 GHz ISMƵ�Ρ���nRF24XXϵ����ȫ���ݡ�

�������ݴ������ʣ�1��2Mbps

SPI�ӿ����ʣ�0��8Mbps

������ѹ��1.9��3.6v������ʹ��3.3V��IO���Գ���5v��ѹ���롣

Ӧ������������꣬���̣���Ϸ���ֱ������С���������˶��豸��


    ֵ��ע����ǣ���Դ��ѹ���ܳ���3.6V����Ȼ�������ջ�оƬ��ֵ��һ����ǣ���оƬ�ڽ���ģʽ�£�֧��1��1��1�Զ�ͨ�ţ����ɽ���6���źš�


�ڵ��Գ����ʱ��ʼ�վ���SSIͦ��ֵģ��Լ��о����������е��Ť����֮ǰ�������Ĳ�����඼����GPIO��ģ�⣬����һЩ����֮�������FSS�����Լ����䡣˵�����ź����о��ü����SPI����Ҳ���Ǻ����ס�Ҫ����TI�к����⣬����������ĺ��鷳���������ִͽ̡�



����������ԭ�򣬾�ֻд��һ���Է����յĳ��򣬱�������߲鿴����ֵ����֤�����д�ĵط���ϣ��ָ�̡��±���Դ���룺


nrf24.h��

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
//***********************��������
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
//****************************����Ĵ�����ַ
#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ�������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define NRFRegSTATUS    0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // �ز����
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��1�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��2�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��3�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��4�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��5�������ݳ���
#define FIFO_STATUS     0x17
#define FEATURE         0x1D  //

//************************************************************
//*********************����
#define RD_RX_PLOAD_W   0x60    // ��ȡ�������ݳ���ָ��
#define RD_RX_PLOAD     0x61   // ��ȡ��������ָ��
#define WR_TX_PLOAD     0xA0   // д��������ָ��
#define FLUSH_TX        0xE1  // ��ϴ���� FIFOָ��
#define FLUSH_RX        0xE2   // ��ϴ���� FIFOָ��
#define REUSE_TX_PL     0xE3   // �����ظ�װ������ָ��
#define NOP             0xFF   // ����

//***************nrf FIFO״̬
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


nrf24.c��


#include "nrf24.h"
extern INT8U addr[5];
extern unsigned long TheSysClock;
//*****************************************************
//******************��ʼ��SPI
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
//******************��SPIд����
void PUT_DATA(INT8U data)
{
    while(!(HWREG(0x4000800C) & SSI_SR_TNF)){}
    HWREG(0x40008008) = data;
}

//**********************************************
//******************��SPI������
INT8U GET_DATA(void)
{
    INT32U feedbk;
    while(!(HWREG(0x4000800C) & SSI_SR_RNE)){}
    feedbk = HWREG(0x40008008);

    return (INT32U) (feedbk);
}

//**********************************************
//*****************д�Ĵ���
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
//*****************���Ĵ���
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
//*****************���õ�ַ
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
//*****************���õ�ַ
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
//******************��������
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
//******************��������
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
//***************��� TX FIFO

INT8U CHK_TF(void)
{
    INT8U stat;
    stat = R_REG(FIFO_STATUS);
    stat &= 0x30;
    return stat;
}

//*****************************************************
//*************** ���RX FIFO

INT8U CHK_RF(void)
{
    INT8U stat;
    stat = R_REG(FIFO_STATUS);
    stat &= 0x03;
    return stat;
}

/*
��������ʱ���׼�������ˣ�������֪����ѧ�ڻ᲻��ҵ����ö�ζ�û��ȥ�Ϲ���
�żټ����ʹ������ѧϰ��
*/
