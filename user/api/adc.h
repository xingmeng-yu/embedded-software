#ifndef _ADC_H_
#define _ADC_H_

#include "stm32f10x.h"

extern u16 smoke_value;

void ADC1_Config(void);
void ADC1_GetValue1(void);
void ADC1_GetValue2(void);
void ADC1_DMAConfig(void);
void ADC1_GetValue3(void);
#endif
