/*******************************************************************************
********************************************************************************
***************************BY : Lilei  2015/7/25********************************
*******************************Version  1.0*************************************
********************************************************************************
*******************************************************************************/
#ifndef __I2C_H
#define __I2C_H
#include<stm32f10x.h>
#include<delay.h>
//          I2C GPIO Configurations
#define  IIC_GPIO           GPIOB
#define  IIC_PeriphClock    RCC_APB2Periph_GPIOB
#define  SCL_Pin            GPIO_Pin_0   
#define  SDA_Pin            GPIO_Pin_1

//           Digital  IO  Define
#define  SCL_0              GPIO_ResetBits(IIC_GPIO,SCL_Pin)
#define  SCL_1              GPIO_SetBits(IIC_GPIO,SCL_Pin)
#define  SDA_0              GPIO_ResetBits(IIC_GPIO,SDA_Pin)
#define  SDA_1              GPIO_SetBits(IIC_GPIO,SDA_Pin)
//             Functions
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_ACK(u8 ack);
u8 IIC_WaitAck(void);
void IIC_Respone(void);
void IIC_Write(u8 dat);
u8 IIC_Read(void);
short Get_MPU(u8 add,u8 reg);
void IIC_WriteReg(u8 add,u8 reg,u8 dat);
u8 IIC_ReadReg(u8 add,u8 reg);
#endif
