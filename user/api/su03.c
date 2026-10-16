#include "su03.h"

void UART4_IT_Config()
{
	//中断源
	USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);
	USART_ITConfig(UART4,USART_IT_IDLE,ENABLE);
	//中断通道
	NVIC_InitTypeDef NVIC_InitStruct={0};
	NVIC_InitStruct.NVIC_IRQChannel=UART4_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruct);
}
void SU03_Config(void)
{
	//1.配置PC10和Pc11
	//1.1 打开c端口时钟
	//1.2 Pc10配置复用推挽输出
	//1.3 Pc11浮空输入
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;//PC10配置复用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//PC11浮空输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	//2.配置USART4 -- 1+8+0+1 波特率 全双工
	//2.1 打开USART4时钟
	//2.2 配置USART4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	
	USART_InitTypeDef USART_InitStruct = {0};
	USART_InitStruct.USART_BaudRate = 9600;//波特率
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//全双工
	USART_InitStruct.USART_Parity = USART_Parity_No;//不使用奇偶校验
	USART_InitStruct.USART_StopBits = USART_StopBits_1;//停止位为1
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;//字长为8
	USART_Init(UART4,&USART_InitStruct);
	//串口中断初始化
	UART4_IT_Config();
	//3.USART1使能
	USART_Cmd(UART4,ENABLE);
}

struct SU03_MESSAGE su03_msg={0};
void UART4_IRQHandler(void)
{
	u8 data=0;
	//判断是否接受中断
	if(USART_GetITStatus(UART4,USART_IT_RXNE)==SET){
		//清除中断标志位
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);
		//数据处理
		data=UART4->DR;
		if(su03_msg.rx_count<SU03_RX_LEN){//保存数据
			su03_msg.rx_buff[su03_msg.rx_count]=data;
			su03_msg.rx_count++;
		}
	}
	else if(USART_GetITStatus(UART4,USART_IT_IDLE)==SET){
		USART_ClearITPendingBit(UART4,USART_IT_IDLE);
		data=UART4->DR;
		su03_msg.over_flag=1;
	}
}
/*
01	打开蜂鸣器
02	关闭蜂鸣器
03	当前室内温度
04	当前室内湿度
05	VOC
06	CO2
07	CH2O
08	RGB打开彩灯
09	RGB关闭彩灯
*/
void SU03_DataAnaly(void){
	if(su03_msg.over_flag==0)return;
	
	u8 data=su03_msg.rx_buff[0];
	switch(data){
		case 1:BEEP_ON();
			break;
		case 2:BEEP_OFF();
			break;
		case 3:_uart_play_t2((int)tem);
			break;
		case 4:_uart_play_h2((int)hum);
			break;
		case 5:_uart_play2_voc((double)kqm6600_data.VOC);
			break;
		case 6:_uart_play2_co2((int)kqm6600_data.CO2);
			break;
		case 7:_uart_play2_ch2o((double)kqm6600_data.CH2O);
			break;
		
	}
	su03_msg.over_flag=0;
	su03_msg.rx_count=0;
}

//uart4
void UART4_SendOneData(u8 data)
{
	while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);
	UART4->DR=data;
}
void UART4_SendNData(u8 *pdata,u32 len)
{
	for(int i=0;i<len;i++){
		UART4_SendOneData(pdata[i]);
	}
}

u8 UART4_ReceOneData(void)
{
	while(USART_GetFlagStatus(UART4,USART_FLAG_RXNE)==RESET);
	return USART_ReceiveData(UART4);
}
