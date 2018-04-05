#include "Rocker.h"
#include "Flash.h"
#include "externParam.h"

/*
    0           2
    |           |
  --+--1      --+--3
    |           |
                    1
                    2
                    3 5
                    4 6
                      7
                      8
左负右正，下负上正
*/


static uint32_t OriginData[4];
static uint32_t OffsetParam[4][3] = 
{
	{0, 2048, 4096},
	{0, 0, 4096},
	{0, 2048, 4096},
	{0, 2048, 4096},
};


void Rocker_Init(void)
{
	HAL_ADC_Start_DMA(&hadc1, OriginData, 4);
	
	//读取偏移数据
	Flash_Read_Rocker(OffsetParam);
}


void Rocker_GetOriginal(uint32_t data[4])
{
	data[0] = OriginData[0];
	data[1] = OriginData[1];
	data[2] = OriginData[2];
	data[3] = OriginData[3];
}


void Rocker_SetOffset(uint32_t data[4][3])
{
	for(uint8_t i = 0; i < 4; ++i)
	{
		for(uint8_t j = 0; j < 3; ++j)
		{
			OffsetParam[i][j] = data[i][j];
		}
	}
}


void Rocker_GetData(int16_t data[4])
{
	int32_t limitData[4];
	
	limitData[1] = OffsetParam[1][0] > OriginData[1] ? OffsetParam[1][0] : OriginData[1];
	limitData[1] = OffsetParam[1][2] < OriginData[1] ? OffsetParam[1][2] : OriginData[1];
	data[0] = (limitData[1] - OffsetParam[1][0]) * 100 / (OffsetParam[1][2] - OffsetParam[1][0]);
	
	limitData[0] = OffsetParam[0][0] > OriginData[0] ? OffsetParam[0][0] : OriginData[0];
	limitData[0] = OffsetParam[0][2] < OriginData[0] ? OffsetParam[0][2] : OriginData[0];
	if(limitData[0] > OffsetParam[0][1])
	{
		data[1] = -(limitData[0] - (int32_t)OffsetParam[0][1]) * 100 / (int32_t)(OffsetParam[0][2] - OffsetParam[0][1]);
	}
	else if(limitData[0] < OffsetParam[0][1])
	{
		data[1] = -(limitData[0] - (int32_t)OffsetParam[0][1]) * 100 / (int32_t)(OffsetParam[0][1] - OffsetParam[0][0]);
	}
	else
	{
		data[1] = 0;
	}
	
	limitData[3] = OffsetParam[3][0] > OriginData[3] ? OffsetParam[3][0] : OriginData[3];
	limitData[3] = OffsetParam[3][2] < OriginData[3] ? OffsetParam[3][2] : OriginData[3];
	if(limitData[3] > OffsetParam[3][1])
	{
		data[2] = (limitData[3] - (int32_t)OffsetParam[3][1]) * 100 / (int32_t)(OffsetParam[3][2] - OffsetParam[3][1]);
	}
	else if(limitData[3] < OffsetParam[3][1])
	{
		data[2] = (limitData[3] - (int32_t)OffsetParam[3][1]) * 100 / (int32_t)(OffsetParam[3][1] - OffsetParam[3][0]);
	}
	else
	{
		data[2] = 0;
	}
	
	limitData[2] = OffsetParam[2][0] > OriginData[2] ? OffsetParam[2][0] : OriginData[2];
	limitData[2] = OffsetParam[2][2] < OriginData[2] ? OffsetParam[2][2] : OriginData[2];
	if(limitData[2] > OffsetParam[2][1])
	{
		data[3] = -(limitData[2] - (int32_t)OffsetParam[2][1]) * 100 / (int32_t)(OffsetParam[2][2] - OffsetParam[2][1]);
	}
	else if(limitData[2] < OffsetParam[2][1])
	{
		data[3] = -(limitData[2] - (int32_t)OffsetParam[2][1]) * 100 / (int32_t)(OffsetParam[2][1] - OffsetParam[2][0]);
	}
	else
	{
		data[3] = 0;
	}
}






