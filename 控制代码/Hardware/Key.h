#ifndef __KEY_H
#define __KEY_H

#include <stdint.h>
#include "stm32f4xx.h"                  // Device header

extern FlagStatus Key_Status;//按键状态 按下进行RESET/SET切换

void Key_TIMScanInit(void);			//按键定时器扫描初始化
void Key_GetStatus_Callback(void);	//按键状态获取回调函数(TIM9定时中断)

#endif
