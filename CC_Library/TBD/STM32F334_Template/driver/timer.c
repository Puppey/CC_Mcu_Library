

#include	"timer.h"

//========================================================================
// ����: void TIM3_init(void)   
// ����: TIM3��ʼ������
// ����: ��		
// ����: ��
// �汾: V1.0, 2013-7-36
// ���ߣ�����
//========================================================================
void TIMER3_init(uint32 nTimer)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period 					= nTimer; 	
//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ������nTimerΪnTimer us
	TIM_TimeBaseStructure.TIM_Prescaler 			=	63;	
//(64-1)����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  1MHZ�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision 	= 0; 										//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up;  	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 									//����TIM_TimeBaseInitStruct��ָ���Ĳ���
																																		//��ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  
		TIM3, 							//TIMx
		TIM_IT_Update,  		//TIM �����ж�Դ 
		ENABLE  						//ʹ��
		);									//ʹ�ܻ���ʧ��ָ����TIM�ж�
	
	NVIC_InitStructure.NVIC_IRQChannel 											= 	TIM3_IRQn;  			//TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 		= 	1;  							//��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 					= 	0;  							//�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd 									= 	ENABLE; 					//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  					//ʹ��TIMx����		
}



/*				[] END OF FILE 				*/


