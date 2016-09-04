/*
 * hp_buck-boost_control.c
 *
 *  Created on: 2015年8月20日
 *      Author: FlyerPower
 */



#include "hp_buck-boost_control.h"
#include "hp_pid_code.h"
#include "math.h"

#define CPU_CLK			90e6	//CPU主频
#define DeadTime		30		//90MHZ, delay time:ns
#define Fsw			150e3	//单位为HZ，PWM频率
#define VOLTAGE_MAX	64		//单位为V，输入输出电压最大电压
#define CURRENT_MAX	4.8		//单位为A，输入输出电压最大电流

#define nominal_voltage_max 	64		//额定电压,最大电压
#define nominal_voltage_min		5.0		//额定电压,最小电压
#define nominal_current			4.5		//额定电流


/*==============函数声明===============*/
Uint16 smps_pwm_init(Uint32 fPWM);
void smps_pwm_upate(float32 DutyA,float32 DutyB);
void smps_adc_init(void);
extern __interrupt void adc_isr(void);
extern __interrupt void ISR_EPWM1(void);
/*=======================================*/


float32	VIPWR;		//输入电压
float32	VOPWR;		//输出电压
float32	IIPWR;		//输入电流
float32	IOPWR;		//输出电流
float32	PWRIN;		//输入功率
float32	PWROUT;	//输出功率
float32	Efficiency;	//效率

PID_TypeDef pid_voltage_loop;	//电压环PID结构体
PID_TypeDef pid_current_loop;	//电流环PID结构体

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
	pid_current_loop.Kp		= 0.104;//临界= 0.22;
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
//	get_volt_and_current_adc();	//读取电压电流值


	vopwr_adc_value	+= AdcResult.ADCRESULT0;
	iopwr_adc_value	+= AdcResult.ADCRESULT1;	
	vipwr_adc_value	+= AdcResult.ADCRESULT2;
	iipwr_adc_value	+= AdcResult.ADCRESULT3;
	
	if(count%40 == 0)//电压环，5KHZ
	{
		VOPWR		= vopwr_adc_value/40.0*3.3/4095*29.55;
		VIPWR		= vipwr_adc_value/40.0*3.3/4095*29.80;
		PWRIN		= VIPWR * IIPWR;
		PWROUT	= VOPWR * IOPWR;
		Efficiency	= PWROUT/PWRIN;
		Vref		= Vout_value;
//		Vref		= temp_value > Vout_value ? Vout_value : (temp_value += Vout_value/1000.0);//软启动,设置PID的设置值			
		pid_voltage_loop.SetPoint 	= Vref;
		pid_voltage_loop.Feedback	= VIPWR;				//输出电压作为电压环反馈
		vopwr_adc_value		= 0;
		vipwr_adc_value		= 0;
		Iref  						= PID_Calc(&pid_voltage_loop);	//电压外环PID计算,输出作为电流环的输入

	}//	if(count%30 == 0)

	if(count%10 == 0)//电流环，20KHZ
	{
		IIPWR			= -(iipwr_adc_value/10.0*3.3/4095-1.65)/0.20;
		IOPWR			= -(iopwr_adc_value/10.0*3.3/4095-1.65)/0.18;
		pid_current_loop.SetPoint 			= Iref;
		pid_current_loop.Feedback		=  IIPWR;//fabs(IOPWR ) > fabs(IIPWR ) ? IOPWR : IIPWR;	//在输入输出中以较大的电流作为反馈	
		PWM_Boost_Duty 	= PID_Calc(&pid_current_loop);		//电流环PID

		smps_pwm_upate(0.5,1.0 - PWM_Boost_Duty);	//PWM占空比更新
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
 *	函数:	void Pwm_init(Uint16 fPWM)
 *	描述:	获取ADC读取电压电流值
 *	输入:	fPWM:   PWM的频率
 *	返回:	PWM时挤计数器值
 *	作者:	何小P
 *	版本:	V1.0 2015/7/1
 *******************************************************************************
 */
void get_volt_and_current_adc(void)
{
	


}

/*
 *******************************************************************************
 *	函数:	void Pwm_init(Uint16 fPWM)
 *	描述:	PWM初始化函数
 *	输入:	fPWM:   PWM的频率
 *	返回:	PWM时挤计数器值
 *	作者:	何小P
 *	版本:	V1.0 2015/7/1
 *******************************************************************************
 */
Uint16 smps_pwm_init(Uint32 fPWM)
{
/*	初始化PWM输出GPIO	*/
	InitEPwm1Gpio();
	InitEPwm2Gpio();
/*==========配置ePWM1=================*/
	EALLOW;		
	PieVectTable.EPWM1_INT	=	&ISR_EPWM1;				//中断服务函数入口地址
	EDIS;
/*	 配置定时器模块(TB)	*/
	EPwm1Regs.TBCTL.bit.PRDLD		= TB_IMMEDIATE; 			//TBPED寄存器采用立即装载模式
	EPwm1Regs.TBPRD 				= CPU_CLK/fPWM/2;         	// 设置PWM周期为2*(CPU_CLK/fPWM/2 + 1)个CBCLK个时钟周期
	EPwm1Regs.TBPHS.half.TBPHS		= 0;						// 清空相位寄存器
	EPwm1Regs.TBCTR 				= 0;						// 清空时间基准计数器

	EPwm1Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN; 	// 配置为向上向下计数模式
	EPwm1Regs.TBCTL.bit.PHSEN 		= TB_DISABLE;        		// 禁止相位控质，
	EPwm1Regs.TBCTL.bit.SYNCOSEL	= TB_CTR_ZERO;			//PWM1为主模块，在计数器为零时发出同步信号
	EPwm1Regs.TBCTL.bit.HSPCLKDIV	= TB_DIV1;				//设置分频器TBCLK = SYSCLKOUT / (HSPCLKDIV × CLKDIV)
	EPwm1Regs.TBCTL.bit.CLKDIV		= TB_DIV1;				//设置高速分频器TBCLK = SYSCLKOUT / (HSPCLKDIV × CLKDIV)



	EPwm1Regs.CMPCTL.bit.SHDWAMODE	= CC_SHADOW;		//设CMPA为映射模式
	EPwm1Regs.CMPCTL.bit.SHDWBMODE	= CC_SHADOW;		//设CMPB为映射模式
	EPwm1Regs.CMPCTL.bit.LOADAMODE	= CC_CTR_ZERO;		//在TBCTR = ZERO 时装载
	EPwm1Regs.CMPCTL.bit.LOADBMODE	= CC_CTR_ZERO;		//在TBCTR = ZERO 时装载
	
/*	配置比较方式预设模块(AQ)	*/
	EPwm1Regs.AQCTLA.bit.CAU			= AQ_SET;			// TBCTR = CAU,PWM1A输出高
	EPwm1Regs.AQCTLA.bit.CAD			= AQ_CLEAR;			// TBCTR = CAD,PWM1A输出低
	EPwm1Regs.AQCTLA.bit.ZRO			= AQ_CLEAR;			// TBCTR = ZRO,PWM1A输出低
	EPwm1Regs.AQCTLA.bit.PRD			= AQ_SET;			// TBCTR = PRD,PWM1A输出高

//	EPwm1Regs.AQCTLB.bit.CBU			= AQ_CLEAR;			// TBCTR = CAU,PWM1B输出低
//	EPwm1Regs.AQCTLB.bit.CBD    		= AQ_SET;			// TBCTR = CAD,PWM1B输出高
//	EPwm1Regs.AQCTLB.bit.ZRO			= AQ_SET;			// TBCTR = ZRO,PWM1B输出高
//	EPwm1Regs.AQCTLB.bit.PRD			= AQ_CLEAR;			// TBCTR = PRD,PWM1A输出低

/*	配置计数器比较模块(CC)	*/
	EPwm1Regs.CMPA.half.CMPA			= 0;						 //设置占空比CMPx/TBPRD 
//	EPwm1Regs.CMPB 					= EPwm1Regs.TBPRD;		//设置占空比CMPx/TBPRD 

/*	配置中断模块(ET)	*/
	EPwm1Regs.ETSEL.bit.INTSEL	= ET_CTR_ZERO;	// Select INT on Zero event
	EPwm1Regs.ETSEL.bit.INTEN	= 0;				// 使能中断位 1:使能，0:不使能
	EPwm1Regs.ETPS.bit.INTPRD	= ET_1ST;           	// Generate INT on 3rd event   
	EPwm1Regs.ETCLR.bit.INT		= 1;				//清除中断标志位

/*	配置死区模块(DB)	*/
	EPwm1Regs.DBCTL.bit.IN_MODE	= DBA_ALL;			//EPWMxA作为上升沿和下降沿的信号源
	EPwm1Regs.DBCTL.bit.OUT_MODE	= DB_FULL_ENABLE;	//使能上升沿和下降沿的死区时间
	EPwm1Regs.DBCTL.bit.POLSEL		= DB_ACTV_HIC;		//配置死区方式为AHC(高电平有效)
	EPwm1Regs.DBFED				= DeadTime;			//上升沿死区时间为50个TBCLK(1/90e6*10)
	EPwm1Regs.DBRED				= DeadTime;			//下降沿死区时间为50个TBCLK(1/90e6*10)

/*	配置高分辩PWM		*/
	EALLOW; 
	EPwm1Regs.HRCNFG.all			= 0x0;			// clear all bits first
	EPwm1Regs.HRCNFG.bit.EDGMODE	= HR_FEP;		// Control Falling Edge Position
	EPwm1Regs.HRCNFG.bit.CTLMODE	= HR_CMP;		// CMPAHR controls the MEP
	EPwm1Regs.HRCNFG.bit.HRLOAD	= HR_CTR_ZERO;	// Shadow load on CTR=Zero
	EDIS;

	
	IER |= M_INT3;			// 使能PIE 的INT3

	PieCtrlRegs.PIECTRL.bit.ENPIE	= 1;			// 使能PIE block
	PieCtrlRegs.PIEIER3.bit.INTx1	= 1; 		//使能INT3.1

	EINT;   // Enable Global interrupt INTM
	ERTM;   // Enable Global realtime interrupt DBGM 	
//==========配置ePWM2=================//
// 配置定时器模块(TB)
	EPwm2Regs.TBCTL.bit.PRDLD		= TB_IMMEDIATE; 			// TBPED寄存器采用立即装载模式
	EPwm2Regs.TBPRD				= EPwm1Regs.TBPRD;        // 周期与PWM1相同
	EPwm2Regs.TBPHS.half.TBPHS		= 0;						// 清空相位寄存器
	EPwm2Regs.TBCTR				= 0;						// 清空时间基准计数器

	EPwm2Regs.TBCTL.bit.CTRMODE 	= TB_COUNT_UPDOWN; 	// 配置为向上向下计数模式
	EPwm2Regs.TBCTL.bit.PHSEN 		= TB_ENABLE;        		// 使能相位装载，在同步信号中必须使能
	EPwm2Regs.TBCTL.bit.SYNCOSEL	= TB_SYNC_IN;			// 接收同步信号
	EPwm2Regs.TBCTL.bit.HSPCLKDIV	= TB_DIV1;				// 设置分频器TBCLK = SYSCLKOUT / (HSPCLKDIV × CLKDIV)
	EPwm2Regs.TBCTL.bit.CLKDIV		= TB_DIV1;				// 设置高速分频器TBCLK = SYSCLKOUT / (HSPCLKDIV × CLKDIV)



	EPwm2Regs.CMPCTL.bit.SHDWAMODE	= CC_SHADOW;		//设CMPA为映射模式
	EPwm2Regs.CMPCTL.bit.SHDWBMODE	= CC_SHADOW;		//设CMPB为映射模式
	EPwm2Regs.CMPCTL.bit.LOADAMODE	= CC_CTR_ZERO;		//在CTR = ZERO 时装载
	EPwm2Regs.CMPCTL.bit.LOADBMODE	= CC_CTR_ZERO;		//在CTR = ZERO 时装载
	

/*	配置比较方式预设模块(AQ)	*/
	EPwm2Regs.AQCTLA.bit.CAU			= AQ_SET;			// TBCTR = CAU,PWM2A输出低
	EPwm2Regs.AQCTLA.bit.CAD			= AQ_CLEAR;			// TBCTR = CAD,PWM2A输出高
	EPwm2Regs.AQCTLA.bit.ZRO			= AQ_CLEAR;			// TBCTR = ZRO,PWM2A输出高
	EPwm2Regs.AQCTLA.bit.PRD			= AQ_SET;			// TBCTR = PRD,PWM2A输出低

//	EPwm2Regs.AQCTLB.bit.CBU			= AQ_CLEAR;			// TBCTR = CAU,PWM1B输出低
//	EPwm2Regs.AQCTLB.bit.CBD		= AQ_SET;			// TBCTR = CAD,PWM1B输出高
//	EPwm2Regs.AQCTLB.bit.ZRO			= AQ_SET;			// TBCTR = ZRO,PWM1B输出高
//	EPwm2Regs.AQCTLB.bit.PRD			= AQ_CLEAR;			// TBCTR = PRD,PWM1B输出低

//配置计数器比较模块(CC)
	EPwm2Regs.CMPA.half.CMPA			= 0;					//设置占空比CMPx/TBPRD 
//	EPwm2Regs.CMPB 					= EPwm2Regs.TBPRD;		//设置占空比CMPx/TBPRD 
	
/*	配置死区模块(DB)	*/
	EPwm2Regs.DBCTL.bit.IN_MODE	= DBA_ALL;			//EPWMxA作为上升沿和下降沿的信号源
	EPwm2Regs.DBCTL.bit.OUT_MODE	= DB_FULL_ENABLE;	//使能上升沿和下降沿的死区时间
	EPwm2Regs.DBCTL.bit.POLSEL		= DB_ACTV_HIC;		//配置死区方式为AHC(高电平有效)
	EPwm2Regs.DBFED				= DeadTime;			//上升沿死区时间为50个TBCLK(1/90e6*10)
	EPwm2Regs.DBRED				= DeadTime;			//下降沿死区时间为50个TBCLK(1/90e6*10)

//配置中断模块(ET)
	EPwm2Regs.ETSEL.bit.INTSEL 	= ET_CTR_ZERO;      	// Select INT on Zero event
	EPwm2Regs.ETSEL.bit.INTEN 	= 0;                 		// 禁止中断
	EPwm2Regs.ETPS.bit.INTPRD 	= ET_1ST;            		// Generate INT on 3rd event 

/*	配置高分辩PWM		*/
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
 *	函数:	void ePWM_update(float32 DutyA,float32 DutyB)
 *	描述:  更新PWM4占空比
 *	输入:	DutyA:PWMxA占空比;DutyB:PWMxB占空比
 *	返回:	无
 *	作者:	何小P
 *	版本:	V1.0
 *******************************************************************************
 */
void smps_pwm_clear(void)
{

}
/*
 *******************************************************************************
 *	函数:	void ePWM_update(float32 DutyA,float32 DutyB)
 *	描述:  更新PWM4占空比
 *	输入:	DutyA:PWMxA占空比;DutyB:PWMxB占空比
 *	返回:	无
 *	作者:	何小P
 *	版本:	V1.0
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
 *	函数:	void smps_adc_init(void)
 *	描述:  片载adc初始化
 *	输入:	无
 *	返回:	无
 *	作者:	何小P
 *	版本:	V1.0
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
	AdcRegs.ADCCTL2.bit.CLKDIV2EN			= 1;//与CLKDIV4EN结合配置ADC时钟，ADC clock	=	CPU clock/2
	AdcRegs.ADCCTL2.bit.CLKDIV4EN			= 0;//与CLKDIV2EN结合配置ADC时钟，ADC clock	=	CPU clock/2
	AdcRegs.ADCCTL2.bit.ADCNONOVERLAP		= 0;// Enable non-overlap mode
	AdcRegs.ADCCTL1.bit.INTPULSEPOS			= 1;// ADCINT1 trips after AdcResults latch
	AdcRegs.INTSEL1N2.bit.INT1E				= 1;// Enabled ADCINT1
	AdcRegs.INTSEL1N2.bit.INT1CONT			= 0;// Disable ADCINT1 Continuous mode
	AdcRegs.INTSEL1N2.bit.INT1SEL				= 1;// setup EOC1 to trigger ADCINT1 to fire

	AdcRegs.ADCSOC0CTL.bit.CHSEL			= CH_ADC_VOUT;//设置转换通道为ADCINA0
	AdcRegs.ADCSOC1CTL.bit.CHSEL			= CH_ADC_IOUT;//设置转换通道为ADCINA1
	AdcRegs.ADCSOC2CTL.bit.CHSEL			= CH_ADC_VIN;//设置转换通道为ADCINB0
	AdcRegs.ADCSOC3CTL.bit.CHSEL			= CH_ADC_IIN;//设置转换通道为ADCINB1
	
	AdcRegs.ADCSOC0CTL.bit.TRIGSEL			= 5;//设置SOC0的触发源为EPWM1A
	AdcRegs.ADCSOC0CTL.bit.ACQPS			= 8;//设置SOC0的采样窗为(6+1)个周期
	AdcRegs.ADCSOC1CTL.bit.TRIGSEL			= 5;//设置SOC1的触发源为EPWM1A
	AdcRegs.ADCSOC1CTL.bit.ACQPS			= 8;//设置SOC1的采样窗为(6+1)个周期
	AdcRegs.ADCSOC2CTL.bit.TRIGSEL			= 5;//设置SOC0的触发源为EPWM1A
	AdcRegs.ADCSOC2CTL.bit.ACQPS			= 8;//设置SOC0的采样窗为(6+1)个周期
	AdcRegs.ADCSOC3CTL.bit.TRIGSEL			= 5;//设置SOC1的触发源为EPWM1A
	AdcRegs.ADCSOC3CTL.bit.ACQPS			= 8;//设置SOC1的采样窗为(6+1)个周期


//	AdcRegs.ADCSOC4CTL.bit.CHSEL			= 1;//设置SOC0的转换通道为ADCINA0
//	AdcRegs.ADCSOC4CTL.bit.TRIGSEL			= 5;//设置SOC0的触发源为EPWM1A
//	AdcRegs.ADCSOC4CTL.bit.ACQPS			= 8;//设置SOC0的采样窗为(6+1)个周期

//	AdcRegs.ADCSOC5CTL.bit.CHSEL			= 9;//设置SOC1的转换通道为ADCINB0
//	AdcRegs.ADCSOC5CTL.bit.TRIGSEL			= 5;//设置SOC1的触发源为EPWM1A
//	AdcRegs.ADCSOC5CTL.bit.ACQPS			= 8;//设置SOC1的采样窗为(6+1)个周期

//	AdcRegs.ADCSOC6CTL.bit.CHSEL			= 1;//设置SOC0的转换通道为ADCINA0
//	AdcRegs.ADCSOC6CTL.bit.TRIGSEL			= 5;//设置SOC0的触发源为EPWM1A
//	AdcRegs.ADCSOC6CTL.bit.ACQPS			= 8;//设置SOC0的采样窗为(6+1)个周期

//	AdcRegs.ADCSOC7CTL.bit.CHSEL			= 9;//设置SOC1的转换通道为ADCINB0
//	AdcRegs.ADCSOC7CTL.bit.TRIGSEL			= 5;//设置SOC1的触发源为EPWM1A
//	AdcRegs.ADCSOC7CTL.bit.ACQPS			= 8;//设置SOC1的采样窗为(6+1)个周期

//	AdcRegs.ADCSOC8CTL.bit.CHSEL			= 0;//设置SOC2的转换通道为ADCINA1
//	AdcRegs.ADCSOC8CTL.bit.TRIGSEL			= 5;//设置SOC2的触发源为EPWM1A
//	AdcRegs.ADCSOC8CTL.bit.ACQPS			= 8;//设置SOC2的采样窗为(6+1)个周期

//	AdcRegs.ADCSOC9CTL.bit.CHSEL			= 8;//设置SOC3的转换通道为ADCINB1
//	AdcRegs.ADCSOC9CTL.bit.TRIGSEL			= 5;//设置SOC3的触发源为EPWM1A
//	AdcRegs.ADCSOC9CTL.bit.ACQPS			= 8;//设置SOC3的采样窗为(6+1)个周期

//	AdcRegs.ADCSOC10CTL.bit.CHSEL			= 0;//设置SOC2的转换通道为ADCINA1
//	AdcRegs.ADCSOC10CTL.bit.TRIGSEL			= 5;//设置SOC2的触发源为EPWM1A
//	AdcRegs.ADCSOC10CTL.bit.ACQPS			= 8;//设置SOC2的采样窗为(6+1)个周期

//	AdcRegs.ADCSOC11CTL.bit.CHSEL			= 8;//设置SOC3的转换通道为ADCINB1
//	AdcRegs.ADCSOC11CTL.bit.TRIGSEL			= 5;//设置SOC3的触发源为EPWM1A
//	AdcRegs.ADCSOC11CTL.bit.ACQPS			= 8;//设置SOC3的采样窗为(6+1)个周期

//	AdcRegs.ADCSOC12CTL.bit.CHSEL			= 0;//设置SOC2的转换通道为ADCINA1
//	AdcRegs.ADCSOC12CTL.bit.TRIGSEL			= 5;//设置SOC2的触发源为EPWM1A
//	AdcRegs.ADCSOC12CTL.bit.ACQPS			= 8;//设置SOC2的采样窗为(6+1)个周期

//	AdcRegs.ADCSOC13CTL.bit.CHSEL			= 8;//设置SOC3的转换通道为ADCINB1
//	AdcRegs.ADCSOC13CTL.bit.TRIGSEL			= 5;//设置SOC3的触发源为EPWM1A
//	AdcRegs.ADCSOC13CTL.bit.ACQPS			= 8;//设置SOC3的采样窗为(6+1)个周期

//	AdcRegs.ADCSOC14CTL.bit.CHSEL			= 0;//设置SOC2的转换通道为ADCINA1
//	AdcRegs.ADCSOC14CTL.bit.TRIGSEL			= 5;//设置SOC2的触发源为EPWM1A
//	AdcRegs.ADCSOC14CTL.bit.ACQPS			= 8;//设置SOC2的采样窗为(6+1)个周期

//	AdcRegs.ADCSOC15CTL.bit.CHSEL			= 8;//设置SOC3的转换通道为ADCINB1
//	AdcRegs.ADCSOC15CTL.bit.TRIGSEL			= 5;//设置SOC3的触发源为EPWM1A
//	AdcRegs.ADCSOC15CTL.bit.ACQPS			= 8;//设置SOC3的采样窗为(6+1)个周期

	EDIS;

// Assumes ePWM1 clock is already enabled in InitSysCtrl();
	EPwm1Regs.ETSEL.bit.SOCAEN				= 1;				// 使能SOCA触发ADC转换
	EPwm1Regs.ETSEL.bit.SOCASEL				= ET_CTR_ZERO;	// 在CTR = PRD时触发
	EPwm1Regs.ETPS.bit.SOCAPRD				= 2;				// 每发生2次触发事件，产生一次ADC启动信号
	DSP28x_usDelay(1000L);		//延时
}




