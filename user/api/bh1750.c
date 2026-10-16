#include "bh1750.h"
#include "stdio.h"
#include "delay.h"
#define IIC_SOFT 1 //1 -- 使用软件IIC实现否则是硬件IIC实现

#define ADDR_L 0x46//0100 0110
#define ADDR_H 0xB8//1011 1000

u16 light_value = 0;

#if IIC_SOFT == 1
void BH1750_Config(void)
{
	//IIC接口初始化
	IIC_Config();
	//BH1750配置为通电模式
	BH1750_Star();
}
/*
把BH1750配置为通电模式
返回值：0 -- 成功
        1 -- 寻找设备失败
        2 -- 设置通电模式失败
*/
u8 BH1750_Star(void)
{
	//1.STM32发送起始信号
	IIC_Start();
	//2.STM32发送7位地址+1读写位(0)
	IIC_SendData(ADDR_L);
	//3.STM32等待BH1750应答信号
	if(IIC_GetAck() == 0){
		return 1;
	}
	//4.STM32发送通电指令
	IIC_SendData(0x01);
	//5.STM32等待BH1750应答信号
	if(IIC_GetAck() == 0){
		return 2;
	}
	//6.STM32发送停止信号
	IIC_Stop();
	return 0;
}
//保存光照值
u8 BH1750_GetValue(void)
{
	u16 temp_h,temp_l;
	//1：主机发从起始信号
	IIC_Start();
	//2：主机发送从机地址+读写位（写=0）0100 0110 = 0x46
	IIC_SendData(0x46);
	//3：等待BH1750返回应答信号
	IIC_GetAck();
	//4：主机发送指令（连续高分辨率模式）：0001 0000 = 0x10
	IIC_SendData(0x10);
	//5：主机等待BH1750返回应答信号
	IIC_GetAck();
	//6：主机发送停止信号
	IIC_Stop();
	//--- 把BH1750设置为连续高分辨率模式
	//7：等待测量完成：延时大于180ms
	Delay_nms_nop(180);
	//--- 读取测量结果
	//8：主机发送起始信号
	IIC_Start();
	//9：主机发送从机地址（0100 011）+读写位（读=1）--0100 0111 = 0x47
	IIC_SendData(0x47);
	//10：主机等待BH1750返回应答信号
	IIC_GetAck();
	//11：BH1750返回光照数据的高8位数据 -- 主机接收
	temp_h = IIC_RecvData();
	//12：主机发送应答信号
	IIC_SendAck(0);
	//13：BH1750返回光照数据的低8位数据 -- 主机接收
	temp_l = IIC_RecvData();
	//14：主机发送非应答信号
	IIC_SendAck(1);
	//15：主机发送停止条件
	IIC_Stop();
	light_value = temp_h<<8 | temp_l;
	light_value = (u16)(light_value/1.2);
	
	return 0;
}

#else 
void BH1750_Config(void)
{
	//IO口初始化 PB6 -- I2C1_SCL  PB7 -- I2C1_SDA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;//复用开漏输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	//IIC初始化
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	I2C_InitTypeDef I2C_InitStruct = {0};
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;//具有应答功能
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//地址7位
	I2C_InitStruct.I2C_ClockSpeed = 100000;//100K
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_16_9;//low/high = 2
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;//I2C模式
	I2C_InitStruct.I2C_OwnAddress1 = 0xA0;//STM32作为从机时，设置自己的地址
	I2C_Init(I2C1,&I2C_InitStruct);
	
	//使能I2C1
	I2C_Cmd(I2C1,ENABLE);
	
	//进入通电模式
	int res = BH1750_Star();
	printf("res1 = %d\r\n",res);
}
///*
//把BH1750配置为通电模式
//返回值：0 -- 成功
//        1 -- 寻找设备失败
//        2 -- 设置通电模式失败
//*/
//u8 BH1750_Star(void)
//{
//	u32 timeout = 0;
//	//1.STM32发送起始信号 + 等待事件5（清除相关标志位）
//	I2C_GenerateSTART(I2C1,ENABLE);
//	timeout = 5000;
//	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS){
//		timeout--;
//		Delay_nus_nop(1);
//		if(timeout == 0)
//			return 1;
//	}
//	//2.STM32发送7位地址+1读写位(0) + 等待事件6
//	I2C_Send7bitAddress(I2C1,0x46,I2C_Direction_Transmitter);
//	timeout = 5000;
//	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS)
//	{
//		timeout--;
//		Delay_nus_nop(1);
//		if(timeout == 0)
//			return 2;
//	}
//	//4.STM32发送通电指令 0x01 + 等待事件8
//	I2C_SendData(I2C1,0x01);
//	timeout=5000;
//	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS){
//		timeout--;
//		Delay_nus_nop(1);
//		if(timeout == 0)
//			return 3;
//	}
//	//6.STM32发送停止信号
//	I2C_GenerateSTOP(I2C1,ENABLE);
//	return 0;
//}
////保存光照值
//u8 BH1750_GetValue(void)
//{
//	u32 timeout = 0;
//	u16 temp_h,temp_l;
//	//1：主机发从起始信号 + 等待事件5
//	I2C_GenerateSTART(I2C1,ENABLE);
//	timeout = 5000;
//	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS){
//		timeout--;
//		Delay_nus_nop(1);
//		if(timeout == 0)
//			return 1;
//	}
//	//2：主机发送从机地址+读写位（写=0）0100 0110 = 0x46 + 等待事件6
//	I2C_Send7bitAddress(I2C1,0x46,I2C_Direction_Transmitter);
//	timeout = 5000;
//	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS){
//		timeout--;
//		Delay_nus_nop(1);
//		if(timeout == 0)
//			return 2;
//	}
//	//4：主机发送指令（连续高分辨率模式）：0001 0000 = 0x10 + 等待事件8
//	I2C_SendData(I2C1,0x10);
//	timeout = 5000;
//	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS){
//		timeout--;
//		Delay_nus_nop(1);
//		if(timeout == 0)
//			return 3;
//	}
//	//6：主机发送停止信号
//	I2C_GenerateSTOP(I2C1,ENABLE);
//	
//	//--- 把BH1750设置为连续高分辨率模式
//	//7：等待测量完成：延时大于180ms
//	Delay_nms_nop(180);
//	//--- 读取测量结果
//	//8：主机发送起始信号 + 等待事件5
//	I2C_GenerateSTART(I2C1,ENABLE);
//	timeout = 5000;
//	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS){
//		timeout--;
//		Delay_nus_nop(1);
//		if(timeout == 0)
//			return 4;
//	}	
//	//9：主机发送从机地址（0100 011）+读写位（读=1）--0100 0111 = 0x47 + 等待事件6 
//	I2C_Send7bitAddress(I2C1,0x46,I2C_Direction_Receiver);
//	timeout = 5000;
//	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS){
//		timeout--;
//		Delay_nus_nop(1);
//		if(timeout == 0)
//			return 5;
//	}	
//	//10：主机发送应答信号 -- 先配置能产生应答信号模式
//	I2C_AcknowledgeConfig(I2C1,ENABLE);
//	//11：BH1750返回光照数据的高8位数据 -- 主机接收
//	//等待数据到来 -- 等待事件7
//	timeout = 5000;
//	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS){
//		timeout--;
//		Delay_nus_nop(1);
//		if(timeout == 0)
//			return 3;
//	}	
//	temp_h = I2C_ReceiveData(I2C1);
//	
//	//14：主机发送非应答信号 -- 先配置能产生非应答信号模式
//	I2C_AcknowledgeConfig(I2C1,DISABLE);
//	//15：主机发送停止条件
//	I2C_GenerateSTOP(I2C1,ENABLE);
//	
//	//13：BH1750返回光照数据的低8位数据 -- 主机接收
//	timeout = 5000;
//	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS){
//		timeout--;
//		Delay_nus_nop(1);
//		if(timeout == 0)
//			return 3;
//	}	
//	temp_l = I2C_ReceiveData(I2C1);

//	light_value = temp_h<<8 | temp_l;
//	light_value = (u16)(light_value/1.2);
//	
//	return 0;
//}

/*
把BH1750配置为通电模式
返回值：0 -- 成功
        1 -- 寻找设备失败
        2 -- 设置通电模式失败
*/
u8 BH1750_Star(void)
{
	u32 timeout = 0;
	//1.STM32发送起始信号 + 等待事件5（清除相关标志位）
	I2C_GenerateSTART(I2C1,ENABLE);
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	//2.STM32发送7位地址+1读写位(0) + 等待事件6
	I2C_Send7bitAddress(I2C1,0x46,I2C_Direction_Transmitter);
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);
	//4.STM32发送通电指令 0x01 + 等待事件8
	I2C_SendData(I2C1,0x01);
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);
	//6.STM32发送停止信号
	I2C_GenerateSTOP(I2C1,ENABLE);
	return 0;
}
//保存光照值
u8 BH1750_GetValue(void)
{
	u32 timeout = 0;
	u16 temp_h,temp_l;
	//1：主机发从起始信号 + 等待事件5
	I2C_GenerateSTART(I2C1,ENABLE);
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	//2：主机发送从机地址+读写位（写=0）0100 0110 = 0x46 + 等待事件6
	I2C_Send7bitAddress(I2C1,0x46,I2C_Direction_Transmitter);
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);
	//4：主机发送指令（连续高分辨率模式）：0001 0000 = 0x10 + 等待事件8
	I2C_SendData(I2C1,0x10);
	timeout = 5000;
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED) != SUCCESS);
	//6：主机发送停止信号
	I2C_GenerateSTOP(I2C1,ENABLE);
	
	//--- 把BH1750设置为连续高分辨率模式
	//7：等待测量完成：延时大于180ms
	Delay_nms_nop(180);
	//--- 读取测量结果
	//8：主机发送起始信号 + 等待事件5
	I2C_GenerateSTART(I2C1,ENABLE);
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	//9：主机发送从机地址（0100 011）+读写位（读=1）--0100 0111 = 0x47 + 等待事件6 
	I2C_Send7bitAddress(I2C1,0x46,I2C_Direction_Receiver);
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS);
	//10：主机发送应答信号 -- 先配置能产生应答信号模式
	I2C_AcknowledgeConfig(I2C1,ENABLE);
	//11：BH1750返回光照数据的高8位数据 -- 主机接收
	//等待数据到来 -- 等待事件7
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
	temp_h = I2C_ReceiveData(I2C1);
	
	//14：主机发送非应答信号 -- 先配置能产生非应答信号模式
	I2C_AcknowledgeConfig(I2C1,DISABLE);
	//15：主机发送停止条件
	I2C_GenerateSTOP(I2C1,ENABLE);
	
	//13：BH1750返回光照数据的低8位数据 -- 主机接收
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
	temp_l = I2C_ReceiveData(I2C1);

	light_value = temp_h<<8 | temp_l;
	light_value = (u16)(light_value/1.2);
	
	return 0;
}
#endif




