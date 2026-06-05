
/*
                         \\\|///
                       \\  - -  //
                        (  @ @  )
+---------------------oOOo-(_)-oOOo-------------------------+
|	Author   :Cherry MCU Technology						    						|
|	Data     :2013-11-09                                    	|
|	Version  :release                                       	|
| QQ       :1625874998	                                	  |
| Function :                                                |
|	shop:https://shop106011731.taobao.com 									  |
|	phone  :+8617051287909			                             	|
|                                                           |
|                             Oooo													|
+-----------------------oooO--(   )-------------------------+
                       (   )   ) /
                        \ (   (_/
                         \_)     
*/
#include "1602_4bit.h"                 // SFR declarations
////lcd part
//#define          LINE1                             0
//#define          LINE2                             1
//#define          LINE1_HEAD                        0x80
//#define          LINE2_HEAD                        0xC0
//#define          CLEARSCREEN                       LCD_en_com(0x01)
///*************************************/
////change this part at different board
//sbit DB7=P1^3;
//sbit DB6=P1^2;
//sbit DB5=P1^1;
//sbit DB4=P1^0;
//sbit LCD1602_RS=P0^4;   //data command select  1 data  0 command  pin 4 
//sbit LCD1602_RW=P0^3;   //read write select   1 read   0 write     pin 5
//sbit LCD1602_EN=P0^5;   //LCD enable signal             pin 6
///********************************************************************/
void LCD_Prot_Init()      
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); // 使能PC端口时钟  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 ;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PC端口
  GPIO_SetBits(GPIOB, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9);	
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); // 使能PC端口时钟  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PC端口
  GPIO_SetBits(GPIOC,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);	
}

void delay(unsigned int n)      
{
    unsigned int i=0,j=0;
    for (i=n;i>0;i--)
     for (j=0;j<200;j++);  
}
void LCD_en_com(unsigned char command)
{
 //       DB7=(command>>7)%2;
//        DB6=(command>>6)%2;
//        DB5=(command>>5)%2;
//        DB4=(command>>4)%2;
        if((command>>7)%2==1){set_DB7;}else{clr_DB7;}
        if((command>>6)%2==1){set_DB6;}else{clr_DB6;}
        if((command>>5)%2==1){set_DB5;}else{clr_DB5;}
        if((command>>4)%2==1){set_DB4;}else{clr_DB4;}
        
        clr_rs;           //RS 0 
        //LCD1602_RW = 0;                //RW 0 
        set_en;        //EN --\|/__        Write command
        delay(2);
        clr_en;

 //       DB7=(command>>3)%2;
 //       DB6=(command>>2)%2;
 //       DB5=(command>>1)%2;
 //       DB4=(command>>0)%2;
        if((command>>3)%2==1){set_DB7;}else{clr_DB7;}
        if((command>>2)%2==1){set_DB6;}else{clr_DB6;}
        if((command>>1)%2==1){set_DB5;}else{clr_DB5;}
        if((command>>0)%2==1){set_DB4;}else{clr_DB4;}
        clr_rs ;           //RS 0 
        //LCD1602_RW = 0;                //RW 0 
        set_en;        //EN --\|/__        Write command
        delay(2);
        clr_en;
}
void LCD_en_dat(unsigned char command)
{
//        DB7=(command>>7)%2;
//        DB6=(command>>6)%2;
//        DB5=(command>>5)%2;
//        DB4=(command>>4)%2;
        if((command>>7)%2==1){set_DB7;}else{clr_DB7;}
        if((command>>6)%2==1){set_DB6;}else{clr_DB6;}
        if((command>>5)%2==1){set_DB5;}else{clr_DB5;}
        if((command>>4)%2==1){set_DB4;}else{clr_DB4;}
        
        set_rs;        //RS 1 
        //LCD1602_RW = 0;                //RW 0 
        set_en;        //EN --\|/__        Write data
        delay(2);
        clr_en;

 //       DB7=(command>>3)%2;
 //       DB6=(command>>2)%2;
 //       DB5=(command>>1)%2;
 //       DB4=(command>>0)%2;
        if((command>>3)%2==1){set_DB7;}else{clr_DB7;}
        if((command>>2)%2==1){set_DB6;}else{clr_DB6;}
        if((command>>1)%2==1){set_DB5;}else{clr_DB5;}
        if((command>>0)%2==1){set_DB4;}else{clr_DB4;}
        
        set_rs;        //RS 1 
        //LCD1602_RW = 0;                //RW 0 
        set_en;        //EN --\|/__        Write data
        delay(2);
        clr_en;
}
void LCD_set_xy(unsigned char x,unsigned char y)
{
        unsigned char address;
        if(y == LINE1)        
                address = LINE1_HEAD + x;
        else                         
                address = LINE2_HEAD + x;
        delay(10);
        LCD_en_com(address); 
}
void LCD_write_string(unsigned char x,unsigned char y,unsigned char *s)
{
        LCD_set_xy(x,y); 
        while(*s)  
        {
                LCD_en_dat(*s);   
                s++;
        }
}
void LCD_init(void)
{
	LCD_Prot_Init();//初始化LCD管脚
	delay(50); 
	LCD_en_com(0x33);//原来1602初始化成4位数据线之前必需先初始化成8位(此时命令发送方式是8位格式,但数据线只需接4位)
	delay(10);
	LCD_en_com(0x32);//然后再改到4位线宽,这样初始化才稳定
	delay(10); 
	LCD_en_com(0x28);//四线模式设置
	delay(10);
	LCD_en_com(0x0c);//显示开--对应开关显示控制指令
	delay(10);
	CLEARSCREEN;
	delay(30);
}
void LCD_clr(void)      
{
	LCD_en_com(0x01);
   	delay(1700);
}
/********************************************************************/
//
//void main(void)
//{
//LCD_init();
//   LCD_write_string(0,LINE1,"0123456789abcdef");
//   LCD_write_string(3,LINE2,"Advanced");
//  while(1);                  
//} 

