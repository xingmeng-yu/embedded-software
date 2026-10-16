#include "timer.h"


void TIM6_Config(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	//定时器初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct = {0};
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStruct.TIM_Period = 10000-1;//重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200-1;//分频
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);
	//配置中断源
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	//定时完成，产生中断
	NVIC_InitTypeDef NVIC_InitStruct = {0};
	NVIC_InitStruct.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
	//使能TIM6
	TIM_Cmd(TIM6,ENABLE);
}
#include "beep.h"
void TIM6_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		BEEP_Toggle();
	}
}

