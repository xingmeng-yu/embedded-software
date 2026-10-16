#include "beep.h"

//PC0配置通用推挽输出、默认选50Mhz
void BEEP_Config_R(void)
{
	//1.开C端口时钟
	RCC->APB2ENR |= (1<<4); 
	//2.PC0配置通用推挽输出、默认选50Mhz
	GPIOC->CRL &= ~(0xf<<0);
	GPIOC->CRL |=  (0x3<<0);
}

void BEEP_Config_Lib(void)
{
	//1.开C端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	//2.PC0配置通用推挽输出、默认选50Mhz
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC,&GPIO_InitStruct);
}

