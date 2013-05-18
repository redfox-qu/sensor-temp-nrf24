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

//** 硬件连接定义
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

//**定义寄存器地址
#define CONFIG              0x00    // 配置收发状态，CRC校验模式以及收发状态响应方式
#define EN_AA               0x01    // 自动应答功能设置
#define EN_RXADDR           0x02    // 可用信道设置
#define SETUP_AW            0x03    // 收发地址宽度设置
#define SETUP_RETR          0x04    // 自动重发功能设置
#define RF_CH               0x05    // 工作频率设置
#define RF_SETUP            0x06    // 发射速率、功耗功能设置
#define STATUS              0x07    // 状态寄存器
#define OBSERVE_TX          0x08    // 发送监测功能
#define CD                  0x09    // 载波检测
#define RX_ADDR_P0          0x0A    // 频道0接收数据地址
#define RX_ADDR_P1          0x0B    // 频道1接收数据地址
#define RX_ADDR_P2          0x0C    // 频道2接收数据地址
#define RX_ADDR_P3          0x0D    // 频道3接收数据地址
#define RX_ADDR_P4          0x0E    // 频道4接收数据地址
#define RX_ADDR_P5          0x0F    // 频道5接收数据地址
#define TX_ADDR             0x10    // 发送地址寄存器
#define RX_PW_P0            0x11    // 接收频道0接收数据长度
#define RX_PW_P1            0x12    // 接收频道1接收数据长度
#define RX_PW_P2            0x13    // 接收频道2接收数据长度
#define RX_PW_P3            0x14    // 接收频道3接收数据长度
#define RX_PW_P4            0x15    // 接收频道4接收数据长度
#define RX_PW_P5            0x16    // 接收频道5接收数据长度
#define FIFO_STATUS         0x17    //
#define FEATURE             0x1D    //

//**命令
#define RD_RX_PLOAD_W       0x60    // 读取接收数据长度指令
#define RD_RX_PLOAD         0x61    // 读取接收数据指令
#define WR_TX_PLOAD         0xA0    // 写待发数据指令
#define FLUSH_TX            0xE1    // 冲洗发送 FIFO指令
#define FLUSH_RX            0xE2    // 冲洗接收 FIFO指令
#define REUSE_TX_PL         0xE3    // 定义重复装载数据指令
#define NOP                 0xFF    // 保留

//**NRF_FIFO状态
#define TX_FULL             0x20
#define TX_EMPTY            0x10
#define TX_NOT_EMPTY        0x00
#define RX_FULL             0x02
#define RX_EMPTY            0x01
#define RX_NOT_EMPTY        0x00

//**NRF状态
#define STATUS_TX_FULL      0x01
#define STATUS_RX_P_NO      0x0e
#define STATUS_MAX_RT       0x10
#define STATUS_TX_DS        0x20
#define STATUS_RX_DS        0x40

//
#define TX_ADDR_WIDTH       5
#define TX_PLOAD_WIDTH      32

//
#define NRF24_FLAG_RX       0x01    // 已经接收到数据
#define NRF24_FLAG_TX       0x02    // 发送成功
#define NRF24_FLAG_TX_E     0x04    // 发送失败

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


