#ifndef _KEY_H_
#define _KEY_H_

#include "stm32f10x.h"

#define KEY1_Port GPIOA
#define KEY1_Pin  GPIO_Pin_0

#define KEY2_Port GPIOC
#define KEY2_Pin  GPIO_Pin_4

//#define KEY1 (KEY1_Port->IDR & (0x1<<0))//Õæ !=0  ¼Ù = 0
#define KEY1 (GPIO_ReadInputDataBit(KEY1_Port,KEY1_Pin))
#define KEY2 (GPIO_ReadInputDataBit(KEY2_Port,KEY2_Pin))


void Key_Config_Lib(void);
void Key_Config_R(void);
u8 Key1_GetValue(void);
u8 Key2_GetValue(void);
#endif
