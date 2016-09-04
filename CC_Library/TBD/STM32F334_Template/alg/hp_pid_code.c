



#include "hp_pid_code.h"

//===================PID===============//
/**
 *******************************************************************************
 * @������void PID_DeInit( PID_TypeDef	*p)
 * @������PID���ݸ�λ
 * @���룺*p�������PID����
 * @���أ��� 
 * @���ߣ���СP
 * @�汾��V1.0
 *******************************************************************************
 */
void PID_DeInit( PID_TypeDef	*p)
{
	p->a0			= 0;
	p->a1			= 0;
	p->a2			= 0;

	p->Ek 			= 0;     //  Error[-1]
	p->Ek_1			= 0;     //  Error[-2]
	p->Ek_2			= 0;     //  Sums of Errors 

	p->T			= 0;
	p->Kp			= 0;
	p->Ti			= 0;
	p->Td			= 0;
	
	p->Feedback		= 0;
	p->SetPoint		= 0;
	
	p->Last_Output	= 0;
	p->Output				= 0;

	p->Increment	= 0;

	p->OutMax		= 0;
	p->OutMin		= 0; 
}
/**
 *******************************************************************************
 * @������void PID_init ( PID_TypeDef *p)
 * @������PID��������
 * @���룺*p�������PID����
 * @���أ��� 
 * @���ߣ���СP
 * @�汾��V1.0
 *******************************************************************************
 */
void PID_init ( PID_TypeDef *p)
{   
	
	p->a0		= p->Kp*(1 + 1.0*p->T/p->Ti + 1.0*p->Td/p->T);
	p->a1		= p->Kp*(1 + 2.0*p->Td/p->T);
	p->a2		= 1.0*p->Kp*p->Td/p->T;
}

/**
 *******************************************************************************
 * @������pid_float32 PID_Calc( PID_TypeDef *p)
 * @������PID����ʽ����	 
 * @���룺*p��PID�ṹ��
 * @���أ�PID������	 
 * @���ߣ���СP
 * @�汾��V1.0
 *******************************************************************************
 */
#pragma arm section code = "RAMCODE"
pid_float32 PID_Calc( PID_TypeDef *p, pid_float32 feedback, pid_float32 ref)
{
	p->Ek	= ref - feedback;		//�������

	p->Increment		= (  p->a0*p->Ek	- p->a1*p->Ek_1 + p->a2*p->Ek_2 );	//PID����

	p->Output = p->Last_Output + p->Increment; 

	if(p->Output > p->OutMax)p->Output   =	p->OutMax;
	if(p->Output < p->OutMin)p->Output   =	p->OutMin;
	p->Ek_2           = p->Ek_1;
	p->Ek_1           = p->Ek;		
	p->Last_Output    = p->Output;
	return p->Output;
}

#pragma arm section
/**
 *******************************************************************************
 * @������void PID_Clean( PID_TypeDef	*p)
 * @���������PID�����еĻ���
 * @���룺*p�������PID����
 * @���أ��� 
 * @���ߣ���СP
 * @�汾��V1.0
 *******************************************************************************
 */
void PID_Clean( PID_TypeDef	*p)
{
	p->Ek 			= 0;     //  Error[-1]
	p->Ek_1			= 0;     //  Error[-2]
	p->Ek_2			= 0;     //  Sums of Errors 

	p->Increment		= 0;
	
	p->Last_Output	= 0;
	p->Output 			= 0;
}

//===================PI===============//

/**
 *******************************************************************************
 * @������void PI_DeInit( PID_TypeDef	*p)
 * @������PI���ݸ�λ
 * @���룺*p�������PID����
 * @���أ��� 
 * @���ߣ���СP
 * @�汾��V1.0
 *******************************************************************************
 */
void PI_DeInit( PID_TypeDef	*p)
{
	p->a0			= 0;
	p->a1			= 0;
	p->a2			= 0;

	p->Ek 			= 0;     //  Error[-1]
	p->Ek_1			= 0;     //  Error[-2]
	p->Ek_2			= 0;     //  Sums of Errors 

	p->T			= 0;
	p->Kp			= 0;
	p->Ti			= 0;
	p->Td			= 0;
	
	p->Feedback		= 0;
	p->SetPoint		= 0;
	
	p->Last_Output	= 0;
	p->Output				= 0;

	p->Increment	= 0;

	p->OutMax		= 0;
	p->OutMin		= 0; 
}
/**
 *******************************************************************************
 * @������void PID_init ( PID_TypeDef *p)
 * @������PI��������
 * @���룺*p�������PID����
 * @���أ��� 
 * @���ߣ���СP
 * @�汾��V1.0
 *******************************************************************************
 */
void PI_init ( PID_TypeDef *p)
{   

	p->a0		= p->Kp*(1 + 1.0*p->T/p->Ti + 1.0*p->Td/p->T);
	p->a1		= p->Kp*(1 + 2.0*p->Td/p->T);

}
/**
 *******************************************************************************
 * @������pid_float32 PID_Calc( PID_TypeDef *p)
 * @������PI����ʽ����	 
 * @���룺*p��PID�ṹ��
 * @���أ�PID������	 
 * @���ߣ���СP
 * @�汾��V1.0
 *******************************************************************************
 */
#pragma arm section code = "RAMCODE"

pid_float32 PI_Calc( PID_TypeDef *p, pid_float32 feedback, pid_float32 ref)
{

	p->Ek	= ref - feedback;		//�������

	p->Increment		= (  p->a0*p->Ek	- p->a1*p->Ek_1 );	//PID����

	p->Output = p->Last_Output + p->Increment; 

	if(p->Output > p->OutMax)p->Output   =	p->OutMax;
	if(p->Output < p->OutMin)p->Output   =	p->OutMin;
	p->Ek_2           = p->Ek_1;
	p->Ek_1           = p->Ek;		
	p->Last_Output    = p->Output;
	return p->Output;
}
#pragma arm section
/**
 *******************************************************************************
 * @������void PID_Clean( PID_TypeDef	*p)
 * @���������PID�����еĻ���
 * @���룺*pp�������PID����
 * @���أ��� 
 * @���ߣ���СP
 * @�汾��V1.0
 *******************************************************************************
 */
void PI_Clean( PID_TypeDef	*p)
{
	p->Ek 			= 0;     //  Error[-1]
	p->Ek_1			= 0;     //  Error[-2]
	p->Ek_2			= 0;     //  Sums of Errors 

	p->Increment		= 0;
	
	p->Last_Output	= 0;
	p->Output 			= 0;
}
//==============================================================
//[] END OF FILE 
//==============================================================




