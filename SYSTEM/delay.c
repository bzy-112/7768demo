#include "delay.h"

void delay_ms(uint32_t time)
{	 		  	  
	 uint32_t i=0;  
   while(time--)
   {
      i=12000;  
      while(i--) ;    
   }
}


void delay_us(uint32_t time)
{
	u16 i=0;  
   while(time--)
   {
      i=10; 
      while(i--) ;    
   }
}


