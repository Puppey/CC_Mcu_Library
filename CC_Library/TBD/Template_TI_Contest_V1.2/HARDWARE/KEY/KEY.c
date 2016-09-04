
/***********************************************
Name    : Key_Read
Function: (1) No key  : ReadData=0;Trg=0;Cont=0;
					(2) D0 is 0 : KEYPORT=0xfe; ReadDate=0x01; Trg=0x01&(0x01^0x00) = 0x01; Cont=0x01; 
					(3) when D0 is always 0 : Trg=0x01&(0x01^0x00)=0 ; Cont=0x01;
					(4) when D0 is 1: Trg=0x00&(0x00^0x01)=0 Cont=0;
Example:
		if(Trg & 0x80)   // one key 
		
		if(Cont & 0x80)  // long key
		{
			time_count++;
			if(time_count==100)
			{
				time_count=0;
				SendString("long !\r\n");
			}
		}
************************************************/

#include "key.h"
unsigned char Trg;
unsigned char Cont;

void KEY_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;				 //LED0-->PB.5 ∂Àø⁄≈‰÷√
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
 GPIO_Init(GPIOA, &GPIO_InitStructure);					
}

void Key_Read( void )
{
    unsigned char ReadData = KEYPORT^0x3f;  		   // 1. Read KEYPORT ^(xor)
    Trg = ReadData & (ReadData ^ Cont);      			//  2 
    Cont = ReadData;                         			//  3
}
