#include "led.h"

//定义4个寄存器
#define GPIOE_BASE_ADDR 0x40011800
#define rGPIOE_CRL   *((volatile unsigned int *)(GPIOE_BASE_ADDR+0x00))
#define rGPIOE_CRH   *((volatile unsigned int *)(GPIOE_BASE_ADDR+0x04))
#define rGPIOE_IDR   *((volatile unsigned int *)(GPIOE_BASE_ADDR+0x08))
#define rGPIOE_ODR   *((volatile unsigned int *)(GPIOE_BASE_ADDR+0x0C))		
#define rGPIOE_BSRR   *((volatile unsigned int *)(GPIOE_BASE_ADDR+0x10))		
#define rGPIOE_BRR   *((volatile unsigned int *)(GPIOE_BASE_ADDR+0x14))		
	
#define rRCC_APB2ENR  *((volatile unsigned int *)(0x40021000+0x18))

void LED_Config(void)
{
	//把E端口时钟打开
	rRCC_APB2ENR |= (1<<6);
	//PE2配置为通用推挽输出模式 50MHz
	//GPIOE_RCL -- 11~8位  00 11 = 3
	//先清零，再置位
	rGPIOE_CRL &= ~(0xf<<8);
	//rGPIOE_CRL &= ~((1<<8)|(1<<9)|(1<<10)|(1<<11));
	rGPIOE_CRL |=  (0x3<<8);
	//rGPIOE_CRL |=  (1<<8)|(1<<9);
}

//点亮LED1 = PE2 输出 0
void LED1_On(void)
{
//	rGPIOE_ODR &= ~(1<<2);
	rGPIOE_BSRR |= (1<<18);
	
}

//点亮LED1 = PE2 输出 1
void LED1_Off(void)
{
//	rGPIOE_ODR |= (1<<2);
	rGPIOE_BSRR |= (1<<2);
}


void LED_Config_Lib(void)
{
	//把E端口时钟打开
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	//PE2配置为通用推挽输出模式 50MHz
	//1.定义一个结构体变量（初始化GPIO口）
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;//管脚2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//通用推挽
	GPIO_Init(GPIOE,&GPIO_InitStructure);
}

//点亮LED1 = PE2 输出 0
void LED1_On_Lib(void)
{
//	rGPIOE_ODR &= ~(1<<2);
	GPIO_WriteBit(GPIOE,GPIO_Pin_2,Bit_RESET);
}

//熄灭亮LED1 = PE2 输出 1
void LED1_Off_Lib(void)
{
//	rGPIOE_ODR |= (1<<2);
	GPIO_WriteBit(GPIOE,GPIO_Pin_2,Bit_SET);
}
//状态机实现流水灯
void LED_LS(void)
{
	static u8 state = 0;
	switch(state)
	{
		case 0:
			LEDx_ON(LED2_Port,LED2_Pin);LEDx_OFF(LED3_Port,LED3_Pin);LEDx_OFF(LED4_Port,LED4_Pin);
			break;
		case 1:
			LEDx_ON(LED3_Port,LED3_Pin);LEDx_OFF(LED2_Port,LED2_Pin);LEDx_OFF(LED4_Port,LED4_Pin);
			break;
		case 2:
			LEDx_ON(LED4_Port,LED4_Pin);LEDx_OFF(LED3_Port,LED3_Pin);LEDx_OFF(LED2_Port,LED2_Pin);
			break;
	}
	state++;
	if(state == 3) state = 0;
}
