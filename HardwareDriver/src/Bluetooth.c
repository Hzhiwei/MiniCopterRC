
#include <string.h>
#include "Bluetooth.h"
#include "externParam.h"
#include "cmsis_os.h"



#define RBLENGTH	64


//��Э��ı�־λ��ϣ�������һ��ѹ�������ڴ����������
typedef struct 
{
	uint8_t LH;
	uint8_t power;
	int8_t LR;
	int8_t FB;
	int8_t SP;
	uint8_t adjust;
}SendProtocolTransmit;


//ͨ��Э��ĸ�����־λ�����ݷֿ���ϸ��
typedef struct 
{
	//ģʽ 0 ֹͣ	1����	2�ȴ��׷�
	uint8_t mode;
	//����ƫ�Ƶ���
	int8_t LRoffset;
	//ǰ��ƫ�Ƶ���	
	int8_t FBoffset;			
}ReceiveProtocolTransmit;


static ReceiveProtocolDetail currentStatus;


static uint8_t SendBuffer[2 * sizeof(SendProtocolTransmit) + 4];
static uint8_t ReceiveBuffer[32];


static void SendString(char *data);


//Bluetooth��ʼ��
uint8_t Bluetooth_Init(uint16_t Net)
{
	return 0;
}

HAL_StatusTypeDef Bluetooth_Pair(void)
{
	osDelay(500);
	ReceiveBuffer[0] = 0;
	SendString("AT+DEFAULT");
	if(HAL_OK != HAL_UART_Receive(&huart1, (uint8_t *)(&ReceiveBuffer), 2, 500))
	{
		return HAL_ERROR;
	}
	if(ReceiveBuffer[0] != 'O')
	{
		return HAL_ERROR;
	}
	osDelay(500);
	ReceiveBuffer[0] = 0;
	SendString("AT+ROLE=M");
	if(HAL_OK != HAL_UART_Receive(&huart1, (uint8_t *)(&ReceiveBuffer), 2, 500))
	{
		return HAL_ERROR;
	}
	if(ReceiveBuffer[0] != 'O')
	{
		return HAL_ERROR;
	}
	osDelay(500);
	ReceiveBuffer[0] = 0;
	SendString("AT+CLEAR");
	if(HAL_OK != HAL_UART_Receive(&huart1, (uint8_t *)(&ReceiveBuffer), 2, 500))
	{
		return HAL_ERROR;
	}
	if(ReceiveBuffer[0] != 'O')
	{
		return HAL_ERROR;
	}
	return HAL_OK;
}

static void SendString(char *data)
{
	uint8_t length = 0;
	while(*data)
	{
		SendBuffer[length++] = *(data++);
	}
	HAL_UART_Transmit(&huart1, (uint8_t *)(&SendBuffer), length, 500);
}


/********************************	����	*************************************/


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


/********************************	����	*************************************/

void Bluetooth_Start(void)
{
	HAL_UART_Receive_IT(&huart1, ReceiveBuffer, RBLENGTH);
}


uint8_t Bluetooth_ReceiveAnalyze(void)
{
	#define DATAREPOLENGTH	128	//���ݴ����λ��泤��
	#define NEXT(x)	(((x)+1)%DATAREPOLENGTH)
	#define PREVIOUS(x)	(((x)+DATAREPOLENGTH-1)%DATAREPOLENGTH)
	
	static uint8_t dataRepo[DATAREPOLENGTH];	//���ݴ����λ���
	static uint8_t dataStartPoint = 0;	//��һ�����������ݴ����λ��濪ʼָ��
	static uint8_t dataEndPoint = 0; //��һ�����������ݴ����λ������ָ��
	
	uint8_t dataArranged[sizeof(ReceiveProtocolTransmit)];	//�������õ�����
	
	//�������������ݴ����λ���
	for(uint8_t i = 0; i < RBLENGTH - huart1.RxXferCount; ++i)
	{
		dataRepo[dataEndPoint] = ReceiveBuffer[i];
		dataEndPoint = NEXT(dataEndPoint);
	}
	
	//���¿�ʼ����
	HAL_UART_AbortReceive_IT(&huart1);
	HAL_UART_Receive_IT(&huart1, ReceiveBuffer, RBLENGTH);
	
	//����֡ͷ��֡β
	static uint8_t Flag = 0;	//����״̬��0������֡β��1����֡β�����֡ͷ��2��������֡
	static uint8_t frameStart;	//��֡ͷ������ݵĵ�һ�����ݵ�λ��
	static uint8_t frameEnd;	//��֡β������ݵ����һ�����ݵ���һ��λ��
	static uint8_t dataLength;
	dataLength = dataStartPoint <= dataEndPoint ? dataEndPoint - dataStartPoint : DATAREPOLENGTH - dataStartPoint - 1 + dataEndPoint;
	
	Flag = 0;
	if(dataLength >= sizeof(ReceiveProtocolTransmit) + 4)
	{
		for(int i = PREVIOUS(PREVIOUS(dataEndPoint)); i != dataStartPoint; i = PREVIOUS(i))
		{
			switch(Flag)
			{
				case 0 :
				{
					if((dataRepo[i] == 0xFF) && (dataRepo[NEXT(i)] == 0x02))
					{
						frameEnd = i;
						Flag = 1;
					}
					break;
				}
				case 1 :
				{
					if((dataRepo[i] == 0xFF) && (dataRepo[NEXT(i)] == 0x01))
					{
						frameStart = NEXT(NEXT(i));
						Flag = 2;
						goto ANALYZE;
					}
					break;
				}
			}
		}
	}
	else
	{
		return 1;
	}
	
	ANALYZE:
	if(Flag == 0)
	{
		return 2;
	}
	dataStartPoint = NEXT(frameEnd);
	if(Flag == 1)
	{
		return 3;
	}
	
	frameEnd = PREVIOUS(frameEnd);
	
	//У��
	uint8_t sum = 0;
	for(uint8_t i = frameStart; i != frameEnd; i = NEXT(i))
	{
		sum += dataRepo[i];
	}
	if(sum != dataRepo[frameEnd])
	{
		return 5;
	}
	
	//��������֡
	uint8_t dataCounter = 0;
	for(uint8_t i = frameStart; i != frameEnd; ++dataCounter, i = NEXT(i))
	{
		if(dataRepo[i] == 0xFF)
		{
			i = NEXT(i);
			if(dataCounter >= sizeof(ReceiveProtocolTransmit))
			{
				return 6;
			}
			switch(dataRepo[i])
			{
				case 0x00 : dataArranged[dataCounter] = 0xFF; break;
				default : break;
			}
		}
		else
		{
			dataArranged[dataCounter] = dataRepo[i];
		}
	}
	
	//��鳤��
	if(sizeof(ReceiveProtocolTransmit) != dataCounter)
	{
		return 7;
	}
	
	//���ݷ���
	ReceiveProtocolTransmit temp;
	for(uint8_t i = 0; i < sizeof(ReceiveProtocolTransmit); ++i)
	{
		((uint8_t *)&temp)[i] = dataArranged[i];
	}
	
	currentStatus.mode = temp.mode;
	currentStatus.FBoffset = temp.FBoffset;
	currentStatus.LRoffset = temp.LRoffset;
	
	return 0;

	
	#undef DATAREPOLENGTH
	#undef NEXT
	#undef PREVIOUS
}




uint8_t Bluetooth_ReceiveAnalyzeAndGetData(ReceiveProtocolDetail *pd)
{
	if(!Bluetooth_ReceiveAnalyze())
	{
		*pd = currentStatus;
		return 0;
	}
	else
	{
		*pd = currentStatus;
		return 1;
	}
}


void Bluetooth_GetData(ReceiveProtocolDetail *pd)
{
	*pd = currentStatus;
}


