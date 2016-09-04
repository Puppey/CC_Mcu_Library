


#include "virtual_i2c.h"

#define TRUE	1		//Õæ
#define FALSE	0		//¼Ù


#define SCL_PIN		GPIO_Pin_6
#define SDA_PIN		GPIO_Pin_7

#define SCL_H			GPIOB->BSRR	= SCL_PIN
#define SCL_L			GPIOB->BRR	= SCL_PIN
#define SCL_read	(bool)GPIOB->IDR	& SCL_PIN

#define SDA_H			GPIOB->BSRR	= SDA_PIN
#define SDA_L			GPIOB->BRR	= SDA_PIN
#define SDA_read	(bool)(GPIOB->IDR	& SDA_PIN)
	


void Virtual_I2C_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin		= SCL_PIN | SDA_PIN;
  GPIO_InitStructure.GPIO_Mode	= GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	SCL_H;
	SDA_H;

}


bool Write(u8 *pBuffer, u8 Address, u8 lenght)
{
	I2C_Start();
	I2C_SendByte(Address);
	while(lenght--)
	{
		I2C_SendByte(*pBuffer);
		pBuffer++;
	}
	I2C_Stop();
	return TRUE;
}

void I2C_Delay(void)
{
	u8 i		= 10;
	while(i--);
}

void I2C_SendByte(u8 data)
{
	u8 i	= 8;
	while(i--)
	{
		if(data&0x80)
			SDA_H;
		else 
			SDA_L;

		SCL_H;
		I2C_Delay();
		SCL_L;
		data <<= 1;
	}
	SCL_H;
	I2C_Delay();
	SCL_L;	
}

bool I2C_Start(void)
{
	
	SDA_H;
	SCL_H;
	I2C_Delay();
	SDA_L;
	I2C_Delay();
	SCL_L;
	return TRUE;
}
void I2C_Stop(void)
{
	

	SDA_L;
	SCL_H;
	I2C_Delay();
	SDA_H;
	I2C_Delay();
}
void I2C_Ack(void)
{
	SCL_L;
	I2C_Delay();
	SDA_L;
	I2C_Delay();
	SCL_H;
	I2C_Delay();
	SCL_L;
	I2C_Delay();
	SCL_H;
	SDA_H;
}
void I2C_NoAck(void)
{
	SCL_L;
	I2C_Delay();
	SDA_H;
	I2C_Delay();
	SCL_H;
	I2C_Delay();
	SCL_L;
	I2C_Delay();
	SCL_H;
	SDA_H;

}







