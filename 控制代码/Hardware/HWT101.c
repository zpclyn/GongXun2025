#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "Delay.h"

uint8_t HWT101_RxData0[25];//HWT101 DMA数据存储器0
uint8_t HWT101_RxData1[25];//HWT101 DMA数据存储器1

uint8_t HWT101_wRxFlag,HWT101_AngleRxFlag;//角速度,角度接收完成标志位
float HWT101_Yaw_W;//Yaw轴角速度(°/s)
float HWT101_LastYaw,HWT101_ThisYaw,HWT101_R,HWT101_Yaw;//Yaw轴角度(°)
float HWT101_CheckYaw;//Yaw轴角度校验值

/*
 *函数简介:HWT101初始化
 *参数说明:无
 *返回类型:无
 *备注:采用USART2空闲中断+DMA双缓冲接收读取数据
 *备注:默认采用PA3
 */
void HWT101_Init(void)
{
	/*===============配置时钟===============*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//开启时钟
	
	/*===============配置GPIO===============*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//复用推挽
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//默认上拉
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);//初始化USART2-Rx(PA3)
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);//开启PA3的USART2复用模式
	
	/*===============配置USART和串口接收DMA===============*/
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=115200;//配置波特率115200
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//配置无硬件流控制
	USART_InitStructure.USART_Mode=USART_Mode_Rx;//配置为接收模式
	USART_InitStructure.USART_Parity=USART_Parity_No;//配置为无校验位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;//配置停止位为1
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;//配置字长8bit
	USART_Init(USART2,&USART_InitStructure);//初始化USART2
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_Channel=DMA_Channel_4;//选择DMA通道4
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;//普通模式(非自动重装)
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralToMemory;//转运方向为外设到存储器
	DMA_InitStructure.DMA_BufferSize=22;//数据传输量为22字节
	DMA_InitStructure.DMA_Priority=DMA_Priority_Low;//最低优先级
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)&(USART2->DR);//外设地址(USART的DR数据接收寄存器)
	DMA_InitStructure.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;//外设数据长度为1字节(8bits)
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//外设地址不自增
	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)HWT101_RxData0;//存储器地址(HWT101 DMA数据存储器0)
	DMA_InitStructure.DMA_MemoryBurst=DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;//存储器数据长度为1字节(8bits)
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//存储器地址自增
	DMA_InitStructure.DMA_FIFOMode=DMA_FIFOMode_Disable;//不使用FIFO模式
	DMA_InitStructure.DMA_FIFOThreshold=DMA_FIFOStatus_1QuarterFull;//设置FIFO阈值为1/4(不使用FIFO模式时,此位无意义)
	DMA_Init(DMA1_Stream5,&DMA_InitStructure);//初始化数据流5
	
	DMA_DoubleBufferModeConfig(DMA1_Stream5,(uint32_t)HWT101_RxData1,DMA_Memory_0);//设置双缓冲搬运从HWT101 DMA数据存储器0开始
	DMA_DoubleBufferModeCmd(DMA1_Stream5,ENABLE);//使能DMA双缓冲功能

	/*===============配置空闲中断===============*/
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);//打通USART2到NVIC的串口空闲中断通道
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);//选择NVIC分组
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;//选择USART2中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//使能中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;//响应优先级
	NVIC_Init(&NVIC_InitStructure);
	
	/*===============使能===============*/
	DMA_Cmd(DMA1_Stream5,ENABLE);//使能DMA1的数据流5
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);//使能串口USART2的DMA搬运
	USART_Cmd(USART2,ENABLE);//启动USART2
}

/*
 *函数简介:HWT101角度校验
 *参数说明:无
 *返回类型:无
 *备注:用于消除陀螺仪初始偏移
 */
void HWT101_AngleCheck(void)
{
	HWT101_CheckYaw=HWT101_Yaw+HWT101_CheckYaw;
}

/*
 *函数简介:HWT101 DMA计数复位
 *参数说明:无
 *返回类型:无
 *备注:双缓冲会自动重装,此函数用以在串口空闲时复位计数值,防止噪声影响后续DMA
 */
void HWT101_DMAReset(void)
{
	DMA_ClearFlag(DMA1_Stream5,DMA_FLAG_TCIF5);//清除接收完成标志位
	DMA_Cmd(DMA1_Stream5,DISABLE);//失能DMA1的数据流5
	while(DMA_GetCmdStatus(DMA1_Stream5)!=DISABLE);//检测DMA1的数据流5为可配置状态
	DMA_SetCurrDataCounter(DMA1_Stream5,22);//恢复传输计数器的值
	DMA_Cmd(DMA1_Stream5,ENABLE);//使能DMA1的数据流5
}

/*
 *函数简介:HWT101数据处理
 *参数说明:无
 *返回类型:无
 *备注:无
 *数据帧1:
 *		0x55 0x52 0x00 0x00 RWzL RWzH WzL WzH 0x00 0x00 SUM
 *	  RWzL	原始角速度Z低8位
 *    RWzH	原始角速度Z高8位
 *	  WzL	角速度Z低8位
 *	  WzH	角速度Z高8位
 *数据帧2:
 *		0x55 0x53 0x00 0x00 0x00 0x00 YawL YawH VL VH SUM
 *	  YawL	偏航角Z低8位
 *    YawH	偏航角Z高8位
 *	  VL	版本号低8位
 *	  VH	版本号高8位
 */
void HWT101_DataProcess(void)
{
	uint8_t *Data;//选择存储器
	if(DMA_GetCurrentMemoryTarget(DMA1_Stream5)==0)Data=HWT101_RxData1;//若当前转运位于存储器0,则存储器1数据完整,采用存储器1进行数据处理
	else Data=HWT101_RxData0;//若当前转运位于存储器1,则存储器0数据完整,采用存储器0进行数据处理
	
	if(Data[0]==0x55 && Data[1]==0x52 && Data[2]==0x00 && Data[3]==0x00 && Data[8]==0x00 && Data[9]==0x00)
	{
		uint8_t HWT101_Sum=(uint8_t)(0x55+0x52+Data[4]+Data[5]+Data[6]+Data[7]);//获取校验位
		if(Data[10]==HWT101_Sum)//校验
		{
			HWT101_Yaw_W=(float)((int16_t)(Data[7]<<8)|Data[6])/32768.0f*2000.0f;//获取偏航角速度
			
			HWT101_wRxFlag=1;//置接收完成标志位
		}
	}
	
	if(Data[11]==0x55 && Data[12]==0x53 && Data[13]==0x00 && Data[14]==0x00 && Data[15]==0x00 && Data[16]==0x00)
	{
		uint8_t HWT101_Sum=(uint8_t)(0x55+0x53+Data[17]+Data[18]+Data[19]+Data[20]);//获取校验位
		if(Data[21]==HWT101_Sum)//校验
		{
			HWT101_LastYaw=HWT101_ThisYaw;
			HWT101_ThisYaw=(float)((int16_t)(Data[18]<<8)|Data[17])/32768.0f*180.0f;//获取偏航角
			
			if(HWT101_LastYaw-HWT101_ThisYaw>180)HWT101_R++;
			else if(HWT101_LastYaw-HWT101_ThisYaw<-180)HWT101_R--;
			HWT101_Yaw=360.0f*HWT101_R+HWT101_ThisYaw-HWT101_CheckYaw;
			
			HWT101_AngleRxFlag=1;//置接收完成标志位
		}
	}
}

/*
 *函数简介:HWT101回调函数
 *参数说明:无
 *返回类型:无
 *备注:在USART2空闲中断调用,用以解算数据
 */
void HWT101_Callback(void)
{
	if(DMA_GetCurrDataCounter(DMA1_Stream5)==22)//转运一次完成,并交换了存储器
		HWT101_DataProcess();//数据处理
	
	HWT101_DMAReset();
}
