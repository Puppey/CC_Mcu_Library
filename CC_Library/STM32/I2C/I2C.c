#include<I2C.h>
#include "stm32f10x_rcc.h"
void IIC_Init()
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 RCC_APB2PeriphClockCmd(IIC_PeriphClock, ENABLE);	 
 GPIO_InitStructure.GPIO_Pin = SCL_Pin;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(IIC_GPIO, &GPIO_InitStructure);
 delay_ms(10);
}
void SDA_OUT()
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 		 
 GPIO_InitStructure.GPIO_Pin = SDA_Pin;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(IIC_GPIO, &GPIO_InitStructure);
}
void SDA_IN()
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 
 GPIO_InitStructure.GPIO_Pin = SDA_Pin;				 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(IIC_GPIO, &GPIO_InitStructure);
}
void IIC_Start()
{
	SDA_OUT();
    SDA_1;
	delay_us(8);
    SCL_1;                   
    delay_us(8);                 
    SDA_0;                    
    delay_us(8);                 
    SCL_0;                    
}
void IIC_Stop()
{
	  SDA_OUT();
    SDA_0;
	  delay_us(8);
    SCL_1;                    
    delay_us(8);               
    SDA_1;                  
    delay_us(8);
    SCL_0;	
}


void IIC_ACK(u8 ack)
{     
    SDA_OUT();
    if(ack)	SDA_1;
	  else    SDA_0;
	  delay_us(4);
    SCL_1;                   
    delay_us(8);                 
    SCL_0;                    
    delay_us(8);               
}

u8 IIC_WaitAck()//0:Ack  1:NoAck
{
	SDA_OUT();
	SDA_1;
	SCL_1;
	delay_us(5); 
	SDA_IN();
	if(IIC_Read())
		return 1;
	SCL_0;
	delay_us(5);
	return 0;
}

void IIC_Respone()
{
    SCL_1;                   
    delay_us(20);                                 
    SCL_0;                    
    delay_us(5);               
}


void IIC_Write(u8 dat)
{ 
	u8 i;
	SDA_OUT();
	for (i=0; i<8; i++)        
	{
			if(dat & 0x80)  SDA_1;
			else    SDA_0;
			dat <<= 1;
			delay_us(2);
			SCL_1;                
			delay_us(8);            
			SCL_0;                
			delay_us(8);            
	}
	IIC_Respone();
}

u8 IIC_Read()
{
   u8 i;
   u8 dat;
	 SDA_IN();
	 delay_us(1);
   for (i=0; i<8; i++)         
   {
        dat <<= 1;
        SCL_1;                
        delay_us(8);             
        dat |= GPIO_ReadInputDataBit(IIC_GPIO,SDA_Pin);                       
        SCL_0;                
        delay_us(8);             
    }
    return dat;
}
void IIC_WriteReg(u8 ADD,u8 reg,u8 dat)
{
   IIC_Start();
   IIC_Write(ADD);
   IIC_Write(reg);
   IIC_Write(dat);
   IIC_Stop();
}
u8 IIC_ReadReg(u8 add,u8 reg)
{
	u8 r;
	IIC_Start();
	IIC_Write(add);
	IIC_Write(reg);
	IIC_Start();
	IIC_Write(add+1);
	r=IIC_Read();
	IIC_ACK(1);
	IIC_Stop();
	return r;
}

