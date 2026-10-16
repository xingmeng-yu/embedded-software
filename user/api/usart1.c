#include "usart1.h"
#include "beep.h"
#include "led.h"

struct USART1_MESSAGE u1_msg={.rx_count=0,.over_flag=0};
/*
函数作用：串口1初始化
参数：boud -- 波特率
返回值：无
*/
void USART1_Config(u32 boud)
{
	//1.配置PA9和PA10
	//1.1 打开A端口时钟
	//1.2 PA9配置复用推挽输出
	//1.3 PA10浮空输入
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//PA9配置复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PA10浮空输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//2.配置USART1 -- 1+8+0+1 波特率 全双工
	//2.1 打开USART1时钟
	//2.2 配置USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	USART_InitTypeDef USART_InitStruct = {0};
	USART_InitStruct.USART_BaudRate = boud;//波特率
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//全双工
	USART_InitStruct.USART_Parity = USART_Parity_No;//不使用奇偶校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//停止位为1
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//字长为8
	USART_Init(USART1,&USART_InitStruct);
	//串口中断初始化
	USART1_IT_Config();
	//3.USART1使能
	USART_Cmd(USART1,ENABLE);
}

void USART1_IT_Config(void)
{
	//中断源
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
	//中断通道
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;//中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;//优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//优先级
	NVIC_Init(&NVIC_InitStruct);
}

////中断服务函数
//void USART1_IRQHandler(void)
//{
//	u8 data = 0;
//	//判断是否是接收中断
//	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET){
//		//请中断标志位
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
//		//数据处理
//		//1.读出来数据
//		data = USART1->DR;
//		USART1->DR = data;
//		//2.判断
//		if(data == 1){
//			BEEP_ON();
//			//printf("打开蜂鸣器\r\n");//所占时间比较长，建议不要写
//		}else if(data == 0){
//			BEEP_OFF();
//			//printf("关闭蜂鸣器\r\n");//所占时间比较长，建议不要写
//		}
//	}
//}

//中断服务函数 == 2个字节  第1个字节：响几次  第2个字节：时间间隔
//void USART1_IRQHandler(void)
//{
//	u8 data = 0;
//	//判断是否是接收中断
//	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET){
//		//请中断标志位
//		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
//		//数据处理
//		//1.读出来数据
//		data = USART1->DR;
////		USART1->DR = data;
//		if(u1_msg.rx_count < USART1_RX_LEN){//保存数据
//			u1_msg.rx_buff[u1_msg.rx_count] = data;
//			u1_msg.rx_count++;
//			if(u1_msg.rx_count == 2)
//				u1_msg.over_flag = 1;
//		}
//	}
//}
void USART1_IRQHandler(void)
{
	u8 data = 0;
	//判断是否是接收中断
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET){
		//请中断标志位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		//数据处理
		//1.读出来数据
		data = USART1->DR;
		if(u1_msg.rx_count < USART1_RX_LEN){//保存数据
			u1_msg.rx_buff[u1_msg.rx_count] = data;
			u1_msg.rx_count++;
		}
	}else if(USART_GetITStatus(USART1,USART_IT_IDLE) == SET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_IDLE);
		data = USART1->DR;//清除空闲中断标志位
		u1_msg.over_flag = 1;
	}
}
void BEEP_Set(u32 count,u32 time);
//数据解析
//void USART1_DataAnaly(void)
//{
//	if(u1_msg.over_flag == 0) return ;
//	u32 count,time;
//	count = u1_msg.rx_buff[0];
//	time = u1_msg.rx_buff[1];
//	BEEP_Set(count,time);
//	
//	u1_msg.over_flag = 0;
//	u1_msg.rx_count = 0;
//}
//void USART1_DataAnaly(void)
//{
//	if(u1_msg.over_flag == 0) return ;
//	u32 count,time;
//	count = u1_msg.rx_buff[0];
//	time = u1_msg.rx_buff[1];
//	BEEP_Set(count,time);
//	
//	u1_msg.over_flag = 0;
//	u1_msg.rx_count = 0;
//}
/*
制定1个新的协议：开始标志 + 设备号 + 功能码 + 结束标志
0XC5  -- 开始标志
X -- 设备号  1 -- 蜂鸣器  2 -- LED2
X -- 功能码  0 -- 关闭    1 -- 打开
0x5C -- 结束标志

上位机发送：0xC5 01 01 0x5C -- 下位机接收：结果 蜂鸣器响
*/
void USART1_DataAnaly(void)
{
	if(u1_msg.over_flag == 0) return ;//判断是否有数据到来
	
	//判断开始标志
	if(u1_msg.rx_buff[0] != 0xC5){
		printf("开始标志接收错误\r\n");
		goto over;
	}
	//判断结束标志
	if(u1_msg.rx_buff[3] != 0x5C){
		printf("结束标志接收错误\r\n");
		goto over;
	}
	u8 no = u1_msg.rx_buff[1];//设备号
	u8 f  = u1_msg.rx_buff[2];//功能码
	switch(no)
	{
		case 1:if(f == 1) BEEP_ON();
					 else if(f == 0) BEEP_OFF();
		break;
		case 2:if(f == 1) LEDx_ON(LED2_Port,LED2_Pin);
		       else if(f == 0)LEDx_OFF(LED2_Port,LED2_Pin);
		break;
	}
over:	
	u1_msg.over_flag = 0;
	u1_msg.rx_count = 0;
}
/*
发送1个字节数据
data:要发送的数据
*/
void USART1_SendOneData(u8 data)
{
	//等待上次数据发送完成
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET);
	//发送新的数据
	USART1->DR = data;//USART_SendData(USART1,data);
}
//发送字符串
//pdata -- 指向要发送的字符串
void USART1_SendString(char *pdata)
{
	while(*pdata)//判断是否遇到'\0'
	{
		USART1_SendOneData((u8)*pdata);
		pdata++;
	}
}

//发送N个字节数据
//pdata -- 指向要发送的数据的首地址
//len -- 要发送数据的长度
void USART1_SendNData(u8 *pdata,u32 len)
{
	for(u32 i=0;i<len;i++)//判断是否遇到'\0'
	{
		USART1_SendOneData(pdata[i]);
	}
}

/*
函数作用：接收1个字节数据
返回值：接收到的数据
*/
//u8 USART1_RecvOneData(void)
//{
//	u8 data = 0;
//	//判断是否接收到数据
//	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == RESET);
//	//读取，并返回
//	data = USART_ReceiveData(USART1);
//	return data;
//}
u8 USART1_RecvOneData(void)
{
	//判断是否接收到数据
	while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) == RESET);
	//读取，并返回
	return USART_ReceiveData(USART1);
}

void USART1_Echo(void)
{
	u8 data = 0;
	data = USART1_RecvOneData();
	USART1_SendOneData(data);
}

int fputc(int c,FILE *fp)
{
	USART1_SendOneData(c);//把原来要给文件的数据，传输到串口中
	return c;
}

void USART1_Test1(void)
{
	u8 data = 0;
	data = USART1_RecvOneData();
	if(data == 1){
		BEEP_ON();
	}else if(data == 0){
		BEEP_OFF();
	}
}


void USART1_DMAConfig(void)
{
	//1.使能USART1的DMA功能
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
	//2.初始化DMA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	DMA_InitTypeDef DMA_InitStruct = {0};
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)NULL;//数据源
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;//外设地址目标
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;//存储器->外设
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据宽度
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据宽度
	DMA_InitStruct.DMA_BufferSize = 0;//要传输的数据次数
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//地址增加
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不增加
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//单次模式
	DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;//优先级很高
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;//不是存储器到存储器
	DMA_Init(DMA1_Channel4,&DMA_InitStruct);
	//3.关闭DMA
	DMA_Cmd(DMA1_Channel4,DISABLE);
}

void USART1_DMASend(char *pdata,u32 tx_len)
{
	//1.关闭DMA
	DMA_Cmd(DMA1_Channel4,DISABLE);
	//2.配置DMA
	DMA_InitTypeDef DMA_InitStruct = {0};
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)pdata;//数据源
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;//外设地址目标
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;//存储器->外设
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;//存储器数据宽度
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据宽度
	DMA_InitStruct.DMA_BufferSize = tx_len;//要传输的数据次数
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//地址增加
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址不增加
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;//单次模式
	DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;//优先级很高
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;//不是存储器到存储器
	DMA_Init(DMA1_Channel4,&DMA_InitStruct);
	//3.使能DMA
	DMA_Cmd(DMA1_Channel4,ENABLE);
	//4.等待DMA传输完成
	while(DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET);
}

	


