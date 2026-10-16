#ifndef _DHT11_H_
#define _DHT11_H_

#include "stm32f10x.h"

#define DHT11_MODE_IN    0
#define DHT11_MODE_OUT   1

extern float tem,hum;

void DHT11_ModeConfig(u8 mode);
u8 DHT11_GetValue(float *tem,float *hum);
void DHT11_Test(void);
#endif
