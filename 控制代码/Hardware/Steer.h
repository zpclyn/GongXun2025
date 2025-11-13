#ifndef __STEER_H
#define __STEER_H

typedef enum
{
	Steer1=0,Steer2,Steer3,Steer4,Steer5,Steer6,Steer7,Steer8,
}Steer_TypeDef;//舵机编号枚举 分别对应PE9 PE11 PE13 PE14 PC6 PC7 PC8 PC9

void Steer_Init(void);									//舵机初始化
void Steer_SetDuty(Steer_TypeDef Steerx,float Duty);	//舵机设置占空比

#endif
