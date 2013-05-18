//*****************************************************************************
//
// nrf24l01.c - The NRF24L01 Project on LM3S8962
//
// author:  qujianning@kyland.com.cn
// update:  2013/05/02
// version: v1.0
//
//*****************************************************************************

#ifndef __NRF24L01_H__
#define __NRF24L01_H__

//** Ӳ�����Ӷ���
/******************************************************************************
    NRF24   8962
------------------------
1   GND     GND     10
2   VCC     VCC     12
3   CE      PC5     13
4   CSN     PC4     11
5   SCK     SSI0CLK 7
6   MOSI    SSI0TX  4
7   MISO    SSI0RX  5
8   IRQ     PC7     15
******************************************************************************/
#define PIN_CSN             GPIO_PIN_4
#define PIN_CE              GPIO_PIN_5
#define PIN_IRQ             GPIO_PIN_7
#define PIN_BASE            GPIO_PORTC_BASE
#define PIN_PERIPH          SYSCTL_PERIPH_GPIOC

//**����Ĵ�����ַ
#define CONFIG              0x00    // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA               0x01    // �Զ�Ӧ��������
#define EN_RXADDR           0x02    // �����ŵ�����
#define SETUP_AW            0x03    // �շ���ַ�������
#define SETUP_RETR          0x04    // �Զ��ط���������
#define RF_CH               0x05    // ����Ƶ������
#define RF_SETUP            0x06    // �������ʡ����Ĺ�������
#define STATUS              0x07    // ״̬�Ĵ���
#define OBSERVE_TX          0x08    // ���ͼ�⹦��
#define CD                  0x09    // �ز����
#define RX_ADDR_P0          0x0A    // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1          0x0B    // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2          0x0C    // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3          0x0D    // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4          0x0E    // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5          0x0F    // Ƶ��5�������ݵ�ַ
#define TX_ADDR             0x10    // ���͵�ַ�Ĵ���
#define RX_PW_P0            0x11    // ����Ƶ��0�������ݳ���
#define RX_PW_P1            0x12    // ����Ƶ��1�������ݳ���
#define RX_PW_P2            0x13    // ����Ƶ��2�������ݳ���
#define RX_PW_P3            0x14    // ����Ƶ��3�������ݳ���
#define RX_PW_P4            0x15    // ����Ƶ��4�������ݳ���
#define RX_PW_P5            0x16    // ����Ƶ��5�������ݳ���
#define FIFO_STATUS         0x17    //
#define FEATURE             0x1D    //

//**����
#define RD_RX_PLOAD_W       0x60    // ��ȡ�������ݳ���ָ��
#define RD_RX_PLOAD         0x61    // ��ȡ��������ָ��
#define WR_TX_PLOAD         0xA0    // д��������ָ��
#define FLUSH_TX            0xE1    // ��ϴ���� FIFOָ��
#define FLUSH_RX            0xE2    // ��ϴ���� FIFOָ��
#define REUSE_TX_PL         0xE3    // �����ظ�װ������ָ��
#define NOP                 0xFF    // ����

//**NRF_FIFO״̬
#define TX_FULL             0x20
#define TX_EMPTY            0x10
#define TX_NOT_EMPTY        0x00
#define RX_FULL             0x02
#define RX_EMPTY            0x01
#define RX_NOT_EMPTY        0x00

//**NRF״̬
#define STATUS_TX_FULL      0x01
#define STATUS_RX_P_NO      0x0e
#define STATUS_MAX_RT       0x10
#define STATUS_TX_DS        0x20
#define STATUS_RX_DS        0x40

//
#define TX_ADDR_WIDTH       5
#define TX_PLOAD_WIDTH      32

//
#define NRF24_FLAG_RX       0x01    // �Ѿ����յ�����
#define NRF24_FLAG_TX       0x02    // ���ͳɹ�
#define NRF24_FLAG_TX_E     0x04    // ����ʧ��

// nrf24l01 functions
void nrf24l01_init(void);
void nrf24l01_prx(void);
void nrf24l01_ptx(void);
void nrf24l01_r_reg(uint8_t addr, uint8_t *buf, uint8_t size);
void nrf24l01_w_reg(uint8_t addr, uint8_t *buf, uint8_t size);
uint8_t nrf24l01_get_addr(uint8_t addr);
uint8_t nrf24l01_set_addr(uint8_t addr);
void nrf24l01_read_data(uint8_t *data);
void nrf24l01_write_data(uint8_t *data, uint8_t size);

#endif  //__NRF24L01_H__


