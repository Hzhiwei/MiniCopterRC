#ifndef __DRIVER_OLED_H
#define __DRIVER_OLED_H


#include "stm32f1xx_hal.h"


void OLED_InitConfig(void);
void OLED_ON(void);
void OLED_SetPos(uint8_t x, uint8_t y);
void OLED_RefreshGRAM(void);
void OLED_ClearDisplay(uint8_t color);
void OLED_ClearGRAM(uint8_t color);
void OLED_DrawF6x8Pic(uint8_t x, uint8_t y, char chr);
void OLED_DrawF6x8Char(uint8_t x, uint8_t y, char chr);
void OLED_DrawF6x8String(uint8_t x, uint8_t y, char *chr);


#endif
