#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "Chassis_Path.h"
#include "zpc_zxc_Parameter.h"
#include "zpc_zxc_Typedef.h"

Chassis_PathPoint Chassis_PathPoint_StartToQR[2];Chassis_Path Chassis_Path_StartToQR;							//出发到二维码
Chassis_PathPoint Chassis_PathPoint_QRToPlate[1];Chassis_Path Chassis_Path_QRToPlate;							//二维码到转盘
Chassis_PathPoint Chassis_PathPoint_PlateToSeHuan1[5];Chassis_Path Chassis_Path_PlateToSeHuan1;					//转盘到粗加工
Chassis_PathPoint Chassis_PathPoint_SeHuan1ToSeHuan2[3][3][5];Chassis_Path Chassis_Path_SeHuan1ToSeHuan2[3][3];	//粗加工到成品
Chassis_PathPoint Chassis_PathPoint_SeHuan2ToPlate[3][4];Chassis_Path Chassis_Path_SeHuan2ToPlate[3];			//成品到转盘
Chassis_PathPoint Chassis_PathPoint_GoHome[3][5];Chassis_Path Chassis_Path_GoHome[3];							//回家

/*
 *函数简介:底盘路径初始化
 *参数说明:无
 *返回类型:无
 *备注:无
 */
void Chassis_PathInit(void)
{
	/*===============出发到二维码===============*/
	#ifdef Fast
		Chassis_PathPoint_StartToQR[0].vx=-100+20;
		Chassis_PathPoint_StartToQR[0].vy=100;
		Chassis_PathPoint_StartToQR[0].Delta_Angle=Straight;
		Chassis_PathPoint_StartToQR[0].t=10;
		Chassis_PathPoint_StartToQR[0].K=10;
		
		Chassis_PathPoint_StartToQR[1].vx=-15;
		Chassis_PathPoint_StartToQR[1].vy=150;
		Chassis_PathPoint_StartToQR[1].Delta_Angle=Straight;
		Chassis_PathPoint_StartToQR[1].t=50-20;
		Chassis_PathPoint_StartToQR[1].K=200;
	#else
		Chassis_PathPoint_StartToQR[0].vx=-100;
		Chassis_PathPoint_StartToQR[0].vy=100;
		Chassis_PathPoint_StartToQR[0].Delta_Angle=Straight;
		Chassis_PathPoint_StartToQR[0].t=10;
		Chassis_PathPoint_StartToQR[0].K=100;
		
		Chassis_PathPoint_StartToQR[1].vx=0;
		Chassis_PathPoint_StartToQR[1].vy=150;
		Chassis_PathPoint_StartToQR[1].Delta_Angle=Straight;
		Chassis_PathPoint_StartToQR[1].t=30;
		Chassis_PathPoint_StartToQR[1].K=180;
	#endif
	
	Chassis_Path_StartToQR.End_K=150;
	Chassis_Path_StartToQR.Path_Size=2;
	Chassis_Path_StartToQR.Path=Chassis_PathPoint_StartToQR;

	/*===============二维码到转盘===============*/
	Chassis_PathPoint_QRToPlate[0].vx=35-20;
	Chassis_PathPoint_QRToPlate[0].vy=200;
	Chassis_PathPoint_QRToPlate[0].Delta_Angle=Straight;
	Chassis_PathPoint_QRToPlate[0].t=32;
	Chassis_PathPoint_QRToPlate[0].K=200;
	
	Chassis_Path_QRToPlate.Path=Chassis_PathPoint_QRToPlate;
	
	/*===============转盘到粗加工===============*/
	#ifdef Fast
		Chassis_PathPoint_PlateToSeHuan1[0].vx=-55-30;
		Chassis_PathPoint_PlateToSeHuan1[0].vy=-150-10;
		Chassis_PathPoint_PlateToSeHuan1[0].Delta_Angle=Straight;
		Chassis_PathPoint_PlateToSeHuan1[0].t=30+35+25;
		Chassis_PathPoint_PlateToSeHuan1[0].K=80;
		
		Chassis_PathPoint_PlateToSeHuan1[1].vx=-20;
		Chassis_PathPoint_PlateToSeHuan1[1].vy=0;
		Chassis_PathPoint_PlateToSeHuan1[1].Delta_Angle=TurnRight;
		Chassis_PathPoint_PlateToSeHuan1[1].t=0;
		Chassis_PathPoint_PlateToSeHuan1[1].K=100;
		
		Chassis_PathPoint_PlateToSeHuan1[2].vx=-10;
		Chassis_PathPoint_PlateToSeHuan1[2].vy=-200;
		Chassis_PathPoint_PlateToSeHuan1[2].Delta_Angle=Straight;
		Chassis_PathPoint_PlateToSeHuan1[2].t=0;
		Chassis_PathPoint_PlateToSeHuan1[2].K=150;

		Chassis_PathPoint_PlateToSeHuan1[3].vx=-10;
		Chassis_PathPoint_PlateToSeHuan1[3].vy=-300;
		Chassis_PathPoint_PlateToSeHuan1[3].Delta_Angle=Straight;
		Chassis_PathPoint_PlateToSeHuan1[3].t=95;
		Chassis_PathPoint_PlateToSeHuan1[3].K=150;

		Chassis_PathPoint_PlateToSeHuan1[4].vx=0;
		Chassis_PathPoint_PlateToSeHuan1[4].vy=-30;
		Chassis_PathPoint_PlateToSeHuan1[4].Delta_Angle=TurnRight;
		Chassis_PathPoint_PlateToSeHuan1[4].t=0;
		Chassis_PathPoint_PlateToSeHuan1[4].K=100;
	#else
		Chassis_PathPoint_PlateToSeHuan1[0].vx=-50;
		Chassis_PathPoint_PlateToSeHuan1[0].vy=-140;
		Chassis_PathPoint_PlateToSeHuan1[0].Delta_Angle=Straight;
		Chassis_PathPoint_PlateToSeHuan1[0].t=80;
		Chassis_PathPoint_PlateToSeHuan1[0].K=100;
		
		Chassis_PathPoint_PlateToSeHuan1[1].Delta_Angle=TurnRight;
		
		Chassis_PathPoint_PlateToSeHuan1[2].vx=-5;
		Chassis_PathPoint_PlateToSeHuan1[2].vy=-250;
		Chassis_PathPoint_PlateToSeHuan1[2].Delta_Angle=Straight;
		Chassis_PathPoint_PlateToSeHuan1[2].t=215;
		Chassis_PathPoint_PlateToSeHuan1[2].K=200;

		Chassis_PathPoint_PlateToSeHuan1[3].Delta_Angle=TurnRight;

		Chassis_PathPoint_PlateToSeHuan1[4].vx=60;
		Chassis_PathPoint_PlateToSeHuan1[4].vy=0;
		Chassis_PathPoint_PlateToSeHuan1[4].Delta_Angle=Straight;
		Chassis_PathPoint_PlateToSeHuan1[4].t=0;
		Chassis_PathPoint_PlateToSeHuan1[4].K=100;
	#endif
	
	Chassis_Path_PlateToSeHuan1.End_K=100;
	Chassis_Path_PlateToSeHuan1.Path_Size=5;
	Chassis_Path_PlateToSeHuan1.Path=Chassis_PathPoint_PlateToSeHuan1;

	/*===============粗加工到成品===============*/
	#ifdef Fast
		float SeHuan1ToSeHuan2_Path1_t[3];
		SeHuan1ToSeHuan2_Path1_t[Red]=100;
		SeHuan1ToSeHuan2_Path1_t[Green]=140;
		SeHuan1ToSeHuan2_Path1_t[Blue]=190;

		float SeHuan1ToSeHuan2_Path3_t[3];
		SeHuan1ToSeHuan2_Path3_t[Red]=97;
		SeHuan1ToSeHuan2_Path3_t[Green]=50;
		SeHuan1ToSeHuan2_Path3_t[Blue]=5;

		for(zpc_zxc_Color StartColor=Red;StartColor<=Blue;StartColor++)
			for(zpc_zxc_Color EndColor=Red;EndColor<=Blue;EndColor++)
			{
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][0].vx=-80;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][0].vy=0;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][0].Delta_Angle=Straight;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][0].t=10;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][0].K=100;
				
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][1].vx=0;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][1].vy=-200;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][1].Delta_Angle=Straight;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][1].t=SeHuan1ToSeHuan2_Path1_t[StartColor];
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][1].K=100;
				
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][2].vx=-50;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][2].vy=0;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][2].Delta_Angle=TurnRight;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][2].t=35;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][2].K=100;
				
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][3].vx=0;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][3].vy=-200;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][3].Delta_Angle=Straight;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][3].t=SeHuan1ToSeHuan2_Path3_t[EndColor];
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][3].K=200;

				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][4].vx=50;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][4].vy=-50;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][4].Delta_Angle=Straight;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][4].t=15;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][4].K=50;

				Chassis_Path_SeHuan1ToSeHuan2[StartColor][EndColor].End_K=150;
				Chassis_Path_SeHuan1ToSeHuan2[StartColor][EndColor].Path_Size=5;
				Chassis_Path_SeHuan1ToSeHuan2[StartColor][EndColor].Path=Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor];
			}
	#else
		float SeHuan1ToSeHuan2_Path1_t[3];
		SeHuan1ToSeHuan2_Path1_t[Red]=100;
		SeHuan1ToSeHuan2_Path1_t[Green]=140;
		SeHuan1ToSeHuan2_Path1_t[Blue]=180;

		float SeHuan1ToSeHuan2_Path3_t[3];
		SeHuan1ToSeHuan2_Path3_t[Red]=165;
		SeHuan1ToSeHuan2_Path3_t[Green]=125;
		SeHuan1ToSeHuan2_Path3_t[Blue]=80;

		for(zpc_zxc_Color StartColor=Red;StartColor<=Blue;StartColor++)
			for(zpc_zxc_Color EndColor=Red;EndColor<=Blue;EndColor++)
			{
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][0].vx=-100;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][0].vy=0;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][0].Delta_Angle=Straight;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][0].t=40;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][0].K=50;
				
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][1].vx=0;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][1].vy=-200;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][1].Delta_Angle=Straight;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][1].t=SeHuan1ToSeHuan2_Path1_t[StartColor];
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][1].K=120;
				
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][2].Delta_Angle=TurnRight;
				
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][3].vx=0;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][3].vy=-200;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][3].Delta_Angle=Straight;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][3].t=SeHuan1ToSeHuan2_Path3_t[EndColor];
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][3].K=120;

				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][4].vx=50;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][4].vy=-50;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][4].Delta_Angle=Straight;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][4].t=15;
				Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor][4].K=50;

				Chassis_Path_SeHuan1ToSeHuan2[StartColor][EndColor].Path_Size=5;
				Chassis_Path_SeHuan1ToSeHuan2[StartColor][EndColor].Path=Chassis_PathPoint_SeHuan1ToSeHuan2[StartColor][EndColor];
			}
	#endif

	/*===============成品到转盘===============*/
	#ifdef Fast
		float SeHuan2ToPlate_Path1_t[3];
		SeHuan2ToPlate_Path1_t[Red]=120;
		SeHuan2ToPlate_Path1_t[Green]=160;
		SeHuan2ToPlate_Path1_t[Blue]=210;

		for(zpc_zxc_Color StartColor=Red;StartColor<=Blue;StartColor++)
		{
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][0].vx=-80-20;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][0].vy=0;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][0].Delta_Angle=Straight;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][0].t=10;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][0].K=100;
			
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][1].vx=0;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][1].vy=-200;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][1].Delta_Angle=Straight;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][1].t=SeHuan2ToPlate_Path1_t[StartColor];
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][1].K=100;
			
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][2].vx=-50;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][2].vy=0;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][2].Delta_Angle=TurnRight;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][2].t=35;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][2].K=100;
			
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][3].vx=40-15;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][3].vy=-180;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][3].Delta_Angle=Straight;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][3].t=0;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][3].K=100;

			Chassis_Path_SeHuan2ToPlate[StartColor].End_K=150;
			Chassis_Path_SeHuan2ToPlate[StartColor].Path_Size=4;
			Chassis_Path_SeHuan2ToPlate[StartColor].Path=Chassis_PathPoint_SeHuan2ToPlate[StartColor];
		}
	#else
		float SeHuan2ToPlate_Path1_t[3];
		SeHuan2ToPlate_Path1_t[Red]=120;
		SeHuan2ToPlate_Path1_t[Green]=160;
		SeHuan2ToPlate_Path1_t[Blue]=200;

		for(zpc_zxc_Color StartColor=Red;StartColor<=Blue;StartColor++)
		{
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][0].vx=-100;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][0].vy=0;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][0].Delta_Angle=Straight;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][0].t=40;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][0].K=50;
			
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][1].vx=0;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][1].vy=-200;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][1].Delta_Angle=Straight;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][1].t=SeHuan2ToPlate_Path1_t[StartColor];
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][1].K=120;
			
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][2].Delta_Angle=TurnRight;
			
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][3].vx=20;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][3].vy=-130;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][3].Delta_Angle=Straight;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][3].t=80;
			Chassis_PathPoint_SeHuan2ToPlate[StartColor][3].K=120;

			Chassis_Path_SeHuan2ToPlate[StartColor].Path_Size=4;
			Chassis_Path_SeHuan2ToPlate[StartColor].Path=Chassis_PathPoint_SeHuan2ToPlate[StartColor];
		}
	#endif

	/*===============成品回家===============*/
	#ifdef Fast
		float GoHome_Path1_t[3];
		GoHome_Path1_t[Red]=120;
		GoHome_Path1_t[Green]=160;
		GoHome_Path1_t[Blue]=210;

		for(zpc_zxc_Color StartColor=Red;StartColor<=Blue;StartColor++)
		{
			Chassis_PathPoint_GoHome[StartColor][0].vx=-80;
			Chassis_PathPoint_GoHome[StartColor][0].vy=0;
			Chassis_PathPoint_GoHome[StartColor][0].Delta_Angle=Straight;
			Chassis_PathPoint_GoHome[StartColor][0].t=10;
			Chassis_PathPoint_GoHome[StartColor][0].K=100;
			
			Chassis_PathPoint_GoHome[StartColor][1].vx=0;
			Chassis_PathPoint_GoHome[StartColor][1].vy=-200;
			Chassis_PathPoint_GoHome[StartColor][1].Delta_Angle=Straight;
			Chassis_PathPoint_GoHome[StartColor][1].t=GoHome_Path1_t[StartColor];
			Chassis_PathPoint_GoHome[StartColor][1].K=100;
			
			Chassis_PathPoint_GoHome[StartColor][2].vx=-50;
			Chassis_PathPoint_GoHome[StartColor][2].vy=0;
			Chassis_PathPoint_GoHome[StartColor][2].Delta_Angle=TurnRight;
			Chassis_PathPoint_GoHome[StartColor][2].t=35;
			Chassis_PathPoint_GoHome[StartColor][2].K=100;
			
			Chassis_PathPoint_GoHome[StartColor][3].vx=-10;
			Chassis_PathPoint_GoHome[StartColor][3].vy=-300;
			Chassis_PathPoint_GoHome[StartColor][3].Delta_Angle=Straight;
			Chassis_PathPoint_GoHome[StartColor][3].t=165;
			Chassis_PathPoint_GoHome[StartColor][3].K=200;
			
			Chassis_PathPoint_GoHome[StartColor][4].vx=100;
			Chassis_PathPoint_GoHome[StartColor][4].vy=-100;
			Chassis_PathPoint_GoHome[StartColor][4].Delta_Angle=Straight;
			Chassis_PathPoint_GoHome[StartColor][4].t=25;
			Chassis_PathPoint_GoHome[StartColor][4].K=100;

			Chassis_Path_GoHome[StartColor].End_K=150;
			Chassis_Path_GoHome[StartColor].Path_Size=5;
			Chassis_Path_GoHome[StartColor].Path=Chassis_PathPoint_GoHome[StartColor];
		}
	#else
		float GoHome_Path1_t[3];
		GoHome_Path1_t[Red]=120;
		GoHome_Path1_t[Green]=160;
		GoHome_Path1_t[Blue]=200;

		for(zpc_zxc_Color StartColor=Red;StartColor<=Blue;StartColor++)
		{
			Chassis_PathPoint_GoHome[StartColor][0].vx=-100;
			Chassis_PathPoint_GoHome[StartColor][0].vy=0;
			Chassis_PathPoint_GoHome[StartColor][0].Delta_Angle=Straight;
			Chassis_PathPoint_GoHome[StartColor][0].t=50;
			Chassis_PathPoint_GoHome[StartColor][0].K=50;
			
			Chassis_PathPoint_GoHome[StartColor][1].vx=0;
			Chassis_PathPoint_GoHome[StartColor][1].vy=-200;
			Chassis_PathPoint_GoHome[StartColor][1].Delta_Angle=Straight;
			Chassis_PathPoint_GoHome[StartColor][1].t=GoHome_Path1_t[StartColor];
			Chassis_PathPoint_GoHome[StartColor][1].K=120;
			
			Chassis_PathPoint_GoHome[StartColor][2].Delta_Angle=TurnRight;
			
			Chassis_PathPoint_GoHome[StartColor][3].vx=-10;
			Chassis_PathPoint_GoHome[StartColor][3].vy=-250;
			Chassis_PathPoint_GoHome[StartColor][3].Delta_Angle=Straight;
			Chassis_PathPoint_GoHome[StartColor][3].t=305;
			Chassis_PathPoint_GoHome[StartColor][3].K=120;
			
			Chassis_PathPoint_GoHome[StartColor][4].vx=120;
			Chassis_PathPoint_GoHome[StartColor][4].vy=-100;
			Chassis_PathPoint_GoHome[StartColor][4].Delta_Angle=Straight;
			Chassis_PathPoint_GoHome[StartColor][4].t=25;
			Chassis_PathPoint_GoHome[StartColor][4].K=50;

			Chassis_Path_GoHome[StartColor].Path_Size=5;
			Chassis_Path_GoHome[StartColor].Path=Chassis_PathPoint_GoHome[StartColor];
		}
	#endif
}
