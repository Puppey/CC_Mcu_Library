#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x.h"

#define KEYPORT  GPIO_ReadInputData(GPIOA)&0x3F

extern unsigned char Trg;
extern unsigned char Cont;
void KEY_Init(void);
void Key_Read( void );
#endif
