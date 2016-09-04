
#ifndef _OLED_H
#define _OLED_H

#include "stm32f30x.h"

void IIC_Start(void);
void IIC_Stop(void);
void Write_IIC_Byte(unsigned char IIC_Byte);
void OLED_WrDat(unsigned char IIC_Data);
void OLED_WrCmd(unsigned char IIC_Command);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char bmp_dat);
void OLED_CLS(void);
void OLED_Init(void);
void OLED_P6x8Str(unsigned char x, unsigned char y,unsigned char ch[]);
void OLED_P8x16Str(unsigned char x, unsigned char y,unsigned char ch[]);


#endif

