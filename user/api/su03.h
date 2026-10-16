#ifndef _SU03_H_
#define _SU03_H_

#include "stm32f10x.h"
#include "stdio.h"
#include "beep.h"
#include "dht11.h"
#include "jx_uart_send.h"
#include "kqm6600.h"
#include "rgb.h"
#define SU03_RX_LEN 16
struct SU03_MESSAGE{
	u8 rx_buff[SU03_RX_LEN];//保存收到的数据
	u32 rx_count;//保存接收到的数据的个数
	u8 over_flag;//接受完成1 无0
};

void SU03_Config(void);
void SU03_DataAnaly(void);

void UART4_SendNData(u8 *pdata,u32 len);
#endif
