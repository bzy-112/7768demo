
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
#include "1602_4bit.h"  
// #include "AD7190.H"
#include "delay.h"
#include "demo7768.h" 
#include "usart.h"
char str[30];
extern u32 SysTimer;
extern u32 Keycode;
extern	u8 _return;

void swap(int *a, int *b)  
{  
    int     c;  
     c = *a;  
    *a = *b;  
    *b =  c;  
} 
int GetAverage(int *dat,u16 leng)//中值滤波
{
	int Average=0;
	u16 i,j;
    //排序，从dat[0]开始排序，从小到大
    for (i = 0; i < leng; i++)  
    {  
        for (j = i + 1; j < leng; j++)  
        {  
            if (dat[i] > dat[j])  
            {  
                swap(&dat[i], &dat[j]);  
            }
        }
    }
		
		Average=dat[leng/2];
    for (i = leng/2-leng/4; i < leng/2+leng/4; i++)
		{
			Average=(Average+dat[i])/2;
		}
		return Average;
}


#define Num 		25//抽取比
/**********************************************************
													主函数
**********************************************************/
int main(void)
{

  
// 	LCD_init();
// 	LCD_clr();
// 	LCD_write_string(0,LINE1,"AD7175");
	uart_init(115200);
	LED_init();
	demo_ad7768();

}

