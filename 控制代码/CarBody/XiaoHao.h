#ifndef __XIAOHAO_H
#define __XIAOHAO_H

#include <stdint.h>

void XiaoHao_Init(void);
void XiaoHao_StartToPlate(void);
void XiaoHao_GetWuLiao(uint8_t Flag);//1-First 2-Second
void XiaoHao_PlateToSeHuan1(uint8_t Flag);
void XiaoHao_SeHuanProcess(uint8_t SeHuan_Select,uint8_t Flag);
void XiaoHao_SeHuan1ToSeHuan2(uint8_t Flag);
void XiaoHao_SeHuan2ToPlate(void);
void XiaoHao_MaDuo(void);
void XiaoHao_GoHome(void);

#endif
