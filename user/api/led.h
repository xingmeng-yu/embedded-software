#ifndef _LED_H_
#define _LED_H_

#include "stm32f10x.h"

void LED_Config(void);
void LED1_On(void);
void LED1_Off(void);

void LED_Config_Lib(void);
void LED1_On_Lib(void);
void LED1_Off_Lib(void);

#define LED1_Port   GPIOE
#define LED1_Pin    GPIO_Pin_2
#define LED2_Port   GPIOE
#define LED2_Pin    GPIO_Pin_3
#define LED3_Port   GPIOE
#define LED3_Pin    GPIO_Pin_4
#define LED4_Port   GPIOE
#define LED4_Pin    GPIO_Pin_5

//#define LED1_ON()    (GPIO_WriteBit(LED1_Port,LED1_Pin,0))
//#define LED1_OFF()   (GPIO_WriteBit(LED1_Port,LED1_Pin,1))

#define LEDx_ON(port,pin)     (GPIO_WriteBit(port,pin,(BitAction)0))
#define LEDx_OFF(port,pin)    (GPIO_WriteBit(port,pin,(BitAction)1))
#define LEDx_Toggle(port,pin) (port->ODR ^= pin)

void LED_LS(void);

#endif
