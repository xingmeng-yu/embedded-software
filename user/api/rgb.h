#ifndef _RGB_H_
#define _RGB_H_

#include "stm32f10x.h"

#define RGB_R_Port   GPIOA
#define RGB_R_Pin    GPIO_Pin_8
#define RGB_G_Port   GPIOA
#define RGB_G_Pin    GPIO_Pin_7
#define RGB_B_Port   GPIOA
#define RGB_B_Pin    GPIO_Pin_6

#define RGBx_ON(port,pin)     (GPIO_WriteBit(port,pin,(BitAction)0))
#define RGBx_OFF(port,pin)    (GPIO_WriteBit(port,pin,(BitAction)1))
#define RGBx_Toggle(port,pin) (port->ODR ^= pin)

void RGB_Config(void);
void RGB_Set(u8 state);
void RGB_Test(void);
#endif
