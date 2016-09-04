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
	//������ʼ����
	IIC_Send_Byte(id);
	//���豸�����豸��ַ
	while( IIC_Wait_Ack());
	//�ȴ�ACK
	IIC_Send_Byte(write_address) ;//���üĴ���
	//�Ĵ�����ַ
	while( IIC_Wait_Ack());
	delay_us(4);
	//�ȴ�ACK
	IIC_Send_Byte(byte1) ;//c-an0 d-an1 e-an2 f-an3  0->6.144 2->4.096,���ֽ�����
	//��������
	while(IIC_Wait_Ack());
	//�������
	IIC_Send_Byte(byte2);//���ֽ� ���ת���ٶ�
	while(IIC_Wait_Ack());
	//�������
	IIC_Stop();
	//���������ź�
}
//----------------------------------------------------//
void I2C_ADpoint(unsigned char id,unsigned char write_address)
{
	IIC_Start();
	//������ʼ����
	IIC_Send_Byte(id);
	//���豸�����豸��ַ
	while(IIC_Wait_Ack());
	//�ȴ�ACK
	IIC_Send_Byte(write_address);
	//�Ĵ�����ַ
	while(IIC_Wait_Ack());

	IIC_Stop();
	//���������ź�
}


//----------------------------------------------------------//
void I2C_Read2(unsigned char  id, unsigned char read_address)
{  
  	IIC_Start();
  	//������ʼ�ź�
  	IIC_Send_Byte(id);
	//���͵�ַ
  	while(IIC_Wait_Ack());
	//�ȴ�ACK
  	//IIC_Send_Byte(read_address);
	//���Ͷ��õ�ַ
  	//while(IIC_Wait_Ack());
	
		BUF[1] = IIC_Read_Byte(1);
	//���·���
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
 //������ַ90�����üĴ���01���Ĵ�����λ���Ĵ�����λe2
 //���ֽ����ã�c-an0 d-an1 e-an2 f-an3 ; 0->6.144 2->4.096 4->2.048
	
}





