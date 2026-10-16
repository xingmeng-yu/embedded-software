#ifndef _RTC_H_
#define _RTC_H_

#include "stm32f10x.h"
#include "stdio.h"
#include "time.h"


void RTC_GetTime(void);
void RTC_Config(void);
void RtC_Adjusttime(struct tm set_time);
void RTC_GetTimeWithLunar(void);

extern struct tm rtc_settime;
extern struct tm *prtc_nowtime;

extern char time_str[64];

#endif
