#ifndef __FMG12864_H__
#define __FMG12864_H__

#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"


#define    RSTB	PBout(8)
#define    RS		PBout(5)
#define    SCL	PBout(6)
#define    SDI	PBout(7)

void Uc1701_Init(void);
void Write_uc1701_COM(u8 COMDADA_4SPI);
void Write_uc1701_DISPLAY_DATA_serial_4SPI(u8 DADA_4SPI);
void HDReset(void);
void Keypress(void);
void clear_screen(u8 xx);
void Display_pic1(void);

void LCD_Write_6x8(u8 c);
void LCD_Write_8x16(u8 x,u8 y,u8 c);
void LCD_Write_Chinese(u8 x,u8 y,u8 c);
#endif
