#ifndef __ZPC_ZXC_H
#define __ZPC_ZXC_H

#include <stdint.h>

typedef enum
{
	NotInPlate=0,	//未发现物料在转盘上
	PlateMoving,	//移动
	PlateStatic,	//静止
}zpc_zxc_PlateStatus;//转盘状态

typedef enum
{
	Normal=0,		//正常
	MaDuo,			//码垛
}zpc_zxc_SeHuanWork;//色环任务

void zpc_zxc_Init(void);					//初始化
void zpc_zxc_StartToPlate(void);			//出发到转盘
void zpc_zxc_GetWuLiao(uint8_t i);			//抓取物料
void zpc_zxc_PlateToSeHuan1(void);			//转盘到粗加工区
void zpc_zxc_SeHuan1Process(uint8_t i);		//粗加工区处理
void zpc_zxc_SeHuan1ToSeHuan2(uint8_t i);	//粗加工区到半成品区
void zpc_zxc_SeHuan2Process(uint8_t i);		//成品区处理
void zpc_zxc_SeHuan2ToPlate(void);			//成品区到转盘
void zpc_zxc_GoHome(void);					//成品区回家

#endif
