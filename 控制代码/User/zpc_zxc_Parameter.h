#ifndef __ZPC_ZXC_PARAMETER_H
#define __ZPC_ZXC_PARAMETER_H

//#define WuLiao0						//初赛物料
#define WuLiao1							//决赛物料

//#define GivenTestNum					//给定任务码 注释-不给定,靠识别任务码 不注释-给定任务码,跳过识别
#define Gimbal_MoveRemain			30	//云台运动延时余量 值越大云台运动越慢
#define PlateHigh					80	//转盘高度 单位mm

//#define PathDebug						//解除注释进入路径调试 用于在比赛中调整移动路径 默认需要停止转盘,小车会在转盘处定位红色物料并在两色环处定位绿色环
//#define Fast							//解除注释进入路径快速移动 会开启路径漂移
#define CarSexENABLE					//决赛物料车震使能

#endif
