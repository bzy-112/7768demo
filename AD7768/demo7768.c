#include "sys.h"
#include "delay.h"
#include "1602_4bit.h" 
#include "AD7768.h"
#include "demo7768.h"
#include "usart.h"
#include <stdio.h>
#include <stdlib.h>
#include "mcp_myspi.h"
#include <stdlib.h>
void ad7768swap(int *a, int *b)  
{  
    int     c;  
     c = *a;  
    *a = *b;  
    *b =  c;  
} 
int ad7768GetAverage(int *dat,u16 leng)//中值滤波
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
                ad7768swap(&dat[i], &dat[j]);  
            }
        }
    }
		
		Average=dat[leng/2];

		return Average;
}


	float value[8];

int demo_ad7768(void)
{ 
  long iTemp;
	long ret      = 0;
	long timeout  = 0x00FFFFFF;


  char buf[16]=" ";
	uint32_t vol[8]={0};

	
	uint32_t* voltage=vol;

	uint8_t i;

	uint32_t number1 = rand() % 20 + 10; 
	/* Initialize AD7768 device. */
	ret = ad7768_Setup();
	if(ret < 0)
	{
			printf("AD7768 Err\r\n");
	}
	else
	{
			printf("AD7768 OK\r\n");
	} 
	delay_ms(1000);//初始化延时
	
	//ret = AD7768_WaitForReady(timeout);
	if(ret < 0)
	{
			printf("AD7768 Timeout\r\n");
	}

  while(1) 
	{
		voltage=AD7768_Recive_ADC_DATA();
		for (i = 0; i < 8; i++)
		{
//			printf("ch%d = 0x0x%x",i,*(voltage + i));
			if(*(voltage+i) & 0x00800000)
			{
				*(voltage+i) = (~*(voltage+i)) & 0x00FFFFFF;
				value[i] = -(*(voltage+i)/8388608.0*2.500000);
			}
			else
			{
				*(voltage+i) = *(voltage+i) & 0X007FFFFF;
				value[i] = *(voltage+i)/8388608.0*2.500000;
			}
		}
//		printf("\n");
		printf("%6f,%6f,%6f,%6f,%6f,%6f,%6f,%6f\n",value[0],value[1],value[2],value[3],value[4],value[5],value[6],value[7]);
		
		
	
	}
	
}
