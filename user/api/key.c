#include "key.h"

//KEY1 -- PA0
//KEY2 -- PC4
//KEY3 -- PC5
//KEY4 -- PC6
void Key_Config_Lib(void)
{
	//1.开A端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	//2.PA0配置通用推挽输出、默认选50Mhz
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
}
void Key_Config_R(void)
{
	//1.开A端口时钟
	RCC->APB2ENR |= (1<<2);
	//2.PA0配置通用推挽输出、默认选50Mhz
	GPIOA->CRL &= ~(0xF<<0);
	GPIOA->CRL |=  (0x4<<0);
}

#include "delay.h"
//获取按键值
//成功返回1 失败返回0
//u8 Key1_GetValue(void)
//{
//	if(KEY1 == 1){
//		Delay_nms_nop(20);//消抖 -- 窗口法（延时检测） -- 根据抖动来
//		if(KEY1 == 1){
//			while(KEY1 == 1);//松手检测
//			return 1;
//		}
//	}
//	return 0;
//}
//每隔10ms调用
u8 Key1_GetValue(void)
{
	static u32 count = 0;
	if(KEY1 == 1) {
		count++;
		return 2;
	}//按下状态
	else if(count >= 15){//松手状态
		count = 0;
		return 1;
	}else{//抖动状态
		count = 0;
	}
	return 0;
}
//每隔10ms调用
u8 Key2_GetValue(void)
{
	static u32 count = 0;
	if(KEY2 == 1) count++;//按下状态
	else if(count >= 15){//松手状态
		count = 0;
		return 1;
	}else{//抖动状态
		count = 0;
	}
	return 0;
}

