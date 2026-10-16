#include "rgb.h"

//RGB灯初始化
//R -- PA8 G -- PA7 B -- PA6
void RGB_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_6;//管脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//通用推挽
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

/*
设置RGB灯的状态
state -- 0 //全灭
state -- 1 //全亮 -- 白灯
state -- 2 //R -- 红灯
state -- 3 //G -- 绿灯
state -- 2 //B -- 蓝灯
*/
void RGB_Set(u8 state)
{
	//所有灯关闭
	RGBx_OFF(RGB_R_Port,RGB_R_Pin);
	RGBx_OFF(RGB_G_Port,RGB_G_Pin);
	RGBx_OFF(RGB_B_Port,RGB_B_Pin);
	switch(state)
	{
		case 1:RGBx_ON(RGB_R_Port,RGB_R_Pin);
				   RGBx_ON(RGB_G_Port,RGB_G_Pin);
		       RGBx_ON(RGB_B_Port,RGB_B_Pin);
			break;
		case 2:RGBx_ON(RGB_R_Port,RGB_R_Pin);break;
		case 3:RGBx_ON(RGB_G_Port,RGB_G_Pin);break;
		case 4:RGBx_ON(RGB_B_Port,RGB_B_Pin);break;
		default:break;
	}
}

void RGB_Test(void)
{
	static u8 state = 0;
	RGB_Set(state++);
	if(state == 5) state = 0;
}

