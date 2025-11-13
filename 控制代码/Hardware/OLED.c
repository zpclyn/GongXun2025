#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "OLED.h"
#include "OLED_Font.h"

/*
 *函数简介:OLED专用us延时
 *参数说明:延时时长,单位us
 *返回类型:无
 *备注:参数范围:0~4294967295
 */
void OLED_Delay_us(uint32_t us)
{
	uint32_t temp;	    	 
	SysTick->LOAD=us*7; 				//时间加载，我们要延时n倍的us, 1us是一个fac_ua周期，所以总共要延时的周期值为二者相乘最后送到Load中。		 
	SysTick->VAL=0x00;        				//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ; //开启使能位 开始倒数
	do temp=SysTick->CTRL;
	while((temp&0x01)&&!(temp&(1<<16)));	//用来判断 systick 定时器是否还处于开启状态，然后在等待时间到达，也就是数到0的时候,此时第十六位设置为1
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk; //关闭计数器使能位
	SysTick->VAL =0x00;       				//清空计数器
}

/*
 *函数简介:OLED专用ms延时
 *参数说明:延时时长,单位ms
 *返回类型:无
 *备注:参数范围:0~4294967295
 */
void OLED_Delay(uint32_t xms)
{
	while(xms--)
		OLED_Delay_us(1000);
}

/*
 *函数简介:OLED专用指数函数
 *参数说明:底数x
 *参数说明:指数y
 *返回类型:x^y
 *备注:x,y均为整数
 */
uint32_t OLED_Pow(uint32_t x,uint32_t y)
{
	uint32_t Result=1;
	while(y--)
		Result*=x;
	return Result;
}

/*
 *函数简介:OLED专属软件I2C写SCL电平
 *参数说明:高低电平,0-低电平 1-高电平
 *返回类型:无
 *备注:默认SCL为I2C2_SCL(PF1)
 *备注:若修改引脚,需要到OLED.h文件中修改
 *备注:由于I2C速度较慢,在更改电平之后需要加入延时
 */
void OLED_SCL(uint8_t x)
{
	GPIO_WriteBit(OLED_SCL_GPIOx,OLED_SCL_Pin,(BitAction)(x));
	OLED_Delay_us(1);
}

/*
 *函数简介:OLED专属软件I2C写SDA电平
 *参数说明:高低电平,0-低电平 1-高电平
 *返回类型:
 *备注:默认SDA为I2C2_SDA(PF0)
 *备注:若修改引脚,需要到OLED.h文件中修改
 *备注:由于I2C速度较慢,在更改电平之后需要加入延时
 */
void OLED_SDA(uint8_t x)
{
	GPIO_WriteBit(OLED_SDA_GPIOx,OLED_SDA_Pin,(BitAction)(x));
	OLED_Delay_us(1);
}

/*
 *函数简介:OLED专属软件I2C初始化
 *参数说明:无
 *返回类型:无
 *备注:默认SCL为I2C2_SCL(PF1),SDA为I2C2_SDA(PF0)
 *备注:若修改引脚,需要到OLED.h文件中修改
 */
void OLED_I2C_Init(void)//OLED专属I2C初始化
{
	RCC_AHB1PeriphClockCmd(OLED_SCL_RCC,ENABLE);//开启SCL时钟
	RCC_AHB1PeriphClockCmd(OLED_SDA_RCC,ENABLE);//开启SDA时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_OD;//开漏输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//默认上拉
	GPIO_InitStructure.GPIO_Pin=OLED_SCL_Pin;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
 	GPIO_Init(OLED_SCL_GPIOx,&GPIO_InitStructure);//配置SCL
	GPIO_InitStructure.GPIO_Pin=OLED_SDA_Pin;
 	GPIO_Init(OLED_SDA_GPIOx,&GPIO_InitStructure);//配置SDA
	
	OLED_SCL(1);//初始化SCL
	OLED_SDA(1);//初始化SDA
}

/*
 *函数简介:OLED专属软件I2C起始
 *参数说明:无
 *返回类型:无
 *I2C协议:
 *	SCL	~~~~/---\___
 *	SDA	~~/---\_____
 */
void OLED_I2C_Start(void)
{
	OLED_SDA(1);
	OLED_SCL(1);//复位SCL和SDA
	OLED_SDA(0);//起始
	OLED_SCL(0);//准备接收数据
}

/*
 *函数简介:OLED专属软件I2C终止
 *参数说明:无
 *返回类型:无
 *I2C协议:
 *	SCL	____/-----
 *	SDA	~~\___/---
 */
void OLED_I2C_Stop(void)
{
	OLED_SDA(0);
	OLED_SCL(1);//准备接收停止信号
	OLED_SDA(1);//终止
}

/*
 *函数简介:OLED专属软件I2C接收应答
 *参数说明:无
 *返回类型:无
 *备注:为节省时间,省去接收功能(不处理应答),故不需要释放总线和读取SDA电平
 *I2C协议:
 *	SCL	____/-\___
 *	SDA	~~/-|==|~~
 *			(  )从机
 *修改为:
 *	SCL	____/-\___
 *	SDA	~~~~~~~~~~
 */
void OLED_I2C_ReceiveAck(void)
{
	OLED_SCL(1);//产生时钟
	OLED_SCL(0);
}

/*
 *函数简介:OLED专属软件I2C发送一个字节数据
 *参数说明:8bits发送数据
 *返回类型:无
 *备注:在此函数中包含了接收应答的功能
 *备注:为节省时间,省去接收功能(不处理应答)
 *I2C协议:
 *	SCL	____/-\___/-\___/-\___/-\___/-\___/-\___/-\___/-\___
 *	SDA	~~x=====x=====x=====x=====x=====x=====x=====x=====~~
 */
void OLED_I2C_SendByte(uint8_t Byte)//I2C发送一个字节
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		OLED_SDA(Byte&(0x80>>i));//发送数据每一位，高位先行
		OLED_SCL(1);//产生时钟
		OLED_SCL(0);
	}
	OLED_I2C_ReceiveAck();//接收应答
}

/*
 *函数简介:OLED写命令
 *参数说明:8bits命令
 *返回类型:无
 *备注:无
 */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);//发送从机地址
	OLED_I2C_SendByte(0x00);//发送写命令
	OLED_I2C_SendByte(Command);//发送命令
	OLED_I2C_Stop();
}

/*
 *函数简介:OLED写数据
 *参数说明:8bits数据
 *返回类型:无
 *备注:无
 */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);//发送从机地址
	OLED_I2C_SendByte(0x40);//发送写数据
	OLED_I2C_SendByte(Data);//发送数据
	OLED_I2C_Stop();
}

/*
 *函数简介:OLED光标定位
 *参数说明:页号(0~7)
 *参数说明:列号(0~127)
 *返回类型:无
 *备注:无
 */
void OLED_SetCursor(uint8_t Page,uint8_t Row)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);//发送从机地址
	OLED_I2C_SendByte(0x00);//发送写命令
	OLED_I2C_SendByte(0xB0 | Page);//设置页地址
	OLED_I2C_SendByte(0x00 | (Row & 0x0F));//设置列地址低8位
	OLED_I2C_SendByte(0x10 | (Row & 0xF0)>>4);//设置列地址高8位
	OLED_I2C_Stop();
}

/*
 *函数简介:OLED清屏
 *参数说明:无
 *返回类型:无
 *备注:无
 */
void OLED_Clean(void)
{
	for(uint8_t i=0;i<8;i++)//遍历OLED
	{
		OLED_SetCursor(i,0);//光标定位每一页
		for(uint8_t j=0;j<128;j++)//清空每一列
			OLED_WriteData(0x00);//OLED写数据0x00
	}
}

/*
 *函数简介:OLED初始化
 *参数说明:无
 *返回类型:无
 *备注:无
 */
void OLED_Init(void)
{	
	OLED_I2C_Init();//I2C初始化
	
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);//发送从机地址
	OLED_I2C_SendByte(0x00);//发送写命令
	OLED_I2C_SendByte(0xAE);//关闭显示
	OLED_I2C_SendByte(0xD5);//设置显示时钟分频比/振荡器频率
	OLED_I2C_SendByte(0x80);
	OLED_I2C_SendByte(0xA8);OLED_I2C_SendByte(0x3F);//设置多路复用率
	OLED_I2C_SendByte(0xD3);OLED_I2C_SendByte(0x00);//设置显示偏移
	OLED_I2C_SendByte(0x40);//设置显示开始行
	OLED_I2C_SendByte(0xA1);//设置左右方向，0xA1正常 0xA0左右反置
	OLED_I2C_SendByte(0xC8);//设置上下方向，0xC8正常 0xC0上下反置
	OLED_I2C_SendByte(0xDA);OLED_I2C_SendByte(0x12);//设置COM引脚硬件配置
	OLED_I2C_SendByte(0x81);OLED_I2C_SendByte(0xCF);//设置对比度控制
	OLED_I2C_SendByte(0xD9);OLED_I2C_SendByte(0xF1);//设置预充电周期
	OLED_I2C_SendByte(0xDB);
	OLED_I2C_SendByte(0x30);//设置VCOMH取消选择级别
	OLED_I2C_SendByte(0xA4);//设置整个显示打开/关闭
	OLED_I2C_SendByte(0xA6);//设置正常/倒转显示
	OLED_I2C_SendByte(0x8D);OLED_I2C_SendByte(0x14);//设置充电泵
	OLED_I2C_SendByte(0xAF);//开启显示
	OLED_I2C_Stop();
		
	OLED_Clean();//OLED清屏
}

/*
 *函数简介:OLED显示字符
 *参数说明:行数(1~4)
 *参数说明:列数(1~16)
 *返回类型:无
 *备注:无
 */
void OLED_ShowChar(uint8_t Line,uint8_t Column,char Char)
{
	OLED_SetCursor((Line-1)*2,(Column-1)*8);//定位上半部分
	for(int i=0;i<8;i++)
	OLED_WriteData(OLED_ASCII[Char-' '][i]);//显示字符上半部分
	OLED_SetCursor((Line-1)*2+1,(Column-1)*8);//定位下半部分
	for(int i=0;i<8;i++)
		OLED_WriteData(OLED_ASCII[Char-' '][i+8]);//显示字符下半部分
}

/*
 *函数简介:OLED显示字符串
 *参数说明:行数(1~4)
 *参数说明:列数(1~16)
 *返回类型:无
 *备注:无
 */
void OLED_ShowString(uint8_t Line,uint8_t Column,char *String)
{
	for(uint8_t i=0;String[i];i++)
		if(Column+i<=16)//单行限制
			OLED_ShowChar(Line,Column+i,String[i]);//显示每一个字符
		else break;
}

/*
 *函数简介:OLED显示任务码大字符
 *参数说明:行数(1)
 *参数说明:列数(1~8)
 *返回类型:无
 *备注:无
 */
void OLED_TestChar(uint8_t Line,uint8_t Column,char Char)
{      	
	uint8_t i;
	for(int j=0;j<8;j++)
	{
		OLED_SetCursor((Line-1)*8+j,(Column-1)*16);
		for(i=0;i<16;i++)
			OLED_WriteData(OLED_TestNum[(Char-'1')*8+j][i]);
	}
}

/*
 *函数简介:OLED显示任务码
 *参数说明:任务码
 *返回类型:无
 *备注:任务码输入格式示例:"123+123\0"
 */
void OLED_ShowTestNum(char *String)
{
	uint8_t i;
	for(i=0;String[i]!='\0';i++)
	{
		if(i!=3)OLED_TestChar(1,1+i,String[i]);
		else OLED_TestChar(1,1+i,'4');//中间+号
	}
}
