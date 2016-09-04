/*
 * hp_buck-boost_control.c
 *
 *  Created on: 2015��8��20��
 *      Author: FlyerPower
 */



#include "hp_buck-boost_control.h"
#include "hp_pid_code.h"
#include "math.h"

#define CPU_CLK			90e6	//CPU��Ƶ
#define DeadTime		30		//90MHZ, delay time:ns
#define Fsw			150e3	//��λΪHZ��PWMƵ��
#define VOLTAGE_MAX	64		//��λΪV�����������ѹ����ѹ
#define CURRENT_MAX	4.8		//��λΪA�����������ѹ������

#define nominal_voltage_max 	64		//���ѹ,����ѹ
#define nominal_voltage_min		5.0		//���ѹ,��С��ѹ
#define nominal_current			4.5		//�����


/*==============��������===============*/
Uint16 smps_pwm_init(Uint32 fPWM);
void smps_pwm_upate(float32 DutyA,float32 DutyB);
void smps_adc_init(void);
extern __interrupt void adc_isr(void);
extern __interrupt void ISR_EPWM1(void);
/*=======================================*/


float32	VIPWR;		//�����ѹ
float32	VOPWR;		//�����ѹ
float32	IIPWR;		//�������
float32	IOPWR;		//�������
float32	PWRIN;		//���빦��
float32	PWROUT;	//�������
float32	Efficiency;	//Ч��

PID_TypeDef pid_voltage_loop;	//��ѹ��PID�ṹ��
PID_TypeDef pid_current_loop;	//������PID�ṹ��

void smps_init(void)
{
	PID_DeInit(&pid_voltage_loop);
	PID_DeInit(&pid_current_loop);

	
	pid_voltage_loop.T		= 0.20;
	pid_voltage_loop.Kp		= 0.35;
	pid_voltage_loop.Ti		= 3.8;
	pid_voltage_loop.Td		= 0;
	pid_voltage_loop.OutMin	= -CURRENT_MAX;
	pid_voltage_loop.OutMax	=   CURRENT_MAX;
	PID_init(&pid_voltage_loop);


	pid_current_loop.T		= 0.05;
	pid_current_loop.Kp		= 0.104;//�ٽ�= 0.22;
	pid_current_loop.Ti		= 0.55;
	pid_current_loop.Td		= 0;
	pid_current_loop.OutMin	= 0;
	pid_current_loop.OutMax	= 0.90;
	PID_init(&pid_current_loop);
	
	
	smps_pwm_init(Fsw);
	smps_adc_init();
	smps_pwm_upate(0.95,0);
}

float32 mppt_inccond(void);
float32 mppt_po(void);

void buck_boost_control(float32 Vout_value)
{
	static Uint32  count		= 0;
	static float32 Vref			= 0;
	static float32 Iref			= 0;
	static float32 temp_value	= 0;
	static Uint32 vipwr_adc_value	= 0;
	static Uint32 iipwr_adc_value	= 0;
	static Uint32 vopwr_adc_value	= 0;
	static Uint32 iopwr_adc_value	= 0;
	
	float32 PWM_Duty			= 0;
	float32 PWM_Boost_Duty	= 0;
	

	count++;
//	get_volt_and_current_adc();	//��ȡ��ѹ����ֵ


	vopwr_adc_value	+= AdcResult.ADCRESULT0;
	iopwr_adc_value	+= AdcResult.ADCRESULT1;	
	vipwr_adc_value	+= AdcResult.ADCRESULT2;
	iipwr_adc_value	+= AdcResult.ADCRESULT3;
	
	if(count%40 == 0)//��ѹ����5KHZ
	{
		VOPWR		= vopwr_adc_value/40.0*3.3/4095*29.55;
		VIPWR		= vipwr_adc_value/40.0*3.3/4095*29.80;
		PWRIN		= VIPWR * IIPWR;
		PWROUT	= VOPWR * IOPWR;
		Efficiency	= PWROUT/PWRIN;
		Vref		= Vout_value;
//		Vref		= temp_value > Vout_value ? Vout_value : (temp_value += Vout_value/1000.0);//������,����PID������ֵ			
		pid_voltage_loop.SetPoint 	= Vref;
		pid_voltage_loop.Feedback	= VIPWR;				//�����ѹ��Ϊ��ѹ������
		vopwr_adc_value		= 0;
		vipwr_adc_value		= 0;
		Iref  						= PID_Calc(&pid_voltage_loop);	//��ѹ�⻷PID����,�����Ϊ������������

	}//	if(count%30 == 0)

	if(count%10 == 0)//��������20KHZ
	{
		IIPWR			= -(iipwr_adc_value/10.0*3.3/4095-1.65)/0.20;
		IOPWR			= -(iopwr_adc_value/10.0*3.3/4095-1.65)/0.18;
		pid_current_loop.SetPoint 			= Iref;
		pid_current_loop.Feedback		=  IIPWR;//fabs(IOPWR ) > fabs(IIPWR ) ? IOPWR : IIPWR;	//������������Խϴ�ĵ�����Ϊ����	
		PWM_Boost_Duty 	= PID_Calc(&pid_current_loop);		//������PID

		smps_pwm_upate(0.5,1.0 - PWM_Boost_Duty);	//PWMռ�ձȸ���
		iopwr_adc_value		= 0;
		iipwr_adc_value		= 0;
	}	


}

#define MPPT_STEP	0.01
float32 mppt_inccond(void)
{
	float32 delta_i	= 0;
	float32 delta_v	= 0;
	float32 delta_p	= 0;
	float32 ineq		= 0;

	static float32 vinref		= 0;
		
	static float32 new_power 	= 0;
	static float32 last_power	= 0;
	static float32 last_iipwr	= 0;
	static float32 last_vipwr	= 0;
	new_power	= VIPWR * IIPWR;
	delta_p		= new_power - last_power;
	delta_i		= IIPWR - last_iipwr;
	delta_v		= VIPWR -last_vipwr;
	if(delta_v)
	{
		ineq = delta_p / delta_v;
		if(ineq > 0) vinref += MPPT_STEP; 
		else
		if(ineq < 0) vinref -= MPPT_STEP;
	} 
	else
	{
		if(delta_i > 0) vinref += MPPT_STEP; 
		else
		if(delta_i < 0) vinref -= MPPT_STEP;
	}
	last_iipwr 	= IIPWR;
	last_vipwr 	= VIPWR;
	last_power	= new_power;

	return vinref;

}
float32 mppt_po(void)
{
	float32 vinref		= 0;
		
	static float32 new_power 	= 0;
	static float32 last_power	= 0;
	
	new_power	= VIPWR * IIPWR;

	if(new_power < last_power) 
		vinref -= MPPT_STEP;
	else 
		vinref += MPPT_STEP;
	last_power	= new_power;
	return vinref;

}
/*
 *******************************************************************************
 *	����:	void Pwm_init(Uint16 fPWM)
 *	����:	��ȡADC��ȡ��ѹ����ֵ
 *	����:	fPWM:   PWM��Ƶ��
 *	����:	PWMʱ��������ֵ
 *	����:	��СP
 *	�汾:	V1.0 2015/7/1
 *******************************************************************************
 */
void get_volt_and_current_adc(void)
{
	


}

/*
 *******************************************************************************
 *	����:	void Pwm_init(Uint16 fPWM)
 *	����:	PWM��ʼ������
 *	����:	fPWM:   PWM��Ƶ��
 *	����:	PWMʱ��������ֵ
 *	����:	��СP
 *	�汾:	V1.0 2015/7/1
 *******************************************************************************
 */
Uint16 smps_pwm_init(Uint32 fPWM)
{
/*	��ʼ��PWM���GPIO	*/
	InitEPwm1Gpio();
	InitEPwm2Gpio();
/*==========����ePWM1=================*/
	EALLOW;		
	PieVectTable.EPWM1_INT	=	&ISR_EPWM1;				//�жϷ�������ڵ�ַ
	EDIS;
/*	 ���ö�ʱ��ģ��(TB)	*/
	EPwm1Regs.TBCTL.bit.PRDLD		= TB_IMMEDIATE; 			//TBPED�Ĵ�����������װ��ģʽ
	EPwm1Regs.TBPRD 				= CPU_CLK/fPWM/2;         	// ����PWM����Ϊ2*(CPU_CLK/fPWM/2 + 1)��CBCLK��ʱ������
	EPwm1Regs.TBPHS.half.TBPHS		= 0;						// �����λ�Ĵ���
	EPwm1Regs.TBCTR 				= 0;						// ���ʱ���׼������

	EPwm1Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN; 	// ����Ϊ�������¼���ģʽ
	EPwm1Regs.TBCTL.bit.PHSEN 		= TB_DISABLE;        		// ��ֹ��λ���ʣ�
	EPwm1Regs.TBCTL.bit.SYNCOSEL	= TB_CTR_ZERO;			//PWM1Ϊ��ģ�飬�ڼ�����Ϊ��ʱ����ͬ���ź�
	EPwm1Regs.TBCTL.bit.HSPCLKDIV	= TB_DIV1;				//���÷�Ƶ��TBCLK = SYSCLKOUT / (HSPCLKDIV �� CLKDIV)
	EPwm1Regs.TBCTL.bit.CLKDIV		= TB_DIV1;				//���ø��ٷ�Ƶ��TBCLK = SYSCLKOUT / (HSPCLKDIV �� CLKDIV)



	EPwm1Regs.CMPCTL.bit.SHDWAMODE	= CC_SHADOW;		//��CMPAΪӳ��ģʽ
	EPwm1Regs.CMPCTL.bit.SHDWBMODE	= CC_SHADOW;		//��CMPBΪӳ��ģʽ
	EPwm1Regs.CMPCTL.bit.LOADAMODE	= CC_CTR_ZERO;		//��TBCTR = ZERO ʱװ��
	EPwm1Regs.CMPCTL.bit.LOADBMODE	= CC_CTR_ZERO;		//��TBCTR = ZERO ʱװ��
	
/*	���ñȽϷ�ʽԤ��ģ��(AQ)	*/
	EPwm1Regs.AQCTLA.bit.CAU			= AQ_SET;			// TBCTR = CAU,PWM1A�����
	EPwm1Regs.AQCTLA.bit.CAD			= AQ_CLEAR;			// TBCTR = CAD,PWM1A�����
	EPwm1Regs.AQCTLA.bit.ZRO			= AQ_CLEAR;			// TBCTR = ZRO,PWM1A�����
	EPwm1Regs.AQCTLA.bit.PRD			= AQ_SET;			// TBCTR = PRD,PWM1A�����

//	EPwm1Regs.AQCTLB.bit.CBU			= AQ_CLEAR;			// TBCTR = CAU,PWM1B�����
//	EPwm1Regs.AQCTLB.bit.CBD    		= AQ_SET;			// TBCTR = CAD,PWM1B�����
//	EPwm1Regs.AQCTLB.bit.ZRO			= AQ_SET;			// TBCTR = ZRO,PWM1B�����
//	EPwm1Regs.AQCTLB.bit.PRD			= AQ_CLEAR;			// TBCTR = PRD,PWM1A�����

/*	���ü������Ƚ�ģ��(CC)	*/
	EPwm1Regs.CMPA.half.CMPA			= 0;						 //����ռ�ձ�CMPx/TBPRD 
//	EPwm1Regs.CMPB 					= EPwm1Regs.TBPRD;		//����ռ�ձ�CMPx/TBPRD 

/*	�����ж�ģ��(ET)	*/
	EPwm1Regs.ETSEL.bit.INTSEL	= ET_CTR_ZERO;	// Select INT on Zero event
	EPwm1Regs.ETSEL.bit.INTEN	= 0;				// ʹ���ж�λ 1:ʹ�ܣ�0:��ʹ��
	EPwm1Regs.ETPS.bit.INTPRD	= ET_1ST;           	// Generate INT on 3rd event   
	EPwm1Regs.ETCLR.bit.INT		= 1;				//����жϱ�־λ

/*	��������ģ��(DB)	*/
	EPwm1Regs.DBCTL.bit.IN_MODE	= DBA_ALL;			//EPWMxA��Ϊ�����غ��½��ص��ź�Դ
	EPwm1Regs.DBCTL.bit.OUT_MODE	= DB_FULL_ENABLE;	//ʹ�������غ��½��ص�����ʱ��
	EPwm1Regs.DBCTL.bit.POLSEL		= DB_ACTV_HIC;		//����������ʽΪAHC(�ߵ�ƽ��Ч)
	EPwm1Regs.DBFED				= DeadTime;			//����������ʱ��Ϊ50��TBCLK(1/90e6*10)
	EPwm1Regs.DBRED				= DeadTime;			//�½�������ʱ��Ϊ50��TBCLK(1/90e6*10)

/*	���ø߷ֱ�PWM		*/
	EALLOW; 
	EPwm1Regs.HRCNFG.all			= 0x0;			// clear all bits first
	EPwm1Regs.HRCNFG.bit.EDGMODE	= HR_FEP;		// Control Falling Edge Position
	EPwm1Regs.HRCNFG.bit.CTLMODE	= HR_CMP;		// CMPAHR controls the MEP
	EPwm1Regs.HRCNFG.bit.HRLOAD	= HR_CTR_ZERO;	// Shadow load on CTR=Zero
	EDIS;

	
	IER |= M_INT3;			// ʹ��PIE ��INT3

	PieCtrlRegs.PIECTRL.bit.ENPIE	= 1;			// ʹ��PIE block
	PieCtrlRegs.PIEIER3.bit.INTx1	= 1; 		//ʹ��INT3.1

	EINT;   // Enable Global interrupt INTM
	ERTM;   // Enable Global realtime interrupt DBGM 	
//==========����ePWM2=================//
// ���ö�ʱ��ģ��(TB)
	EPwm2Regs.TBCTL.bit.PRDLD		= TB_IMMEDIATE; 			// TBPED�Ĵ�����������װ��ģʽ
	EPwm2Regs.TBPRD				= EPwm1Regs.TBPRD;        // ������PWM1��ͬ
	EPwm2Regs.TBPHS.half.TBPHS		= 0;						// �����λ�Ĵ���
	EPwm2Regs.TBCTR				= 0;						// ���ʱ���׼������

	EPwm2Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN; 	// ����Ϊ�������¼���ģʽ
	EPwm2Regs.TBCTL.bit.PHSEN 		= TB_ENABLE;        		// ʹ����λװ�أ���ͬ���ź��б���ʹ��
	EPwm2Regs.TBCTL.bit.SYNCOSEL	= TB_SYNC_IN;			// ����ͬ���ź�
	EPwm2Regs.TBCTL.bit.HSPCLKDIV	= TB_DIV1;				// ���÷�Ƶ��TBCLK = SYSCLKOUT / (HSPCLKDIV �� CLKDIV)
	EPwm2Regs.TBCTL.bit.CLKDIV		= TB_DIV1;				// ���ø��ٷ�Ƶ��TBCLK = SYSCLKOUT / (HSPCLKDIV �� CLKDIV)



	EPwm2Regs.CMPCTL.bit.SHDWAMODE	= CC_SHADOW;		//��CMPAΪӳ��ģʽ
	EPwm2Regs.CMPCTL.bit.SHDWBMODE	= CC_SHADOW;		//��CMPBΪӳ��ģʽ
	EPwm2Regs.CMPCTL.bit.LOADAMODE	= CC_CTR_ZERO;		//��CTR = ZERO ʱװ��
	EPwm2Regs.CMPCTL.bit.LOADBMODE	= CC_CTR_ZERO;		//��CTR = ZERO ʱװ��
	

/*	���ñȽϷ�ʽԤ��ģ��(AQ)	*/
	EPwm2Regs.AQCTLA.bit.CAU			= AQ_SET;			// TBCTR = CAU,PWM2A�����
	EPwm2Regs.AQCTLA.bit.CAD			= AQ_CLEAR;			// TBCTR = CAD,PWM2A�����
	EPwm2Regs.AQCTLA.bit.ZRO			= AQ_CLEAR;			// TBCTR = ZRO,PWM2A�����
	EPwm2Regs.AQCTLA.bit.PRD			= AQ_SET;			// TBCTR = PRD,PWM2A�����

//	EPwm2Regs.AQCTLB.bit.CBU			= AQ_CLEAR;			// TBCTR = CAU,PWM1B�����
//	EPwm2Regs.AQCTLB.bit.CBD		= AQ_SET;			// TBCTR = CAD,PWM1B�����
//	EPwm2Regs.AQCTLB.bit.ZRO			= AQ_SET;			// TBCTR = ZRO,PWM1B�����
//	EPwm2Regs.AQCTLB.bit.PRD			= AQ_CLEAR;			// TBCTR = PRD,PWM1B�����

//���ü������Ƚ�ģ��(CC)
	EPwm2Regs.CMPA.half.CMPA			= 0;					//����ռ�ձ�CMPx/TBPRD 
//	EPwm2Regs.CMPB 					= EPwm2Regs.TBPRD;		//����ռ�ձ�CMPx/TBPRD 
	
/*	��������ģ��(DB)	*/
	EPwm2Regs.DBCTL.bit.IN_MODE	= DBA_ALL;			//EPWMxA��Ϊ�����غ��½��ص��ź�Դ
	EPwm2Regs.DBCTL.bit.OUT_MODE	= DB_FULL_ENABLE;	//ʹ�������غ��½��ص�����ʱ��
	EPwm2Regs.DBCTL.bit.POLSEL		= DB_ACTV_HIC;		//����������ʽΪAHC(�ߵ�ƽ��Ч)
	EPwm2Regs.DBFED				= DeadTime;			//����������ʱ��Ϊ50��TBCLK(1/90e6*10)
	EPwm2Regs.DBRED				= DeadTime;			//�½�������ʱ��Ϊ50��TBCLK(1/90e6*10)

//�����ж�ģ��(ET)
	EPwm2Regs.ETSEL.bit.INTSEL 	= ET_CTR_ZERO;      	// Select INT on Zero event
	EPwm2Regs.ETSEL.bit.INTEN 	= 0;                 		// ��ֹ�ж�
	EPwm2Regs.ETPS.bit.INTPRD 	= ET_1ST;            		// Generate INT on 3rd event 

/*	���ø߷ֱ�PWM		*/
	EALLOW; 
	EPwm2Regs.HRCNFG.all			= 0x0;			// clear all bits first
	EPwm2Regs.HRCNFG.bit.EDGMODE	= HR_FEP;		// Control Falling Edge Position
	EPwm2Regs.HRCNFG.bit.CTLMODE	= HR_CMP;		// CMPAHR controls the MEP
	EPwm2Regs.HRCNFG.bit.HRLOAD	= HR_CTR_ZERO;	// Shadow load on CTR=Zero
	EDIS;
	
	return (Uint16)(EPwm1Regs.TBPRD);
}
/*
 *******************************************************************************
 *	����:	void ePWM_update(float32 DutyA,float32 DutyB)
 *	����:  ����PWM4ռ�ձ�
 *	����:	DutyA:PWMxAռ�ձ�;DutyB:PWMxBռ�ձ�
 *	����:	��
 *	����:	��СP
 *	�汾:	V1.0
 *******************************************************************************
 */
void smps_pwm_clear(void)
{

}
/*
 *******************************************************************************
 *	����:	void ePWM_update(float32 DutyA,float32 DutyB)
 *	����:  ����PWM4ռ�ձ�
 *	����:	DutyA:PWMxAռ�ձ�;DutyB:PWMxBռ�ձ�
 *	����:	��
 *	����:	��СP
 *	�汾:	V1.0
 *******************************************************************************
 */

float32 temp_dutyA, temp_dutyB;
void smps_pwm_upate(float32 DutyA,float32 DutyB)
{

//	DutyA	= DutyA < 0.10 ? 0.10 : DutyA;
	DutyA	= DutyA > 0.90 ? 0.90 : DutyA;
//	DutyB	= DutyB < 0.10 ? 0.10 : DutyB;
	DutyB	= DutyB > 0.90 ? 0.90 : DutyB;


	 temp_dutyA	= DutyA;
	 temp_dutyB	= DutyB;
	 
	EPwm1Regs.CMPA.half.CMPA			= (Uint16)(EPwm1Regs.TBPRD * DutyA);
	EPwm1Regs.CMPA.half.CMPAHR			= (Uint16)((EPwm1Regs.TBPRD * DutyA -(int32)(EPwm1Regs.TBPRD * DutyA)) * 61 + 0.5)<<8;

	EPwm2Regs.CMPA.half.CMPA			= (Uint16)(EPwm2Regs.TBPRD * DutyB);
	EPwm2Regs.CMPA.half.CMPAHR			= (Uint16)((EPwm2Regs.TBPRD * DutyB -(int32)(EPwm2Regs.TBPRD * DutyB)) * 61 + 0.5)<<8;
}
/*
 *******************************************************************************
 *	����:	void smps_adc_init(void)
 *	����:  Ƭ��adc��ʼ��
 *	����:	��
 *	����:	��
 *	����:	��СP
 *	�汾:	V1.0
 *******************************************************************************
 */
 #define CH_ADC_VOUT	0
 #define CH_ADC_VIN		8
 #define CH_ADC_IOUT	1
 #define CH_ADC_IIN		9
 
void smps_adc_init(void)
{
    	extern void DSP28x_usDelay(Uint32 Count);
	EALLOW;  // This is needed to write to EALLOW protected register
	PieVectTable.ADCINT1 = &adc_isr;
	EDIS;    // This is needed to disable write to EALLOW protected registers

	InitAdc();  // For this example, init the ADC
//	AdcOffsetSelfCal();
	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;	// Enable INT 1.1 in the PIE
	IER |= M_INT1; 					// Enable CPU Interrupt 1
	EINT;          						// Enable Global interrupt INTM
	ERTM;          						// Enable Global realtime interrupt DBGM
// Configure ADC
	EALLOW;
	AdcRegs.ADCCTL2.bit.CLKDIV2EN			= 1;//��CLKDIV4EN�������ADCʱ�ӣ�ADC clock	=	CPU clock/2
	AdcRegs.ADCCTL2.bit.CLKDIV4EN			= 0;//��CLKDIV2EN�������ADCʱ�ӣ�ADC clock	=	CPU clock/2
	AdcRegs.ADCCTL2.bit.ADCNONOVERLAP		= 0;// Enable non-overlap mode
	AdcRegs.ADCCTL1.bit.INTPULSEPOS			= 1;// ADCINT1 trips after AdcResults latch
	AdcRegs.INTSEL1N2.bit.INT1E				= 1;// Enabled ADCINT1
	AdcRegs.INTSEL1N2.bit.INT1CONT			= 0;// Disable ADCINT1 Continuous mode
	AdcRegs.INTSEL1N2.bit.INT1SEL				= 1;// setup EOC1 to trigger ADCINT1 to fire

	AdcRegs.ADCSOC0CTL.bit.CHSEL			= CH_ADC_VOUT;//����ת��ͨ��ΪADCINA0
	AdcRegs.ADCSOC1CTL.bit.CHSEL			= CH_ADC_IOUT;//����ת��ͨ��ΪADCINA1
	AdcRegs.ADCSOC2CTL.bit.CHSEL			= CH_ADC_VIN;//����ת��ͨ��ΪADCINB0
	AdcRegs.ADCSOC3CTL.bit.CHSEL			= CH_ADC_IIN;//����ת��ͨ��ΪADCINB1
	
	AdcRegs.ADCSOC0CTL.bit.TRIGSEL			= 5;//����SOC0�Ĵ���ԴΪEPWM1A
	AdcRegs.ADCSOC0CTL.bit.ACQPS			= 8;//����SOC0�Ĳ�����Ϊ(6+1)������
	AdcRegs.ADCSOC1CTL.bit.TRIGSEL			= 5;//����SOC1�Ĵ���ԴΪEPWM1A
	AdcRegs.ADCSOC1CTL.bit.ACQPS			= 8;//����SOC1�Ĳ�����Ϊ(6+1)������
	AdcRegs.ADCSOC2CTL.bit.TRIGSEL			= 5;//����SOC0�Ĵ���ԴΪEPWM1A
	AdcRegs.ADCSOC2CTL.bit.ACQPS			= 8;//����SOC0�Ĳ�����Ϊ(6+1)������
	AdcRegs.ADCSOC3CTL.bit.TRIGSEL			= 5;//����SOC1�Ĵ���ԴΪEPWM1A
	AdcRegs.ADCSOC3CTL.bit.ACQPS			= 8;//����SOC1�Ĳ�����Ϊ(6+1)������


//	AdcRegs.ADCSOC4CTL.bit.CHSEL			= 1;//����SOC0��ת��ͨ��ΪADCINA0
//	AdcRegs.ADCSOC4CTL.bit.TRIGSEL			= 5;//����SOC0�Ĵ���ԴΪEPWM1A
//	AdcRegs.ADCSOC4CTL.bit.ACQPS			= 8;//����SOC0�Ĳ�����Ϊ(6+1)������

//	AdcRegs.ADCSOC5CTL.bit.CHSEL			= 9;//����SOC1��ת��ͨ��ΪADCINB0
//	AdcRegs.ADCSOC5CTL.bit.TRIGSEL			= 5;//����SOC1�Ĵ���ԴΪEPWM1A
//	AdcRegs.ADCSOC5CTL.bit.ACQPS			= 8;//����SOC1�Ĳ�����Ϊ(6+1)������

//	AdcRegs.ADCSOC6CTL.bit.CHSEL			= 1;//����SOC0��ת��ͨ��ΪADCINA0
//	AdcRegs.ADCSOC6CTL.bit.TRIGSEL			= 5;//����SOC0�Ĵ���ԴΪEPWM1A
//	AdcRegs.ADCSOC6CTL.bit.ACQPS			= 8;//����SOC0�Ĳ�����Ϊ(6+1)������

//	AdcRegs.ADCSOC7CTL.bit.CHSEL			= 9;//����SOC1��ת��ͨ��ΪADCINB0
//	AdcRegs.ADCSOC7CTL.bit.TRIGSEL			= 5;//����SOC1�Ĵ���ԴΪEPWM1A
//	AdcRegs.ADCSOC7CTL.bit.ACQPS			= 8;//����SOC1�Ĳ�����Ϊ(6+1)������

//	AdcRegs.ADCSOC8CTL.bit.CHSEL			= 0;//����SOC2��ת��ͨ��ΪADCINA1
//	AdcRegs.ADCSOC8CTL.bit.TRIGSEL			= 5;//����SOC2�Ĵ���ԴΪEPWM1A
//	AdcRegs.ADCSOC8CTL.bit.ACQPS			= 8;//����SOC2�Ĳ�����Ϊ(6+1)������

//	AdcRegs.ADCSOC9CTL.bit.CHSEL			= 8;//����SOC3��ת��ͨ��ΪADCINB1
//	AdcRegs.ADCSOC9CTL.bit.TRIGSEL			= 5;//����SOC3�Ĵ���ԴΪEPWM1A
//	AdcRegs.ADCSOC9CTL.bit.ACQPS			= 8;//����SOC3�Ĳ�����Ϊ(6+1)������

//	AdcRegs.ADCSOC10CTL.bit.CHSEL			= 0;//����SOC2��ת��ͨ��ΪADCINA1
//	AdcRegs.ADCSOC10CTL.bit.TRIGSEL			= 5;//����SOC2�Ĵ���ԴΪEPWM1A
//	AdcRegs.ADCSOC10CTL.bit.ACQPS			= 8;//����SOC2�Ĳ�����Ϊ(6+1)������

//	AdcRegs.ADCSOC11CTL.bit.CHSEL			= 8;//����SOC3��ת��ͨ��ΪADCINB1
//	AdcRegs.ADCSOC11CTL.bit.TRIGSEL			= 5;//����SOC3�Ĵ���ԴΪEPWM1A
//	AdcRegs.ADCSOC11CTL.bit.ACQPS			= 8;//����SOC3�Ĳ�����Ϊ(6+1)������

//	AdcRegs.ADCSOC12CTL.bit.CHSEL			= 0;//����SOC2��ת��ͨ��ΪADCINA1
//	AdcRegs.ADCSOC12CTL.bit.TRIGSEL			= 5;//����SOC2�Ĵ���ԴΪEPWM1A
//	AdcRegs.ADCSOC12CTL.bit.ACQPS			= 8;//����SOC2�Ĳ�����Ϊ(6+1)������

//	AdcRegs.ADCSOC13CTL.bit.CHSEL			= 8;//����SOC3��ת��ͨ��ΪADCINB1
//	AdcRegs.ADCSOC13CTL.bit.TRIGSEL			= 5;//����SOC3�Ĵ���ԴΪEPWM1A
//	AdcRegs.ADCSOC13CTL.bit.ACQPS			= 8;//����SOC3�Ĳ�����Ϊ(6+1)������

//	AdcRegs.ADCSOC14CTL.bit.CHSEL			= 0;//����SOC2��ת��ͨ��ΪADCINA1
//	AdcRegs.ADCSOC14CTL.bit.TRIGSEL			= 5;//����SOC2�Ĵ���ԴΪEPWM1A
//	AdcRegs.ADCSOC14CTL.bit.ACQPS			= 8;//����SOC2�Ĳ�����Ϊ(6+1)������

//	AdcRegs.ADCSOC15CTL.bit.CHSEL			= 8;//����SOC3��ת��ͨ��ΪADCINB1
//	AdcRegs.ADCSOC15CTL.bit.TRIGSEL			= 5;//����SOC3�Ĵ���ԴΪEPWM1A
//	AdcRegs.ADCSOC15CTL.bit.ACQPS			= 8;//����SOC3�Ĳ�����Ϊ(6+1)������

	EDIS;

// Assumes ePWM1 clock is already enabled in InitSysCtrl();
	EPwm1Regs.ETSEL.bit.SOCAEN				= 1;				// ʹ��SOCA����ADCת��
	EPwm1Regs.ETSEL.bit.SOCASEL				= ET_CTR_ZERO;	// ��CTR = PRDʱ����
	EPwm1Regs.ETPS.bit.SOCAPRD				= 2;				// ÿ����2�δ����¼�������һ��ADC�����ź�
	DSP28x_usDelay(1000L);		//��ʱ
}




