
/*
                         \\\|///
                       \\  - -  //
                        (  @ @  )
+---------------------oOOo-(_)-oOOo-------------------------+
|	Author   :Cherry MCU Technology						    						|
|	Data     :2016-02-20                                    	|
|	Version  :Bata2.1                                       	|
| QQ       :1625874998	                                	  |
| Function :                                                |
|	shop	   :https://shop106011731.taobao.com 								|
|	phone    :+8617051287909			                            |
|                                                           |
|                             Oooo													|
+-----------------------oooO--(   )-------------------------+
                       (   )   ) /
                        \ (   (_/
                         \_)     
*/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "ad7768.h"
#include "sys.h"
#include "mcp_myspi.h"

/**************************************************************************//**
* @brief Initializes the ad7768 
*
* @return Returns 0 for success or negative error code.
******************************************************************************/
int32_t ad7768_Setup(void)
{
	uint16_t i=0;
	uint16_t read=0;
	char buf[10]=" ";
	uint8_t reg=0;
	//��ʼ��SPI�˿�
	AD7768_reg_spi();
 	AD7768_data_spi();
	

		AD7768_DelaySCLK();
	AD7768_SendRecive_REG( 0x04,0x00);	//最高功耗
		AD7768_DelaySCLK();
	AD7768_SendRecive_REG(0x80 | 0x04,0x00);	//最高功耗
		AD7768_DelaySCLK();
	AD7768_SendRecive_REG( 0x07,0x00); // DCLK=fMOD/8=250kHz for bit-bang
		AD7768_DelaySCLK();
	AD7768_SendRecive_REG(0x80 | 0x07,0x00); // DCLK=fMOD/8=250kHz for bit-bang
	AD7768_SendRecive_REG( 0x06,0x80);
	
	AD7768_SendRecive_REG( 0x11,0xFF);
	AD7768_SendRecive_REG( 0x12,0xFF);
	AD7768_SendRecive_REG( 0x13,0xFF);
	AD7768_SendRecive_REG( 0x14,0xFF);
	
	//READ_ALL_REG
	for (i = 0; i < 0x5A; i++)
	{
		read= AD7768_SendRecive_REG( reg | 0x80,0x00);
		sprintf(buf,"%x=%x",reg,read);
		printf("%s\r\n",buf);
		reg++;
	}
	
	AD7768_START();//��ʼ�ɼ�

	return read;
}

void AD7768_SPI_START(void)
{
	uint16_t read=0;
	read=AD7768_SendRecive_REG(0x06,0x00);
	read=AD7768_SendRecive_REG(0x06,0x80);
}


