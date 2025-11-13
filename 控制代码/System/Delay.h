#ifndef __DELAY_H
#define __DELAY_H

#include <stdint.h>

void Delay_us(uint32_t us);	//微秒级延时
void Delay_ms(uint32_t ms);	//毫秒级延时
void Delay_s(uint32_t s);	//秒级延时

#endif
