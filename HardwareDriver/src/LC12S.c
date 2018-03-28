
#include <string.h>
#include "LC12S.h"
#include "externParam.h"
#include "cmsis_os.h"


#define	LS12S_SET	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET)
#define	LS12S_RESET	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET)


//将协议的标志位结合，与数据一起压缩后并用于传输的联合体
typedef struct 
{
	uint8_t head[2];
	uint8_t mode;
	uint8_t power;
	int8_t LR;
	int8_t FB;
	uint8_t adjust;
	uint8_t sum;
	uint8_t end[2];
}protocolTransmit;


static protocolTransmit SendBuffer;
static uint8_t ReceiveBuffer[32];

//LC12S初始化参数
static uint8_t LS12SParam[18] = {0xAA,0x5A,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x12,0x00,0x00};


static void LC12S_PT2PD(const protocolDetail *pd, protocolTransmit *pt);

	
//LC12S初始化
uint8_t LC12S_Init(uint16_t Net)
{
	LS12S_SET;
	osDelay(30);
	LS12S_RESET;  
	osDelay(5);
	
	LS12SParam[4] = Net >> 8;
	LS12SParam[5] = Net;
	LS12SParam[17] = 0;
	for(uint8_t i = 0; i < 17; ++i)
	{
		LS12SParam[17] += LS12SParam[i];
	}
	if(HAL_UART_Transmit(&huart1, LS12SParam, 18, 100) != HAL_OK)
	{
		return 0;
	}
	if(HAL_UART_Receive(&huart1, ReceiveBuffer, 18, 100) != HAL_OK)
	{
		return 0;
	}
	
	uint8_t sun = 0;
	for(uint8_t i = 0; i < 17; ++i)
	{
		sun += ReceiveBuffer[i];
	}
	if(sun != ReceiveBuffer[17])
	{
		return 0;
	}
	
	LS12S_SET;
	
	return 1;
}


void LC12S_Send(const protocolDetail *pd)
{
	LC12S_PT2PD(pd, &SendBuffer);
	HAL_UART_Transmit_IT(&huart1, (uint8_t *)(&SendBuffer), sizeof(protocolTransmit));
}


//将protocolDetail转化为protocolTransmit
static void LC12S_PT2PD(const protocolDetail *pd, protocolTransmit *pt)
{
	memset(pt, 0, sizeof(protocolTransmit));
	
	pt->head[0] = 0xFF;
	pt->head[1] = 0x01;
	if(pd->locked)
	{
		pt->mode |= 0x80;
	}
	if(pd->headMode)
	{
		pt->mode |= 0x40;
	}
	pt->power = pd->power;
	pt->LR = pd->LR;
	pt->FB = pd->FB;
	pt->adjust = pd->adjust;
	uint8_t sum = 0;
	for(uint8_t i = 0; i < sizeof(protocolTransmit) - 1; ++i)
	{
		sum += *((uint8_t *)pt + i);
	}
	pt->sum = sum;
	pt->end[0] = 0xFF;
	pt->end[1] = 0x02;
}





