#include "TLV5616.h"
/******************** Cc Code ******************************/

void TLV5616_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(TLV5616_CLK, ENABLE);	
	
 GPIO_InitStructure.GPIO_Pin = TLV_DIN|TLV_SCLK|TLV_FS|TLV_CS;			
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		
 GPIO_Init(TLV5616_PORT, &GPIO_InitStructure);				
}

void TLV5616_SetOutput_Voltage(unsigned int nVoltage)
{
    unsigned char i=0;
    nVoltage = nVoltage & 0x0FFF;      //屏蔽高四位
    nVoltage |= (Mode_Byte<<8);
    //时序开始
    SET_SCLK;
    SET_FS;
    SET_CS;
		delay_us(2);
    CLR_CS;
    CLR_FS;
    for(i=0;i<16;i++)
    {
        if((nVoltage & 0x8000) == 0x8000)
        {
            SET_DIN;
        }
        else
        {
            CLR_DIN;
        }
        SET_SCLK;
				delay_us(2);
        CLR_SCLK;
        nVoltage = nVoltage<<1;
    }
    SET_FS;
    SET_CS;
    SET_SCLK;
}
