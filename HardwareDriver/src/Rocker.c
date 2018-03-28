#include "Rocker.h"
#include "externParam.h"

/*
    1           3
    |           |
  --+--2      --+--4
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
	{0, 4096, 0},
	{0, 2048, 4096},
	{0, 2048, 4096},
};


void Rocker_Init(void)
{
	HAL_ADC_Start_DMA(&hadc1, OriginData, 4);
}


void Rocker_GetData(int8_t data[4])
{
	OriginData[1] = OffsetParam[1][0] > OriginData[1] ? OffsetParam[1][0] : OriginData[1];
	OriginData[1] = OffsetParam[1][1] < OriginData[1] ? OffsetParam[1][1] : OriginData[1];
	data[0] = (OriginData[1] - OffsetParam[1][0]) * 100 / (OffsetParam[1][1] - OffsetParam[1][0]);
	
	OriginData[0] = OffsetParam[0][0] > OriginData[0] ? OffsetParam[0][0] : OriginData[0];
	OriginData[0] = OffsetParam[0][2] < OriginData[0] ? OffsetParam[0][2] : OriginData[0];
	if(OriginData[0] > OffsetParam[0][1])
	{
		data[1] = -(OriginData[0] - OffsetParam[0][1]) * 100 / (OffsetParam[0][2] - OffsetParam[0][1]);
	}
	else if(OriginData[0] < OffsetParam[0][1])
	{
		data[1] = -(OriginData[0] - OffsetParam[0][1]) * 100 / (OffsetParam[0][1] - OffsetParam[0][0]);
	}
	else
	{
		data[1] = 0;
	}
	
	OriginData[3] = OffsetParam[3][0] > OriginData[3] ? OffsetParam[3][0] : OriginData[3];
	OriginData[3] = OffsetParam[3][2] < OriginData[3] ? OffsetParam[3][2] : OriginData[3];
	if(OriginData[3] > OffsetParam[3][1])
	{
		data[2] = (OriginData[3] - OffsetParam[3][1]) * 100 / (OffsetParam[3][2] - OffsetParam[3][1]);
	}
	else if(OriginData[3] < OffsetParam[3][1])
	{
		data[2] = (OriginData[3] - OffsetParam[3][1]) * 100 / (OffsetParam[3][1] - OffsetParam[3][0]);
	}
	else
	{
		data[2] = 0;
	}
	
	OriginData[2] = OffsetParam[2][0] > OriginData[2] ? OffsetParam[2][0] : OriginData[2];
	OriginData[2] = OffsetParam[2][2] < OriginData[2] ? OffsetParam[2][2] : OriginData[2];
	if(OriginData[2] > OffsetParam[2][1])
	{
		data[3] = -(OriginData[2] - OffsetParam[2][1]) * 100 / (OffsetParam[2][2] - OffsetParam[2][1]);
	}
	else if(OriginData[2] < OffsetParam[2][1])
	{
		data[3] = -(OriginData[2] - OffsetParam[2][1]) * 100 / (OffsetParam[2][1] - OffsetParam[2][0]);
	}
	else
	{
		data[3] = 0;
	}
}






