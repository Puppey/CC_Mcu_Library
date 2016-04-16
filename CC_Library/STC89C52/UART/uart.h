#ifndef __UART_H__
#define __UART_H__

#define FOSC 11059200L      //System frequency
#define BAUD 9600           //UART baudrate

typedef unsigned char BYTE;
typedef unsigned int WORD;

void SendData(BYTE dat);
void SendString(char *s);
void Uart_Init(void);

#endif