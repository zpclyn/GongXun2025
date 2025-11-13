#ifndef __ARM_H
#define __ARM_H

#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <stdint.h>
#include "zpc_zxc_Parameter.h"
#include "zpc_zxc_Typedef.h"
#include "StepMotor.h"
#include "Steer.h"

#define Arm_Yaw				Steer6		//Yaw舵机
#define Arm_Store			Steer7		//物料盘舵机
#define Arm_Gripper			Steer8		//夹爪舵机
#define Arm_Arm				5			//升降台步进ID

typedef enum
{
	OUT=0,			//伸出来
	Back,			//回去
}Arm_YawStatus;//Yaw舵机状态枚举

typedef enum
{
	IDLE=0,			//空闲
	Catch,			//抓取
	Open,			//张开
}Arm_GripperStatus;//夹爪状态枚举

typedef enum
{
	Init=0,			//初始
	Ground,			//地面
	Plate,			//转盘
	Store_Up,		//物料盘上
	Store_Down,		//物料盘下
	MaDuo_Up,		//码垛上
	MaDuo_Down,		//码垛下
	Watch,			//观察
}Arm_ArmStatus;//升降台位置设置

void Arm_Init(void);													//爪臂初始化
void Arm_YawSet(Arm_YawStatus YawStatus);								//Yaw轴设置
void Arm_StoreSet(zpc_zxc_Color Color);									//物料盘设置
void Arm_StoreGSCarSex_ENABLE(FunctionalState NewState);				//物料盘震颤使能
void Arm_GripperSet(Arm_GripperStatus GripperStatus);					//夹爪设置
void Arm_ArmSet(Arm_ArmStatus ArmStatus,uint16_t Speed);				//升降台位置设置
void Arm_ArmSetExt(Arm_ArmStatus ArmStatus,uint16_t Speed,uint8_t Acc);	//升降台位置设置(扩展)
void Arm_Steer_Callback(void);											//Yaw舵机加减速回调函数

#endif
