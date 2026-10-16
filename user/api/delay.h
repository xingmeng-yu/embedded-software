#ifndef _DELAY_H_
#define _DELAY_H_

#include "stm32f10x.h"

void Delay_nus_nop(unsigned int n);
void Delay_nms_nop(u32 n);

void Delay_Config(void);

extern u32 time_led1;
extern u32 time_key1;
extern u32 time_led2;
extern u32 time_beep[2];//时间片初始值  时间片的时长
extern u32 time_dht11[2];
extern u32 time_adc[2];
extern u32 time_rtc[2];
extern u32 time_wifi[2];
#endif
