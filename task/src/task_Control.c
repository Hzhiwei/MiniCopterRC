#include "task_Control.h"
#include "cmsis_os.h"
#include "Rocker.h"
#include "LC12S.h"
#include "Key.h"
#include "OLED.h"
#include "Switch.h"

#include <stdio.h>


typedef enum 
{
	Status_detail,
	Status_copterAdjust,
	Status_rcokerAdjust,
}StatusMachineType;


static void Status_Update(void);
static void SendData(void);
static void Status_Detail(uint8_t frist);
static void Status_LeaveDetail(void);
static void Status_CopterAdjust(uint8_t frist);
static void Status_LeaveCopterAdjust(void);
static void Status_RcokerAdjust(uint8_t frist);
static void Status_LeaveRcokerAdjust(void);



static protocolDetail pd;
static KeyStatusType KeyStatus;
static int8_t RockerData[4];
static uint8_t MotorLocker = 1;


void task_Control(const void *Parameters)
{
	TickType_t tick;
	
	while(1)
	{
		tick = xTaskGetTickCount();
		Key_Update(&KeyStatus);
		Rocker_GetData(RockerData);
		Status_Update();
		SendData();
		
		vTaskDelayUntil(&tick, 20);
	}
}


static void Status_Update(void)
{
	static StatusMachineType lastStatus = Status_rcokerAdjust;
	static StatusMachineType currentStatus = Status_detail;
	
	static TickType_t pushUnlockTick = 0;
	
	//加解锁
	if(Switch_Get(0))
	{
		MotorLocker = 1;
	}
	else
	{
		if(KeyStatus.trigger[7])
		{
			pushUnlockTick = xTaskGetTickCount();
		}
		
		if((!KeyStatus.pushed[7]) && (pushUnlockTick + 2000 <= xTaskGetTickCount()))
		{
			if(RockerData[0] <= 5)
			{
				MotorLocker = 0;
			}
		}
	}
	
	//模式切换
	uint8_t changeMode = 0;
	if(KeyStatus.trigger[4] == 1)
	{
		if(Status_rcokerAdjust == currentStatus)
		{
			currentStatus = Status_detail;
		}
		else
		{
			++currentStatus;
		}
	}
	if(lastStatus != currentStatus)
	{
		changeMode = 1;
	}
	lastStatus = currentStatus;
	
	if(changeMode)
	{
		switch(lastStatus)
		{
			case Status_detail : Status_LeaveDetail(); break;
			case Status_copterAdjust : Status_LeaveCopterAdjust(); break;
			case Status_rcokerAdjust : Status_LeaveRcokerAdjust(); break;
		}
	}
	
	switch(currentStatus)
	{
		case Status_detail : Status_Detail(changeMode); break;
		case Status_copterAdjust : Status_CopterAdjust(changeMode); break;
		case Status_rcokerAdjust : Status_RcokerAdjust(changeMode); break;
	}
	
	//显示 锁定 锁
	if(MotorLocker)
	{
		OLED_DrawF6x8Pic(118, 0, 0);
	}
	else
	{
		OLED_DrawF6x8Pic(118, 0, 1);
	}
	
	//缓冲显示
	OLED_RefreshGRAM();
}


static void SendData(void)
{
	pd.adjust = 1;
	pd.FB = RockerData[2];
	pd.headMode = 1;
	pd.locked = MotorLocker;
	pd.LR = RockerData[3];
	pd.power = RockerData[0];
	pd.SP = RockerData[1];
	LC12S_Send(&pd);
}


static void Status_Detail(uint8_t frist)
{
	char temp[8];
	
	if(frist)
	{
		OLED_ClearGRAM(0);
		OLED_DrawF6x8String(0, 0, "detail");
		OLED_DrawF6x8String(0, 20, "0:");
		OLED_DrawF6x8String(0, 40, "1:");
		OLED_DrawF6x8String(60, 20, "2:");
		OLED_DrawF6x8String(60, 40, "3:");
	}
	
	sprintf(temp, "%d  ", RockerData[0]);
	OLED_DrawF6x8String(15, 20, temp);
	sprintf(temp, "%d  ", RockerData[1]);
	OLED_DrawF6x8String(15, 40, temp);
	sprintf(temp, "%d  ", RockerData[2]);
	OLED_DrawF6x8String(75, 20, temp);
	sprintf(temp, "%d  ", RockerData[3]);
	OLED_DrawF6x8String(75, 40, temp);
}
static void Status_LeaveDetail(void)
{
	
}


static void Status_CopterAdjust(uint8_t frist)
{
	if(frist)
	{
		OLED_ClearGRAM(0);
		OLED_DrawF6x8String(0, 0, "copterAdjust");
	}
}
static void Status_LeaveCopterAdjust(void)
{
	
}


static void Status_RcokerAdjust(uint8_t frist)
{
	if(frist)
	{
		OLED_ClearGRAM(0);
		OLED_DrawF6x8String(0, 0, "rockerAdjust");
	}
}
static void Status_LeaveRcokerAdjust(void)
{
	
}

