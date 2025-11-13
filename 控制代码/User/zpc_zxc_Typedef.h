#ifndef __ZPC_ZXC_TYPEDEF_H
#define __ZPC_ZXC_TYPEDEF_H

typedef enum
{
	Red=0,		//红
	Green,		//绿
	Blue,		//蓝
}zpc_zxc_Color;//颜色枚举

typedef enum
{
	Straight=0,		//直行
	TurnRight=-90,	//右转
}Chassis_YawTurn;//底盘Yaw旋转

extern char Test[];		//任务码

#endif
