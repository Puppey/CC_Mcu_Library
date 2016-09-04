
#ifndef	_USART_H
#define	_USART_H

#include	"stm32f30x_usart.h"


void USART1_init(u32 BaudRate);
void USART1_TxSend(u8 Data);
void USART1_PrintString(u8 *puts,u16 length);

void USART1_printf(USART_TypeDef* USARTx, uint8_t *Data,...);


#endif
/*			 									[] END OF FILE 														*/
