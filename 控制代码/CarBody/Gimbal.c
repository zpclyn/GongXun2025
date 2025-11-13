#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "Delay.h"
#include "Arm.h"
#include "Camera.h"
#include "Gimbal.h"
#include "zpc_zxc_Parameter.h"

/*
 *函数简介:云台初始化
 *参数说明:i		第i次抓取	i=0,1,2
 *返回类型:无
 *备注:无
 *运行后姿态:Yaw轴Back,夹爪IDLE,,物料盘Red,升降台Init
 */
void Gimbal_Init(void)
{
	Arm_Init();
	Camera_Init();
	
	Arm_ArmSetExt(Init,1000,200);
	Arm_YawSet(Back);
	Arm_StoreSet(Red);
	Arm_GripperSet(IDLE);
	Camera_GotoMode(SeHuan_R);
}

/*
 *函数简介:云台休眠
 *参数说明:无
 *返回类型:无
 *备注:无
 *运行后姿态:Yaw轴Back,夹爪IDLE,物料盘Red,升降台Init
 */
void Gimbal_Sleep(void)
{
	Arm_ArmSetExt(Init,1000,200);
	Delay_ms(100);
	Arm_YawSet(Back);
	Arm_StoreSet(Red);
	Arm_GripperSet(IDLE);
}

/*
 *函数简介:云台从转盘抓取
 *参数说明:i		第i次抓取	i=0,1,2
 *返回类型:无
 *备注:无
 *运行前姿态:
 *		i=0		Yaw轴OUT,夹爪Open,升降台Init
 *		i=1,2	Yaw轴OUT,夹爪Open,升降台Plate
 *运行后姿态:
 *		i=0,1	Yaw轴OUT,夹爪Open,升降台Plate
 *		i=2		Yaw轴Back,夹爪IDLE,升降台Init
 */
void Gimbal_ArmGetFromPlate(uint8_t i)
{
	if(i==0)	{Arm_ArmSet(Plate,1000);			Delay_ms(25+Gimbal_MoveRemain);}	//下降(第一次)
				{Arm_GripperSet(Catch);				Delay_ms(50+Gimbal_MoveRemain);}	//抓取
				{Arm_ArmSet(Store_Up,1000);			Delay_ms(0+Gimbal_MoveRemain);}		//抬升
				{Arm_YawSet(Back);					Delay_ms(140+Gimbal_MoveRemain);}	//旋转
				{Arm_ArmSet(Store_Down,300);		Delay_ms(90+Gimbal_MoveRemain);}	//放下
				{Arm_GripperSet(IDLE);				Delay_ms(30+Gimbal_MoveRemain);}	//松开
	if(i!=2)	{Arm_ArmSet(Init,1000);				Delay_ms(10+Gimbal_MoveRemain);}	//抬起来(第一次和第二次)
	if(i==2)	{Arm_ArmSet(Init,1000);				}									//抬起来(第三次)
	if(i!=2)	{Arm_YawSet(OUT);					Delay_ms(80+Gimbal_MoveRemain);}	//转回来(第一次和第二次)
	if(i!=2)	{Arm_GripperSet(Open);				Delay_ms(180+Gimbal_MoveRemain);}	//张开(第一次和第二次)
	if(i!=2)	{Arm_ArmSetExt(Plate,1000,250);		Delay_ms(100+Gimbal_MoveRemain);}	//下降(第一次和第二次)
}

/*
 *函数简介:云台从地面抓取
 *参数说明:i		第i次抓取	i=0,1,2
 *返回类型:无
 *备注:无
 *运行前姿态:
 *		i=0		Yaw轴OUT,夹爪Open,升降台Init
 *		i=1,2	Yaw轴OUT,夹爪Open,升降台Watch
 *运行后姿态:
 *		i=0,1	Yaw轴OUT,夹爪Open,升降台Watch
 *		i=2		Yaw轴Back,夹爪IDLE,升降台Init
 */
void Gimbal_ArmGetFromGround(uint8_t i)
{
				{Arm_ArmSet(Ground,1000);		Delay_ms(10+Gimbal_MoveRemain);}	//下降
				{Arm_GripperSet(Catch);			Delay_ms(40+Gimbal_MoveRemain);}	//抓取
				{Arm_ArmSet(Store_Up,1000);		Delay_ms(20+Gimbal_MoveRemain);}	//抬升
				{Arm_YawSet(Back);				Delay_ms(170+Gimbal_MoveRemain);}	//旋转
				{Arm_ArmSet(Store_Down,300);	Delay_ms(75+Gimbal_MoveRemain);}	//放下
				{Arm_GripperSet(IDLE);			Delay_ms(30+Gimbal_MoveRemain);}	//松开
	#ifdef WuLiao1
	if(i!=2)	{Arm_ArmSet(Init,1000);		Delay_ms(0+50+Gimbal_MoveRemain);}		//抬升(第一次和第二次)
	#else
	if(i!=2)	{Arm_ArmSet(Store_Up,1000);		Delay_ms(0+Gimbal_MoveRemain);}		//抬升(第一次和第二次)
	#endif
	if(i==2)	{Arm_ArmSet(Init,1000);			}									//抬升(第三次)
	if(i!=2)	{Arm_YawSet(OUT);				Delay_ms(70+Gimbal_MoveRemain);}	//转回来(第一次和第二次)
	if(i!=2)	{Arm_ArmSet(Watch,1000);		Delay_ms(30+Gimbal_MoveRemain);}	//下降(第一次和第二次)
	if(i!=2)	{Arm_GripperSet(Open);			}									//张开(第一次和第二次)
}

/*
 *函数简介:云台放置到地面
 *参数说明:i		第i次放置	i=-1,0,1,2(-1表示粗加工区放置,0~2表示精加工区得到第i次放置)
 *返回类型:无
 *备注:无
 *运行前姿态:Yaw轴OUT,夹爪Open,升降台Watch
 *运行后姿态:
 *		i=-1~1	Yaw轴OUT,夹爪Open,升降台Watch
 *		i=2		Yaw轴Back,夹爪IDLE,升降台Init
 */
void Gimbal_ArmPutToGround(int8_t i)
{
				{Arm_GripperSet(IDLE);				}									//缩回夹爪
				{Arm_ArmSet(Store_Up,1000);			Delay_ms(0+Gimbal_MoveRemain);}		//抬起来
				{Arm_YawSet(Back);					Delay_ms(140+Gimbal_MoveRemain);}	//旋转
				{Arm_ArmSet(Store_Down,300);		Delay_ms(60+Gimbal_MoveRemain);}	//下降
				{Arm_GripperSet(Catch);				Delay_ms(50+Gimbal_MoveRemain);}	//抓取
				{Arm_ArmSet(Store_Up,1000);			Delay_ms(0+Gimbal_MoveRemain);}		//抬起来
				{Arm_YawSet(OUT);					Delay_ms(35+Gimbal_MoveRemain);}	//转回来
				{Arm_ArmSetExt(Ground,1000,250);	Delay_ms(250+Gimbal_MoveRemain);}	//下降
	#ifdef WuLiao1
	if(i!=2)	{Arm_GripperSet(Open);				Delay_ms(50+100+Gimbal_MoveRemain);}	//松开(粗加工和精加工第一次第二次)
	if(i==2)	{Arm_GripperSet(IDLE);				Delay_ms(50+100+Gimbal_MoveRemain);}	//松开(精加工第三次)
	#else
	if(i!=2)	{Arm_GripperSet(Open);				Delay_ms(50+Gimbal_MoveRemain);}	//松开(粗加工和精加工第一次第二次)
	if(i==2)	{Arm_GripperSet(IDLE);				Delay_ms(30+Gimbal_MoveRemain);}	//松开(精加工第三次)
	#endif
	if(i!=2)	{Arm_ArmSet(Watch,1000);			}									//抬起来(粗加工和精加工第一次第二次)
	if(i==2)	{Arm_ArmSet(Init,1000);				Delay_ms(0+Gimbal_MoveRemain);}		//抬起来(精加工第三次)
	if(i==2)	{Arm_YawSet(Back);					}									//旋转(精加工第三次)
}

/*
 *函数简介:云台放置到物料
 *参数说明:i		第i次放置	i=0,1,2
 *返回类型:无
 *备注:无
 *运行前姿态:Yaw轴OUT,夹爪Open,升降台Watch
 *运行后姿态:
 *		i=0,1	Yaw轴OUT,夹爪Open,升降台Watch
 *		i=2		Yaw轴Back,夹爪IDLE,升降台Init
 */
void Gimbal_ArmPutToWuliao(uint8_t i)
{
				{Arm_GripperSet(IDLE);					}									//缩回夹爪
				{Arm_YawSet(Back);						Delay_ms(0+Gimbal_MoveRemain);}		//旋转
				{Arm_ArmSet(Store_Up,1000);				Delay_ms(140+Gimbal_MoveRemain);}	//抬起来
				{Arm_ArmSet(Store_Down,300);			Delay_ms(60+Gimbal_MoveRemain);}	//下降
				{Arm_GripperSet(Catch);					Delay_ms(30+Gimbal_MoveRemain);}	//抓取
				{Arm_ArmSet(Store_Up,1000);				Delay_ms(0+Gimbal_MoveRemain);}		//抬起来
				{Arm_YawSet(OUT);						Delay_ms(35+Gimbal_MoveRemain);}	//转回来
				{Arm_ArmSetExt(MaDuo_Up,1000,250);		Delay_ms(120+Gimbal_MoveRemain);}	//下降
				{Arm_ArmSetExt(MaDuo_Down,500,200);		Delay_ms(200+Gimbal_MoveRemain);}	//精下降
	if(i!=2)	{Arm_GripperSet(Open);					Delay_ms(30+Gimbal_MoveRemain);}	//松开(第一次和第二次)
	if(i==2)	{Arm_GripperSet(IDLE);					Delay_ms(30+Gimbal_MoveRemain);}	//松开(第三次)
	if(i!=2)	{Arm_ArmSet(Init,1000);					}									//抬起来(第一次和第二次)
	if(i==2)	{Arm_ArmSet(Init,1000);					Delay_ms(0+Gimbal_MoveRemain);}		//抬起来(第三次)
	if(i==2)	{Arm_YawSet(Back);						}									//旋转(第三次)
}
