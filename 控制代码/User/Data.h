#ifndef __DATA_H
#define __DATA_H

#include <stdint.h>

#define PI					3.1415926535897932384626433832795f	//PI
#define Data_Rad2Deg        57.295779513082320876798154814105f  //180/PI    弧度转角度
#define Data_Deg2Rad        0.01745329251994329576923690768489f //PI/180    角度转弧度
#define Data_Rad2RPM        9.5492965855137201461330258023509f  //60/2PI    rad/s转rpm
#define Data_RPM2Rad        0.10471975511965977461542144610932f //2PI/60    rpm转rad/s

int Data_Clipping(int Data,int Data_Min,int Data_Max);				//数据限幅
float Data_Clippingf(float Data,float Data_Min,float Data_Max);		//浮点数数据限幅
uint8_t Data_RangeCheck(int Data,int Data_Min,int Data_Max);		//数据范围判断
int Data_MIN(int Data1,int Data2);									//最小值
int Data_MAX(int Data1,int Data2);									//最大值

#endif
