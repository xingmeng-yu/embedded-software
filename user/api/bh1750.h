#ifndef _BH1750_H_
#define _BH1750_H_

#include "stm32f10x.h"
#include "iic.h"

extern u16 light_value;

void BH1750_Config(void);
u8 BH1750_Star(void);
u8 BH1750_GetValue(void);
#endif
