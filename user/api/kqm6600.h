#ifndef _KQM6600_H_
#define _KQM6600_H_

#include "stm32f10x.h"

#define KQM6600_RX_LEN 16
struct KQM6600_MESSAGE
{
	u8 rx_buff[KQM6600_RX_LEN];//保存接收到的数据
	u32 rx_count;             //保存接收到数据的个数
	u8 over_flag;             //接收完成1 -- 完成  0 -- 没有
};
//保存读取到的数据
struct KQM6600_DATA
{
	float VOC;//有机挥发化合物 PPM
	float CH2O;//甲醛          mg/m3
	u32 CO2;//二氧化碳         PPM
};
extern struct KQM6600_MESSAGE kqm6600_msg;
extern struct KQM6600_DATA kqm6600_data;
void KQM6600_Config(void);
void KQM6600_DataAnaly(void);
#endif
