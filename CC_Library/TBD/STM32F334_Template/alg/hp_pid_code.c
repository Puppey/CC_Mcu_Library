



#include "hp_pid_code.h"

//===================PID===============//
/**
 *******************************************************************************
 * @函数：void PID_DeInit( PID_TypeDef	*p)
 * @描述：PID数据复位
 * @输入：*p：定义的PID数据
 * @返回：无 
 * @作者：何小P
 * @版本：V1.0
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
 * @函数：void PID_init ( PID_TypeDef *p)
 * @描述：PID参数计算
 * @输入：*p：定义的PID数据
 * @返回：无 
 * @作者：何小P
 * @版本：V1.0
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
 * @函数：pid_float32 PID_Calc( PID_TypeDef *p)
 * @描述：PID增量式结算	 
 * @输入：*p：PID结构体
 * @返回：PID运算结果	 
 * @作者：何小P
 * @版本：V1.0
 *******************************************************************************
 */
#pragma arm section code = "RAMCODE"
pid_float32 PID_Calc( PID_TypeDef *p, pid_float32 feedback, pid_float32 ref)
{
	p->Ek	= ref - feedback;		//计算误差

	p->Increment		= (  p->a0*p->Ek	- p->a1*p->Ek_1 + p->a2*p->Ek_2 );	//PID计算

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
 * @函数：void PID_Clean( PID_TypeDef	*p)
 * @描述：清楚PID计算中的缓存
 * @输入：*p：定义的PID数据
 * @返回：无 
 * @作者：何小P
 * @版本：V1.0
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
 * @函数：void PI_DeInit( PID_TypeDef	*p)
 * @描述：PI数据复位
 * @输入：*p：定义的PID数据
 * @返回：无 
 * @作者：何小P
 * @版本：V1.0
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
 * @函数：void PID_init ( PID_TypeDef *p)
 * @描述：PI参数计算
 * @输入：*p：定义的PID数据
 * @返回：无 
 * @作者：何小P
 * @版本：V1.0
 *******************************************************************************
 */
void PI_init ( PID_TypeDef *p)
{   

	p->a0		= p->Kp*(1 + 1.0*p->T/p->Ti + 1.0*p->Td/p->T);
	p->a1		= p->Kp*(1 + 2.0*p->Td/p->T);

}
/**
 *******************************************************************************
 * @函数：pid_float32 PID_Calc( PID_TypeDef *p)
 * @描述：PI增量式结算	 
 * @输入：*p：PID结构体
 * @返回：PID运算结果	 
 * @作者：何小P
 * @版本：V1.0
 *******************************************************************************
 */
#pragma arm section code = "RAMCODE"

pid_float32 PI_Calc( PID_TypeDef *p, pid_float32 feedback, pid_float32 ref)
{

	p->Ek	= ref - feedback;		//计算误差

	p->Increment		= (  p->a0*p->Ek	- p->a1*p->Ek_1 );	//PID计算

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
 * @函数：void PID_Clean( PID_TypeDef	*p)
 * @描述：清楚PID计算中的缓存
 * @输入：*pp：定义的PID数据
 * @返回：无 
 * @作者：何小P
 * @版本：V1.0
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




