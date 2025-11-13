#ifndef __OLED_H
#define __OLED_H

#include <stdint.h>

#define OLED_SCL_RCC		RCC_AHB1Periph_GPIOB	//SCL引脚时钟
#define OLED_SCL_GPIOx		GPIOB					//SCL引脚的GPIO
#define OLED_SCL_Pin		GPIO_Pin_6				//SCL引脚的Pin号
#define OLED_SDA_RCC		RCC_AHB1Periph_GPIOB	//SDA引脚时钟
#define OLED_SDA_GPIOx		GPIOB					//SDA引脚的GPIO
#define OLED_SDA_Pin		GPIO_Pin_7				//SDA引脚的Pin号

void OLED_Init(void);											//OLED初始化
void OLED_Clean(void);											//OLED清屏
void OLED_ShowChar(uint8_t Line,uint8_t Column,char Char);		//OLED显示字符
void OLED_ShowString(uint8_t Line,uint8_t Column,char *String);	//OLED显示字符串
void OLED_ShowTestNum(char *String);							//OLED显示任务码

#endif
