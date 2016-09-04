#ifndef _TLV5616_H
#define _TLV5616_H

#include "stm32f10x.h"
#include "delay.h"
/******************** Cc Code ******************************                       
*          CS                            
*          FS                      
*          SCLK                 
*          DIN                   
*          VDD	5V 或 3.3V                       
*          对应参考电压REF只能选择对应VDD的一半或者更小    
*          如VDD=5V,REFIN最大2.5V              
*          Vout = 2* REFIN * (nVoltage)/4096  
***********************************************************/
#define TLV5616_CLK		 RCC_APB2Periph_GPIOB
#define TLV5616_PORT	 GPIOB

#define TLV_CS         GPIO_Pin_4
#define TLV_FS         GPIO_Pin_2
#define TLV_SCLK       GPIO_Pin_1
#define TLV_DIN        GPIO_Pin_0

#define SET_CS         GPIO_SetBits(TLV5616_PORT,TLV_CS)
#define CLR_CS         GPIO_ResetBits(TLV5616_PORT,TLV_CS)
#define SET_SCLK       GPIO_SetBits(TLV5616_PORT,TLV_SCLK)
#define CLR_SCLK     	 GPIO_ResetBits(TLV5616_PORT,TLV_SCLK)
#define SET_FS         GPIO_SetBits(TLV5616_PORT,TLV_FS)
#define CLR_FS         GPIO_ResetBits(TLV5616_PORT,TLV_FS)
#define SET_DIN        GPIO_SetBits(TLV5616_PORT,TLV_DIN)
#define CLR_DIN        GPIO_ResetBits(TLV5616_PORT,TLV_DIN)


#define High_Speed_Mode   0x40
#define Slow_Speed_Mode   0x00
#define Power_Down_Mode   0x20
#define Normal_Power_Mode 0x00

#define Mode_Byte  High_Speed_Mode

void TLV5616_Init(void);
void TLV5616_SetOutput_Voltage(unsigned int nVoltage);

#endif
