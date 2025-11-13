#ifndef __HWT101_H
#define __HWT101_H

#include <stdint.h>

extern uint8_t HWT101_wRxFlag,HWT101_AngleRxFlag;	//角速度,角度接收完成标志位
extern float HWT101_Yaw_W;							//Yaw轴角速度(°/s)
extern float HWT101_Yaw;							//Yaw轴角度(°)
extern float HWT101_CheckYaw;						//Yaw轴角度校验值

void HWT101_Init(void);			//HWT101初始化
void HWT101_AngleCheck(void);	//HWT101角度校验
void HWT101_Callback(void);		//HWT101回调函数(USART2空闲中断)

#endif
