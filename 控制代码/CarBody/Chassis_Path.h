#ifndef __CHASSIS_PATH_H
#define __CHASSIS_PATH_H

#include <stdint.h>

typedef struct
{
	float vx;			//横向速度								向右为正
	float vy;			//纵向速度								向前为正
	float Delta_Angle;	//Yaw旋转角度							逆时针为正
	float t;			//匀速时间								理论上匀速时间为2ms*t
	float K;			//上段路径点到这次路径点的加减速时间	理论上加减速时间为2ms*K
}Chassis_PathPoint;//底盘路径点结构体

typedef struct
{
	float End_K;				//最后一段路径点到停止的加减速时间		理论上加减速时间为2ms*K
	
	uint8_t Path_Size;			//路径点个数
	Chassis_PathPoint *Path;	//路径点数组
}Chassis_Path;//底盘路径结构体

extern Chassis_Path Chassis_Path_StartToQR;				//出发到二维码
extern Chassis_Path Chassis_Path_QRToPlate;				//二维码到转盘
extern Chassis_Path Chassis_Path_PlateToSeHuan1;		//转盘到粗加工
extern Chassis_Path Chassis_Path_SeHuan1ToSeHuan2[][3];	//粗加工到成品
extern Chassis_Path Chassis_Path_SeHuan2ToPlate[];		//成品到转盘
extern Chassis_Path Chassis_Path_GoHome[];				//回家

void Chassis_PathInit(void);	//底盘路径初始化

#endif
