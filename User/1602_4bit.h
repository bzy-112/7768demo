#ifndef __1602_4bit_H
#define __1602_4bit_H

#include "stm32f10x.h"             // SFR declarations
//lcd part
#define          LINE1                             0
#define          LINE2                             1
#define          LINE1_HEAD                        0x80
#define          LINE2_HEAD                        0xC0
#define          CLEARSCREEN                       LCD_en_com(0x01)
/*************************************/
//change this part at different board
#define set_DB7 GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define set_DB6 GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define set_DB5 GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define set_DB4 GPIO_SetBits(GPIOC,GPIO_Pin_13)

#define clr_DB7 GPIO_ResetBits(GPIOB,GPIO_Pin_7)
#define clr_DB6 GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define clr_DB5 GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define clr_DB4 GPIO_ResetBits(GPIOC,GPIO_Pin_13)

#define set_rs GPIO_SetBits(GPIOC,GPIO_Pin_15)
#define clr_rs GPIO_ResetBits(GPIOC,GPIO_Pin_15)

//sbit LCD1602_RS=P1^4;   //data command select  1 data  0 command  pin 4 
//sbit LCD1602_RW=P0^3;   //read write select   1 read   0 write     pin 5

#define set_en GPIO_SetBits(GPIOC,GPIO_Pin_14)
#define clr_en GPIO_ResetBits(GPIOC,GPIO_Pin_14)

//sbit LCD1602_EN=P1^5;   //LCD enable signal             pin 6
/********************************************************************/
//void LCD_Prot_Init() ;
void delay(unsigned int n);     

void LCD_en_com(unsigned char command);
void LCD_en_dat(unsigned char command);

void LCD_set_xy(unsigned char x,unsigned char y);

void LCD_write_string(unsigned char x,unsigned char y,unsigned char *s);

void LCD_init(void);
void LCD_clr(void);

#endif

