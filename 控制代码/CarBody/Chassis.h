#ifndef __CHASSIS_H
#define __CHASSIS_H

#include <stdint.h>
#include "Chassis_Path.h"
#include "Chassis_PID.h"
#include "zpc_zxc_Typedef.h"

typedef enum
{
	BackOneSeHuan=-1,	//后退一个色环
	BackTwoSeHuan=-2,	//后退两个色环
	MoveNone=0,			//不移动			用于绿色环定位的情况
	GoOneSeHuan=1,		//前进一个色环
	GoTwoSeHuan=2,		//前进两个色环
}Chassis_DeltaMoveEnum;//底盘增量移动枚举

void Chassis_Init(void);																					//底盘初始化
void Chassis_Reset(void);																					//底盘复位
void Chassis_MoveOnce(float vx,float vy,float Delta_Angle,float t,float K);									//底盘单次移动
void Chassis_MoveOncePath(Chassis_Path Path);																//底盘单次路径移动
void Chassis_Move(Chassis_Path Path);																		//底盘单路径移动
void Chassis_MovePath(Chassis_Path Path);																	//底盘路径移动
void Chassis_DeltaMove(Chassis_DeltaMoveEnum DeltaMove);													//底盘增量移动
void Chassis_TurnRight(void);																				//底盘右转
void Chassis_WuLiaoOrientate(Orientation_PIDStruct *OrientationPID,zpc_zxc_Color Color,uint16_t TimeOut);	//底盘物料定位
void Chassis_SeHuanOrientate(Orientation_PIDStruct *OrientationPID,zpc_zxc_Color Color,uint16_t TimeOut);	//底盘色环定位

#endif
