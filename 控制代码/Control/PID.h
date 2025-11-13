#ifndef __PID_H__
#define __PID_H__

typedef struct
{
	float Need_Value;//预期值
	float Now_Value;//当前值

	float Ek;//本次误差
	float Ek_1;//上一次误差
	float Sum_Ek;//误差积分
	float Del_Ek;//误差差分
	float Ek_low;//误差为0阈值下限
	float Ek_up;//误差为0阈值上限
	float Ek_Sumlow;//误差死区阈值下限
	float Ek_Sumup;//误差死区阈值上限

	float Kp;//Kp
	float Ki;//Ki
	float Kd;//Kd

	float P_OUT;//比例输出
	float I_OUT;//积分输出
	float D_OUT;//微分输出
	float OUT;//总输出
	float OUT_low;//输出限幅下限
	float OUT_up;//输出限幅上限
	
	float IOUT_low;//误差为0阈值下限
	float IOUT_up;//误差为0阈值上限
}PID_PositionInitTypedef;//位置式PID参数结构体

void PID_PositionStructureInit(PID_PositionInitTypedef* PID_InitStructure,float NeedValue);//位置式PID初始化结构体
void PID_PositionSetParameter(PID_PositionInitTypedef* PID_InitStructure,float kp,float ki,float kd);//位置式PID设置参数
void PID_PositionSetEkRange(PID_PositionInitTypedef* PID_InitStructure,float ek_low,float ek_up);//位置式PID设置误差死区阈值
void PID_PositionSetEkSumRange(PID_PositionInitTypedef* PID_InitStructure,float ek_sumlow,float ek_sumup);//位置式PID设置积分分离阈值
void PID_PositionSetOUTRange(PID_PositionInitTypedef* PID_InitStructure,float out_low,float out_up);//位置式PID设置输出限幅
void PID_PositionSetIOUTRange(PID_PositionInitTypedef* PID_InitStructure,float Iout_low,float Iout_up);//位置式PID设置积分限幅
void PID_PositionClean(PID_PositionInitTypedef* PID_InitStructure);//位置式PID清理
void PID_PositionCalc(PID_PositionInitTypedef* PID_InitStructure,float NowValue);//位置式PID计算

#endif
