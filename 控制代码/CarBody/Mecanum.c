#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <math.h>
#include "Delay.h"
#include "M3508.h"
#include "Remote.h"
#include "PID.h"

#define Mecanum_LeftFrontWheel								M3508_2//左前轮
#define Mecanum_RightFrontWheel								M3508_1//右前轮
#define Mecanum_LeftRearWheel								M3508_3//左后轮
#define Mecanum_RightRearWheel								M3508_4//右后轮
#define Mecanum_WheelRadius									7.0f//麦轮半径(单位cm)

#define Mecanum_rx											18.5f//底盘中心到轮子中心的距离的x轴分量(单位cm)
#define Mecanum_ry											18.0f//底盘中心到轮子中心的距离的y轴分量(单位cm)

#define Mecanum_LeverSpeedMapRate							(1/660.0f)//拨杆速度映射比例

#define Mecanum_PowerControl								1//功率控制,0-不使用功率控制,1-开启可功率控制模式
#define Mecanum_PowerLimit									35.0f//平均功率限制
#define Mecanum_PowerControlSpeedNormalizationValue			3.0f//功率控制归一化速度标准值,范围[0,3]
#define Mecanum_PowerControlGainCoefficientInitialValue		0.3f//功率控制增益系数初始值

PID_PositionInitTypedef Mecanum_SpeedPID[4];//底盘四个电机的转速PID
float Mecanum_Power;//底盘功率

/*
 *函数简介:麦轮初始化
 *参数说明:无
 *返回类型:无
 *备注:即四个转速PID的初始化
 *备注:四个电机的ID参照上方的宏定义,最好使用M3508电机的ID1~4,否则下方电机控制的代码需要修改
 */
void Mecanum_Init(void)
{
	PID_PositionStructureInit(&Mecanum_SpeedPID[0],0);//左前轮
	PID_PositionSetParameter(&Mecanum_SpeedPID[0],16,0,30);
	PID_PositionSetEkRange(&Mecanum_SpeedPID[0],-5,5);
	PID_PositionSetOUTRange(&Mecanum_SpeedPID[0],-15000,15000);
	
	PID_PositionStructureInit(&Mecanum_SpeedPID[1],0);//右前轮
	PID_PositionSetParameter(&Mecanum_SpeedPID[1],16,0,30);
	PID_PositionSetEkRange(&Mecanum_SpeedPID[1],-5,5);
	PID_PositionSetOUTRange(&Mecanum_SpeedPID[1],-15000,15000);
	
	PID_PositionStructureInit(&Mecanum_SpeedPID[2],0);//左后轮
	PID_PositionSetParameter(&Mecanum_SpeedPID[2],16,0,30);
	PID_PositionSetEkRange(&Mecanum_SpeedPID[2],-5,5);
	PID_PositionSetOUTRange(&Mecanum_SpeedPID[2],-15000,15000);
	
	PID_PositionStructureInit(&Mecanum_SpeedPID[3],0);//右后轮
	PID_PositionSetParameter(&Mecanum_SpeedPID[3],16,0,30);
	PID_PositionSetEkRange(&Mecanum_SpeedPID[3],-5,5);
	PID_PositionSetOUTRange(&Mecanum_SpeedPID[3],-15000,15000);
}

/*
 *函数简介:麦轮PID清理
 *参数说明:无
 *返回类型:无
 *备注:清理四个转速位置式PID
 */
void Mecanum_CleanPID(void)
{
	PID_PositionClean(&Mecanum_SpeedPID[0]);//左前轮
	PID_PositionClean(&Mecanum_SpeedPID[1]);//右前轮
	PID_PositionClean(&Mecanum_SpeedPID[2]);//左后轮
	PID_PositionClean(&Mecanum_SpeedPID[3]);//右后轮
}

/*
 *函数简介:麦轮速度控制
 *参数说明:左前轮速度
 *参数说明:右前轮速度
 *参数说明:左后轮速度
 *参数说明:右后轮速度
 *返回类型:无
 *备注:单独控制四个轮子的速度
 */
void Mecanum_ControlSpeed(int16_t LeftFrontSpeed,int16_t RightFrontSpeed,int16_t LeftRearSpeed,int16_t RightRearSpeed)
{
	//更改期望
	Mecanum_SpeedPID[0].Need_Value=LeftFrontSpeed;//左前轮
	Mecanum_SpeedPID[1].Need_Value=RightFrontSpeed;//右前轮;
	Mecanum_SpeedPID[2].Need_Value=LeftRearSpeed;//左后轮
	Mecanum_SpeedPID[3].Need_Value=RightRearSpeed;//右后轮
	
	//PID计算
//	PID_PositionCalc(&Mecanum_SpeedPID[0],M3508_MotorStatus[Mecanum_LeftFrontWheel-0x201].Speed);//左前轮
//	PID_PositionCalc(&Mecanum_SpeedPID[1],M3508_MotorStatus[Mecanum_RightFrontWheel-0x201].Speed);//右前轮
//	PID_PositionCalc(&Mecanum_SpeedPID[2],M3508_MotorStatus[Mecanum_LeftRearWheel-0x201].Speed);//左后轮
//	PID_PositionCalc(&Mecanum_SpeedPID[3],M3508_MotorStatus[Mecanum_RightRearWheel-0x201].Speed);//右后轮

	M3508_CANSetLIDCurrent(Mecanum_SpeedPID[1].OUT,Mecanum_SpeedPID[0].OUT,Mecanum_SpeedPID[2].OUT,Mecanum_SpeedPID[3].OUT);//M3508控制
}

/*
 *函数简介:麦轮逆运动解算
 *参数说明:x轴速度,单位m/s(以前为正)
 *参数说明:y轴速度,单位m/s(以左为正)
 *参数说明:z轴转速,单位rad/s(以逆时针为正)
 *返回类型:无
 *备注:速度转速转换系数:
 *	   w'=v/R (rad/s)=v/(2Π×R) (r/s)=60×v/(2Π×R) (r/min)=1/19 w
 *	   ⇒ w=19×60×v/(2Π×R/100)=19×60×100×v/(2Π×R)=18143.663512×v/R,R单位cm
 *备注:麦轮半径参数在上方宏定义Mecanum_WheelRadius修改,默认7cm,转换参数2591.95
 *备注:底盘中心到轮子中心的距离由上方宏定义x轴分量Mecanum_rx和y轴分量Mecanum_ry决定
 */
void Mecanum_InverseMotionControl(float v_x,float v_y,float w)
{
	//逆运动解算
	int16_t LeftFrontSpeed=(int16_t)((-v_x-v_y-w*(Mecanum_rx+Mecanum_ry)/100.0f)*18143.663512f/Mecanum_WheelRadius);//左前轮
	int16_t RightFrontSpeed=(int16_t)((v_x-v_y-w*(Mecanum_rx+Mecanum_ry)/100.0f)*18143.663512f/Mecanum_WheelRadius);//右前轮
	int16_t LeftRearSpeed=(int16_t)((-v_x+v_y-w*(Mecanum_rx+Mecanum_ry)/100.0f)*18143.663512f/Mecanum_WheelRadius);//左后轮
	int16_t RightRearSpeed=(int16_t)((v_x+v_y-w*(Mecanum_rx+Mecanum_ry)/100.0f)*18143.663512f/Mecanum_WheelRadius);//右后轮
	
	Mecanum_ControlSpeed(LeftFrontSpeed,RightFrontSpeed,LeftRearSpeed,RightRearSpeed);//M3508速度控制
}

/*
 *函数简介:麦轮运动控制
 *参数说明:无
 *返回类型:无
 *备注:根据拨杆获得速度值,映射比例在上方宏定义Mecanum_LeverSpeedMapRate更改
 */
void Mecanum_MoveControl(void)
{
	float vx=(1024-Remote_RxData.Remote_R_UD)*(Remote_RxData.Remote_RS-1)*Mecanum_LeverSpeedMapRate;
	float vy=(1024-Remote_RxData.Remote_R_RL)*(Remote_RxData.Remote_RS-1)*Mecanum_LeverSpeedMapRate;
	float w=(1024-Remote_RxData.Remote_L_RL)*(Remote_RxData.Remote_RS-1)*2*Mecanum_LeverSpeedMapRate;//获取三个轴的速度
	
	Mecanum_Power=M3508_MotorStatus[Mecanum_LeftFrontWheel-0x201].Power
				  +M3508_MotorStatus[Mecanum_RightFrontWheel-0x201].Power
				  +M3508_MotorStatus[Mecanum_LeftRearWheel-0x201].Power
				  +M3508_MotorStatus[Mecanum_RightRearWheel-0x201].Power;//获取底盘功率
	Mecanum_InverseMotionControl(vx,vy,w);//麦轮逆解算控制
}

/*
 *函数简介:麦轮功率控制
 *参数说明:无
 *返回类型:无
 *备注:麦轮是否开启可功率控制模式决定于上方宏定义Mecanum_PowerControl
 *备注:麦轮处于可功率控制模式时,右拨杆调至功率控制挡位即开启功率控制
 *备注:麦轮处于无功率控制模式,右拨杆调至功率控制挡位视为慢速的普通运动控制
 *备注:麦轮平均功率限制值决定于上方宏定义Mecanum_PowerLimit
 *备注:100ms取一次底盘平均功率,通过速度增益系数和速度归一化控制速度大小,速度增益系数在[0,2]范围内
 *备注:速度归一化的标准值在上方宏定义Mecanum_PowerControlSpeedNormalizationValue修改,范围[0,3]
 *备注:增益系数的初始值在上方宏定义Mecanum_PowerControlGainCoefficientInitialValue修改
 *备注:速度转速转换系数:
 *	   w'=v/R (rad/s)=v/(2Π×R) (r/s)=60×v/(2Π×R) (r/min)=1/19 w
 *	   ⇒ w=19×60×v/(2Π×R/100)=19×60×100×v/(2Π×R)=18143.663512×v/R,R单位cm
 *备注:麦轮半径参数在上方宏定义Mecanum_WheelRadius修改,默认7cm,转换参数2591.95
 *备注:底盘中心到轮子中心的距离由上方宏定义x轴分量Mecanum_rx和y轴分量Mecanum_ry决定
 */
void Mecanum_PowerMoveControl(void)
{
	static float GainCoefficient=1.0f;//速度增益系数
	
	Mecanum_Power=M3508_MotorStatus[Mecanum_LeftFrontWheel-0x201].Power
				  +M3508_MotorStatus[Mecanum_RightFrontWheel-0x201].Power
				  +M3508_MotorStatus[Mecanum_LeftRearWheel-0x201].Power
				  +M3508_MotorStatus[Mecanum_RightRearWheel-0x201].Power;//获取底盘功率
	if(Mecanum_PowerControl==1)//麦轮处于可功率控制模式
	{
		static uint8_t Count=0;//计数器,100ms取一次平均功率
		static float PowerSum=0;//100ms内的功率积分
		PowerSum+=Mecanum_Power;
		
		float vx=1024-Remote_RxData.Remote_R_UD;
		float vy=1024-Remote_RxData.Remote_R_RL;
		float w=1024-Remote_RxData.Remote_L_RL;//获取三个轴的速度参量
		float sigma=sqrtf(vx*vx+vy*vy);//获取xy轴速度归一化系数
		if(sigma!=0)//x,y轴速度归一化(正交合成速度不变为标准值)
		{
			vx=vx/sigma*Mecanum_PowerControlSpeedNormalizationValue;
			vy=vy/sigma*Mecanum_PowerControlSpeedNormalizationValue;
		}
		if(w>0)w=2*Mecanum_PowerControlSpeedNormalizationValue;
		else if(w<0)w=-2*Mecanum_PowerControlSpeedNormalizationValue;
		else w=0;//z轴角速度归一化
		
		if(Count==50)//100ms时
		{
			float Power_avg=PowerSum/100.0f;//平均功率
			Count=0;//计数器清零
			PowerSum=0;//积分数据清零
			
			if(Power_avg>Mecanum_PowerLimit+5)//高于平均功率限制
			{
				GainCoefficient-=0.01f;//增益系数递增
				if(GainCoefficient<0)GainCoefficient=0;//增益系数限下幅0
			}
			if(Power_avg<Mecanum_PowerLimit-5)//低于平均功率限制
			{
				GainCoefficient+=0.01f;//增益系数递减
				if(GainCoefficient>2)GainCoefficient=2;//增益系数限上幅2
			}
		}
		else if(vx!=0 || vy!=0 || w!=0)//正常运动
			Count++;
		else//停止运动
			GainCoefficient=Mecanum_PowerControlGainCoefficientInitialValue;//增益系数回归初始值
		
		int16_t LeftFrontSpeed=(int16_t)((-vx-vy-w*(Mecanum_rx+Mecanum_ry)/100.0f)*18143.663512f/Mecanum_WheelRadius*GainCoefficient);//左前
		int16_t RightFrontSpeed=(int16_t)((vx-vy-w*(Mecanum_rx+Mecanum_ry)/100.0f)*18143.663512f/Mecanum_WheelRadius*GainCoefficient);//右前
		int16_t LeftRearSpeed=(int16_t)((-vx+vy-w*(Mecanum_rx+Mecanum_ry)/100.0f)*18143.663512f/Mecanum_WheelRadius*GainCoefficient);//左后
		int16_t RightRearSpeed=(int16_t)((vx+vy-w*(Mecanum_rx+Mecanum_ry)/100.0f)*18143.663512f/Mecanum_WheelRadius*GainCoefficient);//右后
		
		Mecanum_ControlSpeed(LeftFrontSpeed,RightFrontSpeed,LeftRearSpeed,RightRearSpeed);
	}
	else//麦轮处于无功率控制模式,视为慢速的普通运动控制
		Mecanum_MoveControl();//普通麦轮运动
}
