

#ifndef __HRPWM_H
#define __HRPWM_H

#include "stm32f30x.h"
#include "system_typedef.h"


uint32 HrPWM_CHA_Init(uint32 fsw);
uint32 HrPWM_CHA_Update(uint32 Period, uint32 duty);

#endif






