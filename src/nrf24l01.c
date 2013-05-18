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
    // SPIʹ��
    SPI_spi0_master_init();

    // NRF24 PIN�ܽ�����
    SysCtlPeripheralEnable(PIN_PERIPH);

    // CE�ܽ����
    GPIOPinTypeGPIOOutput(PIN_BASE, PIN_CE);

    // CSN�ܽ����
    GPIOPinTypeGPIOOutput(PIN_BASE, PIN_CSN);
    GPIOPinWrite(PIN_BASE, PIN_CSN, PIN_CSN);

    // IRQ�ܽ������ж�
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

    //��ȡGPIO�ж�״̬
    status = GPIOPinIntStatus(PIN_BASE, true);
    //���GPIO�ж�״̬,��Ҫ
    GPIOPinIntClear(PIN_BASE, status);

    //����ж�״̬��Ч
    if (status & PIN_IRQ)
    {
        //��ȡ״̬�Ĵ���
        nrf24l01_r_reg(STATUS, &val, 1);

        //�ж��Ƿ���յ�����
        if (val & STATUS_RX_DS) {
            //��ȡ������������
            nrf24l01_read_data(rx_buf);
            nrf24_flag |= NRF24_FLAG_RX;

        //����ﵽ��󸴷�����
        } else if (val & STATUS_MAX_RT) {
            //������ͻ�����
            nrf24l01_w_reg(FLUSH_TX, 0, 0);
            //�������ģʽ
            nrf24l01_prx();
            nrf24_flag |= NRF24_FLAG_TX_E;

        //������յ�Ӧ��
        } else if (val & STATUS_TX_DS) {
            //������ͻ�����
            nrf24l01_w_reg(FLUSH_TX, 0, 0);
            //�������ģʽ
            nrf24l01_prx();
            nrf24_flag |= NRF24_FLAG_TX;
        }

        //����ж�
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

    //NRF ģʽ����
    nrf24l01_ce(0);

    //����״̬�Ĵ�����ʼ��
    reg = 0xff;
    nrf24l01_w_reg(STATUS, &reg, 1);
    //���TX FIFO�Ĵ���
    nrf24l01_w_reg(FLUSH_TX, 0, 0);
    //���RX FIFO�Ĵ���
    nrf24l01_w_reg(FLUSH_RX, 0, 0);

    //����Ϊ����ģʽ
    nrf24l01_ptx();

    // ��ʱ1����
    SysCtlDelay(50000);

    for (loop = 0; loop < size; loop++) {
        tx_buf[loop] = data[loop];
    }

    //�����յ���USB���⴮������С��32������Ч������Ŀռ���0����
    if (size < 32) {
        for (loop = size; loop < 32; loop++)
            tx_buf[loop] = 0;
    }

    nrf24l01_ce(0);

    //����32�ֽڵĻ��������ݵ�NRF24L01
    nrf24l01_w_reg(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);

    //����10us���ϣ������ݷ��ͳ�ȥ
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

    //�Զ��ط���ʱ500us+86us,�Զ��ط�����10��
    reg = 0x1a;
    nrf24l01_w_reg(SETUP_RETR, &reg, 1);


    //���÷��͵�ַ
    if (nrf24_pipe == 0)
        nrf24l01_w_reg(TX_ADDR, tx_addr0, TX_ADDR_WIDTH);     //����ͨ��0���͵�ַ,���5���ֽ�
    else if (nrf24_pipe == 1)
        nrf24l01_w_reg(TX_ADDR, tx_addr1, TX_ADDR_WIDTH);     //����ͨ��1���͵�ַ,���5���ֽ�
    else if (nrf24_pipe == 2)
        nrf24l01_w_reg(TX_ADDR, tx_addr2, TX_ADDR_WIDTH);     //����ͨ��2���͵�ַ,���5���ֽ�
    else if (nrf24_pipe == 3)
        nrf24l01_w_reg(TX_ADDR, tx_addr3, TX_ADDR_WIDTH);     //����ͨ��3���͵�ַ,���5���ֽ�
    else if (nrf24_pipe == 4)
        nrf24l01_w_reg(TX_ADDR, tx_addr4, TX_ADDR_WIDTH);     //����ͨ��4���͵�ַ,���5���ֽ�
    else if (nrf24_pipe == 5)
        nrf24l01_w_reg(TX_ADDR, tx_addr5, TX_ADDR_WIDTH);     //����ͨ��5���͵�ַ,���5���ֽ�

    //���ý��յ�ַ
    if (nrf24_pipe == 0)
        nrf24l01_w_reg(RX_ADDR_P0, tx_addr0, TX_ADDR_WIDTH);  // ��0ͨ���Ľ��յ�ַ����Ϊ0ͨ���ķ����ַ
    else if(nrf24_pipe == 1)
        nrf24l01_w_reg(RX_ADDR_P0, tx_addr1, TX_ADDR_WIDTH);  // ��0ͨ���Ľ��յ�ַ����Ϊ1ͨ���ķ����ַ
    else if(nrf24_pipe == 2)
        nrf24l01_w_reg(RX_ADDR_P0, tx_addr2, TX_ADDR_WIDTH);  // ��0ͨ���Ľ��յ�ַ����Ϊ2ͨ���ķ����ַ
    else if(nrf24_pipe == 3)
        nrf24l01_w_reg(RX_ADDR_P0, tx_addr3, TX_ADDR_WIDTH);  // ��0ͨ���Ľ��յ�ַ����Ϊ3ͨ���ķ����ַ
    else if(nrf24_pipe == 4)
        nrf24l01_w_reg(RX_ADDR_P0, tx_addr4, TX_ADDR_WIDTH);  // ��0ͨ���Ľ��յ�ַ����Ϊ4ͨ���ķ����ַ
    else if(nrf24_pipe == 5)
        nrf24l01_w_reg(RX_ADDR_P0, tx_addr5, TX_ADDR_WIDTH);  // ��0ͨ���Ľ��յ�ַ����Ϊ5ͨ���ķ����ַ

    reg = 0x0e;
    nrf24l01_w_reg(CONFIG, &reg, 1);    /* bit6 �����жϲ���ʱ,IRQ���Ų����͵�ƽ
                                           bit5 �����жϲ���ʱ,IRQ���Ų����͵�ƽ
                                           bit4 ����ظ����ʹ������ʱ IRQ���Ų����͵�ƽ
                                           bit3 CRCУ������
                                           bit2 16λCRC
                                           bit1 �ϵ�
                                           bit0 ����ģʽ */

    nrf24l01_ce(1);                     /* ʹ�ܷ���ģʽ */
}

void nrf24l01_prx(void)
{
    uint8_t reg;

    //Ĭ������2Mbps
    nrf24_baud = 0;

    tx_addr0[0] = 0x34;       //ͨ��0�����ַ
    tx_addr0[1] = 0x43;
    tx_addr0[2] = 0x10;
    tx_addr0[3] = 0x10;
    tx_addr0[4] = 0x01;

    tx_addr1[0] = 0x01;       //ͨ��1�����ַ
    tx_addr1[1] = 0xE1;
    tx_addr1[2] = 0xE2;
    tx_addr1[3] = 0xE3;
    tx_addr1[4] = 0x02;

    tx_addr2[0] = 0x02;       //ͨ��2�����ַ
    tx_addr2[1] = 0xE1;
    tx_addr2[2] = 0xE2;
    tx_addr2[3] = 0xE3;
    tx_addr2[4] = 0x02;

    tx_addr3[0] = 0x03;       //ͨ��3�����ַ
    tx_addr3[1] = 0xE1;
    tx_addr3[2] = 0xE2;
    tx_addr3[3] = 0xE3;
    tx_addr3[4] = 0x02;

    tx_addr4[0] = 0x04;       //ͨ��4�����ַ
    tx_addr4[1] = 0xE1;
    tx_addr4[2] = 0xE2;
    tx_addr4[3] = 0xE3;
    tx_addr4[4] = 0x02;

    tx_addr5[0] = 0x05;       //ͨ��5�����ַ
    tx_addr5[1] = 0xE1;
    tx_addr5[2] = 0xE2;
    tx_addr5[3] = 0xE3;
    tx_addr5[4] = 0x02;

    nrf24l01_ce(0);

    nrf24l01_w_reg(RX_ADDR_P0, tx_addr0, TX_ADDR_WIDTH);    //����ͨ��0���յ�ַ,���5���ֽ�,�˴����յ�ַ�ͷ��͵�ַ��ͬ
    nrf24l01_w_reg(RX_ADDR_P1, tx_addr1, TX_ADDR_WIDTH);    //����ͨ��1���յ�ַ,���5���ֽ�,�˴����յ�ַ�ͷ��͵�ַ��ͬ
    nrf24l01_w_reg(RX_ADDR_P2, tx_addr2, 1);    //����ͨ��2���յ�ַ,5���ֽ�,���ֽ���TX_ADDRESS1[39:8]��ͬ,���ֽ�ͬTX_ADDRESS2[0]
    nrf24l01_w_reg(RX_ADDR_P3, tx_addr3, 1);    //����ͨ��3���յ�ַ,5���ֽ�,���ֽ���TX_ADDRESS1[39:8]��ͬ,���ֽ�ͬTX_ADDRESS3[0]
    nrf24l01_w_reg(RX_ADDR_P4, tx_addr4, 1);    //����ͨ��4���յ�ַ,5���ֽ�,���ֽ���TX_ADDRESS1[39:8]��ͬ,���ֽ�ͬTX_ADDRESS4[0]
    nrf24l01_w_reg(RX_ADDR_P5, tx_addr5, 1);    //����ͨ��5���յ�ַ,5���ֽ�,���ֽ���TX_ADDRESS1[39:8]��ͬ,���ֽ�ͬTX_ADDRESS5[0]

    reg = TX_PLOAD_WIDTH;
    nrf24l01_w_reg(RX_PW_P0, &reg, 1);          // ��������ͨ��0��Ч���ݿ��32��Χ1-32
    nrf24l01_w_reg(RX_PW_P1, &reg, 1);          // ��������ͨ��1��Ч���ݿ��32��Χ1-32
    nrf24l01_w_reg(RX_PW_P2, &reg, 1);          // ��������ͨ��2��Ч���ݿ��32��Χ1-32
    nrf24l01_w_reg(RX_PW_P3, &reg, 1);          // ��������ͨ��3��Ч���ݿ��32��Χ1-32
    nrf24l01_w_reg(RX_PW_P4, &reg, 1);          // ��������ͨ��4��Ч���ݿ��32��Χ1-32
    nrf24l01_w_reg(RX_PW_P5, &reg, 1);          // ��������ͨ��5��Ч���ݿ��32��Χ1-32

    reg = 0x3f;
    nrf24l01_w_reg(EN_AA, &reg, 1);             // ʹ��ͨ��0-5�����Զ�Ӧ��
    nrf24l01_w_reg(EN_RXADDR, &reg, 1);         // ����ͨ��0-5ʹ��
    reg = 0;
    nrf24l01_w_reg(RF_CH, &reg, 1);             // ѡ����Ƶ����Ƶ��0,��Χ0-127

    if (nrf24_baud == 0) {
        reg = 0x0f;
        nrf24l01_w_reg(RF_SETUP, &reg, 1);  // 0db,2MPS��Ƶ�Ĵ���,��������bit5:bit3,���书��bit2-bit1
        //                    00:1M BPS         00:-18dB
        //                    01:2M BPS         01:-12dB
        //                    10:250K BPS       10:-6dB
        //                    11:����           11:0dB
    } else {
        reg = 0x07;
        nrf24l01_w_reg(RF_SETUP, &reg, 1);  // 0db, 1MPS
    }

    reg = 0x0f;
    nrf24l01_w_reg(CONFIG, &reg, 1);        // bit6 �����жϲ���ʱ,IRQ���Ų����͵�ƽ
    // bit5 �����жϲ���ʱ,IRQ���Ų����͵�ƽ
    // bit4 ����ظ����ʹ������ʱ IRQ���Ų����͵�ƽ
    // bit3 CRCУ������
    // bit2 16λCRC
    // bit1 �ϵ�
    // bit0 ����ģʽ

    nrf24l01_ce(1);                         // ʹ�ܽ���ģʽ
}

