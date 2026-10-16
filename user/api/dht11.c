#include "dht11.h"
#include "delay.h"

#define DHT11_High  (GPIO_WriteBit(GPIOG,GPIO_Pin_11,Bit_SET)) 
#define DHT11_Low   (GPIO_WriteBit(GPIOG,GPIO_Pin_11,Bit_RESET))

#define DHT11_DATA  (GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_11))



//PG11
void DHT11_ModeConfig(u8 mode)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(mode == DHT11_MODE_IN){
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空
	}else if(mode == DHT11_MODE_OUT){
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;//推挽
	}
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOG,&GPIO_InitStruct);
	//拉高
	GPIO_WriteBit(GPIOG,GPIO_Pin_11,Bit_SET);
}
/*
获取温湿度值
参数：tem -- 保存温度    hum -- 保存湿度
返回值：
0 -- 成功
1 -- 失败 -- 校验错误
2 -- 失败 -- 应答低电平失败
*/
u8 DHT11_GetValue(float *tem,float *hum)
{
	u32 timeout = 0;
	u8 buff[5] = {0};
	//1.PG11配置为输出模式
	DHT11_ModeConfig(DHT11_MODE_OUT);
	//2.STM32产生开始信号 >18ms低电平
	DHT11_Low;
	Delay_nms_nop(18);
	//3.STM32释放SDA线
	DHT11_High;
	//4.等待应答 -- 输入模式
	DHT11_ModeConfig(DHT11_MODE_IN);
	timeout = 5000;
	while(DHT11_DATA == 1)//等待变为低电平
	{
		Delay_nus_nop(1);
		timeout--;
		if(timeout == 0) return 2;
	}
	timeout = 5000;
	while(DHT11_DATA == 0)//等待变为高电平
	{
		Delay_nus_nop(1);
		timeout--;
		if(timeout == 0) return 3;
	}
	for(int i=0;i<5;i++){
		for(int j=0;j<8;j++){
			timeout = 5000;
			while(DHT11_DATA == 1)//等待变为低电平
			{
				Delay_nus_nop(1);
				timeout--;
				if(timeout == 0) return 4;
			}
			timeout = 5000;
			while(DHT11_DATA == 0)//等待变为高电平
			{
				Delay_nus_nop(1);
				timeout--;
				if(timeout == 0) return 5;
			}
			Delay_nus_nop(30);//延时30us
			if(DHT11_DATA == 1)
				buff[i] |=  (1<<(7-j));
			else
				buff[i] &= ~(1<<(7-j));
		}
	}
	//校验
	if(buff[4] != (buff[0]+buff[1]+buff[2]+buff[3])){
		return 1;
	}
	*hum = buff[0]+buff[1]/10.0;//建议不使用小数
	*tem = buff[2]+(buff[3]&0x7F)/100.0;//建议不使用小数
	if(buff[3] & 0x80)
		*tem = -*tem;
	
	return 0;
}
/*
			buff[i]<<=1;
			if(DHT11_DATA == 1)
				buff[i] |= (1<<j);
*/
#include "stdio.h"
float tem,hum;//保存温度和湿度
void DHT11_Test(void)
{
	if(DHT11_GetValue(&tem,&hum) == 1){
		printf("DHT11读取数据失败\r\n");
	}else{
		printf("tem:%.2f\r\n",tem);
		printf("hum:%.1f\r\n",hum);
	}
}

