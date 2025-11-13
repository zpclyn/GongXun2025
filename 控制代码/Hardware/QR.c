#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "zpc_zxc_Parameter.h"
#include "zpc_zxc_Typedef.h"
#include "Delay.h"

uint8_t QR_TxData[9]={0x7E,0x00,0x08,0x01,0x00,0x02,0x01,0xAB,0xCD};//二维码模块DMA发送数据存储器
uint8_t QR_RxData[10];//二维码模块DMA接收数据存储器

uint8_t QR_RxFlag;//二维码模块接收完成标志位

/*
 *函数简介:二维码模块初始化
 *参数说明:无
 *返回类型:无
 *备注:采用DMA发送+DMA接收
 *备注:默认采用PC10(UART4-Tx)和PC11(UART4-Rx)
 */
void QR_Init(void)
{
	/*===============配置时钟===============*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);//开启时钟
	
	/*===============配置GPIO===============*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//复用推挽
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//默认上拉
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);//初始化UART4-Tx(PC10)和UART4-Rx(PC11)
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);//开启PC10的UART4复用模式
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4);//开启PC11的UART4复用模式
	
	/*===============配置USART和串口收发DMA===============*/
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=115200;//配置波特率115200
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//配置无硬件流控制
	USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;//配置为收发模式
	USART_InitStructure.USART_Parity=USART_Parity_No;//配置为无校验位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;//配置停止位为1
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;//配置字长8bit
	USART_Init(UART4,&USART_InitStructure);//初始化USART2
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_Channel=DMA_Channel_4;//选择DMA通道4
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;//普通模式(非自动重装)
	DMA_InitStructure.DMA_DIR=DMA_DIR_MemoryToPeripheral;//转运方向为存储器到外设
	DMA_InitStructure.DMA_BufferSize=1;//数据传输量为1字节
	DMA_InitStructure.DMA_Priority=DMA_Priority_VeryHigh;//最高优先级
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)&(UART4->DR);//外设地址(USART的DR数据接收寄存器)
	DMA_InitStructure.DMA_PeripheralBurst=DMA_PeripheralBurst_Single;//外设突发单次传输
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;//外设数据长度为1字节(8bits)
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//外设地址不自增
	DMA_InitStructure.DMA_Memory0BaseAddr=(uint32_t)QR_TxData;//存储器地址(二维码模块DMA发送数据存储器)
	DMA_InitStructure.DMA_MemoryBurst=DMA_MemoryBurst_Single;//存储器突发单次传输
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;//存储器数据长度为1字节(8bits)
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;//存储器地址自增
	DMA_InitStructure.DMA_FIFOMode=DMA_FIFOMode_Disable;//不使用FIFO模式
	DMA_InitStructure.DMA_FIFOThreshold=DMA_FIFOStatus_1QuarterFull;//设置FIFO阈值为1/4(不使用FIFO模式时,此位无意义)
	DMA_Init(DMA1_Stream4,&DMA_InitStructure);//初始化数据流4(UART4-Tx)
	
	/*===============配置空闲中断===============*/
	USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);//打通USART2到NVIC的串口接收中断通道
		
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);//选择NVIC分组
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=UART4_IRQn;//选择UART4中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;//使能中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;//响应优先级
	NVIC_Init(&NVIC_InitStructure);

	/*===============使能===============*/
	DMA_Cmd(DMA1_Stream4,ENABLE);//使能DMA1的数据流4(UART4-Tx)
	USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);//使能串口UART4的DMA搬运
	USART_Cmd(UART4,ENABLE);//启动UART4
	
	Test[7]='\0';
}

/*
 *函数简介:二维码模块发送DMA复位
 *参数说明:无
 *返回类型:无
 *备注:无
 *触发扫描指令:
 *		7E 00 08 01 00 02 01 AB CD
 */
void QR_DMASendReset(void)
{
	while(DMA_GetFlagStatus(DMA1_Stream4,DMA_FLAG_TCIF4)==RESET);//判断发送完成
	DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);//清除发送完成标志位
	DMA_Cmd(DMA1_Stream4,DISABLE);//失能DMA1的数据流4
	while(DMA_GetCmdStatus(DMA1_Stream4)!=DISABLE);//检测DMA1的数据流4为可配置状态
	DMA_SetCurrDataCounter(DMA1_Stream4,9);//恢复传输计数器的值
	DMA_Cmd(DMA1_Stream4,ENABLE);//使能DMA1的数据流4
}

/*
 *函数简介:二维码模块接收DMA复位
 *参数说明:无
 *返回类型:无
 *备注:无
 *接收数据格式:
 *		02 00 00 01 00 33 31 + 扫描结果
 *	  说明:在发送扫描触发指令后,二维码模块会返回前者以响应,如果有扫描结果会在一定时间后以另一帧的形式发送
 */
void QR_DataProcess(void)
{
	if(QR_RxData[3]=='+')
	{
		Test[0]=QR_RxData[0];
		Test[1]=QR_RxData[1];
		Test[2]=QR_RxData[2];
		Test[3]=QR_RxData[3];
		Test[4]=QR_RxData[4];
		Test[5]=QR_RxData[5];
		Test[6]=QR_RxData[6];
		
		QR_RxFlag=1;
	}
}

/*
 *函数简介:二维码模块扫描任务码
 *参数说明:无
 *返回类型:无
 *备注:如果任务码数组中有任务码则会跳过扫描
 */
void QR_Scan(void)
{
	if(Test[0]!=0)return;
	
	while(1)
	{
		QR_DMASendReset();
		
		Delay_ms(500);
		if(Test[0]!=0)break;
	}
}

/*
 *函数简介:二维码模块回调函数
 *参数说明:无
 *返回类型:无
 *备注:在UART4接收中断调用,用以接收和解算数据
 *接收数据格式:
 *		02 00 00 01 00 33 31 + 扫描结果
 *	  说明:在发送扫描触发指令后,二维码模块会返回前者以响应,如果有扫描结果会在一定时间后以另一帧的形式发送
 */
void QR_Callback(void)
{
	static int RxHEXState=0;
	static int pRxHEXState=0;
	
	uint8_t QR_Data=USART_ReceiveData(UART4);
	
	switch(RxHEXState)
	{
		case 0:if(QR_Data==0x02)RxHEXState=1;break;
		case 1:if(QR_Data==0x00)RxHEXState=2;else RxHEXState=0;break;
		case 2:if(QR_Data==0x00)RxHEXState=3;else RxHEXState=0;break;
		case 3:if(QR_Data==0x01)RxHEXState=4;else RxHEXState=0;break;
		case 4:if(QR_Data==0x00)RxHEXState=5;else RxHEXState=0;break;
		case 5:if(QR_Data==0x33)RxHEXState=6;else RxHEXState=0;break;
		case 6:if(QR_Data==0x31)RxHEXState=7;else RxHEXState=0;break;
		case 7:
			QR_RxData[pRxHEXState]=QR_Data;
			pRxHEXState++;
		
			if(pRxHEXState>6)
			{
				if(QR_RxData[0]>'0' && QR_RxData[3]=='+')QR_DataProcess();
				RxHEXState=0;
				pRxHEXState=0;
			}
			break;
	}
}
