#include "mcp_myspi.h"
#include "delay.h"
#include "sys.h"
#include "stm32f10x_spi.h"
//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI口初始化


void AD7768_Send8Bit(uint8_t _data);
uint8_t AD7768_Recive8Bit(void);
void AD7768_WaitDRDY(void);
void AD7768_DelaySCLK(void);
void AD7768_DelayDATA(void);

void AD7768_reg_spi(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); // 使能PA端口时钟 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); // 使能PB端口时钟 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); // 使能PC端口时钟 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG, ENABLE); // 使能PG端口时钟 

	/* 配置几个推完输出IO */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	/* 推挽输出模式 */

	GPIO_InitStructure.GPIO_Pin = PIN_SCK;
	GPIO_Init(PORT_SCK, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_DIN;
	GPIO_Init(PORT_DIN, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PIN_CS;
	GPIO_Init(PORT_CS, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PIN_START;
	GPIO_Init(PORT_START, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PIN_RESET;
	GPIO_Init(PORT_RESET, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		/* PIN_DOUT 设置为输入上拉 */
	GPIO_InitStructure.GPIO_Pin = PIN_DOUT;
	GPIO_Init(PORT_DOUT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PIN_SYNC_OUT;    /* SYNC_OUT 设置为输入上拉 */
	GPIO_Init(PORT_SYNC_OUT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = PIN_SYNC_IN;     /* SYNC_IN 设置为输入上拉 */
	GPIO_Init(PORT_SYNC_IN, &GPIO_InitStructure);

	CS_1();
	SCK_0();		/* SPI总线空闲时，钟线是低电平 */
	DI_1();	
	
	START_1();//如果不使用此引脚，应通过一个上拉电阻将其连接到逻辑1电平。
	RESET_0();
	delay_ms(2);
	RESET_1();
	delay_ms(50);
}

void AD7768_data_spi(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); // 使能PA端口时钟 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); // 使能PB端口时钟 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); // 使能PC端口时钟 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG, ENABLE); // 使能PG端口时钟 
	/* 配置几个推完输出IO */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	/* PIN_DCLK 设置为输入上拉 */
	GPIO_InitStructure.GPIO_Pin = PIN_DCLK;
	GPIO_Init(PORT_DCLK, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		/* PIN_DRDY 设置为输入上拉 */
	GPIO_InitStructure.GPIO_Pin = PIN_DRDY;
	GPIO_Init(PORT_DRDY, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		/* PIN_DOUT0 设置为输入上拉 */
	GPIO_InitStructure.GPIO_Pin = PIN_DOUT0;
	GPIO_Init(PORT_DOUT0, &GPIO_InitStructure);
}



void AD7768_DelaySCLK(void)
{
	uint16_t i;
	for (i = 0; i < 10; i++);
}


void AD7768_DelayDATA(void)
{
	delay_ms(3);	/*1ms*/
}


uint16_t AD7768_SendRecive_REG(uint8_t _address, uint8_t _data)
{
	
	uint8_t i;
	uint16_t read = 0;
	uint16_t tempdata=0;
	uint16_t temp=0;
	
	tempdata=_address;
	tempdata=tempdata << 8 ;
	tempdata=tempdata | _data;
	
	//CMD 1
	CS_0();
	AD7768_DelaySCLK();
	for(i = 0; i < 16; i++)//(R/W、地址)
	{
		temp = tempdata & 0x8000;
		if (temp)
		{
			DI_1();
		}
		else
		{
			DI_0();
		}
		tempdata <<= 1;	
		SCK_1();
		AD7768_DelaySCLK();
		SCK_0();
		AD7768_DelaySCLK();						
	}
	CS_1();
	AD7768_DelayDATA();	
	
	//CMD 2 & READ RESP 1
	CS_0();
	AD7768_DelaySCLK();
	
	for (i = 0; i < 16; i++)//其后是8位数据
	{
		SCK_1();
		AD7768_DelaySCLK();
		read = read<<1;
		read= read | GPIO_ReadInputDataBit(PORT_DOUT, PIN_DOUT);
		SCK_0();
		AD7768_DelaySCLK();
	}
	CS_1();
	
	AD7768_DelayDATA();
	return read;
}

uint32_t* AD7768_Recive_ADC_DATA(void)
{
	static uint32_t read[8];
	uint32_t val;
	uint8_t i,j;
	

	while(DRDY_IS_0());
	while(DRDY_IS_1());//μè′y×a??íê3é
	while(DRDY_IS_1());//μè′y×a??íê3é
	for (j = 0; j < 8; j++)
	{
		//HEADER 8bit + ADC DATA 24bit
		for (i = 0; i < 32; i++)
		{
			while(DCLK_IS_1());//DCLk???μ???áêy
			val = val<<1;
			val=  val | GPIO_ReadInputDataBit(PORT_DOUT0, PIN_DOUT0);
			while(DCLK_IS_0());//μè′yDCLk??1
		}
		read[j]=val;
	}
	return read;
}

void AD7768_START(void)
{
	START_0();
	AD7768_DelayDATA();
	START_1();
}



