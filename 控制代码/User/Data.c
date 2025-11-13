#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

/*
 *函数简介:数据限幅
 *参数说明:Data			原始数据
 *参数说明:Data_Min		限幅下限
 *参数说明:Data_Max		限幅上限
 *返回类型:int			限幅后数据
 *备注:无
 */
int Data_Clipping(int Data,int Data_Min,int Data_Max)
{
    if(Data>Data_Max)return Data_Max;
    else if(Data<Data_Min)return Data_Min;
    return Data;
}

/*
 *函数简介:浮点数数据限幅
 *参数说明:Data			原始数据
 *参数说明:Data_Min		限幅下限
 *参数说明:Data_Max		限幅上限
 *返回类型:float		限幅后数据
 *备注:无
 */
float Data_Clippingf(float Data,float Data_Min,float Data_Max)
{
    if(Data>Data_Max)return Data_Max;
    else if(Data<Data_Min)return Data_Min;
    return Data;
}

/*
 *函数简介:数据范围判断
 *参数说明:Data			原始数据
 *参数说明:Data_Min		限幅下限
 *参数说明:Data_Max		限幅上限
 *返回类型:uint8		是否在范围内  0-不在范围内 1-在范围内
 *备注:这个范围包括边界
 */
uint8_t Data_RangeCheck(int Data,int Data_Min,int Data_Max)
{
    if(Data>Data_Max || Data<Data_Min)return 0;
    return 1;
}

/*
 *函数简介:最小值
 *参数说明:Data1		数据1
 *参数说明:Data2		数据2
 *返回类型:int			两个数中的较小值
 *备注:无
 */
int Data_MIN(int Data1,int Data2)
{
    if(Data1>Data2)return Data2;
    return Data1;
}

/*
 *函数简介:最大值
 *参数说明:Data1		数据1
 *参数说明:Data2		数据2
 *返回类型:int			两个数中的较大值
 *备注:无
 */
int Data_MAX(int Data1,int Data2)
{
    if(Data1<Data2)return Data2;
    return Data1;
}
