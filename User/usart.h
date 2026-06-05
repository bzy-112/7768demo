#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "misc.h"
//////////////////////////////////////////////////////////////////////////////////	 
//串口1初始化		    
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
////////////////////////////////////////////////////////////////////////////////// 
extern u8 USART_RX_BUF[64];     //接收缓冲,最大63个字节.末字节为换行符 
extern u8 USART_RX_STA;         //接收状态标记	

void uart_init(u32 bound);

#define LEDA_ON GPIO_SetBits(GPIOB,GPIO_Pin_3)
#define LEDA_OFF GPIO_ResetBits(GPIOB,GPIO_Pin_3)

#endif
