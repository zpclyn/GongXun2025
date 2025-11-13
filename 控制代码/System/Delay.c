#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

/*
 *函数简介:微秒级延时
 *参数说明:延时时长,单位us
 *返回类型:无
 *备注:参数范围:0~4294967295
 */
void Delay_us(uint32_t us)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=us*21; 						//时间加载，我们要延时n倍的us, 1us是一个fac_ua周期，所以总共要延时的周期值为二者相乘最后送到Load中。		 
	SysTick->VAL=0x00;        					//清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	//开启使能位 开始倒数
	do temp=SysTick->CTRL;
	while((temp&0x01) && !(temp&(1<<16)));		//用来判断 systick 定时器是否还处于开启状态，然后在等待时间到达，也就是数到0的时候,此时第十六位设置为1
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器使能位
	SysTick->VAL=0x00;       					//清空计数器
}

/*
 *函数简介:毫秒级延时
 *参数说明:延时时长,单位ms
 *返回类型:无
 *备注:参数范围:0~4294967295
 */
void Delay_ms(uint32_t ms)
{
	while(ms--)
		Delay_us(1000);
}
 
/*
 *函数简介:秒级延时
 *参数说明:延时时长,单位s
 *返回类型:无
 *备注:参数范围:0~4294967295
 */
void Delay_s(uint32_t s)
{
	while(s--)
		Delay_ms(1000);
} 
