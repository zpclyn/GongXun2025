#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "Delay.h"

typedef struct
{
	uint8_t _Flag;		//修改标志位	0-目标状态被修改 1-目标状态未修改
	
	int8_t Dir;			//方向			0-逆时针 1-顺时针
	uint16_t Speed;		//速度
	uint8_t Acc;		//加速度
	uint32_t Step;		//步数
}StepMotor_Struct;//步进目标状态结构体

uint8_t StepMotor_TxData[13];//步进发送数据存储器
StepMotor_Struct StepMotor_Target[5];//步进目标状态(用以DMA发送)

/*
 *函数简介:步进硬件初始化
 *参数说明:无
 *返回类型:无
 *备注:采用TIM6定时中断+DMA发送
 *备注:默认采用PA9(USART1-Tx),默认定时1.5ms
 */
void StepMotor_HardwareInit(void)
{
	/*===============配置时钟===============*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//开启时钟
	
	/*===============配置GPIO===============*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//复用推挽
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//默认上拉
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);//初始化USART1-Tx(PA9)
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);//开启PA9的USART1复用模式
	
	/*===============配置USART和串口发送DMA===============*/
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=115200;//配置波特率115200
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//配置无硬件流控制
	USART_InitStructure.USART_Mode=USART_Mode_Tx;//配置为发送模式
	USART_InitStructure.USART_Parity=USART_Parity_No;//配置为无校验位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;//配置停止位为1
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;//配置字长8bit
	USART_Init(USART1,&USART_InitStructure);//初始化USART1
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_Channel=DMA_Channel_4;//选择DMA通道4
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;//普通模式(非自动重装)
	DMA_InitStructure.DMA_DIR=DMA_DIR_MemoryToPeripheral;//转运方向为存储器到外设
	DMA_InitStructure.DMA_BufferSize=1;//数据传输量为1字节
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;//高优先级
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)&(USART1->DR);//外设地址(USART的DR数据接收寄存器)
	DMA_InitStructure.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;//外设数据长度为1字节(8bits)
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//外设地址不自增
	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)StepMotor_TxData;//存储器地址(步进DMA数据存储器0)
	DMA_InitStructure.DMA_MemoryBurst=DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;//存储器数据长度为1字节(8bits)
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//存储器地址自增
	DMA_InitStructure.DMA_FIFOMode=DMA_FIFOMode_Disable;//不使用FIFO模式
	DMA_InitStructure.DMA_FIFOThreshold=DMA_FIFOStatus_1QuarterFull;//设置FIFO阈值为1/4(不使用FIFO模式时,此位无意义)
	DMA_Init(DMA2_Stream7,&DMA_InitStructure);//初始化数据流7
	
	/*===============配置定时器===============*/
	TIM_InternalClockConfig(TIM6);//选择时基单元的时钟
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;//配置时基单元（配置参数）
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;//配置时钟分频为1分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;//配置计数器模式为向上计数
	TIM_TimeBaseInitStructure.TIM_Period=42-1;//配置自动重装值ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler=1500-1;//配置分频值PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter=0;//配置重复计数单元的置为0
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);//初始化TIM6

	TIM_ClearFlag(TIM6,TIM_FLAG_Update);//清除配置时基单元产生的中断标志位

	/*===============定时器中断===============*/
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);//使能更新中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);//选择NVIC分组
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=TIM6_DAC_IRQn;//选择TIM6中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//使能中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;//响应优先级
	NVIC_Init(&NVIC_InitStructure);//初始化USART6的NVIC
	
	/*===============使能===============*/
	DMA_Cmd(DMA2_Stream7,ENABLE);//使能DMA2的数据流7
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);//使能串口USART1的DMA搬运
	USART_Cmd(USART1,ENABLE);//启动USART1
	TIM_Cmd(TIM6,DISABLE);//不启动定时器
}

/*
 *函数简介:步进DMA计数复位
 *参数说明:Size		发送数据个数
 *返回类型:无
 *备注:无
 */
void StepMotor_DMAReset(uint8_t Size)
{
	while(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)==RESET);//判断发送完成
	DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);//清除发送完成标志位
	DMA_Cmd(DMA2_Stream7,DISABLE);//失能DMA2的数据流7
	while(DMA_GetCmdStatus(DMA2_Stream7)!=DISABLE);//检测DMA2的数据流7为可配置状态
	DMA_SetCurrDataCounter(DMA2_Stream7,Size);//恢复传输计数器的值
	DMA_Cmd(DMA2_Stream7,ENABLE);//使能DMA2的数据流7
}

/*
 *函数简介:步进张大头使能控制指令
 *参数说明:ID		ID
 *返回类型:无
 *备注:默认不进行多机同步
 *指令:
 *		ID 0xF3 0xAB En Sync 0x6B
 *	  ID	地址
 *	  En	使能状态 		0-失能 1-使能
 *	  Sync	多机同步标志 	0-不多机同步 1-多机同步
 */
void StepMotor_ZDTENABLE(uint8_t ID)
{	
	StepMotor_TxData[0]=ID;
	StepMotor_TxData[1]=0xF3;
	StepMotor_TxData[2]=0xAB;
	StepMotor_TxData[3]=0x01;
	StepMotor_TxData[4]=0x00;
	StepMotor_TxData[5]=0x6B;
	
	StepMotor_DMAReset(6);
}

/*
 *函数简介:步进张大头速度模式控制指令
 *参数说明:ID		ID
 *参数说明:Dir		方向			0-逆时针 1-顺时针
 *参数说明:Speed	速度			单位RPM
 *参数说明:Acc		加速度			0表示无加减速
 *参数说明:Sync		多机同步标志	0-不多机同步 1-多机同步
 *返回类型:无
 *备注:无
 *指令:
 *		ID 0xF6 Dir Speed_H Speed_L Acc Sync 0x6B
 *	  ID		地址
 *	  Dir		方向			0-逆时针 1-顺时针
 *	  Speed_H	速度高四位		单位RPM
 *	  Speed_L	速度低四位		单位RPM
 *	  Acc		加速度			0表示无加减速
 *	  Sync		多机同步标志	0-不多机同步 1-多机同步
 */
void StepMotor_ZDTSetSpeed(uint8_t ID,int8_t Dir,uint16_t Speed,uint8_t Acc,uint8_t Sync)
{
	StepMotor_TxData[0]=ID;
	StepMotor_TxData[1]=0xF6;
	StepMotor_TxData[2]=Dir;
	StepMotor_TxData[3]=Speed>>8;
	StepMotor_TxData[4]=Speed & 0x00FF;
	StepMotor_TxData[5]=Acc;
	StepMotor_TxData[6]=Sync;
	StepMotor_TxData[7]=0x6B;
	
	StepMotor_DMAReset(8);
}

/*
 *函数简介:步进张大头位置模式控制指令
 *参数说明:ID		ID
 *参数说明:Dir		方向			0-逆时针 1-顺时针
 *参数说明:Speed	速度			单位RPM
 *参数说明:Acc		加速度			0表示无加减速
 *参数说明:Step		脉冲数
 *参数说明:Mode		模式标志		0-相对位置模式 1-绝对位置模式
 *参数说明:Sync		多机同步标志	0-不多机同步 1-多机同步
 *返回类型:无
 *备注:无
 *指令:
 *		ID 0xFD Dir Speed_H Speed_L Acc Step[31:23] Step[23:16] Step[15:8] Step[7:0] Mode Sync 0x6B
 *	  ID			地址
 *	  Dir			方向			0-逆时针 1-顺时针
 *	  Speed_H		速度高四位		单位RPM
 *	  Speed_L		速度低四位		单位RPM
 *	  Acc			加速度			0表示无加减速
 *	  Step[31:23]	脉冲数[31:23]
 *	  Step[23:16]	脉冲数[23:16]
 *	  Step[15:8]	脉冲数[15:8]
 *	  Step[7:0]		脉冲数[7:0]
 *	  Mode			模式标志		0-相对位置模式 1-绝对位置模式
 *	  Sync			多机同步标志	0-不多机同步 1-多机同步
 */
void StepMotor_ZDTSetPosition(uint8_t ID,int8_t Dir,uint16_t Speed,uint8_t Acc,uint32_t Step,uint8_t Mode,uint8_t Sync)
{
	StepMotor_TxData[0]=ID;
	StepMotor_TxData[1]=0xFD;
	StepMotor_TxData[2]=Dir;
	StepMotor_TxData[3]=Speed>>8;
	StepMotor_TxData[4]=Speed & 0x00FF;
	StepMotor_TxData[5]=Acc;
	StepMotor_TxData[6]=Step>>24;
	StepMotor_TxData[7]=(Step>>16) & 0x000000FF;
	StepMotor_TxData[8]=(Step>>8) & 0x000000FF;
	StepMotor_TxData[9]=Step & 0x000000FF;
	StepMotor_TxData[10]=Mode;
	StepMotor_TxData[11]=Sync;
	StepMotor_TxData[12]=0x6B;
	
	StepMotor_DMAReset(13);
}

/*
 *函数简介:步进张大头多机同步运动指令
 *参数说明:ID		ID
 *返回类型:无
 *备注:无
 *指令:
 *		ID 0xFF 0x66 0x6B
 *	  ID		地址 0表示广播地址
 */
void StepMotor_ZDTSyncMove(uint8_t ID)
{
	StepMotor_TxData[0]=ID;
	StepMotor_TxData[1]=0xFF;
	StepMotor_TxData[2]=0x66;
	StepMotor_TxData[3]=0x6B;
	
	StepMotor_DMAReset(4);
}

/*
 *函数简介:步进张大头立即停止指令
 *参数说明:ID		ID
 *参数说明:Sync		多机同步标志 0-不多机同步 1-多机同步
 *返回类型:无
 *备注:无
 *指令:
 *		ID 0xFE 0x98 Sync 0x6B
 *	  ID		地址
 *	  Sync		多机同步标志 0-不多机同步 1-多机同步
 */
void StepMotor_ZDTStop(uint8_t ID,uint8_t Sync)
{
	StepMotor_TxData[0]=ID;
	StepMotor_TxData[1]=0xFE;
	StepMotor_TxData[2]=0x98;
	StepMotor_TxData[3]=Sync;
	StepMotor_TxData[4]=0x6B;
	
	StepMotor_DMAReset(5);
}

/*
 *函数简介:步进初始化
 *参数说明:无
 *返回类型:无
 *备注:在使能步进之后才会启动定时器TIM6,防止使能帧被定时器中断打断
 */
void StepMotor_Init(void)
{
	StepMotor_HardwareInit();
	
	StepMotor_ZDTENABLE(1);Delay_us(500);
	StepMotor_ZDTENABLE(2);Delay_us(500);
	StepMotor_ZDTENABLE(3);Delay_us(500);
	StepMotor_ZDTENABLE(4);Delay_us(500);
	StepMotor_ZDTENABLE(5);Delay_us(500);
	
	TIM_Cmd(TIM6,ENABLE);//启动定时器
}

/*
 *函数简介:步进设置速度
 *参数说明:ID		ID
 *参数说明:Speed	速度	单位RPM
 *返回类型:无
 *备注:默认采用多机同步,无加减速
 *备注:会设置StepMotor_Target结构体来调整DMA发送值
 */
void StepMotor_SetSpeed(uint8_t ID,int32_t Speed)
{
	if(Speed>=0)
	{
		StepMotor_Target[ID-1].Dir=0;
		StepMotor_Target[ID-1].Speed=Speed;
	}
	else
	{
		StepMotor_Target[ID-1].Dir=1;
		StepMotor_Target[ID-1].Speed=-Speed;
	}

	StepMotor_Target[ID-1]._Flag=0;
}

/*
 *函数简介:步进设置位置
 *参数说明:ID		ID
 *参数说明:Speed	速度	单位RPM
 *参数说明:Step		脉冲数
 *返回类型:无
 *备注:默认采用逆时针方向,无加减速,绝对位置模式,不采用多机同步
 *备注:会设置StepMotor_Target结构体来调整DMA发送值
 */
void StepMotor_SetPosition(uint8_t ID,uint16_t Speed,uint32_t Step)
{
	StepMotor_Target[ID-1].Speed=Speed;
	StepMotor_Target[ID-1].Acc=0;
	StepMotor_Target[ID-1].Step=Step;
	
	StepMotor_Target[ID-1]._Flag=0;
}

/*
 *函数简介:步进设置位置(扩展)
 *参数说明:ID		ID
 *参数说明:Speed	速度	单位RPM
 *参数说明:Step		脉冲数
 *返回类型:无
 *备注:默认采用逆时针方向,无加减速,绝对位置模式,不采用多机同步
 *备注:会设置StepMotor_Target结构体来调整DMA发送值
 */
void StepMotor_SetPositionExt(uint8_t ID,uint16_t Speed,uint8_t Acc,uint32_t Step)
{
	StepMotor_Target[ID-1].Speed=Speed;
	StepMotor_Target[ID-1].Acc=Acc;
	StepMotor_Target[ID-1].Step=Step;
	
	StepMotor_Target[ID-1]._Flag=0;
}

/*
 *函数简介:步进定时发送回调函数
 *参数说明:无
 *返回类型:无
 *备注:在TIM6定时中断调用,用以DMA发送
 *备注:通过计数器循环发送各指令
 */
void StepMotor_Callack(void)
{
	static uint8_t Count=0;
	Count=(Count+1)%6;
	
	switch(Count)
	{
		case 0:
			if(StepMotor_Target[0]._Flag==0)
			{
				StepMotor_Target[0]._Flag=1;
				StepMotor_ZDTSetSpeed(1,StepMotor_Target[0].Dir,StepMotor_Target[0].Speed,0,1);
			}
			break;
		case 1:
			if(StepMotor_Target[1]._Flag==0)
			{
				StepMotor_Target[1]._Flag=1;
				StepMotor_ZDTSetSpeed(2,StepMotor_Target[1].Dir,StepMotor_Target[1].Speed,0,1);
			}
			break;
		case 2:
			if(StepMotor_Target[2]._Flag==0)
			{
				StepMotor_Target[2]._Flag=1;
				StepMotor_ZDTSetSpeed(3,StepMotor_Target[2].Dir,StepMotor_Target[2].Speed,0,1);
			}
			break;
		case 3:
			if(StepMotor_Target[3]._Flag==0)
			{
				StepMotor_Target[3]._Flag=1;
				StepMotor_ZDTSetSpeed(4,StepMotor_Target[3].Dir,StepMotor_Target[3].Speed,0,1);
			}
			break;
		case 4:StepMotor_ZDTSyncMove(0);break;
		case 5:
			if(StepMotor_Target[4]._Flag==0)
			{
				StepMotor_Target[4]._Flag=1;
				StepMotor_ZDTSetPosition(5,1,StepMotor_Target[4].Speed,StepMotor_Target[4].Acc,StepMotor_Target[4].Step,1,0);
			}
			break;
	}
}
