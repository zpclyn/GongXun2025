#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <math.h>
#include "Arm.h"
#include "Data.h"

/*
 *函数简介:爪臂初始化
 *参数说明:无
 *返回类型:无
 *备注:初始化TIM7定时中断,用以处理Yaw轴舵机正弦加减速
 */
void Arm_Init(void)
{
	/*===============配置时钟===============*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);
		
	/*===============配置定时器===============*/
	TIM_InternalClockConfig(TIM7);//选择时基单元的时钟
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//配置时基单元（配置参数）
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//配置时钟分频为1分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//配置计数器模式为向上计数
	TIM_TimeBaseInitStructure.TIM_Period=42-1;//配置自动重装值ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler=1000-1;//配置分频值PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//配置重复计数单元的置为0
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);//初始化TIM6

	TIM_ClearFlag(TIM7,TIM_FLAG_Update);//清除配置时基单元产生的中断标志位

	/*===============定时器中断===============*/
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);//使能更新中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);//选择NVIC分组
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn;//选择TIM6中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//使能中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;//响应优先级
	NVIC_Init(&NVIC_InitStructure);//初始化USART6的NVIC
	
	/*===============使能===============*/
	TIM_Cmd(TIM7,ENABLE);//启动定时器

	Steer_Init();
}

float Arm_Yaw_NowDuty,Arm_Yaw_TargetDuty;//Yaw轴舵机当前占空比以及目标占空比用以正弦加减速
uint8_t Arm_ChangeFlag=0;//Yaw轴舵机占空比变化标志位 0-未变化 1-目标占空比发生变化

/*
 *函数简介:Yaw轴设置
 *参数说明:YawStatus		Yaw舵机状态 OUT-伸出来 Back-回去
 *返回类型:无
 *备注:上电第一次设置没有加减速,之后每次设置都会启用正弦加减速
 */
void Arm_YawSet(Arm_YawStatus YawStatus)
{
	static uint8_t Start=1;
	
	if(Start==1)
	{
		switch(YawStatus)
		{
			case OUT:	Steer_SetDuty(Arm_Yaw,5.7f);Arm_Yaw_NowDuty=5.7f;break;		//伸出来
			case Back:	Steer_SetDuty(Arm_Yaw,12.5f);Arm_Yaw_NowDuty=12.5f;break;	//回去
		}
		Start=0;
	}
	else
	{
		switch(YawStatus)
		{
			case OUT:	Arm_Yaw_TargetDuty=5.7f;break;	//伸出来
			case Back:	Arm_Yaw_TargetDuty=12.5f;break;	//回去
		}
		Arm_ChangeFlag=1;
	}
}

/*
 *函数简介:Yaw舵机加减速控制
 *参数说明:无
 *返回类型:无
 *备注:在TIM7定时中断调用,用以Yaw舵机正弦加减速
 */
void Arm_Yaw_SINAccelControl(void)
{
	#define K	350.0f		//加减速过渡时间 单位ms
	static uint16_t Count=0;
	
	if(Arm_ChangeFlag==1)
	{
		Steer_SetDuty(Arm_Yaw,Arm_Yaw_NowDuty+(Arm_Yaw_TargetDuty-Arm_Yaw_NowDuty)*(0.5f-0.5f*cosf(PI/K*Count)));
		
		Count++;
		if(Count>K)
		{
			Count=0;
			Arm_ChangeFlag=0;
			Arm_Yaw_NowDuty=Arm_Yaw_TargetDuty;
		}
	}
}

float Arm_Store_NowDuty;//物料盘当前占空比
FunctionalState Arm_StoreCarSexENABLE=DISABLE;//物料盘震颤使能标志位

/*
 *函数简介:物料盘设置
 *参数说明:Color		物料盘颜色设置
 *返回类型:无
 *备注:无
 */
void Arm_StoreSet(zpc_zxc_Color Color)
{
	switch(Color)
	{
		case Red:	Arm_Store_NowDuty=3.0f+0.25f;Steer_SetDuty(Arm_Store,Arm_Store_NowDuty);break;	//红
		case Green:	Arm_Store_NowDuty=7.45f+0.25f;Steer_SetDuty(Arm_Store,Arm_Store_NowDuty);break;	//绿
		case Blue:	Arm_Store_NowDuty=11.95f+0.25f;Steer_SetDuty(Arm_Store,Arm_Store_NowDuty);break;	//蓝
	}
}

/*
 *函数简介:物料盘震颤使能
 *参数说明:NewState		使能标志位
 *返回类型:无
 *备注:无
 */
void Arm_StoreGSCarSex_ENABLE(FunctionalState NewState)
{
	Arm_StoreCarSexENABLE=NewState;
}

/*
 *函数简介:物料盘震颤控制
 *参数说明:无
 *返回类型:无
 *备注:无
 */
void Arm_StoreGSCarSex_Control(void)
{
	#define CarSex_A		1.0f
	#define CarSex_T		200
	#define CarSex_Count	100
	
	static uint16_t Count=0;
	if(Arm_StoreCarSexENABLE==DISABLE){Count=0;return;}
	Count++;
	
	if(Count%CarSex_T==0)
	{
		if(Arm_Store_NowDuty<4.0f)Steer_SetDuty(Arm_Store,Arm_Store_NowDuty+CarSex_A);
		else if(Arm_Store_NowDuty>11.0f)Steer_SetDuty(Arm_Store,Arm_Store_NowDuty+0.0f);
		else Steer_SetDuty(Arm_Store,Arm_Store_NowDuty+CarSex_A/2.0f);
	}
	else if(Count%(CarSex_T/2)==0)
	{
		if(Arm_Store_NowDuty<4.0f)Steer_SetDuty(Arm_Store,Arm_Store_NowDuty-0.0f);
		else if(Arm_Store_NowDuty>11.0f)Steer_SetDuty(Arm_Store,Arm_Store_NowDuty-CarSex_A);
		else Steer_SetDuty(Arm_Store,Arm_Store_NowDuty-CarSex_A/2.0f);
	}

	if(Count>CarSex_T*CarSex_Count)
	{
		Count=0;
		Arm_StoreCarSexENABLE=DISABLE;
		Steer_SetDuty(Arm_Store,Arm_Store_NowDuty);
	}
}

/*
 *函数简介:夹爪设置
 *参数说明:GripperStatus		夹爪状态 IDLE-空闲 Catch-抓取 Open-张开
 *返回类型:无
 *备注:无
 */
void Arm_GripperSet(Arm_GripperStatus GripperStatus)
{
	switch(GripperStatus)
	{
		case IDLE:	Steer_SetDuty(Arm_Gripper,9.6f);break;	//空闲
		case Catch:	Steer_SetDuty(Arm_Gripper,11.3f);break;	//抓取
		case Open:	Steer_SetDuty(Arm_Gripper,7.2f);break;	//张开
	}
}

/*
 *函数简介:升降台位置设置
 *参数说明:Arm_ArmStatus		升降台位置设置
 *参数说明:Speed				速度 单位RPM
 *返回类型:无
 *备注:无
 */
void Arm_ArmSet(Arm_ArmStatus ArmStatus,uint16_t Speed)
{
	Arm_ArmSetExt(ArmStatus,Speed,0);
}

/*
 *函数简介:升降台位置设置(扩展)
 *参数说明:Arm_ArmStatus		升降台位置设置
 *参数说明:Speed				速度 单位RPM
 *参数说明:Acc					加速度
 *返回类型:无
 *备注:无
 */
void Arm_ArmSetExt(Arm_ArmStatus ArmStatus,uint16_t Speed,uint8_t Acc)
{
	switch(ArmStatus)
	{
		case Init:			StepMotor_SetPositionExt(Arm_Arm,Speed,Acc,0);break;						//初始
		case Ground:		StepMotor_SetPositionExt(Arm_Arm,Speed,Acc,177000);break;					//地面
		//case Plate:			StepMotor_SetPositionExt(Arm_Arm,Speed,Acc,79000);break;				//转盘(80mm)
		case Plate:			StepMotor_SetPositionExt(Arm_Arm,Speed,Acc,-1225*PlateHigh+177000);break;	//转盘
		case Store_Up:		StepMotor_SetPositionExt(Arm_Arm,Speed,Acc,30000);break;					//物料盘上
		case Store_Down:	StepMotor_SetPositionExt(Arm_Arm,Speed,Acc,75000);break;					//物料盘下
		case MaDuo_Up:		StepMotor_SetPositionExt(Arm_Arm,Speed,Acc,85000);break;					//码垛上
		
		#ifndef WuLiao1
			case MaDuo_Down:	StepMotor_SetPositionExt(Arm_Arm,Speed,Acc,93000);break;	//码垛下
		#else
			case MaDuo_Down:	StepMotor_SetPositionExt(Arm_Arm,Speed,Acc,96000);break;	//码垛下
		#endif
		
		case Watch:			StepMotor_SetPositionExt(Arm_Arm,Speed,Acc,100000);break;	//观察
	}
}

/*
 *函数简介:Yaw舵机加减速回调函数
 *参数说明:无
 *返回类型:无
 *备注:在TIM7定时中断调用,用以Yaw舵机正弦加减速和物料盘震颤
 */
void Arm_Steer_Callback(void)
{
	Arm_Yaw_SINAccelControl();
	Arm_StoreGSCarSex_Control();
}
