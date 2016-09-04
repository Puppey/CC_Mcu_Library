#include "ads1115.h"
#include "usart.h"
#include "math.h"
#include "delay.h"
#include "i2c.h"
unsigned char BUF[8]; 

int   value_ads1115;

//---------------------------------------------------------//
void AD_init(unsigned char id,unsigned char write_address,unsigned char byte1,unsigned char byte2)
{
	IIC_Start();
	//产生起始条件
	IIC_Send_Byte(id);
	//向设备发送设备地址
	while( IIC_Wait_Ack());
	//等待ACK
	IIC_Send_Byte(write_address) ;//配置寄存器
	//寄存器地址
	while( IIC_Wait_Ack());
	delay_us(4);
	//等待ACK
	IIC_Send_Byte(byte1) ;//c-an0 d-an1 e-an2 f-an3  0->6.144 2->4.096,高字节配置
	//发送数据
	while(IIC_Wait_Ack());
	//发送完成
	IIC_Send_Byte(byte2);//低字节 最高转换速度
	while(IIC_Wait_Ack());
	//发送完成
	IIC_Stop();
	//产生结束信号
}
//----------------------------------------------------//
void I2C_ADpoint(unsigned char id,unsigned char write_address)
{
	IIC_Start();
	//产生起始条件
	IIC_Send_Byte(id);
	//向设备发送设备地址
	while(IIC_Wait_Ack());
	//等待ACK
	IIC_Send_Byte(write_address);
	//寄存器地址
	while(IIC_Wait_Ack());

	IIC_Stop();
	//产生结束信号
}


//----------------------------------------------------------//
void I2C_Read2(unsigned char  id, unsigned char read_address)
{  
  	IIC_Start();
  	//产生起始信号
  	IIC_Send_Byte(id);
	//发送地址
  	while(IIC_Wait_Ack());
	//等待ACK
  	//IIC_Send_Byte(read_address);
	//发送读得地址
  	//while(IIC_Wait_Ack());
	
		BUF[1] = IIC_Read_Byte(1);
	//重新发送
  	delay_us(5);
		BUF[2] = IIC_Read_Byte(1);
		IIC_Stop();		
}

//-----------------------------------------------//
__IO float voltage;
void Read_ADS1115(u8 channel)
{   
		
		ADS1115_Init(channel);
		I2C_ADpoint(0x90,0x00);;	
		I2C_Read2(0x91,0x00);
     
		value_ads1115 = (BUF[1] << 8) | BUF[2]; 
		if(value_ads1115>0x7fff)	value_ads1115=0;	  
		voltage = ((float)value_ads1115/(float)0xffff) *8.192;  
}

void ADS1115_Init(u8 channel)
{
	 u8 HReg=0;
	
	 switch(channel)
	 {
		 case 0:HReg=0xC2;break;
		 case 1:HReg=0xD2;break;
		 case 2:HReg=0xE2;break;
		 case 3:HReg=0xF2;break;
	 }
	 AD_init(0x90,0x01,HReg,0xE3);
 //器件地址90，配置寄存器01，寄存器高位，寄存器低位e2
 //高字节配置：c-an0 d-an1 e-an2 f-an3 ; 0->6.144 2->4.096 4->2.048
	
}





