#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "Chassis_PID.h"
#include "zpc_zxc_Typedef.h"

PID_PositionInitTypedef Chassis_AnglePID;//底盘转向PID
Orientation_PIDStruct Chassis_PlateOrientation;//转盘物料定位PID
Orientation_PIDStruct Chassis_SeHuanOrientation[3];//色环定位PID
Orientation_PIDStruct Chassis_WuLiaoOnSeHuanOrientation;//色环上物料定位PID
Orientation_PIDStruct Chassis_MaDuoOrientation[3];//码垛物料定位PID

/*
 *函数简介:底盘PID初始化
 *参数说明:无
 *返回类型:无
 *备注:无
 */
void Chassis_PID_Init(void)
{
	/*===============底盘转向PID===============*/
	float Chassis_AnglePID_Kp=4.0f;			//Kp
	float Chassis_AnglePID_Ki=0.0f;			//Ki
	float Chassis_AnglePID_Kd=3.0f;			//Kd
	float Chassis_AnglePID_EkRange=0.5f;	//误差死区
	float Chassis_AnglePID_OUTRange=200.0f;	//输出限幅

	PID_PositionStructureInit(&Chassis_AnglePID,0);
	PID_PositionSetParameter(&Chassis_AnglePID,Chassis_AnglePID_Kp,Chassis_AnglePID_Ki,Chassis_AnglePID_Kd);
	PID_PositionSetEkRange(&Chassis_AnglePID,-Chassis_AnglePID_EkRange,Chassis_AnglePID_EkRange);
	PID_PositionSetOUTRange(&Chassis_AnglePID,-Chassis_AnglePID_OUTRange,Chassis_AnglePID_OUTRange);

	/*===============转盘物料定位PID===============*/
	float Chassis_PlateOrientation_TargetX=0;			//横向期望(向右为正方向)
	float Chassis_PlateOrientation_TargetY=15;			//纵向期望(向下为正方向)
	
	float Chassis_PlateOrientation_Kp=0.2f;				//Kp
	float Chassis_PlateOrientation_Ki=0.0003f;			//Ki
	float Chassis_PlateOrientation_Kd=0.4f;				//Kd
	float Chassis_PlateOrientation_EkRange=5.0f;		//误差死区
	float Chassis_PlateOrientation_EkSumRange=10.0f;	//积分分离阈值
	float Chassis_PlateOrientation_OUTRange=50.0f;		//输出限幅
	
	PID_PositionStructureInit(&Chassis_PlateOrientation.OrientationX,Chassis_PlateOrientation_TargetX);
	PID_PositionSetParameter(&Chassis_PlateOrientation.OrientationX,Chassis_PlateOrientation_Kp,Chassis_PlateOrientation_Ki,Chassis_PlateOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_PlateOrientation.OrientationX,-Chassis_PlateOrientation_EkRange,Chassis_PlateOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_PlateOrientation.OrientationX,-Chassis_PlateOrientation_EkSumRange,Chassis_PlateOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_PlateOrientation.OrientationX,-Chassis_PlateOrientation_OUTRange,Chassis_PlateOrientation_OUTRange);

	PID_PositionStructureInit(&Chassis_PlateOrientation.OrientationY,Chassis_PlateOrientation_TargetY);
	PID_PositionSetParameter(&Chassis_PlateOrientation.OrientationY,Chassis_PlateOrientation_Kp,Chassis_PlateOrientation_Ki,Chassis_PlateOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_PlateOrientation.OrientationY,-Chassis_PlateOrientation_EkRange,Chassis_PlateOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_PlateOrientation.OrientationY,-Chassis_PlateOrientation_EkSumRange,Chassis_PlateOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_PlateOrientation.OrientationY,-Chassis_PlateOrientation_OUTRange,Chassis_PlateOrientation_OUTRange);

	/*===============色环定位PID===============*/
	float Chassis_SeHuanOrientation_TargetRedX=-1;		//红色环横向期望(向右为正方向)
	float Chassis_SeHuanOrientation_TargetRedY=14;		//红色环纵向期望(向下为正方向)
	float Chassis_SeHuanOrientation_TargetGreenX=0;		//绿色环横向期望(向右为正方向)
	float Chassis_SeHuanOrientation_TargetGreenY=14;	//绿色环纵向期望(向下为正方向)
	float Chassis_SeHuanOrientation_TargetBlueX=-1;		//蓝色环横向期望(向右为正方向)
	float Chassis_SeHuanOrientation_TargetBlueY=14;		//蓝色环纵向期望(向下为正方向)
	
	float Chassis_SeHuanOrientation_Kp=0.5f;			//Kp
	float Chassis_SeHuanOrientation_Ki=0.002f;			//Ki
	float Chassis_SeHuanOrientation_Kd=0.4f;			//Kd
	float Chassis_SeHuanOrientation_EkRange=0.0f;		//误差死区
	float Chassis_SeHuanOrientation_EkSumRange=5.0f;	//积分分离阈值
	float Chassis_SeHuanOrientation_OUTRange=50.0f;		//输出限幅

	//红色环
	PID_PositionStructureInit(&Chassis_SeHuanOrientation[Red].OrientationX,Chassis_SeHuanOrientation_TargetRedX);
	PID_PositionSetParameter(&Chassis_SeHuanOrientation[Red].OrientationX,Chassis_SeHuanOrientation_Kp,Chassis_SeHuanOrientation_Ki,Chassis_SeHuanOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_SeHuanOrientation[Red].OrientationX,-Chassis_SeHuanOrientation_EkRange,Chassis_SeHuanOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_SeHuanOrientation[Red].OrientationX,-Chassis_SeHuanOrientation_EkSumRange,Chassis_SeHuanOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_SeHuanOrientation[Red].OrientationX,-Chassis_SeHuanOrientation_OUTRange,Chassis_SeHuanOrientation_OUTRange);

	PID_PositionStructureInit(&Chassis_SeHuanOrientation[Red].OrientationY,Chassis_SeHuanOrientation_TargetRedY);
	PID_PositionSetParameter(&Chassis_SeHuanOrientation[Red].OrientationY,Chassis_SeHuanOrientation_Kp,Chassis_SeHuanOrientation_Ki,Chassis_SeHuanOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_SeHuanOrientation[Red].OrientationY,-Chassis_SeHuanOrientation_EkRange,Chassis_SeHuanOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_SeHuanOrientation[Red].OrientationY,-Chassis_SeHuanOrientation_EkSumRange,Chassis_SeHuanOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_SeHuanOrientation[Red].OrientationY,-Chassis_SeHuanOrientation_OUTRange,Chassis_SeHuanOrientation_OUTRange);

	//绿色环
	PID_PositionStructureInit(&Chassis_SeHuanOrientation[Green].OrientationX,Chassis_SeHuanOrientation_TargetGreenX);
	PID_PositionSetParameter(&Chassis_SeHuanOrientation[Green].OrientationX,Chassis_SeHuanOrientation_Kp,Chassis_SeHuanOrientation_Ki,Chassis_SeHuanOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_SeHuanOrientation[Green].OrientationX,-Chassis_SeHuanOrientation_EkRange,Chassis_SeHuanOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_SeHuanOrientation[Green].OrientationX,-Chassis_SeHuanOrientation_EkSumRange,Chassis_SeHuanOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_SeHuanOrientation[Green].OrientationX,-Chassis_SeHuanOrientation_OUTRange,Chassis_SeHuanOrientation_OUTRange);

	PID_PositionStructureInit(&Chassis_SeHuanOrientation[Green].OrientationY,Chassis_SeHuanOrientation_TargetGreenY);
	PID_PositionSetParameter(&Chassis_SeHuanOrientation[Green].OrientationY,Chassis_SeHuanOrientation_Kp,Chassis_SeHuanOrientation_Ki,Chassis_SeHuanOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_SeHuanOrientation[Green].OrientationY,-Chassis_SeHuanOrientation_EkRange,Chassis_SeHuanOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_SeHuanOrientation[Green].OrientationY,-Chassis_SeHuanOrientation_EkSumRange,Chassis_SeHuanOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_SeHuanOrientation[Green].OrientationY,-Chassis_SeHuanOrientation_OUTRange,Chassis_SeHuanOrientation_OUTRange);

	//蓝色环
	PID_PositionStructureInit(&Chassis_SeHuanOrientation[Blue].OrientationX,Chassis_SeHuanOrientation_TargetBlueX);
	PID_PositionSetParameter(&Chassis_SeHuanOrientation[Blue].OrientationX,Chassis_SeHuanOrientation_Kp,Chassis_SeHuanOrientation_Ki,Chassis_SeHuanOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_SeHuanOrientation[Blue].OrientationX,-Chassis_SeHuanOrientation_EkRange,Chassis_SeHuanOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_SeHuanOrientation[Blue].OrientationX,-Chassis_SeHuanOrientation_EkSumRange,Chassis_SeHuanOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_SeHuanOrientation[Blue].OrientationX,-Chassis_SeHuanOrientation_OUTRange,Chassis_SeHuanOrientation_OUTRange);

	PID_PositionStructureInit(&Chassis_SeHuanOrientation[Blue].OrientationY,Chassis_SeHuanOrientation_TargetBlueY);
	PID_PositionSetParameter(&Chassis_SeHuanOrientation[Blue].OrientationY,Chassis_SeHuanOrientation_Kp,Chassis_SeHuanOrientation_Ki,Chassis_SeHuanOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_SeHuanOrientation[Blue].OrientationY,-Chassis_SeHuanOrientation_EkRange,Chassis_SeHuanOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_SeHuanOrientation[Blue].OrientationY,-Chassis_SeHuanOrientation_EkSumRange,Chassis_SeHuanOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_SeHuanOrientation[Blue].OrientationY,-Chassis_SeHuanOrientation_OUTRange,Chassis_SeHuanOrientation_OUTRange);

	/*===============色环上物料定位PID===============*/
	float Chassis_WuLiaoOnSeHuanOrientation_TargetX=0;			//横向期望(向右为正方向)
	float Chassis_WuLiaoOnSeHuanOrientation_TargetY=18;			//纵向期望(向下为正方向)
	
	float Chassis_WuLiaoOnSeHuanOrientation_Kp=0.3f;			//Kp
	float Chassis_WuLiaoOnSeHuanOrientation_Ki=0.0003f;			//Ki
	float Chassis_WuLiaoOnSeHuanOrientation_Kd=0.4f;			//Kd
	float Chassis_WuLiaoOnSeHuanOrientation_EkRange=5.0f;		//误差死区
	float Chassis_WuLiaoOnSeHuanOrientation_EkSumRange=10.0f;	//积分分离阈值
	float Chassis_WuLiaoOnSeHuanOrientation_OUTRange=50.0f;		//输出限幅
	
	PID_PositionStructureInit(&Chassis_WuLiaoOnSeHuanOrientation.OrientationX,Chassis_WuLiaoOnSeHuanOrientation_TargetX);
	PID_PositionSetParameter(&Chassis_WuLiaoOnSeHuanOrientation.OrientationX,Chassis_WuLiaoOnSeHuanOrientation_Kp,Chassis_WuLiaoOnSeHuanOrientation_Ki,Chassis_WuLiaoOnSeHuanOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_WuLiaoOnSeHuanOrientation.OrientationX,-Chassis_WuLiaoOnSeHuanOrientation_EkRange,Chassis_WuLiaoOnSeHuanOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_WuLiaoOnSeHuanOrientation.OrientationX,-Chassis_WuLiaoOnSeHuanOrientation_EkSumRange,Chassis_WuLiaoOnSeHuanOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_WuLiaoOnSeHuanOrientation.OrientationX,-Chassis_WuLiaoOnSeHuanOrientation_OUTRange,Chassis_WuLiaoOnSeHuanOrientation_OUTRange);

	PID_PositionStructureInit(&Chassis_WuLiaoOnSeHuanOrientation.OrientationY,Chassis_WuLiaoOnSeHuanOrientation_TargetY);
	PID_PositionSetParameter(&Chassis_WuLiaoOnSeHuanOrientation.OrientationY,Chassis_WuLiaoOnSeHuanOrientation_Kp,Chassis_WuLiaoOnSeHuanOrientation_Ki,Chassis_WuLiaoOnSeHuanOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_WuLiaoOnSeHuanOrientation.OrientationY,-Chassis_WuLiaoOnSeHuanOrientation_EkRange,Chassis_WuLiaoOnSeHuanOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_WuLiaoOnSeHuanOrientation.OrientationY,-Chassis_WuLiaoOnSeHuanOrientation_EkSumRange,Chassis_WuLiaoOnSeHuanOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_WuLiaoOnSeHuanOrientation.OrientationY,-Chassis_WuLiaoOnSeHuanOrientation_OUTRange,Chassis_WuLiaoOnSeHuanOrientation_OUTRange);

	/*===============转盘物料定位PID===============*/
	float Chassis_MaDuoOrientation_TargetRedX=0;			//横向期望(向右为正方向)
	float Chassis_MaDuoOrientation_TargetRedY=12;			//纵向期望(向下为正方向)
	float Chassis_MaDuoOrientation_TargetGreenX=0;			//横向期望(向右为正方向)
	float Chassis_MaDuoOrientation_TargetGreenY=10;			//纵向期望(向下为正方向)
	float Chassis_MaDuoOrientation_TargetBlueX=0;			//横向期望(向右为正方向)
	float Chassis_MaDuoOrientation_TargetBlueY=12;			//纵向期望(向下为正方向)
	
	float Chassis_MaDuoOrientation_Kp=0.5f;				//Kp
	float Chassis_MaDuoOrientation_Ki=0.002f;			//Ki
	float Chassis_MaDuoOrientation_Kd=0.4f;				//Kd
	float Chassis_MaDuoOrientation_EkRange=0.0f;		//误差死区
	float Chassis_MaDuoOrientation_EkSumRange=3.0f;		//积分分离阈值
	float Chassis_MaDuoOrientation_OUTRange=50.0f;		//输出限幅

	//红色物料
	PID_PositionStructureInit(&Chassis_MaDuoOrientation[Red].OrientationX,Chassis_MaDuoOrientation_TargetRedX);
	PID_PositionSetParameter(&Chassis_MaDuoOrientation[Red].OrientationX,Chassis_MaDuoOrientation_Kp,Chassis_MaDuoOrientation_Ki,Chassis_MaDuoOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_MaDuoOrientation[Red].OrientationX,-Chassis_MaDuoOrientation_EkRange,Chassis_MaDuoOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_MaDuoOrientation[Red].OrientationX,-Chassis_MaDuoOrientation_EkSumRange,Chassis_MaDuoOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_MaDuoOrientation[Red].OrientationX,-Chassis_MaDuoOrientation_OUTRange,Chassis_MaDuoOrientation_OUTRange);

	PID_PositionStructureInit(&Chassis_MaDuoOrientation[Red].OrientationY,Chassis_MaDuoOrientation_TargetRedY);
	PID_PositionSetParameter(&Chassis_MaDuoOrientation[Red].OrientationY,Chassis_MaDuoOrientation_Kp,Chassis_MaDuoOrientation_Ki,Chassis_MaDuoOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_MaDuoOrientation[Red].OrientationY,-Chassis_MaDuoOrientation_EkRange,Chassis_MaDuoOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_MaDuoOrientation[Red].OrientationY,-Chassis_MaDuoOrientation_EkSumRange,Chassis_MaDuoOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_MaDuoOrientation[Red].OrientationY,-Chassis_MaDuoOrientation_OUTRange,Chassis_MaDuoOrientation_OUTRange);

	//绿色物料
	PID_PositionStructureInit(&Chassis_MaDuoOrientation[Green].OrientationX,Chassis_MaDuoOrientation_TargetGreenX);
	PID_PositionSetParameter(&Chassis_MaDuoOrientation[Green].OrientationX,Chassis_MaDuoOrientation_Kp,Chassis_MaDuoOrientation_Ki,Chassis_MaDuoOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_MaDuoOrientation[Green].OrientationX,-Chassis_MaDuoOrientation_EkRange,Chassis_MaDuoOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_MaDuoOrientation[Green].OrientationX,-Chassis_MaDuoOrientation_EkSumRange,Chassis_MaDuoOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_MaDuoOrientation[Green].OrientationX,-Chassis_MaDuoOrientation_OUTRange,Chassis_MaDuoOrientation_OUTRange);

	PID_PositionStructureInit(&Chassis_MaDuoOrientation[Green].OrientationY,Chassis_MaDuoOrientation_TargetGreenY);
	PID_PositionSetParameter(&Chassis_MaDuoOrientation[Green].OrientationY,Chassis_MaDuoOrientation_Kp,Chassis_MaDuoOrientation_Ki,Chassis_MaDuoOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_MaDuoOrientation[Green].OrientationY,-Chassis_MaDuoOrientation_EkRange,Chassis_MaDuoOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_MaDuoOrientation[Green].OrientationY,-Chassis_MaDuoOrientation_EkSumRange,Chassis_MaDuoOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_MaDuoOrientation[Green].OrientationY,-Chassis_MaDuoOrientation_OUTRange,Chassis_MaDuoOrientation_OUTRange);

	//蓝色物料
	PID_PositionStructureInit(&Chassis_MaDuoOrientation[Blue].OrientationX,Chassis_MaDuoOrientation_TargetBlueX);
	PID_PositionSetParameter(&Chassis_MaDuoOrientation[Blue].OrientationX,Chassis_MaDuoOrientation_Kp,Chassis_MaDuoOrientation_Ki,Chassis_MaDuoOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_MaDuoOrientation[Blue].OrientationX,-Chassis_MaDuoOrientation_EkRange,Chassis_MaDuoOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_MaDuoOrientation[Blue].OrientationX,-Chassis_MaDuoOrientation_EkSumRange,Chassis_MaDuoOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_MaDuoOrientation[Blue].OrientationX,-Chassis_MaDuoOrientation_OUTRange,Chassis_MaDuoOrientation_OUTRange);

	PID_PositionStructureInit(&Chassis_MaDuoOrientation[Blue].OrientationY,Chassis_MaDuoOrientation_TargetBlueY);
	PID_PositionSetParameter(&Chassis_MaDuoOrientation[Blue].OrientationY,Chassis_MaDuoOrientation_Kp,Chassis_MaDuoOrientation_Ki,Chassis_MaDuoOrientation_Kd);
	PID_PositionSetEkRange(&Chassis_MaDuoOrientation[Blue].OrientationY,-Chassis_MaDuoOrientation_EkRange,Chassis_MaDuoOrientation_EkRange);
	PID_PositionSetEkSumRange(&Chassis_MaDuoOrientation[Blue].OrientationY,-Chassis_MaDuoOrientation_EkSumRange,Chassis_MaDuoOrientation_EkSumRange);
	PID_PositionSetOUTRange(&Chassis_MaDuoOrientation[Blue].OrientationY,-Chassis_MaDuoOrientation_OUTRange,Chassis_MaDuoOrientation_OUTRange);
}
