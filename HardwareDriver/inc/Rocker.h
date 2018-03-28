#ifndef __ROCKER_H
#define __ROCKER_H


#include "stm32f1xx_hal.h"


void Rocker_Init(void);
void Rocker_GetData(int8_t data[4]);


#endif
