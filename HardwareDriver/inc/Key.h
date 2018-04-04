#ifndef __KEY_H
#define __KEY_H

#include "stm32f1xx_hal.h"

#define KEYNUM	8


typedef struct
{
	uint8_t pushed[KEYNUM];	//0 ����	1 ����
	uint8_t trigger[KEYNUM];	//0 ��	1 ����˲��	2 �ɿ�˲��
}KeyStatusType;


void Key_InitConfig(void);
void Key_Update(KeyStatusType *output);


#endif
