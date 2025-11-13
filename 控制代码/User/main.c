#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "zpc_zxc_Headfile.h"

//	APB1/APB1_TIM	42MHz
//	APB2/APB2_TIM	84MHz
int main(void)
{
	/*====================初始化====================*/
	zpc_zxc_Init();

	while(Key_Status==RESET);
	Chassis_Reset();

	/*====================起动====================*/
	zpc_zxc_StartToPlate();
	
	/*====================第一次抓取物料====================*/
	zpc_zxc_GetWuLiao(0);
	
	/*====================第一次物料盘到粗加工====================*/
	zpc_zxc_PlateToSeHuan1();
	
	/*====================第一次粗加工====================*/
	zpc_zxc_SeHuan1Process(0);
	
	/*====================第一次粗加工到成品====================*/
	zpc_zxc_SeHuan1ToSeHuan2(0);

	/*====================第一次成品====================*/
	zpc_zxc_SeHuan2Process(0);
	
	/*====================成品到物料盘====================*/
	zpc_zxc_SeHuan2ToPlate();

	/*====================第二次抓取物料====================*/
	zpc_zxc_GetWuLiao(1);
	
	/*====================第二次物料盘到粗加工====================*/
	zpc_zxc_PlateToSeHuan1();
	
	/*====================第二次粗加工====================*/
	zpc_zxc_SeHuan1Process(1);
	
	/*====================第二次粗加工到成品====================*/
	zpc_zxc_SeHuan1ToSeHuan2(1);
	
	/*====================第二次成品====================*/
	zpc_zxc_SeHuan2Process(1);
	
	/*====================回家====================*/
	zpc_zxc_GoHome();

	while(1)
	{
	}
}

