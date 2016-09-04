


/*--------Includes--------*/
#include "system_typedef.h"
#include "hp_pid_code.h"
#include "onboard_led.h"
#include "DPlib.h"
#include "timer.h"
#include "usart.h" 
#include "hrpwm.h"
#include "stdio.h"
#include "adc.h"
#include "stdio.h"
#include "string.h"


#define fabs(x)	(x < 0 ? -x : x)		//绝对值计算


uint16 Status			= 0;


float32	VIPWR			= 0;
float32	VOPWR			= 0;
float32	VOEXT			= 0;
float32	IOPWR			= 0;
float32	PWRIN			= 0;
float32	PWROUT		= 0;
float32 VoutREF 	= 24;
float32 VinREF  	= 28;

uint16  ADC_VIPWR_Value;
uint16  ADC_VOPWR_Value;
uint16  ADC_VOEXT_Value;
uint16  ADC_IOPWR_Value;

uint32 PWM_Period;
uint32 PWM_Duty;
	
PID_TypeDef pid_voltage_loop; //PID数据


int main(void)
{
  SysTick_Config(SystemCoreClock / 1000);//Systick初始化,SysTick end of count event each 1ms
	Delay_ms(100);  

  LED_Init();
	LED1_On();
	LED2_On();
	Delay_ms(200);
	LED1_Off();
	LED2_Off();	
	Delay_ms(200);
	LED1_On();
	LED2_On();
	Delay_ms(200);
	LED1_Off();
	LED2_Off();		

	while (1)
  {
		LED1_On();
		Delay_ms(200);
		LED1_Off();
		Delay_ms(200);
	}
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
