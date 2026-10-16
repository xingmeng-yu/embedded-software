#ifndef _IIC_H_
#define _IIC_H_

#include "stm32f10x.h"

#define SDA_H (GPIO_SetBits(GPIOB,GPIO_Pin_7))
#define SDA_L (GPIO_ResetBits(GPIOB,GPIO_Pin_7))

#define SCL_H (GPIO_SetBits(GPIOB,GPIO_Pin_6))
#define SCL_L (GPIO_ResetBits(GPIOB,GPIO_Pin_6))

#define DATA_SDA (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7))


void IIC_Config(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_SendData(u8 data);
u8 IIC_RecvData(void);
u8 IIC_GetAck(void);
void IIC_SendAck(u8 flag);
#endif
