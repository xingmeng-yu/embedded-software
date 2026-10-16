#ifndef _WIFI_H_
#define _WIFI_H_

#include "stm32f10x.h"

#define WIFI_RX_LEN 256
struct WIFI_MESSAGE
{
	u8 rx_buff[WIFI_RX_LEN];//保存接收到的数据
	u32 rx_count;             //保存接收到数据的个数
	u8 over_flag;             //接收完成1 -- 完成  0 -- 没有
};

extern struct WIFI_MESSAGE wifi_msg;
extern u8 net_time_state_flag;
extern u8 wifi_state;
#define ESP_EN_Port  GPIOE
#define ESP_EN_Pin   GPIO_Pin_6

#define ESP_EN_HIGH  (GPIO_SetBits(ESP_EN_Port,ESP_EN_Pin))
#define ESP_EN_LOW   (GPIO_ResetBits(ESP_EN_Port,ESP_EN_Pin))

//第3块中第5个扇区中第2页0地址写入
#define WIFI_FLASH_ADDR  0x035200

void WIFI_Config(void);
void WIFI_SendNData(u8 *pdata,u32 len);
u8 WIFI_SendCmdAndRecvAck(char *pcmd,char *pack,u32 timeout,u8 flag);
void WIFI_Run(void);
void WIFI_DataAnaly_AIThinker(void);
void WIFI_SetSTAAndConnectServer(void);
void WIFI_SetAPModeAndGetInfo(void);
void WIFI_StateChange(u8 new_state);
//从flash中读取ESP8266要连接热点的名称和密码
void WIFI_ReadInfoFormFlash(void);
int parse_net_time(const char *raw);
void WIFI_GET_TIME(void);

#endif
