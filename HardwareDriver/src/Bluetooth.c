
#include <string.h>
#include "Bluetooth.h"
#include "externParam.h"
#include "cmsis_os.h"


//将协议的标志位结合，与数据一起压缩后并用于传输的联合体
typedef struct 
{
	uint8_t LH;
	uint8_t power;
	int8_t LR;
	int8_t FB;
	int8_t SP;
	uint8_t adjust;
}SendProtocolTransmit;


static uint8_t SendBuffer[2 * sizeof(SendProtocolTransmit) + 4];
static uint8_t ReceiveBuffer[32];

//Bluetooth初始化参数
static uint8_t LS12SParam[18] = {0xAA,0x5A,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x12,0x00,0x00};


static void Bluetooth_PT2PD(const SendProtocolDetail *pd, SendProtocolTransmit *pt);

	
//Bluetooth初始化
uint8_t Bluetooth_Init(uint16_t Net)
{
	return 1;
}


void Bluetooth_Send(const SendProtocolDetail *pd)
{
	SendProtocolTransmit pt;
	
	memset(&pt, 0, sizeof(SendProtocolTransmit));
	
	if(pd->locked)
	{
		pt.LH |= 0x80;
	}
	if(pd->headMode)
	{
		pt.LH |= 0x40;
	}
	pt.power = pd->power;
	pt.LR = pd->LR;
	pt.FB = pd->FB;
	pt.SP = pd->SP;
	pt.adjust = pd->adjust;
	
	uint8_t lendgth = 0;
	SendBuffer[lendgth++] = 0xFF;
	SendBuffer[lendgth++] = 0x01;
	for(uint8_t i = 0; i < sizeof(SendProtocolTransmit); ++i)
	{
		if(*((uint8_t *)(&pt) + i) == 0xFF)
		{
			SendBuffer[lendgth++] = 0xFF;
			SendBuffer[lendgth++] = 0x00;
		}
		else
		{
			SendBuffer[lendgth++] = *((uint8_t *)(&pt) + i);
		}
	}
	
	SendBuffer[lendgth] = 0;
	for(uint8_t i = 2; i < lendgth; ++i)
	{
		SendBuffer[lendgth] += SendBuffer[i];
	}
	
	lendgth++;
	
	SendBuffer[lendgth++] = 0xFF;
	SendBuffer[lendgth++] = 0x02;
	
	HAL_UART_Transmit_IT(&huart1, (uint8_t *)(&SendBuffer), lendgth);
}






