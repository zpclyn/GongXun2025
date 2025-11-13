#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "Camera.h"
#include "Delay.h"

uint8_t Camera_TxData[2];//摄像头DMA发送数据存储器
uint8_t Camera_RxData0[20];//摄像头DMA接收数据存储器0
uint8_t Camera_RxData1[20];//摄像头DMA接收数据存储器1

uint8_t Camera_WuLiaoRxFlag,Camera_SeHuanRxFlag;//物料,色环接收完成标志位
int16_t Camera_WuLiao[3][2]={0};//物料坐标
int16_t Camera_SeHuan[3][2]={0};//色环坐标

/*
 *函数简介:摄像头初始化
 *参数说明:无
 *返回类型:无
 *备注:采用DMA发送+DMA双缓冲接收
 *备注:默认采用PC12(UART5-Tx)和PD2(UART5-Rx)
 */
void Camera_Init(void)
{
	/*===============配置时钟===============*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);//开启时钟
	
	/*===============配置GPIO===============*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//复用推挽
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//默认上拉
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);//初始化UART5-Tx(PC12)
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_Init(GPIOD,&GPIO_InitStructure);//初始化UART5-Rx(PD2)
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5);//开启PC12的UART5复用模式
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5);//开启PD2的UART5复用模式
	
	/*===============配置USART和串口收发DMA===============*/
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=115200;//配置波特率115200
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//配置无硬件流控制
	USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;//配置为收发模式
	USART_InitStructure.USART_Parity=USART_Parity_No;//配置为无校验位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;//配置停止位为1
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;//配置字长8bit
	USART_Init(UART5,&USART_InitStructure);//初始化UART5
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_Channel=DMA_Channel_4;//选择DMA通道4
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;//普通模式(非自动重装)
	DMA_InitStructure.DMA_DIR=DMA_DIR_MemoryToPeripheral;//转运方向为存储器到外设
	DMA_InitStructure.DMA_BufferSize=1;//数据传输量为1字节
	DMA_InitStructure.DMA_Priority=DMA_Priority_High;//高优先级
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)&(UART5->DR);//外设地址(USART的DR数据接收寄存器)
	DMA_InitStructure.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;//外设数据长度为1字节(8bits)
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//外设地址不自增
	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)Camera_TxData;//存储器地址(摄像头DMA发送数据存储器)
	DMA_InitStructure.DMA_MemoryBurst=DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;//存储器数据长度为1字节(8bits)
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//存储器地址自增
	DMA_InitStructure.DMA_FIFOMode=DMA_FIFOMode_Disable;//不使用FIFO模式
	DMA_InitStructure.DMA_FIFOThreshold=DMA_FIFOStatus_1QuarterFull;//设置FIFO阈值为1/4(不使用FIFO模式时,此位无意义)
	DMA_Init(DMA1_Stream7,&DMA_InitStructure);//初始化数据流7(UART5-Tx)
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralToMemory;//转运方向为外设到存储器
	DMA_InitStructure.DMA_BufferSize=19;//数据传输量为19字节
	DMA_InitStructure.DMA_Priority=DMA_Priority_Medium;//中间优先级
	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)Camera_RxData0;//存储器地址(摄像头DMA接收数据存储器0)
	DMA_Init(DMA1_Stream0,&DMA_InitStructure);//初始化数据流0(UART5-Rx)
	
	DMA_DoubleBufferModeConfig(DMA1_Stream0,(uint32_t)Camera_RxData1,DMA_Memory_0);//设置双缓冲搬运从摄像头DMA接收数据存储器0开始
	DMA_DoubleBufferModeCmd(DMA1_Stream0,ENABLE);//使能DMA双缓冲功能

	/*===============配置空闲中断===============*/
	USART_ITConfig(UART5,USART_IT_IDLE,ENABLE);//打通UART5到NVIC的串口空闲中断通道
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);//选择NVIC分组
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=UART5_IRQn;//选择UART5中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//使能中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;//响应优先级
	NVIC_Init(&NVIC_InitStructure);

	/*===============使能===============*/
	DMA_Cmd(DMA1_Stream7,ENABLE);//使能DMA1的数据流7(UART5-Tx)
	DMA_Cmd(DMA1_Stream0,ENABLE);//使能DMA1的数据流0(UART5-Rx)
	USART_DMACmd(UART5,USART_DMAReq_Tx | USART_DMAReq_Rx,ENABLE);//使能串口UART5的DMA搬运
	USART_Cmd(UART5,ENABLE);//启动UART5
}

/*
 *函数简介:摄像头DMA发送复位
 *参数说明:Size		发送数据个数
 *返回类型:无
 *备注:无
 */
void Camera_DMASendReset(uint8_t Size)
{
	while(DMA_GetFlagStatus(DMA1_Stream7,DMA_FLAG_TCIF7)==RESET);//判断发送完成
	DMA_ClearFlag(DMA1_Stream7,DMA_FLAG_TCIF7);//清除发送完成标志位
	DMA_Cmd(DMA1_Stream7,DISABLE);//失能DMA1的数据流7
	while(DMA_GetCmdStatus(DMA1_Stream7)!=DISABLE);//检测DMA1的数据流7为可配置状态
	DMA_SetCurrDataCounter(DMA1_Stream7,Size);//恢复传输计数器的值
	DMA_Cmd(DMA1_Stream7,ENABLE);//使能DMA1的数据流7
}

/*
 *函数简介:摄像头DMA接收复位
 *参数说明:无
 *返回类型:无
 *备注:无
 */
void Camera_DMAReceiveReset(void)
{
	DMA_ClearFlag(DMA1_Stream0,DMA_FLAG_TCIF0);//清除发送完成标志位
	DMA_Cmd(DMA1_Stream0,DISABLE);//失能DMA1的数据流0
	while(DMA_GetCmdStatus(DMA1_Stream0)!=DISABLE);//检测DMA1的数据流0为可配置状态
	DMA_SetCurrDataCounter(DMA1_Stream0,19);//恢复传输计数器的值
	DMA_Cmd(DMA1_Stream0,ENABLE);//使能DMA1的数据流0
}

/*
 *函数简介:摄像头发送模式指令
 *参数说明:Mode		模式枚举
 *返回类型:无
 *备注:无
 *模式指令:
 *	物料模式:		0xB2
 *	红色色环模式:	0xB3 0xC0
 *	绿色色环模式:	0xB3 0xC1
 *	蓝色色环模式:	0xB3 0xC2
 */
void Camera_SendMode(Camera_Mode Mode)
{
	if(Mode==WuLiao){Camera_TxData[0]=0xB2;Camera_DMASendReset(1);}
	else if(Mode==SeHuan_R){Camera_TxData[0]=0xB3;Camera_TxData[1]=0xC0;Camera_DMASendReset(2);}
	else if(Mode==SeHuan_G){Camera_TxData[0]=0xB3;Camera_TxData[1]=0xC1;Camera_DMASendReset(2);}
	else if(Mode==SeHuan_B){Camera_TxData[0]=0xB3;Camera_TxData[1]=0xC2;Camera_DMASendReset(2);}
}

/*
 *函数简介:摄像头切换模式
 *参数说明:Mode		模式枚举
 *返回类型:无
 *备注:无
 */
void Camera_GotoMode(Camera_Mode Mode)
{
	Camera_WuLiaoRxFlag=Camera_SeHuanRxFlag=0;
	while(1)
	{
		Camera_SendMode(Mode);
		
		if(Mode==WuLiao && Camera_WuLiaoRxFlag==1)break;
		if((Mode==SeHuan_R || Mode==SeHuan_G || Mode==SeHuan_B) && Camera_SeHuanRxFlag==1)break;
		
		Delay_ms(10);
	}
}

/*
 *函数简介:摄像头数据处理
 *参数说明:无
 *返回类型:无
 *备注:无
 *数据帧1:
 *		0xAA 0x5A Mode 0xC0 R_xL R_xH R_yL R_yH 0xC1 G_xL G_xH G_yL G_yH 0xC2 B_xL B_xH B_yL B_yH 0xBB
 *	  Mode				模式 0xB2-物料模式 0xB3-色环模式  
 *    R_xL/G_xL/B_xL	红/绿/蓝x坐标低位
 *	  R_xH/G_xH/B_xH	红/绿/蓝x坐标高位
 *	  R_yL/G_yL/B_yL	红/绿/蓝y坐标低位
 *	  R_yH/G_yH/B_yH	红/绿/蓝y坐标高位
 */
void Camera_DataProcess(void)
{
	uint8_t *Data;//选择存储器
	if(DMA_GetCurrentMemoryTarget(DMA1_Stream0)==0)Data=Camera_RxData1;//若当前转运位于存储器0,则存储器1数据完整,采用存储器1进行数据处理
	else Data=Camera_RxData0;//若当前转运位于存储器1,则存储器0数据完整,采用存储器0进行数据处理
	
	if(Data[0]==0xAA && Data[1]==0x5A && Data[2]==0xB2 && Data[3]==0xC0 && Data[8]==0xC1 && Data[13]==0xC2 && Data[18]==0xBB)//物料
	{
		Camera_WuLiao[Red][X]=(int16_t)((Data[5]<<8) | Data[4]);
		Camera_WuLiao[Red][Y]=(int16_t)((Data[7]<<8) | Data[6]);
		Camera_WuLiao[Green][X]=(int16_t)((Data[10]<<8) | Data[9]);
		Camera_WuLiao[Green][Y]=(int16_t)((Data[12]<<8) | Data[11]);
		Camera_WuLiao[Blue][X]=(int16_t)((Data[15]<<8) | Data[14]);
		Camera_WuLiao[Blue][Y]=(int16_t)((Data[17]<<8) | Data[16]);
		
		Camera_WuLiaoRxFlag=1;
	}
	else if(Data[0]==0xAA && Data[1]==0x5A && Data[2]==0xB3 && Data[3]==0xC0 && Data[8]==0xC1 && Data[13]==0xC2 && Data[18]==0xBB)//色环
	{
		Camera_SeHuan[Red][X]=(int16_t)((Data[5]<<8) | Data[4]);
		Camera_SeHuan[Red][Y]=(int16_t)((Data[7]<<8) | Data[6]);
		Camera_SeHuan[Green][X]=(int16_t)((Data[10]<<8) | Data[9]);
		Camera_SeHuan[Green][Y]=(int16_t)((Data[12]<<8) | Data[11]);
		Camera_SeHuan[Blue][X]=(int16_t)((Data[15]<<8) | Data[14]);
		Camera_SeHuan[Blue][Y]=(int16_t)((Data[17]<<8) | Data[16]);
		
		Camera_SeHuanRxFlag=1;
	}
}

/*
 *函数简介:摄像头接收回调函数
 *参数说明:无
 *返回类型:无
 *备注:在UART5空闲中断调用,用以解算数据
 */
void Camera_Callback(void)
{
	if(DMA_GetCurrDataCounter(DMA1_Stream0)==19)//转运一次完成,并交换了存储器
		Camera_DataProcess();//数据处理
	
	Camera_DMAReceiveReset();
}
