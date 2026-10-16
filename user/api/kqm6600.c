#include "kqm6600.h"
#include "stdio.h"
//定义一个接收KQM6600的传感器数据缓冲区
struct KQM6600_MESSAGE kqm6600_msg = {0};

struct KQM6600_DATA kqm6600_data = {0};


void USART2_IT_Config(void)
{
	//中断源
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
	//中断通道
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;//中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;//优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//优先级
	NVIC_Init(&NVIC_InitStruct);
}

//初始化USART2
//管脚 + 1+8+0+1 波特率  全双工  使能
void KQM6600_Config(void)
{
	//1.配置PA3
	//1.1 打开A端口时钟
	//1.3 PA3浮空输入
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PA3浮空输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	//2.配置USART2 -- 1+8+0+1 波特率 只接收
	//2.1 打开USART2时钟
	//2.2 配置USART2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	USART_InitTypeDef USART_InitStruct = {0};
	USART_InitStruct.USART_BaudRate = 9600;//波特率
	USART_InitStruct.USART_Mode = USART_Mode_Rx;//只接收
	USART_InitStruct.USART_Parity = USART_Parity_No;//不使用奇偶校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//停止位为1
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//字长为8
	USART_Init(USART2,&USART_InitStruct);
	//串口中断初始化
	USART2_IT_Config();
	//3.USART2使能
	USART_Cmd(USART2,ENABLE);
}

//void USART2_IRQHandler(void)
//{
//	u8 data = 0;
//	//判断是否是接收中断
//	if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET){
//		//请中断标志位
//		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
//		//数据处理
//		//1.读出来数据
//		data = USART2->DR;
//		if(kqm6600_msg.rx_count < KQM6600_RX_LEN){//保存数据
//			kqm6600_msg.rx_buff[kqm6600_msg.rx_count] = data;
//			kqm6600_msg.rx_count++;
//		}
//	}else if(USART_GetITStatus(USART2,USART_IT_IDLE) == SET)
//	{
//		USART_ClearITPendingBit(USART2,USART_IT_IDLE);
//		data = USART2->DR;//清除空闲中断标志位
//		kqm6600_msg.over_flag = 1;
//		//发送一个信号量
//	}
//}


/*
KQM6600自动每秒发送8个字节
BYTE1  BYTE2      BYTE3      BYTE4      BYTE5     BYTE6      BYTE7       BYTE8
识别码 数据1高位  数据1低位  数据2高位  数据2低位 数据3高位  数据3低位   校验值
*/
void KQM6600_DataAnaly(void)
{
	if(kqm6600_msg.over_flag == 0) return ;//判断是否有数据到来
	
	//判断开始标志
	if(kqm6600_msg.rx_buff[0] != 0x5f){
		printf("KQM6600开始标志接收错误\r\n");
		goto over;
	}
	//校验
	u32 check = 0;
	for(u32 i=0;i<7;i++){
		check += kqm6600_msg.rx_buff[i];
	}
	if((check & 0xff) != kqm6600_msg.rx_buff[7]){
		printf("KQM6600校验错误\r\n");
		goto over;
	}
	//解析数据
	kqm6600_data.VOC = ((kqm6600_msg.rx_buff[1]*256)+kqm6600_msg.rx_buff[2])*0.1;
	kqm6600_data.CH2O = ((kqm6600_msg.rx_buff[3]*256)+kqm6600_msg.rx_buff[4])*0.01;
	kqm6600_data.CO2= ((kqm6600_msg.rx_buff[5]*256)+kqm6600_msg.rx_buff[6]);
	//打印传感器数据
	printf("VOC = %.1f\r\n",kqm6600_data.VOC);
	printf("CH2O = %.2f\r\n",kqm6600_data.CH2O);
	printf("CO2 = %5d\r\n",kqm6600_data.CO2);
over:	
	kqm6600_msg.over_flag = 0;
	kqm6600_msg.rx_count = 0;
}



