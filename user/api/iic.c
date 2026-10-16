#include "iic.h"
#include "delay.h"
//PB6 -- SCL
//PB7 -- SDA
void IIC_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;//开漏输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//IIC总线空闲处于高电平
	SDA_H;
	SCL_H;
}

void IIC_Start(void)
{
	SDA_H;SCL_H;
	Delay_nus_nop(10);
	SDA_L;
	Delay_nus_nop(10);
	SCL_L;//拉低时钟线，占住总线
}

void IIC_Stop(void)
{
	SDA_L;SCL_H;
	Delay_nus_nop(10);
	SDA_H;
	Delay_nus_nop(10);
}
//发送8位数据 -- STM32作为主机，发送器
void IIC_SendData(u8 data)
{
	for(int i=0;i<8;i++){
		//修改SDA状态--准备数据 -- SCL为低电平
		if(data & (1<<(7-i))){
			SDA_H;
		}else{
			SDA_L;
		}
		Delay_nus_nop(10);
		//SCL拉高，产生上升沿，通知接收器开始接收数据
		//延时 -- 保持SDA不能变
		SCL_H;
		Delay_nus_nop(10);
		//拉低SCL -- 占住总线
		SCL_L;
	}
	//主机发送完数据后，主动拉高SDA -- 释放SDA总线
	SDA_H;
}
//IIC总线接收数据 -- STM32作为主机，接收器
//返回值：接收到的数据
u8 IIC_RecvData(void)
{
	u8 data = 0;//保存接收到的数据
	for(int i=0;i<8;i++)
	{
		Delay_nus_nop(10);
		SCL_H;//产生1个上升沿
		//读取数据
		if(DATA_SDA == 1){
			data |= (1<<(7-i));
		}else{
			data &= ~(1<<(7-i));
		}
		Delay_nus_nop(10);
		SCL_L;//拉低SCL
	}
	return data;
}
//主机等待从机应答
u8 IIC_GetAck(void)//___|————
{
	u8 res = 0;
	//主机产生1个CLK
	Delay_nus_nop(10);
	SCL_H;//产生1个上升沿
	Delay_nus_nop(10);
	//SDA == 0应答
	if(DATA_SDA == 1){
		res = 0;//应答失败
	}else if(DATA_SDA == 0){
		res = 1;//应答成功
	}
	SCL_L;
	return res;
}

//发送应答信号和非应答信号
//flag = 0 -- 发送应答信号    -- 1个CLK下，SDA保持低电平
//flag = 1 -- 发送非应答信号  -- 1个CLK下，SDA保持高电平
void IIC_SendAck(u8 flag)
{
	if(flag == 0)
		SDA_L;
	else if(flag == 1)
		SDA_H;
	Delay_nus_nop(10);
	SCL_H;//产生上升沿
	Delay_nus_nop(10);
	//占住SCL
	SCL_L;
}


