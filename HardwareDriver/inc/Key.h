#ifndef __KEY_H
#define __KEY_H

#include "stm32f1xx_hal.h"

#define KEYNUM	8


typedef struct
{
	uint8_t pushed[KEYNUM];	//0 按下	1 弹起
	uint8_t trigger[KEYNUM];	//0 无	1 按下瞬间	2 松开瞬间
}KeyStatusType;


void Key_InitConfig(void);
void Key_Update(KeyStatusType *output);


#endif
