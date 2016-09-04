

#ifndef _VIRTUAL_I2C_H
#define _VIRTUAL_I2C_H

#include "stm32f30x.h"
#include "stdbool.h"

extern void Virtual_I2C_init(void);
extern void I2C_Delay(void);
extern bool I2C_Start(void);
extern void I2C_Stop(void);
extern void I2C_Ack(void);
extern void I2C_NoAck(void);
extern bool I2C_WaitAck(void);
extern void I2C_SendByte(u8 data);
extern u8 I2C_ReceiveByte(void);
extern bool I2C_ReadByte(u8* pBuffer, u8 lenght, u8 Address);
extern bool Write(u8 *pBuffer, u8 Address, u8 lenght);

#endif

