#ifndef __GPIO_H
#define __GPIO_H 
#include "sys.h"

// Out
#define RED PBout(9)	// PB9

// In
#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8) //��ȡ����0

void Usr_Gpio_Init(void); //��ʼ��
u8 Key_Scan(u8 mode);

#endif

