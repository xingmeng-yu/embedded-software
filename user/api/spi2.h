#ifndef _SPI2_H_
#define _SPI2_H_

#include "stm32f10x.h"

#define	FLASH_W25Q64_CS(x)	(x?(GPIO_SetBits(GPIOB, GPIO_Pin_12)):(GPIO_ResetBits(GPIOB, GPIO_Pin_12)))	


void SPI2_Config(void);
u8 SPI2_SendAndRecv(u8 data) ;
#endif

