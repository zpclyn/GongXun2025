#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "Steer.h"

#define Steer_Freq			300		//舵机控制频率
#define Steer_DutyRange		10000	//PWM占空比量程

/*
 *函数简介:舵机初始化
 *参数说明:无
 *返回类型:无
 *备注:采用引脚PE9 PE11 PE13 PE14 PC6 PC7 PC8 PC9分别对应TIM1CH1 TIM1CH2 TIM1CH3 TIM1CH4 TIM8CH1 TIM8CH2 TIM8CH3 TIM8CH4
 */
void Steer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//开启时钟
	
	TIM_InternalClockConfig(TIM1);//选择时基单元TIM1
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;   
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//复用推挽
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//默认上拉
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_TIM1);//开启TIM1复用模式
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM8);//开启TIM2复用模式
	
	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_InitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//配置时钟分频为1分频
	TIM_InitStructure.TIM_CounterMode=TIM_CounterMode_Up;//配置计数器模式为向上计数
	TIM_InitStructure.TIM_Period=Steer_DutyRange-1;//ARR
	TIM_InitStructure.TIM_Prescaler=(uint16_t)(84000000/Steer_DutyRange/Steer_Freq)-1;//PSC
	TIM_InitStructure.TIM_RepetitionCounter=0;//配置重复计数单元的置为0
	TIM_TimeBaseInit(TIM1,&TIM_InitStructure);
	TIM_TimeBaseInit(TIM8,&TIM_InitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;//配置输出比较模式
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;//配置输出比较的极性
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;//输出使能
	TIM_OCInitStructure.TIM_Pulse=(uint32_t)(Steer_DutyRange*0/100.0f);//配置输出比较寄存器CCR的值,确定占空比
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);
	TIM_OC3Init(TIM1,&TIM_OCInitStructure);
	TIM_OC4Init(TIM1,&TIM_OCInitStructure);//配置TIM1输出PWM
	TIM_OC1Init(TIM8,&TIM_OCInitStructure);
	TIM_OC2Init(TIM8,&TIM_OCInitStructure);
	TIM_OC3Init(TIM8,&TIM_OCInitStructure);
	TIM_OC4Init(TIM8,&TIM_OCInitStructure);//配置TIM8输出PWM
	
	TIM_Cmd(TIM1,ENABLE);
	TIM_Cmd(TIM8,ENABLE);//启动定时器
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	TIM_CtrlPWMOutputs(TIM8,ENABLE);//开启TIM1和TIM8的PWM输出
}

/*
 *函数简介:舵机设置占空比
 *参数说明:Steerx		舵机编号
 *参数说明:Duty			占空比		2.5f~12.5f
 *返回类型:无
 *备注:占空比会默认折算到20ms(50Hz)时的占空比
 */
void Steer_SetDuty(Steer_TypeDef Steerx,float Duty)
{
	switch(Steerx)
	{
		case Steer1:TIM_SetCompare1(TIM1,(uint32_t)(Steer_DutyRange*(Duty/100.0f)*(Steer_Freq/50.0f)));break;//配置C1输出PWM
		case Steer2:TIM_SetCompare2(TIM1,(uint32_t)(Steer_DutyRange*(Duty/100.0f)*(Steer_Freq/50.0f)));break;//配置C1输出PWM
		case Steer3:TIM_SetCompare3(TIM1,(uint32_t)(Steer_DutyRange*(Duty/100.0f)*(Steer_Freq/50.0f)));break;//配置C1输出PWM
		case Steer4:TIM_SetCompare4(TIM1,(uint32_t)(Steer_DutyRange*(Duty/100.0f)*(Steer_Freq/50.0f)));break;//配置C1输出PWM
		case Steer5:TIM_SetCompare1(TIM8,(uint32_t)(Steer_DutyRange*(Duty/100.0f)*(Steer_Freq/50.0f)));break;//配置C1输出PWM
		case Steer6:TIM_SetCompare2(TIM8,(uint32_t)(Steer_DutyRange*(Duty/100.0f)*(Steer_Freq/50.0f)));break;//配置C1输出PWM
		case Steer7:TIM_SetCompare3(TIM8,(uint32_t)(Steer_DutyRange*(Duty/100.0f)*(Steer_Freq/50.0f)));break;//配置C1输出PWM
		case Steer8:TIM_SetCompare4(TIM8,(uint32_t)(Steer_DutyRange*(Duty/100.0f)*(Steer_Freq/50.0f)));break;//配置C1输出PWM
	}
}
