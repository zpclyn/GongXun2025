#ifndef __ZPC_ZXC_HEADFILE_H
#define __ZPC_ZXC_HEADFILE_H

#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

#include "Delay.h"						//延时

#include "Camera.h"						//摄像头
#include "HWT101.h"						//陀螺仪
#include "Key.h"						//按键
#include "OLED.h"						//屏幕
#include "QR.h"							//二维码模块
#include "Steer.h"						//舵机
#include "StepMotor.h"					//步进

#include "PID.h"

#include "Arm.h"						//爪臂机构
#include "Gimbal.h"						//云台
#include "Chassis_Path.h"				//底盘路径
#include "Chassis_PID.h"				//底盘PID
#include "Chassis.h"					//底盘

#include "Data.h"						//数据处理
#include "zpc_zxc_Parameter.h"			//参数
#include "zpc_zxc_Typedef.h"			//自定义类型
#include "zpc_zxc.h"					//主函数

#endif
