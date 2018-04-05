#ifndef __FLASH_H
#define __FLASH_H


#include "stm32f1xx_hal.h"


void Flash_Read_Rocker(uint32_t Offset[4][3]);
void Flash_Write_Rocker(uint32_t Offset[4][3]);


#endif
