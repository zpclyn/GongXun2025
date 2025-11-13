#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "Delay.h"

FlagStatus Key_Status=RESET;//按键状态 按下进行RESET/SET切换

/*
 *函数简介:按键定时器扫描初始化
 *参数说明:无
 *返回类型:无
 *备注:按键引脚PB15,默认定时20ms
 */
void Key_TIMScanInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);//开启时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);//初始化GPIO_PB15
	
	TIM_InternalClockConfig(TIM9);//选择时基单元的时钟

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//配置时基单元（配置参数）
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//配置时钟分频为1分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//配置计数器模式为向上计数
	TIM_TimeBaseInitStructure.TIM_Period=840-1;//配置自动重装值ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler=2000-1;//配置分频值PSC,默认频率50Hz
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//配置重复计数单元的置为0
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseInitStructure);//初始化TIM9
	
	TIM_ClearFlag(TIM9,TIM_FLAG_Update);//清除配置时基单元产生的中断标志位
	
	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE);//使能更新中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);//选择NVIC分组
	
	NVIC_InitTypeDef NVIC_InitStructure;//配置NVIC（配置参数）
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn;//选择中断通道为TIM9
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//使能中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=7;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;//响应优先级
	NVIC_Init(&NVIC_InitStructure);//初始化NVIC
	
	TIM_Cmd(TIM9,ENABLE);//启动定时器
}

/*
 *函数简介:按键状态获取回调函数
 *参数说明:无
 *返回类型:无
 *备注:在TIM9定时中断调用,20ms扫描一次,默认检测上升沿
 */
void Key_GetStatus_Callback(void)
{
	static uint8_t Key_LastStatus=1,Key_NowStatus=1;//获取此次按键电平和上一次电平
	Key_LastStatus=Key_NowStatus;
	Key_NowStatus=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15);
	
	if(Key_LastStatus==0 && Key_NowStatus==1)Key_Status=!Key_Status;//判断为上升沿,即按键按下	
}
