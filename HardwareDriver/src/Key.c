#include "Key.h"


GPIO_TypeDef *KeyPort[KEYNUM] = 
{
	GPIOB, 
	GPIOB, 
	GPIOB, 
	GPIOA,
	GPIOB, 
	GPIOB, 
	GPIOA,
	GPIOA,
};
uint16_t KeyPin[KEYNUM] = 
{
	GPIO_PIN_10, 
	GPIO_PIN_11, 
	GPIO_PIN_15,
	GPIO_PIN_8,
	GPIO_PIN_13, 
	GPIO_PIN_14, 
	GPIO_PIN_11, 
	GPIO_PIN_12, 
};


static KeyStatusType CurrentStatus, LastStatus;


void Key_InitConfig(void)
{
	for(uint8_t i = 0; i < KEYNUM; ++i)
	{
		LastStatus.pushed[i] = 1;
		LastStatus.trigger[i] = 0;
	}
}


void Key_Update(KeyStatusType *output)
{
	LastStatus = CurrentStatus;
	
	for(uint8_t i = 0; i < KEYNUM; ++i)
	{
		CurrentStatus.pushed[i] = HAL_GPIO_ReadPin(KeyPort[i], KeyPin[i]);
		if((CurrentStatus.pushed[i] == 0)
			&& (LastStatus.pushed[i] != 0))
		{
			CurrentStatus.trigger[i] = 1;
		}
		else if((CurrentStatus.pushed[i] != 0)
			&& (LastStatus.pushed[i] == 0))
		{
			CurrentStatus.trigger[i] = 2;
		}
		else
		{
			CurrentStatus.trigger[i] = 0;
		}
	}
	
	*output = CurrentStatus;
}

