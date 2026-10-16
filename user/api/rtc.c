#include "rtc.h"

struct tm rtc_settime={
	.tm_year=2026-1900,.tm_mon=7-1,.tm_mday=10,.tm_hour=17,.tm_min=40,.tm_sec=16
};
char time_str[64];  // 用于存放格式化后的时间字符串
struct tm *prtc_nowtime=NULL;

void RTC_Config(void)
{
	//首次配置
	if(BKP_ReadBackupRegister(BKP_DR1)!=0xA5A5){
		//1.打开时钟（PWR BKP）
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP|RCC_APB1Periph_PWR,ENABLE);
		//2.后备区域电源使能（PWR BKP ENABLE）
		PWR_BackupAccessCmd(ENABLE);
		//3.启动LSE，等待LSE 稳定
		RCC_LSEConfig(RCC_LSE_ON);
		//4.选着LSE 作为RTC 的时钟源
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);
		//5.等待APB1 与RTC 同步
		RTC_WaitForSynchro();
		//6.进入配置模式
		RTC_EnterConfigMode();
		//7.等待上次写寄存器完成
		RTC_WaitForLastTask();
		//8.分频、给RTC_CNT（计数寄存器）赋初值(32 位的数)
		RTC_SetPrescaler(32768-1);
		RTC_WaitForLastTask();
		
		//通过串口输入进来。
		RtC_Adjusttime(rtc_settime);
		//9.等待上次写寄存器完成
		RTC_WaitForLastTask();
		BKP_WriteBackupRegister(BKP_DR1,0xA5A5);
	}else{
		//1.打开时钟（PWR BKP）
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP|RCC_APB1Periph_PWR,ENABLE);
		//2.后备区域电源使能（PWR BKP ENABLE）
		PWR_BackupAccessCmd(ENABLE);
		//5.等待APB1与RTC同步
		RTC_WaitForSynchro();
	}
}

void RtC_Adjusttime(struct tm set_time)
{
	u32 sec=0;
	sec=mktime(&set_time);
	RTC_SetCounter(sec);
	RTC_WaitForLastTask();
}
void RTC_GetTime(void)
{
    u32 now_time = 0;
    
    
    now_time = RTC_GetCounter();
    prtc_nowtime = localtime(&now_time);
    
    // 使用 sprintf 拼接时间字符串
    sprintf(time_str, "%4d-%02d-%02d %02d:%02d:%02d %1d",
        prtc_nowtime->tm_year + 1900,
        prtc_nowtime->tm_mon + 1,
        prtc_nowtime->tm_mday,
        prtc_nowtime->tm_hour,
        prtc_nowtime->tm_min,
        prtc_nowtime->tm_sec,
				prtc_nowtime->tm_wday
    );
    
    // 现在 time_str 里存放着完整的时间字符串
    // 你可以根据需要处理它，例如：
    printf("%s", time_str);           // 打印到串口
		printf("\r\n");
    // LCD_ShowString(0, 0, time_str); // 显示到LCD
    // 或存入日志缓冲区等
}
