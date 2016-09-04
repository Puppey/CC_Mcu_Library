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
// KEY: PA0-PA5
// TLV5616(DA): PB0-PB2 3wire
// ADS1115(AD): PC0 PC1 2wire
// LCD12864:		PB5 PB6 PB7 PB8
extern float voltage;
u16 voltage0,voltage1,voltage2,voltage3;
int main(void)
{	
	delay_init();	    	 //��ʱ������ʼ��	  
	uart_init(115200);
	LED_Init();
	
	TLV5616_Init();
	TLV5616_SetOutput_Voltage(0000);
	
	IIC_Init();					//��ʼ��ADS1115��I2C	
	
	KEY_Init();
	
	Uc1701_Init();
	clear_screen(0x00);
//	LCD_Write_Chinese(0,4,3);	//�趨
//	LCD_Write_Chinese(16,4,4);
	
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
		LED0=0;
		Key_Read();
		if(Trg & 0x01)									//��
		{
			LCD_Write_8x16(0,0,'1');
			LCD_Write_8x16(48,4,'3');
		}
		if(Trg & 0x02)									//��
		{
			LCD_Write_8x16(0,0,'2');
		}
		if(Trg & 0x04)									//��
		{
			LCD_Write_8x16(0,0,'3');			
		}
		if(Trg & 0x08)
		{
			LCD_Write_8x16(0,0,'4');			//��
		}
		if(Trg & 0x10)
		{
			LCD_Write_8x16(0,0,'5');			//A
		}
		if(Trg & 0x20)
		{
			LCD_Write_8x16(0,0,'6');			//B
		}
		
		Read_ADS1115(0);
		delay_ms(2);
		voltage0=(u16)(voltage*1000);
		LCD_Write_8x16(16,0,voltage0/1000+0x30);
		LCD_Write_8x16(24,0,'.');
		LCD_Write_8x16(32,0,voltage0/100%10+0x30);
		LCD_Write_8x16(40,0,voltage0/10%10+0x30);
		LCD_Write_8x16(48,0,voltage0%10+0x30);
		
		Read_ADS1115(1);
		delay_ms(2);
		voltage1=(u16)(voltage*1000);
		LCD_Write_8x16(16,2,voltage1/1000+0x30);
		LCD_Write_8x16(24,2,'.');
		LCD_Write_8x16(32,2,voltage1/100%10+0x30);
		LCD_Write_8x16(40,2,voltage1/10%10+0x30);
		LCD_Write_8x16(48,2,voltage1%10+0x30);
		
		Read_ADS1115(2);
		delay_ms(2);
		voltage2=(u16)(voltage*1000);
		LCD_Write_8x16(16,4,voltage2/1000+0x30);
		LCD_Write_8x16(24,4,'.');
		LCD_Write_8x16(32,4,voltage2/100%10+0x30);
		LCD_Write_8x16(40,4,voltage2/10%10+0x30);
		LCD_Write_8x16(48,4,voltage2%10+0x30);
		
		Read_ADS1115(3);
		delay_ms(2);
		voltage3=(u16)(voltage*1000);
		LCD_Write_8x16(16,6,voltage3/1000+0x30);
		LCD_Write_8x16(24,6,'.');
		LCD_Write_8x16(32,6,voltage3/100%10+0x30);
		LCD_Write_8x16(40,6,voltage3/10%10+0x30);
		LCD_Write_8x16(48,6,voltage3%10+0x30);
		LED0=1;
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
