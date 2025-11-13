#ifndef __CHASSIS_PID_H
#define __CHASSIS_PID_H

#include <stdint.h>
#include "PID.h"

typedef struct
{
	PID_PositionInitTypedef OrientationX;	//横向定位PID(向右为摄像头数据正方向)
	PID_PositionInitTypedef OrientationY;	//纵向定位PID(向下为摄像头数据正方向)
}Orientation_PIDStruct;//定位PID结构体

extern PID_PositionInitTypedef Chassis_AnglePID;				//底盘转向PID
extern Orientation_PIDStruct Chassis_PlateOrientation;			//转盘物料定位PID
extern Orientation_PIDStruct Chassis_SeHuanOrientation[];		//色环定位PID
extern Orientation_PIDStruct Chassis_WuLiaoOnSeHuanOrientation;	//色环上物料定位PID
extern Orientation_PIDStruct Chassis_MaDuoOrientation[];		//码垛物料定位PID

void Chassis_PID_Init(void);	//底盘PID初始化

#endif
