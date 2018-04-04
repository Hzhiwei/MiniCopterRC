#include "Switch.h"


void Switch_InitConfig(void)
{
	
}

uint8_t Switch_Get(uint8_t index)
{
	switch(index)
	{
		case 0 : return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3);
		case 1 : return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);
		default: return 0;
	}
}
