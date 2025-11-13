#ifndef __CAMERA_H
#define __CAMERA_H

#include <stdint.h>
#include "zpc_zxc_Typedef.h"

typedef enum
{
	WuLiao=-1,	//物料
	SeHuan_R=Red,	//红色色环
	SeHuan_G=Green,	//绿色色环
	SeHuan_B=Blue,	//蓝色色环
}Camera_Mode;//摄像头识别模式枚举

typedef enum
{
	X=0,		//横坐标(左右,中心为原点) 向右为正方向
	Y,			//纵坐标(上下,中心为原点) 向下为正方向
}Camera_Coordinate;//摄像头坐标系枚举

extern uint8_t Camera_WuLiaoRxFlag,Camera_SeHuanRxFlag;	//物料,色环接收完成标志位
extern int16_t Camera_WuLiao[][2];						//物料坐标
extern int16_t Camera_SeHuan[][2];						//色环坐标

void Camera_Init(void);					//摄像头初始化
void Camera_GotoMode(Camera_Mode Mode);	//摄像头切换模式
void Camera_Callback(void);				//摄像头接收回调函数

#endif
