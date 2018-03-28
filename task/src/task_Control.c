#include "task_Control.h"
#include "cmsis_os.h"
#include "Rocker.h"
#include "LC12S.h"


int8_t data[4];
void task_Control(const void *Parameters)
{
	TickType_t tick;
	protocolDetail pd;
	
	while(1)
	{
		tick = xTaskGetTickCount();
		
		pd.adjust = 1;
		pd.headMode = 1;
		pd.power = 56;
		LC12S_Send(&pd);
		
		vTaskDelayUntil(&tick, 20);
	}
}


