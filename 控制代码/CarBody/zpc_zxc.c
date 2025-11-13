#include "zpc_zxc_Headfile.h"

/*
 *函数简介:初始化
 *参数说明:无
 *返回类型:无
 *备注:无
 */
void zpc_zxc_Init(void)
{
	Delay_ms(100);
	
	Key_TIMScanInit();
	OLED_Init();
	QR_Init();
	
	Gimbal_Init();
	Chassis_Init();
	
	OLED_ShowString(1,1,"Initing...");
	Delay_ms(100);
	Chassis_Reset();
	OLED_ShowString(1,1,"          ");
}

/*
 *函数简介:出发到转盘
 *参数说明:无
 *返回类型:无
 *备注:无
 *运行前姿态:Yaw轴Back,夹爪IDLE,物料盘Red,升降台Init
 *运行后姿态:Yaw轴Back,夹爪IDLE,物料盘Red,升降台Init
 */
void zpc_zxc_StartToPlate(void)
{
	#ifdef Fast
		Chassis_MovePath(Chassis_Path_StartToQR);//发车区到二维码
	#else
		Chassis_MoveOncePath(Chassis_Path_StartToQR);//发车区到二维码
	#endif
		
	QR_Scan();//扫描二维码
	OLED_ShowTestNum(Test);//显示任务码
	
	Chassis_Move(Chassis_Path_QRToPlate);//二维码到转盘
}

/*
 *函数简介:转盘准备抓取
 *参数说明:Color		应传入转盘定位颜色,即本轮抓取第一个颜色
 *返回类型:无
 *备注:无
 *运行前姿态:Yaw轴Back,夹爪IDLE,物料盘Red,升降台Init
 *运行后姿态:Yaw轴OUT,夹爪Open,物料盘Color,升降台Init
 */
void zpc_zxc_ReadyGetFromPlate(zpc_zxc_Color Color)
{
	Arm_YawSet(OUT);
	Arm_GripperSet(Open);
	Arm_StoreSet(Color);
	Camera_GotoMode(WuLiao);
	Delay_ms(350);
}

/*
 *函数简介:检查转盘状态
 *参数说明:Color					物料颜色
 *返回类型:zpc_zxc_PlateStatus		转盘状态
 *备注:无
 */
zpc_zxc_PlateStatus zpc_zxc_CheckPlateStatus(zpc_zxc_Color Color)
{
	if(Camera_WuLiao[Color][X]==0 && Camera_WuLiao[Color][Y]==0)return NotInPlate;//未发现物料
	
	int16_t StartPosition_X=Camera_WuLiao[Color][X],StartPosition_Y=Camera_WuLiao[Color][Y];
	Delay_ms(100);
	int16_t EndPosition_X=Camera_WuLiao[Color][X],EndPosition_Y=Camera_WuLiao[Color][Y];
	
	if((StartPosition_X==0 && StartPosition_Y==0) || (EndPosition_X==0 && EndPosition_Y==0))return NotInPlate;//两帧中出现没发现物料的情况
	
	float DeltaL_Square=(EndPosition_X-StartPosition_X)*(EndPosition_X-StartPosition_X)+(EndPosition_Y-StartPosition_Y)*(EndPosition_Y-StartPosition_Y);//两帧的物料位移
	if(DeltaL_Square<3)return PlateStatic;
	else return PlateMoving;
}

/*
 *函数简介:抓取物料
 *参数说明:i		第i圈	i=0,1
 *返回类型:无
 *备注:在路径调试时会仅仅进行红色物料定位
 *运行前姿态:Yaw轴Back,夹爪IDLE,物料盘Red,升降台Init
 *运行后姿态:Yaw轴Back,夹爪IDLE,升降台Init
 */
void zpc_zxc_GetWuLiao(uint8_t i)
{
	#ifndef PathDebug
		zpc_zxc_Color TargetCatchColor;
		
		//第一个物料
		TargetCatchColor=Test[4*i]-'1';
		zpc_zxc_ReadyGetFromPlate(TargetCatchColor);

		while(zpc_zxc_CheckPlateStatus(TargetCatchColor)!=PlateMoving);
		while(zpc_zxc_CheckPlateStatus(TargetCatchColor)!=PlateStatic);//等待第一个物料从转动到停止
		
		Chassis_WuLiaoOrientate(&Chassis_PlateOrientation,TargetCatchColor,2000);
		Gimbal_ArmGetFromPlate(0);
		
		//第二个物料
		TargetCatchColor=Test[1+4*i]-'1';
		Arm_StoreSet(TargetCatchColor);
		while(zpc_zxc_CheckPlateStatus(TargetCatchColor)!=PlateStatic);
		Gimbal_ArmGetFromPlate(1);

		//第三个物料
		TargetCatchColor=Test[2+4*i]-'1';
		Arm_StoreSet(TargetCatchColor);
		while(zpc_zxc_CheckPlateStatus(TargetCatchColor)!=PlateStatic);
		Gimbal_ArmGetFromPlate(2);
	#else
		//路径调试
		zpc_zxc_ReadyGetFromPlate(Red);
		Chassis_WuLiaoOrientate(&Chassis_PlateOrientation,Red,2000);
		Gimbal_Sleep();
	#endif
}

/*
 *函数简介:转盘到粗加工区
 *参数说明:无
 *返回类型:无
 *备注:无
 *运行前姿态:Yaw轴Back,夹爪IDLE,升降台Init
 *运行后姿态:Yaw轴Back,夹爪IDLE,升降台Init
 */
void zpc_zxc_PlateToSeHuan1(void)
{
	#if defined(CarSexENABLE) && defined(WuLiao1)
		Arm_StoreGSCarSex_ENABLE(ENABLE);
	#endif
	
	#ifdef Fast
		Chassis_MovePath(Chassis_Path_PlateToSeHuan1);
	#else
		Chassis_MoveOncePath(Chassis_Path_PlateToSeHuan1);
	#endif
	
	#if defined(CarSexENABLE) && defined(WuLiao1)
		Arm_StoreGSCarSex_ENABLE(DISABLE);
	#endif
}

/*
 *函数简介:色环准备放物料
 *参数说明:i			第i个物料		i=0,1,2
 *参数说明:Color		待放物料颜色
 *参数说明:DeltaMove	增量位移
 *参数说明:Work			色环工作
 *返回类型:无
 *备注:无
 *运行前姿态:
 *		i=0		Yaw轴Back,夹爪IDLE,升降台Init
 *		i=1,2	Yaw轴OUT,夹爪Open,升降台Watch
 *运行后姿态:Yaw轴OUT,夹爪Open,物料盘Color,升降台Watch,增量移动DeltaMove
 */
void zpc_zxc_ReadyPutToSeHuan(uint8_t i,zpc_zxc_Color Color,Chassis_DeltaMoveEnum DeltaMove,zpc_zxc_SeHuanWork Work)
{
	if(i==0)Arm_YawSet(OUT);
	
	if(Work==Normal)Camera_GotoMode(Color+0);
	else if(Work==MaDuo && i==0){Camera_GotoMode(WuLiao);Delay_ms(100);}
	
	Arm_StoreSet(Color);
	Chassis_DeltaMove(DeltaMove);
	if(i==0)Arm_GripperSet(Open);
	if(i==0 || Work==MaDuo)Arm_ArmSet(Watch,1000);
}

/*
 *函数简介:色环准备抓物料
 *参数说明:i			第i个物料		i=0,1,2
 *参数说明:Color		待抓物料颜色
 *参数说明:DeltaMove	增量位移
 *返回类型:无
 *备注:无
 *运行前姿态:Yaw轴OUT,夹爪Open,升降台Watch
 *运行后姿态:Yaw轴OUT,夹爪Open,物料盘Color,升降台Watch,增量移动DeltaMove
 */
void zpc_zxc_ReadyGetFromSeHuan(uint8_t i,zpc_zxc_Color Color,Chassis_DeltaMoveEnum DeltaMove)
{
	if(i==0)Camera_GotoMode(WuLiao);
	Arm_StoreSet(Color);
	Chassis_DeltaMove(DeltaMove);
}

/*
 *函数简介:粗加工区处理
 *参数说明:i		第i圈	i=0,1
 *返回类型:无
 *备注:在路径调试时会仅仅进行绿色色环定位
 *运行前姿态:Yaw轴Back,夹爪IDLE,升降台Init
 *运行后姿态:Yaw轴Back,夹爪IDLE,升降台Init
 */
void zpc_zxc_SeHuan1Process(uint8_t i)
{
	#ifndef PathDebug
		zpc_zxc_Color NowSeHuanColor=Green,NextSeHuanColor;
		
		//放物料
		for(uint8_t j=0;j<3;j++)
		{
			NextSeHuanColor=Test[j+4*i]-'1';
			zpc_zxc_ReadyPutToSeHuan(j,NextSeHuanColor,NowSeHuanColor-NextSeHuanColor,Normal);
			NowSeHuanColor=NextSeHuanColor;
			Chassis_SeHuanOrientate(&Chassis_SeHuanOrientation[NowSeHuanColor],NowSeHuanColor,750);
			Gimbal_ArmPutToGround(-1);
		}
		
		//抓物料
		for(uint8_t j=0;j<3;j++)
		{
			NextSeHuanColor=Test[j+4*i]-'1';
			zpc_zxc_ReadyGetFromSeHuan(j,NextSeHuanColor,NowSeHuanColor-NextSeHuanColor);
			NowSeHuanColor=NextSeHuanColor;
			Chassis_WuLiaoOrientate(&Chassis_WuLiaoOnSeHuanOrientation,NowSeHuanColor,750);
			Gimbal_ArmGetFromGround(j);
		}
	#else
		//路径调试
		zpc_zxc_ReadyPutToSeHuan(0,Green,0,Normal);
		Chassis_SeHuanOrientate(&Chassis_SeHuanOrientation[Green],Green,750);
		Gimbal_Sleep();
	#endif
}

/*
 *函数简介:粗加工区到半成品区
 *参数说明:i		第i圈	i=0,1
 *返回类型:无
 *备注:在路径调试时会仅仅进行绿色色环到绿色色环移动
 *运行前姿态:Yaw轴Back,夹爪IDLE,升降台Init
 *运行后姿态:Yaw轴Back,夹爪IDLE,升降台Init
 */
void zpc_zxc_SeHuan1ToSeHuan2(uint8_t i)
{
	#if defined(CarSexENABLE) && defined(WuLiao1)
		Arm_StoreGSCarSex_ENABLE(ENABLE);
	#endif

	#ifndef PathDebug
		zpc_zxc_Color NowPositionColor=Test[2+4*i]-'1',TargetPositionColor=Test[4*i]-'1';
	#else
		zpc_zxc_Color NowPositionColor=Green,TargetPositionColor=Green;
	#endif
	
	#ifdef Fast
		Chassis_MovePath(Chassis_Path_SeHuan1ToSeHuan2[NowPositionColor][TargetPositionColor]);
	#else
		Chassis_MoveOncePath(Chassis_Path_SeHuan1ToSeHuan2[NowPositionColor][TargetPositionColor]);
	#endif
	
	#if defined(CarSexENABLE) && defined(WuLiao1)
		Arm_StoreGSCarSex_ENABLE(DISABLE);
	#endif
}

/*
 *函数简介:成品区处理
 *参数说明:i		第i圈	i=0,1
 *返回类型:无
 *备注:在路径调试时会仅仅进行绿色色环定位
 *运行前姿态:Yaw轴Back,夹爪IDLE,升降台Init
 *运行后姿态:Yaw轴Back,夹爪IDLE,升降台Init
 */
void zpc_zxc_SeHuan2Process(uint8_t i)
{
	#ifndef PathDebug
		zpc_zxc_Color NowSeHuanColor=Test[4*i]-'1',NextSeHuanColor;
		
		for(uint8_t j=0;j<3;j++)
		{
			NextSeHuanColor=Test[j+4*i]-'1';
			if(i==0)zpc_zxc_ReadyPutToSeHuan(j,NextSeHuanColor,NowSeHuanColor-NextSeHuanColor,Normal);
			else if(i==1)zpc_zxc_ReadyPutToSeHuan(j,NextSeHuanColor,NowSeHuanColor-NextSeHuanColor,MaDuo);
			NowSeHuanColor=NextSeHuanColor;
			
			if(i==0)
			{
				Chassis_SeHuanOrientate(&Chassis_SeHuanOrientation[NowSeHuanColor],NowSeHuanColor,750);
				Gimbal_ArmPutToGround(j);
			}
			else if(i==1)
			{
				Chassis_WuLiaoOrientate(&Chassis_MaDuoOrientation[NowSeHuanColor],NowSeHuanColor,750);
				Gimbal_ArmPutToWuliao(j);
			}
		}
	#else
		//路径调试
		zpc_zxc_ReadyPutToSeHuan(0,Green,0,Normal);
		Chassis_SeHuanOrientate(&Chassis_SeHuanOrientation[Green],Green,750);
		Gimbal_Sleep();
	#endif
}

/*
 *函数简介:成品区到转盘
 *参数说明:无
 *返回类型:无
 *备注:在路径调试时会仅仅进行绿色色环到转盘的移动
 *运行前姿态:Yaw轴Back,夹爪IDLE,升降台Init
 *运行后姿态:Yaw轴Back,夹爪IDLE,升降台Init
 */
void zpc_zxc_SeHuan2ToPlate(void)
{
	#ifndef PathDebug
		zpc_zxc_Color NowPositionColor=Test[2]-'1';
	#else
		zpc_zxc_Color NowPositionColor=Green;
	#endif
	
	#ifdef Fast
		Chassis_MovePath(Chassis_Path_SeHuan2ToPlate[NowPositionColor]);
	#else
		Chassis_MoveOncePath(Chassis_Path_SeHuan2ToPlate[NowPositionColor]);
	#endif
}

/*
 *函数简介:成品区回家
 *参数说明:无
 *返回类型:无
 *备注:在路径调试时会仅仅进行绿色色环回家的移动
 *运行前姿态:Yaw轴Back,夹爪IDLE,升降台Init
 *运行后姿态:Yaw轴Back,夹爪IDLE,升降台Init
 */
void zpc_zxc_GoHome(void)
{
	#ifndef PathDebug
		zpc_zxc_Color NowPositionColor=Test[6]-'1';
	#else
		zpc_zxc_Color NowPositionColor=Green;
	#endif
	
	#ifdef Fast
		Chassis_MovePath(Chassis_Path_GoHome[NowPositionColor]);
	#else
		Chassis_MoveOncePath(Chassis_Path_GoHome[NowPositionColor]);
	#endif
}
