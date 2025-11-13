#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "RM_C.h"
#include "XiaoHao.h"

int Sehuan_Position;

void XiaoHao_Init(void)
{
	Key_TIMScanInit();
	StepMotor_Init();
	Chassis_Init();
	HWT101_Init();
	
	Camera_Init();
	OLED_Init();
	QR_Init();
	
	OLED_ShowString(1,1,"Initing...");
	Gimbal_Init();
	OLED_ShowString(1,1,"          ");
}

void XiaoHao_StartToPlate(void)//出发到物料盘
{
//	Chassis_MovePlus(-120,120,0,50);//出发到二维码处
//	Chassis_MovePlus(0,200,10,50);
	Chassis_MoveTwoPath(-100,100,10,0,200,0,10,90,50);//出发到二维码处

	while(QR_Data[0]==0)//扫描二维码
	{
		QR_Open();
		Delay_ms(30);
	}
	OLED_Show(QR_Data);//显示识别结果
	
	Chassis_MovePlus(30,200,5,100);//前往物料盘
	Gimbal_Ready2();//准备抓物料
	while(1);
}

void XiaoHao_GetWuLiao(uint8_t Flag)//1-First 2-Second
{
	static uint8_t Count=0;
	do {Camera_SendByte(0xB2);Delay_ms(10);}//摄像头进入物料模式
	while(Camera_FLAG!=0xB2);

	while(Camera_Wuliao[QR_Data[4*(Flag-1)]-'1'][0]!=0 || Camera_Wuliao[QR_Data[4*(Flag-1)]-'1'][1]!=0);//物料从视野外进入
	for(int i=0;i<3;i++)//抓取
	{
		Arm_WuliaoSet(QR_Data[i+4*(Flag-1)]-'0');//物料盘转到对应颜色
		while(1)//物料稳定下来
		{
			int16_t Dingweix=Camera_Wuliao[QR_Data[i+4*(Flag-1)]-'1'][0];
			Delay_ms(100);
			int16_t Dingweix2=Camera_Wuliao[QR_Data[i+4*(Flag-1)]-'1'][0];
			if(Dingweix==0 || Dingweix2==0)continue;
			if(Dingweix-Dingweix2<2 && Dingweix-Dingweix2>-2)Count++;
			
			if(Count>2 || (i>0 && Count>0))
			{
				Count=0;
				break;
			}
		}
		if(i==0)Chassis_Orientation2(QR_Data[i+4*(Flag-1)]-'0');//初始物料定位
		else{Delay_ms(1000);}
		Arm_CatchSet(3);
		Gimbal_ArmGetFromPlate();//抓取
	}
}

void XiaoHao_PlateToSeHuan1(uint8_t Flag)
{
	Gimbal_Sleep();

	Chassis_MovePlus(-50,-150,0,80);
	Chassis_TurnRight();
	Chassis_MovePlus(0,-200,130,100);
	Chassis_TurnRight();
	Chassis_MovePlus(100,0,0,30);

	Gimbal_Ready1();
}

void XiaoHao_SeHuanProcess(uint8_t SeHuan_Select,uint8_t Flag)
{
	do {Camera_SendByte(0xB3);Camera_SendByte(0xC1);Delay_ms(10);}
	while(Camera_FLAG!=0xB3);
	
	float Delta_Position;
	Sehuan_Position=0;
	uint16_t SehuanSelect;
	for(uint8_t i=0;i<3;i++)
	{
		Arm_WuliaoSet(QR_Data[i+4*(Flag-1)]-'0');
		
		switch(QR_Data[i+4*(Flag-1)]-'0')
		{
			case 1:
				Delta_Position=150-Sehuan_Position;
				Sehuan_Position=150;
				SehuanSelect=0xC0;
				break;
			case 2:
				Delta_Position=0-Sehuan_Position;
				Sehuan_Position=0;
				SehuanSelect=0xC1;
				break;
			case 3:
				Delta_Position=-150-Sehuan_Position;
				Sehuan_Position=-150;
				SehuanSelect=0xC2;
				break;
		}
		Chassis_DeltaMove(Delta_Position,0);
		
		do {Camera_SendByte(0xB3);Camera_SendByte(SehuanSelect);Delay_ms(10);}
		while(Camera_FLAG!=0xB3);
		
		Chassis_Orientation(QR_Data[i+4*(Flag-1)]-'0');
		//Delay_ms(100);
		Gimbal_ArmPutToGround();
	}
	
	if(SeHuan_Select==1)
	{
		do {Camera_SendByte(0xB2);Delay_ms(10);}
		while(Camera_FLAG!=0xB2);
	}
	for(uint8_t i=0;i<3 && SeHuan_Select==1;i++)
	{
		Arm_WuliaoSet(QR_Data[i+4*(Flag-1)]-'0');
		
		switch(QR_Data[i+4*(Flag-1)]-'0')
		{
			case 1:
				Delta_Position=150-Sehuan_Position;
				Sehuan_Position=150;
				break;
			case 2:
				Delta_Position=0-Sehuan_Position;
				Sehuan_Position=0;
				break;
			case 3:
				Delta_Position=-150-Sehuan_Position;
				Sehuan_Position=-150;
				break;
		}
		Chassis_DeltaMove(Delta_Position,0);
		
		Chassis_Orientation4(QR_Data[i+4*(Flag-1)]-'0');
		//Delay_ms(100);
		Gimbal_ArmGetFromGround();
	}
}

void XiaoHao_SeHuan1ToSeHuan2(uint8_t Flag)
{
	Gimbal_Sleep();

	Chassis_MovePlus(-100,0,0,30);
	switch(Sehuan_Position)
	{
		case 150:Chassis_MovePlus(0,-200,0,100);break;
		case 0:Chassis_MovePlus(0,-200,15,100);break;
		case -150:Chassis_MovePlus(0,-200,35,100);break;
	}
	Chassis_TurnRight();
	//Chassis_MovePlus(20,-200,15,100);
	Chassis_MovePlus(5,-200,15,100);

	Gimbal_Ready1();
}
void XiaoHao_SeHuan2ToPlate(void)
{
	Gimbal_Sleep();
	
	Chassis_MovePlus(-100,0,0,30);
	switch(Sehuan_Position)
	{
		case 150:Chassis_MovePlus(0,-200,5,100);break;
		case 0:Chassis_MovePlus(0,-200,25,100);break;
		case -150:Chassis_MovePlus(0,-200,50,100);break;
	}
	Chassis_TurnRight();
	Chassis_MovePlus(20,-150,10,80);

	Gimbal_Ready2();
}

void XiaoHao_MaDuo(void)
{
	Delay_ms(500);
	Camera_SendByte(0xB2);
	while(Camera_FLAG!=0xB2)
	{
		Camera_SendByte(0xB2);
		Delay_ms(10);
	}
	
	float Sehuan_Position=0,Delta_Position;
	for(uint8_t i=0;i<3;i++)
	{
		Arm_WuliaoSet(QR_Data[i+4]-'0');
		
		switch(QR_Data[i+4]-'0')
		{
			case 1:
				Delta_Position=150-Sehuan_Position;
				Sehuan_Position=150;
				break;
			case 2:
				Delta_Position=0-Sehuan_Position;
				Sehuan_Position=0;
				break;
			case 3:
				Delta_Position=-150-Sehuan_Position;
				Sehuan_Position=-150;
				break;
		}
		Chassis_DeltaMove(Delta_Position,0);
		
		Chassis_Orientation3(QR_Data[i+4]-'0');
		Delay_ms(100);
		Gimbal_ArmPutToWuliao();
	}
	Arm_WuliaoSet(QR_Data[4]-'0');
}
void XiaoHao_GoHome(void)
{
	Gimbal_Sleep();
	
	Chassis_MovePlus(-100,0,0,30);
	switch(Sehuan_Position)
	{
		case 150:Chassis_MovePlus(0,-200,5,100);break;
		case 0:Chassis_MovePlus(0,-200,25,100);break;
		case -150:Chassis_MovePlus(0,-200,50,100);break;
	}
	Chassis_TurnRight();
	Chassis_MovePlus(0,-200,160,100);
	Chassis_MovePlus(100,0,10,50);
}
