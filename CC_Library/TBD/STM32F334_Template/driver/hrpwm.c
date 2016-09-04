




#include "hrpwm.h"

HRTIM_BaseInitTypeDef 			HRTIM_BaseInitStructure; 

u32 HrPWM_CHA_Init(u32 fsw)
{

	GPIO_InitTypeDef GPIO_InitStructure;
 
  HRTIM_TimerCfgTypeDef 			HRTIM_TimerWaveStructure;
  HRTIM_TimerInitTypeDef 			HRTIM_TimerInitStructure;
  HRTIM_OutputCfgTypeDef 			HRTIM_TIM_OutputStructure;
  HRTIM_CompareCfgTypeDef 		HRTIM_CompareStructure;
	HRTIM_DeadTimeCfgTypeDef		HRTIM_DeadTimeStructure;
//	HRTIM_BurstModeCfgTypeDef		HRTIM_BurstStructure;
  HRTIM_ADCTriggerCfgTypeDef	HRTIM_ADCTrigStructure;
	NVIC_InitTypeDef    NVIC_InitStructure;
  /* ----------------------------*/
  /*			 配置PWM输出GPIO			 */
  /* ----------------------------*/	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
//--------------HRTIM_CHA1 == PA8; HRTIM_CHA2 == PA9--------------//
  GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8 | GPIO_Pin_9;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_13);//PA8复用
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_13);//PA9复用  

  
  /* ----------------------------*/
  /* HRTIM Global initialization */
  /* ----------------------------*/
  RCC_HRTIM1CLKConfig(RCC_HRTIM1CLK_PLLCLK);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_HRTIM1, ENABLE);
  HRTIM_DLLCalibrationStart(HRTIM1, HRTIM_CALIBRATIONRATE_14);

  while(HRTIM_GetCommonFlagStatus(HRTIM1, HRTIM_ISR_DLLRDY) == RESET);
  
  /* --------------------------------------------------- */
  /* TIMERD initialization: timer mode and PWM frequency */
  /* --------------------------------------------------- */
  HRTIM_TimerInitStructure.HalfModeEnable 	= HRTIM_HALFMODE_DISABLED;
  HRTIM_TimerInitStructure.StartOnSync 			= HRTIM_SYNCSTART_DISABLED;
  HRTIM_TimerInitStructure.ResetOnSync 			= HRTIM_SYNCRESET_DISABLED;
  HRTIM_TimerInitStructure.DACSynchro 			= HRTIM_DACSYNC_NONE;
  HRTIM_TimerInitStructure.PreloadEnable 		= HRTIM_PRELOAD_DISABLED;	//直接加载
  HRTIM_TimerInitStructure.UpdateGating 		= HRTIM_UPDATEGATING_INDEPENDENT;
  HRTIM_TimerInitStructure.BurstMode 				= HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
  HRTIM_TimerInitStructure.RepetitionUpdate = HRTIM_UPDATEONREPETITION_ENABLED;
  
  HRTIM_BaseInitStructure.Period 						= 512e6 / fsw;	// HRTIM周期寄存器, Period = fHRCK / fsw
  HRTIM_BaseInitStructure.RepetitionCounter = 9;   				// 1 ISR every 128 PWM periods
  HRTIM_BaseInitStructure.PrescalerRatio 		= HRTIM_PRESCALERRATIO_MUL4;	//HRTIM时钟倍频系数，512MHZ
  HRTIM_BaseInitStructure.Mode 							= HRTIM_MODE_CONTINOUS;   		//HETIM运行在连续模式       
  HRTIM_Waveform_Init(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, &HRTIM_BaseInitStructure, &HRTIM_TimerInitStructure);
  
  
  /* ------------------------------------------------ */
  /* TIMERD output and registers update configuration */
  /* ------------------------------------------------ */
  HRTIM_TimerWaveStructure.DeadTimeInsertion 			= HRTIM_TIMDEADTIMEINSERTION_ENABLED;		//死区时间：使能
  HRTIM_TimerWaveStructure.DelayedProtectionMode 	= HRTIM_TIMDELAYEDPROTECTION_DISABLED;	//延时保护：无
  HRTIM_TimerWaveStructure.FaultEnable 						= HRTIM_TIMFAULTENABLE_NONE;						//故障使能：无
  HRTIM_TimerWaveStructure.FaultLock 							= HRTIM_TIMFAULTLOCK_READWRITE;					//故障位	：可读写
  HRTIM_TimerWaveStructure.PushPull 							= HRTIM_TIMPUSHPULLMODE_DISABLED;
  HRTIM_TimerWaveStructure.ResetTrigger 					= HRTIM_TIMRESETTRIGGER_NONE;						//复位时间计数器：不可用
  HRTIM_TimerWaveStructure.ResetUpdate 						= HRTIM_TIMUPDATEONRESET_DISABLED;
  HRTIM_TimerWaveStructure.UpdateTrigger 					= HRTIM_TIMUPDATETRIGGER_NONE;
	HRTIM_WaveformTimerConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, &HRTIM_TimerWaveStructure);
  
  
  /* -------------------------------- */
  /* TD1 and TD2 waveform description */
  /* -------------------------------- */
  /* PWM on TD1, protected by Fault input */
  HRTIM_TIM_OutputStructure.Polarity 								= HRTIM_OUTPUTPOLARITY_HIGH; 					//极性控制位：正极性
  HRTIM_TIM_OutputStructure.SetSource 							= HRTIM_OUTPUTRESET_TIMCMP1; 					//PWM输出高的触发源：CMP1
  HRTIM_TIM_OutputStructure.ResetSource 						= HRTIM_OUTPUTRESET_TIMCMP3; 					//PWM输出低的触发源：CMP3
  HRTIM_TIM_OutputStructure.IdleMode 								= HRTIM_OUTPUTIDLEMODE_NONE;  				//输出不受空闲状态影响
  HRTIM_TIM_OutputStructure.IdleState 							= HRTIM_OUTPUTIDLESTATE_INACTIVE;   	//输出不受空闲状态影响   
  HRTIM_TIM_OutputStructure.FaultState 							= HRTIM_OUTPUTFAULTSTATE_NONE;  			//故障状态：PWM输出不受故障影响        
  HRTIM_TIM_OutputStructure.ChopperModeEnable 			= HRTIM_OUTPUTCHOPPERMODE_DISABLED; 	//不使用斩波器      
  HRTIM_TIM_OutputStructure.BurstModeEntryDelayed 	= HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;	//死区时间不插入空闲状态
  HRTIM_WaveformOutputConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_OUTPUT_TD1, &HRTIM_TIM_OutputStructure);

//  PWM配置成对称互补，不用设置TD2  //
//  PWM on TD2, not affected by Fault input //
//  HRTIM_TIM_OutputStructure.Polarity 								= HRTIM_OUTPUTPOLARITY_HIGH; 					//极性控制位：正极性
//  HRTIM_TIM_OutputStructure.SetSource 							= HRTIM_OUTPUTSET_TIMPER; 						//PWM输出高的触发源：PER 
//  HRTIM_TIM_OutputStructure.ResetSource 						= HRTIM_OUTPUTRESET_TIMCMP2; 					//PWM输出低的触发源：CMP2
//  HRTIM_TIM_OutputStructure.IdleMode 								= HRTIM_OUTPUTIDLEMODE_NONE;  				//输出不受空闲状态影响
//  HRTIM_TIM_OutputStructure.IdleState 							= HRTIM_OUTPUTIDLESTATE_INACTIVE;   	//输出不受空闲状态影响   
//  HRTIM_TIM_OutputStructure.FaultState 							= HRTIM_OUTPUTFAULTSTATE_NONE;  			//故障状态：PWM输出不受故障影响        
//  HRTIM_TIM_OutputStructure.ChopperModeEnable 			= HRTIM_OUTPUTCHOPPERMODE_DISABLED; 	//不使用斩波器      
//  HRTIM_TIM_OutputStructure.BurstModeEntryDelayed 	= HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;	//死区时间不插入空闲状态
//  HRTIM_WaveformOutputConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_OUTPUT_TD2, &HRTIM_TIM_OutputStructure);

  /* Set compare registers for duty cycle on TD1 */
  HRTIM_CompareStructure.AutoDelayedMode 		= HRTIM_AUTODELAYEDMODE_REGULAR;
  HRTIM_CompareStructure.AutoDelayedTimeout = 0;
  HRTIM_CompareStructure.CompareValue 			= (0.5 - 0.05) * HRTIM_BaseInitStructure.Period;//设定占空比
  HRTIM_WaveformCompareConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_1, &HRTIM_CompareStructure);

  HRTIM_CompareStructure.AutoDelayedMode 		= HRTIM_AUTODELAYEDMODE_REGULAR;
  HRTIM_CompareStructure.AutoDelayedTimeout = 0;
  HRTIM_CompareStructure.CompareValue 			= (0.5 + 0.05) * HRTIM_BaseInitStructure.Period;//设定占空比
  HRTIM_WaveformCompareConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_3, &HRTIM_CompareStructure);
	
  /* Set compare registers for duty cycle on TD2 */
  HRTIM_CompareStructure.AutoDelayedMode 		= HRTIM_AUTODELAYEDMODE_REGULAR;
  HRTIM_CompareStructure.AutoDelayedTimeout = 0;
  HRTIM_CompareStructure.CompareValue 			= 0.5 * HRTIM_BaseInitStructure.Period; //设定占空比
  HRTIM_WaveformCompareConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_2, &HRTIM_CompareStructure);


	/*	配置死区时间	*/

	HRTIM_DeadTimeStructure.Prescaler 			= 0x1;//死区时间预分频器，tDTG = (2^Prescaler)*(tHRTIM/8){(tHRTIM = 1/128MHZ}
  HRTIM_DeadTimeStructure.FallingLock 		= HRTIM_TIMDEADTIME_FALLINGLOCK_WRITE;			
  HRTIM_DeadTimeStructure.FallingSign 		= HRTIM_TIMDEADTIME_FALLINGSIGN_POSITIVE;		//下降沿加死区
  HRTIM_DeadTimeStructure.FallingSignLock = HRTIM_TIMDEADTIME_FALLINGSIGNLOCK_WRITE;
  HRTIM_DeadTimeStructure.FallingValue 		= 150;//设定死区时间值// tDT = DeadTime_Value * tDTG(DeadTime_Value最大为255)
  HRTIM_DeadTimeStructure.RisingLock 			= HRTIM_TIMDEADTIME_RISINGLOCK_WRITE;
  HRTIM_DeadTimeStructure.RisingSign 			= HRTIM_TIMDEADTIME_RISINGSIGN_POSITIVE;		//下降沿加死区
  HRTIM_DeadTimeStructure.RisingSignLock 	= HRTIM_TIMDEADTIME_RISINGSIGNLOCK_WRITE;
  HRTIM_DeadTimeStructure.RisingValue 		= 150;//设置上升沿死区时间值
  HRTIM_DeadTimeConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, &HRTIM_DeadTimeStructure);

  /* --------------------------*/
  /* ADC trigger initialization */
  /* --------------------------*/
  /* Set compare 3 registers for ADC trigger */
  HRTIM_CompareStructure.AutoDelayedMode    = HRTIM_AUTODELAYEDMODE_REGULAR;
  HRTIM_CompareStructure.AutoDelayedTimeout = 0;
  HRTIM_CompareStructure.CompareValue       = 0.5 * HRTIM_BaseInitStructure.Period;
  HRTIM_WaveformCompareConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_2, &HRTIM_CompareStructure);
  
  HRTIM_ADCTrigStructure.Trigger      = HRTIM_ADCTRIGGEREVENT13_TIMERA_CMP2;
  HRTIM_ADCTrigStructure.UpdateSource = HRTIM_ADCTRIGGERUPDATE_TIMER_A;
  HRTIM_ADCTriggerConfig(HRTIM1, HRTIM_ADCTRIGGER_2, &HRTIM_ADCTrigStructure);
  /* -------------------------*/
  /* Interrupt initialization */
  /* -------------------------*/
  /* Configure and enable HRTIM TIMERA interrupt channel in NVIC */
  NVIC_InitStructure.NVIC_IRQChannel 										= HRTIM1_TIMA_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 0;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* TIMER A issues an interrupt on each repetition event */
//  HRTIM_ITConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_TIM_IT_REP, ENABLE);
  /* ---------------*/
  /* HRTIM start-up */
  /* ---------------*/
  /* Enable HRTIM's outputs TD1 and TD2 */
  /* Note: it is necessary to enable also GPIOs to have outputs functional */
  HRTIM_WaveformOutputStart(HRTIM1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); 
  
  HRTIM_WaveformCounterStart(HRTIM1, HRTIM_TIMERID_TIMER_A);//启动HRTIM PWM模式下的CHA
	
	return HRTIM_BaseInitStructure.Period;
}

//#pragma arm section code = "RAM1CODE"
uint32 HrPWM_CHA_Update(uint32 Period, uint32 duty)
{

	HRTIM_SlaveSetCompare(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_1, (Period - duty)/2 );  
	HRTIM_SlaveSetCompare(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_COMPAREUNIT_3, (Period + duty)/2 );
	return duty;
}
//#pragma arm section

void HRTIM1_TIMA_IRQHandler(void)
{

	if( HRTIM_GetITStatus( HRTIM1,  HRTIM_TIMERINDEX_TIMER_A, HRTIM_TIM_FLAG_REP) != RESET)
	{
	
	}	

	HRTIM_ClearITPendingBit(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_TIM_FLAG_REP);
}
