

#include	"timer.h"

//========================================================================
// 函数: void TIM3_init(void)   
// 描述: TIM3初始化函数
// 参数: 无		
// 返回: 无
// 版本: V1.0, 2013-7-36
// 作者：何鹏
//========================================================================
void TIMER3_init(uint32 nTimer)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_TimeBaseStructure.TIM_Period 					= nTimer; 	
//设置在下一个更新事件装入活动的自动重装载寄存器周期的值计数到nTimer为nTimer us
	TIM_TimeBaseStructure.TIM_Prescaler 			=	63;	
//(64-1)设置用来作为TIMx时钟频率除数的预分频值  1MHZ的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision 	= 0; 										//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up;  	//TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 									//根据TIM_TimeBaseInitStruct中指定的参数
																																		//初始化TIMx的时间基数单位
 
	TIM_ITConfig(  
		TIM3, 							//TIMx
		TIM_IT_Update,  		//TIM 触发中断源 
		ENABLE  						//使能
		);									//使能或者失能指定的TIM中断
	
	NVIC_InitStructure.NVIC_IRQChannel 											= 	TIM3_IRQn;  			//TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 		= 	1;  							//抢占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 					= 	0;  							//从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd 									= 	ENABLE; 					//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  					//使能TIMx外设		
}



/*				[] END OF FILE 				*/


