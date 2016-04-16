#ifndef __KEY_H__
#define __KEY_H__

#include "reg52.h"

#define KEYPORT  P3

extern unsigned char Trg;
extern unsigned char Cont;

void Key_Read( void );
#endif
