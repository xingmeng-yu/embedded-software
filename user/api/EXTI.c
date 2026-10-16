#include "exti.h"

void EXTI0_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//2.EXT0功能映射到PA0
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource0);
	//3.EXTI0配置（边沿、使能）
	EXTI_InitTypeDef EXTI_InitStruct = {0};
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;//EXTI0
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;//使能
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿
	EXTI_Init(&EXTI_InitStruct);
	//4.NVIC配置（通道使能和优先级）
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;//中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
}

void BEEP_Set(u32 count,u32 time);
#include "wifi.h"
////中断服务函数
//void EXTI0_IRQHandler(void)
//{
//	//1.清楚中断标志位
//	EXTI_ClearITPendingBit(EXTI_Line0);
//	//2.执行相关代码
//	BEEP_Set(3,50);
////	WIFI_StateChange(2);
//}

void EXTI4_Config(void)
{
	//1.打开AFIO的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//2.EXT4功能映射到PC4
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);
	//3.EXTI4配置（边沿、使能）
	EXTI_InitTypeDef EXTI_InitStruct = {0};
	EXTI_InitStruct.EXTI_Line = EXTI_Line4;//EXTI4
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;//使能
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿
//	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//双边沿
	EXTI_Init(&EXTI_InitStruct);
	//4.NVIC配置（通道使能和优先级）
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;//中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;//优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//优先级
	NVIC_Init(&NVIC_InitStruct);
}

//中断服务函数
void EXTI4_IRQHandler(void)
{
	static u8 flag = 0;
	//1.清楚中断标志位
	EXTI_ClearITPendingBit(EXTI_Line4);
	//2.执行相关代码
	
}
//void EXTI4_IRQHandler(void)
//{
//	static u8 flag = 0;
//	//1.清楚中断标志位
//	EXTI_ClearITPendingBit(EXTI_Line4);
//	//2.执行相关代码
//	if(flag == 0){//下降沿 -- 第一个沿（按键按下）
//		BEEP_Set(2,100);
//		flag = 1;
//	}else if(flag == 1){//上升沿 -- 第二个沿（按键松开）
//		BEEP_Set(2,1000);
//		flag = 0;
//	}
//}

//中断服务函数
//void EXTI4_IRQHandler(void)
//{
//	static u8 flag = 0;
//	EXTI_InitTypeDef EXTI_InitStruct = {0};
//	EXTI_InitStruct.EXTI_Line = EXTI_Line4;//EXTI4
//	EXTI_InitStruct.EXTI_LineCmd = ENABLE;//使能
//	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式
//	//1.清楚中断标志位
//	EXTI_ClearITPendingBit(EXTI_Line4);
//	//2.执行相关代码
//	if(flag == 0){//下降沿 -- 第一个沿（按键按下）
//		BEEP_Set(2,100);
//		flag = 1;
//		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;//上升沿
//		EXTI_Init(&EXTI_InitStruct);
//	}else if(flag == 1){//上升沿 -- 第二个沿（按键松开）
//		BEEP_Set(2,1000);
//		flag = 0;
//		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿
//		EXTI_Init(&EXTI_InitStruct);
//	}
//}


void EXTI5_Config(void)
{
	//1.打开AFIO的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//2.EXT5功能映射到PC5
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource5);
	//3.EXTI5配置（边沿、使能）
	EXTI_InitTypeDef EXTI_InitStruct = {0};
	EXTI_InitStruct.EXTI_Line = EXTI_Line5;//EXTI5
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;//使能
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿
	EXTI_Init(&EXTI_InitStruct);
	//4.NVIC配置（通道使能和优先级）
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;//中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
}

void EXTI6_Config(void)
{
	//1.打开AFIO的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	//2.EXT5功能映射到PC5
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource6);
	//3.EXTI5配置（边沿、使能）
	EXTI_InitTypeDef EXTI_InitStruct = {0};
	EXTI_InitStruct.EXTI_Line = EXTI_Line6;//EXTI5
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;//使能
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;//中断模式
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿
	EXTI_Init(&EXTI_InitStruct);
	//4.NVIC配置（通道使能和优先级）
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;//中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
}

//中断服务函数 -- EXTI5 6共用一个中断服务函数
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5) == SET){
		//1.清楚中断标志位
		EXTI_ClearITPendingBit(EXTI_Line5);
		//2.执行相关代码
		
	}
	if(EXTI_GetITStatus(EXTI_Line6) == SET){
		//1.清楚中断标志位
		EXTI_ClearITPendingBit(EXTI_Line6);
		//2.执行相关代码
		
	}
}


