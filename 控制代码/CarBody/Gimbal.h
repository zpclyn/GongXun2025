#ifndef __GIMBAL_H
#define __GIMBAL_H

#include <stdint.h>
#include "zpc_zxc_Typedef.h"

void Gimbal_Init(void);						//云台初始化
void Gimbal_Sleep(void);					//云台休眠
void Gimbal_ArmGetFromPlate(uint8_t i);		//云台从转盘抓取
void Gimbal_ArmGetFromGround(uint8_t i);	//云台从地面抓取
void Gimbal_ArmPutToGround(int8_t i);		//云台放置到地面
void Gimbal_ArmPutToWuliao(uint8_t i);		//云台放置到物料

#endif
