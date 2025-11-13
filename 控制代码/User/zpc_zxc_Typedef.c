#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "zpc_zxc_Parameter.h"

#ifdef GivenTestNum//给定任务码
	char Test[8]={'1','3','2','+','3','1','2'};//任务码 
#else
	char Test[8]={0};//任务码
#endif
