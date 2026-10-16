#ifndef _USART1_H_
#define _USART1_H_

#include "stm32f10x.h"
#include "stdio.h"
void USART1_Config(u32 boud);
void USART1_SendOneData(u8 data);
void USART1_SendString(char *pdata);
void USART1_SendNData(u8 *pdata,u32 len);
u8 USART1_RecvOneData(void);
void USART1_Echo(void);
void USART1_Test1(void);
void USART1_IT_Config(void);

#define USART1_RX_LEN 64
struct USART1_MESSAGE
{
	u8 rx_buff[USART1_RX_LEN];//保存接收到的数据
	u32 rx_count;             //保存接收到数据的个数
	u8 over_flag;             //接收完成1 -- 完成  0 -- 没有
};
extern struct USART1_MESSAGE u1_msg;
void USART1_DataAnaly(void);
void USART1_DMAConfig(void);
void USART1_DMASend(char *pdata,u32 tx_len);
#endif
