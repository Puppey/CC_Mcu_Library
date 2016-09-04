#ifndef __TIMER_H__
#define __TIMER_H__
#include "stm32f10x.h"
#include "sys.h"

void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc); 
void TIM4_Int_Init(u16 arr,u16 psc);

#endif
