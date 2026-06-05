#ifndef __SPI_H
#define __SPI_H
#include "sys.h"


/* 定义GPIO端口 */
//寄存器SPI
/*
SCK     ->   PB13
DIN     ->   PB15
CS      ->   PB12
DOUT    ->   PB14
SYNC_OUT ->   PA1
SYNC_IN  ->   PA3
*/
//数据SPI
/*
DCLK     ->   PA7
DRDY     ->   PA6
DOUT0    ->   PA4
*/

#define RCC_SCK 	RCC_APB2Periph_GPIOB
#define PORT_SCK	GPIOB
#define PIN_SCK		GPIO_Pin_13

#define RCC_DIN 	RCC_APB2Periph_GPIOB
#define PORT_DIN	GPIOB
#define PIN_DIN		GPIO_Pin_15

#define RCC_CS 		RCC_APB2Periph_GPIOB
#define PORT_CS		GPIOB
#define PIN_CS		GPIO_Pin_12


#define RCC_DOUT 	RCC_APB2Periph_GPIOB
#define PORT_DOUT	GPIOB
#define PIN_DOUT	GPIO_Pin_14


#define RCC_RESET 	RCC_APB2Periph_GPIOA
#define PORT_RESET	GPIOA
#define PIN_RESET 	GPIO_Pin_5

#define RCC_START   	RCC_APB2Periph_GPIOA
#define PORT_START	  GPIOA
#define PIN_START		  GPIO_Pin_2

#define RCC_SYNC_OUT   	RCC_APB2Periph_GPIOA
#define PORT_SYNC_OUT	  GPIOA
#define PIN_SYNC_OUT		GPIO_Pin_1

#define RCC_SYNC_IN   	RCC_APB2Periph_GPIOA
#define PORT_SYNC_IN	  GPIOA
#define PIN_SYNC_IN		  GPIO_Pin_3


#define CS_0()		GPIO_ResetBits(PORT_CS, PIN_CS)
#define CS_1()		GPIO_SetBits(PORT_CS, PIN_CS)

#define SCK_0()		GPIO_ResetBits(PORT_SCK, PIN_SCK)
#define SCK_1()		GPIO_SetBits(PORT_SCK, PIN_SCK)

#define DI_0()		GPIO_ResetBits(PORT_DIN, PIN_DIN)
#define DI_1()		GPIO_SetBits(PORT_DIN, PIN_DIN)

#define START_0()		GPIO_ResetBits(PORT_START, PIN_START)
#define START_1()		GPIO_SetBits(PORT_START, PIN_START)

#define RESET_0()		GPIO_ResetBits(PORT_RESET, PIN_RESET)
#define RESET_1()		GPIO_SetBits(PORT_RESET, PIN_RESET)

#define DO_IS_HIGH()	(GPIO_ReadInputDataBit(PORT_DOUT, PIN_DOUT) == Bit_SET)


//数据SPI PB3，PB4，PA13，PA14，PA15
#define RCC_DCLK 	RCC_APB2Periph_GPIOA
#define PORT_DCLK	GPIOA
#define PIN_DCLK	GPIO_Pin_7

#define RCC_DRDY 	RCC_APB2Periph_GPIOA
#define PORT_DRDY	GPIOA
#define PIN_DRDY	GPIO_Pin_6

#define RCC_DOUT0  		RCC_APB2Periph_GPIOA
#define PORT_DOUT0 		GPIOA
#define PIN_DOUT0 		GPIO_Pin_4


#define DCLK_IS_1()	(GPIO_ReadInputDataBit(PORT_DCLK, PIN_DCLK) == Bit_SET)
#define DCLK_IS_0()	(GPIO_ReadInputDataBit(PORT_DCLK, PIN_DCLK) == Bit_RESET)


#define DRDY_IS_1()	(GPIO_ReadInputDataBit(PORT_DRDY, PIN_DRDY) == Bit_SET)
#define DRDY_IS_0()	(GPIO_ReadInputDataBit(PORT_DRDY, PIN_DRDY) == Bit_RESET)



void AD7768_reg_spi(void);
void AD7768_data_spi(void);
uint8_t AD7768_Recive8Bit(void);
void AD7768_Send8Bit(uint8_t _data);
uint16_t AD7768_SendRecive_REG(uint8_t _address,uint8_t _data);
uint32_t* AD7768_Recive_ADC_DATA(void);
void AD7768_START(void);

#endif

