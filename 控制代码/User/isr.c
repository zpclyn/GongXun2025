#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "isr.h"

//	0		0
void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4,USART_IT_RXNE)==SET)//查询空闲中断标志位
	{
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);//清除接收中断标志位
		
		QR_Callback();
	}
}

//	1		0
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2,USART_IT_IDLE)==SET)//查询空闲中断标志位
	{		
		(void)USART2->SR;
		(void)USART2->DR;//清除空闲中断标志位
		
		HWT101_Callback();
	}
}

//	2		0
void UART5_IRQHandler(void)
{
	if(USART_GetITStatus(UART5,USART_IT_IDLE)==SET)//查询空闲中断标志位
	{		
		(void)UART5->SR;
		(void)UART5->DR;//清除空闲中断标志位
		
		Camera_Callback();
	}
}

//	3		0
void TIM7_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET)//检测TIM9更新
	{
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);//清除标志位
		
		Arm_Steer_Callback();
	}
}

//	5		0
void TIM6_DAC_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)==SET)//检测TIM9更新
	{
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);//清除标志位
		
		StepMotor_Callack();
	}
}

//	7		1
void TIM1_BRK_TIM9_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM9,TIM_IT_Update)==SET)//检测TIM9更新
	{
		TIM_ClearITPendingBit(TIM9,TIM_IT_Update);//清除标志位
		
		Key_GetStatus_Callback();
	}
}
