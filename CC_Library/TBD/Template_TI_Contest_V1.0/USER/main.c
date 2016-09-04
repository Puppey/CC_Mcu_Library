#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "led.h"
#include "usart.h"
#include "TLV5616.h"
#include "I2C.h"
#include "ADS1115.h"
#include "KEY.h"
#include "FMG12864.h"
#include "Timer.h"
// PB3 PB4 ��JTAG
unsigned int Initdata[4];
u16 value;
u16 key_val;
double Volt;
int main(void)
{	
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(115200);
	TLV5616_Init();
	TLV5616_SetOutput_Voltage(500);
	IIC_Init();					//��ʼ��ADS1115��I2C	
	KEY_Init();
	Uc1701_Init();
	clear_screen(0x55);
	TIM2_Int_Init(14999,71);//15ms
	TIM3_Int_Init(19999,71);//1Mhz�ļ���Ƶ�ʣ�������19999Ϊ20ms  
	TIM4_Int_Init(4999,71);//5ms
	while(1)
	{
		delay_ms(2);
	}
}

//TIM2�ж�--------15ms
void TIM2_IRQHandler(void)   
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) 
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  
	}
}
//TIM3�ж�--------20ms
void TIM3_IRQHandler(void)   
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ 
	}
}

//TIM4�ж�---------5ms
void TIM4_IRQHandler(void)   
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) 
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update); 
	}
}
//	clear_screen(0x00);
//	LCD_Write_8x16(0,0,'!');
//	LCD_Write_8x16(0,2,'H');
//	LCD_Write_Chinese(0,4,0);
//	LCD_Write_Chinese(16,4,1);
//	LCD_Write_Chinese(32,4,2);
//	LCD_Write_Chinese(48,4,3);
//	LCD_Write_Chinese(64,4,4);

//		Read_ADS1115(0);
//		delay_ms(2);
//		Read_ADS1115(1);
//		delay_ms(2);
//		Read_ADS1115(2);
//		delay_ms(2);
//		Read_ADS1115(3);


//		Display_pic1();

//		Key_Read();
//		if(Trg & 0x01)
//		{
//			printf("1");
//		}
//		if(Trg & 0x02)
//		{
//			printf("2");
//		}
//		if(Trg & 0x04)
//		{
//			printf("3");
//		}
//		if(Trg & 0x08)
//		{
//			printf("4");
//		}
//		
//		if(Trg & 0x10)
//		{
//			printf("5");
//		}
//		if(Trg & 0x20)
//		{
//			printf("6");
//		}
