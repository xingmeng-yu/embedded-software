#ifndef _BEEP_H_
#define _BEEP_H_

#include "stm32f10x.h"

#define BEEP_Port   GPIOC
#define BEEP_Pin    GPIO_Pin_0

#define BEEP_ON()    (GPIO_WriteBit(BEEP_Port,BEEP_Pin,1))
#define BEEP_OFF()   (GPIO_WriteBit(BEEP_Port,BEEP_Pin,0))
#define BEEP_Toggle()  (BEEP_Port->ODR ^= GPIO_Pin_0)//GPIOC->ODR ^= (1<<0)

void BEEP_Config_R(void);
void BEEP_Config_Lib(void);
#endif
