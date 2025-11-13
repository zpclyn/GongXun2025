#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "PID.h"

/*
 *函数简介:位置式PID初始化结构体
 *参数说明:位置式PID参数结构体
 *参数说明:预期值
 *返回类型:无
 *备注:无
 */
void PID_PositionStructureInit(PID_PositionInitTypedef* PID_InitStructure,float NeedValue)
{
	PID_InitStructure->Need_Value=NeedValue;
	PID_InitStructure->Ek=0;
	PID_InitStructure->Sum_Ek=0;
	PID_InitStructure->Ek_low=0;
	PID_InitStructure->Ek_up=0;
	PID_InitStructure->Ek_Sumlow=-1e10;
	PID_InitStructure->Ek_Sumup=1e10;
	PID_InitStructure->Kp=0;
	PID_InitStructure->Ki=0;
	PID_InitStructure->Kd=0;
	PID_InitStructure->OUT_low=-1e10;
	PID_InitStructure->OUT_up=1e10;
	PID_InitStructure->IOUT_low=-1e10;
	PID_InitStructure->IOUT_up=1e10;
}

/*
 *函数简介:位置式PID设置参数
 *参数说明:位置式PID参数结构体
 *参数说明:单精度浮点型Kp
 *参数说明:单精度浮点型Ki
 *参数说明:单精度浮点型Kd
 *返回类型:无
 *备注:无
 */
void PID_PositionSetParameter(PID_PositionInitTypedef* PID_InitStructure,float kp,float ki,float kd)
{
	PID_InitStructure->Kp=kp;
	PID_InitStructure->Ki=ki;
	PID_InitStructure->Kd=kd;
}

/*
 *函数简介:位置式PID设置误差死区阈值
 *参数说明:位置式PID参数结构体
 *参数说明:误差死区阈值下限
 *参数说明:误差死区阈值上限
 *返回类型:无
 *备注:无
 */
void PID_PositionSetEkRange(PID_PositionInitTypedef* PID_InitStructure,float ek_low,float ek_up)
{
	PID_InitStructure->Ek_low=ek_low;
	PID_InitStructure->Ek_up=ek_up;
}

/*
 *函数简介:位置式PID设置积分分离阈值
 *参数说明:位置式PID参数结构体
 *参数说明:积分分离阈值下限
 *参数说明:积分分离阈值上限
 *返回类型:无
 *备注:无
 */
void PID_PositionSetEkSumRange(PID_PositionInitTypedef* PID_InitStructure,float ek_sumlow,float ek_sumup)
{
	PID_InitStructure->Ek_Sumlow=ek_sumlow;
	PID_InitStructure->Ek_Sumup=ek_sumup;
}

/*
 *函数简介:位置式PID设置输出限幅
 *参数说明:位置式PID参数结构体
 *参数说明:输出限幅下限
 *参数说明:输出限幅上限
 *返回类型:无
 *备注:无
 */
void PID_PositionSetOUTRange(PID_PositionInitTypedef* PID_InitStructure,float out_low,float out_up)
{
	PID_InitStructure->OUT_low=out_low;
	PID_InitStructure->OUT_up=out_up;
}

/*
 *函数简介:位置式PID设置积分限幅
 *参数说明:位置式PID参数结构体
 *参数说明:积分限幅下限
 *参数说明:积分限幅上限
 *返回类型:无
 *备注:无
 */
void PID_PositionSetIOUTRange(PID_PositionInitTypedef* PID_InitStructure,float Iout_low,float Iout_up)
{
	PID_InitStructure->IOUT_low=Iout_low;
	PID_InitStructure->IOUT_up=Iout_up;
}

/*
 *函数简介:位置式PID清理
 *参数说明:位置式PID参数结构体
 *返回类型:无
 *备注:使Ek和Sum为0
 */
void PID_PositionClean(PID_PositionInitTypedef* PID_InitStructure)
{
	PID_InitStructure->Ek=0;
	PID_InitStructure->Sum_Ek=0;
}

/*
 *函数简介:位置式PID计算
 *参数说明:位置式PID参数结构体
 *参数说明:当前值
 *返回类型:无
 *备注:OUT=POUT+IOUT+DOUT=Kp*Ek+Ki*ΣEk+Kd*(Ek-Ek_1)
 *备注:计算结果保存在位置式PID参数结构体中
 */
void PID_PositionCalc(PID_PositionInitTypedef* PID_InitStructure,float NowValue)
{
	PID_InitStructure->Now_Value=NowValue;
	PID_InitStructure->Ek_1=PID_InitStructure->Ek;
	PID_InitStructure->Ek=PID_InitStructure->Need_Value-PID_InitStructure->Now_Value;
	if(PID_InitStructure->Ek_low<PID_InitStructure->Ek&&PID_InitStructure->Ek<PID_InitStructure->Ek_up)//误差死区
		PID_InitStructure->Ek=0;
	PID_InitStructure->Del_Ek=PID_InitStructure->Ek-PID_InitStructure->Ek_1;
	
	if(PID_InitStructure->Ek>PID_InitStructure->Ek_Sumlow && PID_InitStructure->Ek<PID_InitStructure->Ek_Sumup)PID_InitStructure->Sum_Ek+=PID_InitStructure->Ek;//积分分离
	else PID_InitStructure->Sum_Ek=0;

	PID_InitStructure->P_OUT=PID_InitStructure->Kp*PID_InitStructure->Ek;
	PID_InitStructure->I_OUT=PID_InitStructure->Ki*PID_InitStructure->Sum_Ek;
	PID_InitStructure->D_OUT=PID_InitStructure->Kd*PID_InitStructure->Del_Ek;
	PID_InitStructure->OUT=PID_InitStructure->P_OUT+PID_InitStructure->I_OUT+PID_InitStructure->D_OUT;
	
	if(PID_InitStructure->I_OUT<PID_InitStructure->IOUT_low)//积分限幅
		PID_InitStructure->I_OUT=PID_InitStructure->IOUT_low;
	if(PID_InitStructure->I_OUT>PID_InitStructure->IOUT_up)
		PID_InitStructure->I_OUT=PID_InitStructure->IOUT_up;
	
	if(PID_InitStructure->OUT<PID_InitStructure->OUT_low)//输出限幅
		PID_InitStructure->OUT=PID_InitStructure->OUT_low;
	if(PID_InitStructure->OUT>PID_InitStructure->OUT_up)
		PID_InitStructure->OUT=PID_InitStructure->OUT_up;
}
