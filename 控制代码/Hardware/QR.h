#ifndef __QR_H
#define __QR_H

#include <stdint.h>

extern uint8_t QR_RxFlag;	//二维码模块接收完成标志位

void QR_Init(void);		//二维码模块初始化
void QR_Scan(void);		//二维码模块扫描任务码
void QR_Callback(void);	//二维码模块回调函数

#endif
