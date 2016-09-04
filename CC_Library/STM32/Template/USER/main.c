#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "led.h"
#include "usart.h"
int main(void)
{	
	delay_init();	    	 //延时函数初始化	  
	uart_init(115200);
	while(1)
	{
		printf("hello world!");
		delay_ms(300);
	}
}

