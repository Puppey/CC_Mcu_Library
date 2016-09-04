

#include "adc.h"

void ADC_init(void)
{
 
  ADC_InitTypeDef          ADC_InitStructure;
  ADC_CommonInitTypeDef    ADC_CommonInitStructure;
  GPIO_InitTypeDef         GPIO_InitStructure;
  ADC_InjectedInitTypeDef  ADC_InjectedInitStruct;
  
  
  RCC_ADCCLKConfig(RCC_ADC12PLLCLK_Div1);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ADC12, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  

  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  

  ADC_VoltageRegulatorCmd(ADC1, ENABLE);
  
  Delay_ms(10);
  
  ADC_SelectCalibrationMode(ADC1, ADC_CalibrationMode_Single);
  ADC_StartCalibration(ADC1);
  
  while(ADC_GetCalibrationStatus(ADC1) != RESET );
  
  ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_Independent;                                                                    
  ADC_CommonInitStructure.ADC_Clock            = ADC_Clock_AsynClkMode;                    
  ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;             
  ADC_CommonInitStructure.ADC_DMAMode          = ADC_DMAMode_OneShot;                  
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = 0;          
  ADC_CommonInit(ADC1, &ADC_CommonInitStructure);
  
  ADC_InitStructure.ADC_ContinuousConvMode     = ADC_ContinuousConvMode_Disable;
  ADC_InitStructure.ADC_Resolution             = ADC_Resolution_12b; 
  ADC_InitStructure.ADC_ExternalTrigConvEvent  = ADC_ExternalTrigConvEvent_0;         
  ADC_InitStructure.ADC_ExternalTrigEventEdge  = ADC_ExternalTrigEventEdge_None;
  ADC_InitStructure.ADC_DataAlign              = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_OverrunMode            = ADC_OverrunMode_Disable;   
  ADC_InitStructure.ADC_AutoInjMode            = ADC_AutoInjec_Disable;  
  ADC_InitStructure.ADC_NbrOfRegChannel        = 1;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  ADC_InjectedInitStruct.ADC_ExternalTrigInjecConvEvent = ADC_ExternalTrigInjecConvEvent_9;
  ADC_InjectedInitStruct.ADC_ExternalTrigInjecEventEdge = ADC_ExternalTrigInjecEventEdge_RisingEdge;
  ADC_InjectedInitStruct.ADC_InjecSequence1             = ADC_InjectedChannel_1; /* to PA0 (Iout) */
  ADC_InjectedInitStruct.ADC_InjecSequence2             = ADC_InjectedChannel_2; /* to PA1 (Vout) */
  ADC_InjectedInitStruct.ADC_InjecSequence3             = ADC_InjectedChannel_3; /* to PA2 (Vext) */
  ADC_InjectedInitStruct.ADC_InjecSequence4             = ADC_InjectedChannel_4; /* to PA3 (Vin)  */
	ADC_InjectedInitStruct.ADC_NbrOfInjecChannel          = 4;
  ADC_InjectedInit(ADC1, &ADC_InjectedInitStruct);
  
  /* ADC1 channel2 configuration */ 
	ADC_InjectedChannelSampleTimeConfig(ADC1, ADC_Channel_1, ADC_SampleTime_2Cycles5); /* Iout  */
  ADC_InjectedChannelSampleTimeConfig(ADC1, ADC_Channel_2, ADC_SampleTime_2Cycles5); /* Vout  */
  ADC_InjectedChannelSampleTimeConfig(ADC1, ADC_Channel_3, ADC_SampleTime_2Cycles5); /* Vext  */
  ADC_InjectedChannelSampleTimeConfig(ADC1, ADC_Channel_4, ADC_SampleTime_2Cycles5); /* Vin   */
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  
  /* wait for ADRDY */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_RDY));   
  
  /* Start ADC1 Injected Conversions */ 
  ADC_StartInjectedConversion(ADC1);

}

