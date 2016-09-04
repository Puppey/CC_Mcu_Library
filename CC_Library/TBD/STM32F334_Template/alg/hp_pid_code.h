

#ifndef HP_PID_CODE_H
#define HP_PID_CODE_H


#include "system_typedef.h"

#ifndef PID_DATA_TYPES
#define PID_DATA_TYPES

typedef int			pid_int16;
typedef long		pid_init32;
typedef float		pid_float32;
typedef double	pid_float64;

#endif

typedef struct  _PID{    
	pid_float32	SetPoint;		//输入:给定值
	pid_float32	Feedback;		//输入:反馈值
//PID参数	
	pid_float64	T;		//输入:周期
	pid_float64	Kp;		//输入:比例
	pid_float64	Ti;		//输入:积分
	pid_float64	Td;		//输入:微分

	pid_float32	a0;		//计算值:a0 = Kp(1 + T/Ti + Td/T)
	pid_float32	a1;		//计算值:a1 = Kp(1 + 2Td/T)
	pid_float32	a2;		//计算值:a2 = Kp*Td/T

	pid_float32	Ek;    	//计算值:Error[k]，当前误差
	pid_float32	Ek_1;     //计算值:Error[k-1]，
	pid_float32	Ek_2;     //计算值:Error[k-2]

	pid_float32	Output;			//计算值:PID输出值
	pid_float32	Last_Output;		//计算值:上次输出值

	pid_float32	Increment;		//增量值(输出PID值与上一时刻的PID值之差)

	pid_float32	OutMax;			//输入:PID输出最大值
	pid_float32	OutMin;			//输入:PID输出最小值
	
} PID_TypeDef;

//-----------------------------------------------------------------------------
//  函数声明                               
//-----------------------------------------------------------------------------
void PID_DeInit( PID_TypeDef *p);
void PID_init (PID_TypeDef *p);
void PID_Clean( PID_TypeDef	*p);
pid_float32 PID_Calc( PID_TypeDef *p, pid_float32 feedback, pid_float32 ref);

void PI_DeInit( PID_TypeDef *p);
void PI_init (PID_TypeDef *p);
void PI_init (PID_TypeDef *p);
void PI_Clean( PID_TypeDef	*p);
pid_float32  PI_Calc( PID_TypeDef *p, pid_float32 feedback, pid_float32 ref);
#endif


