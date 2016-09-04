

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
	pid_float32	SetPoint;		//����:����ֵ
	pid_float32	Feedback;		//����:����ֵ
//PID����	
	pid_float64	T;		//����:����
	pid_float64	Kp;		//����:����
	pid_float64	Ti;		//����:����
	pid_float64	Td;		//����:΢��

	pid_float32	a0;		//����ֵ:a0 = Kp(1 + T/Ti + Td/T)
	pid_float32	a1;		//����ֵ:a1 = Kp(1 + 2Td/T)
	pid_float32	a2;		//����ֵ:a2 = Kp*Td/T

	pid_float32	Ek;    	//����ֵ:Error[k]����ǰ���
	pid_float32	Ek_1;     //����ֵ:Error[k-1]��
	pid_float32	Ek_2;     //����ֵ:Error[k-2]

	pid_float32	Output;			//����ֵ:PID���ֵ
	pid_float32	Last_Output;		//����ֵ:�ϴ����ֵ

	pid_float32	Increment;		//����ֵ(���PIDֵ����һʱ�̵�PIDֵ֮��)

	pid_float32	OutMax;			//����:PID������ֵ
	pid_float32	OutMin;			//����:PID�����Сֵ
	
} PID_TypeDef;

//-----------------------------------------------------------------------------
//  ��������                               
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


