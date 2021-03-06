#include "task_Control.h"
#include "cmsis_os.h"
#include "Rocker.h"
#include "Bluetooth.h"
#include "Key.h"
#include "OLED.h"
#include "Switch.h"
#include "Flash.h"
#include "externParam.h"

#include <stdio.h>



#define LOSTSTOP	20

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



static KeyStatusType KeyStatus;
static int16_t RockerData[4];
static uint8_t MotorLocker = 1;
static uint8_t Linking = 0;
static uint8_t EulerAdjustFlag = 0;

static SendProtocolDetail spd;
static ReceiveProtocolDetail rpd = 
	{
		.mode = 0,
		.LRoffset = 1,
		.FBoffset = 2
	};


void task_Control(const void *Parameters)
{
	TickType_t tick;
	uint16_t lostCounter = LOSTSTOP;
	
	uint32_t Offset[4][3] = {0};
	
	Key_Update(&KeyStatus);
	if(KeyStatus.pushed[7] == 0)
	{
		OLED_ClearGRAM(0);
		OLED_DrawF6x8String(15, 20, "Are you sure?");
		OLED_RefreshGRAM();
		
		while(1)
		{
			Key_Update(&KeyStatus);
			if(KeyStatus.trigger[4] == 1)
			{
				OLED_ClearGRAM(0);
				if(HAL_OK == Bluetooth_Pair())
				{
					OLED_DrawF6x8String(10, 20, "waiting pair...");
				}
				else
				{
					OLED_DrawF6x8String(20, 20, "pair failure");
				}
				OLED_RefreshGRAM();
				while(1);
			}
			else if((KeyStatus.trigger[0] == 1)
				|| (KeyStatus.trigger[1] == 1)
				|| (KeyStatus.trigger[2] == 1)
				|| (KeyStatus.trigger[3] == 1)
				|| (KeyStatus.trigger[5] == 1)
				|| (KeyStatus.trigger[6] == 1)
				|| (KeyStatus.trigger[7] == 1))
			{
				break;
			}
		}
	}

	Bluetooth_Start();
	
	while(1)
	{
		tick = xTaskGetTickCount();
		Key_Update(&KeyStatus);
		Rocker_GetData(RockerData);
		Status_Update();
		SendData();
		
		//分析无线数据
		if(!Bluetooth_ReceiveAnalyzeAndGetData(&rpd))
		{
			lostCounter = 0;
		}
		else if(lostCounter < LOSTSTOP)
		{
			++lostCounter;
		}
		
		//根据无线帧率判断连接状态
		if(lostCounter >= LOSTSTOP)
		{
			Linking = 0;
		}
		else
		{
			Linking = 1;
		}
		
		
		vTaskDelayUntil(&tick, 50);
	}
}


static void Status_Update(void)
{
	static StatusMachineType lastStatus = Status_rcokerAdjust;
	static StatusMachineType currentStatus = Status_detail;
	
	static TickType_t pushUnlockTick = 0;
	static TickType_t pushSendTick = 0;
	
	EulerAdjustFlag = 0;
	
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
		
		if((!KeyStatus.pushed[7]) && (pushUnlockTick + 1000 <= xTaskGetTickCount()))
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
	
	
	if(Linking)
	{
		OLED_DrawF6x8Pic(110, 0, 2);
	}
	else
	{
		OLED_DrawF6x8Char(110, 0, ' ');
	}
	
	//缓冲显示
	OLED_RefreshGRAM();
}



static void SendData(void)
{
	spd.adjust = EulerAdjustFlag;
	spd.FB = RockerData[2];
	spd.headMode = 1;
	spd.locked = MotorLocker;
	spd.LR = RockerData[3];
	spd.power = RockerData[0];
	spd.SP = RockerData[1];
	Bluetooth_Send(&spd);
}


//显示详情
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

//显示飞机偏移调节
static void Status_CopterAdjust(uint8_t frist)
{
	static uint8_t adjustDir = 0;	//0 前后	1 左右
	char temp[8];
	
	if(frist)
	{
		OLED_ClearGRAM(0);
		OLED_DrawF6x8String(0, 0, "copterAdjust");
		OLED_DrawF6x8String(20, 20, "FB:");
		OLED_DrawF6x8String(80, 20, "LR:");
		OLED_DrawF6x8String(10, 30, " ");
		OLED_DrawF6x8String(70, 30, "*");
	}
	
	if(Linking)
	{
		sprintf(temp, "%.1f  ", (rpd.FBoffset / 10.0));
		OLED_DrawF6x8String(20, 30, temp);
		sprintf(temp, "%.1f  ", (rpd.LRoffset / 10.0));
		OLED_DrawF6x8String(80, 30, temp);
		
		if(rpd.mode & 0x04)
		{
			OLED_DrawF6x8String(15, 50, "offset unsaved");
		}
		else
		{
			OLED_DrawF6x8String(15, 50, " offset saved ");
		}
		
		if(KeyStatus.trigger[2] == 1)
		{
			adjustDir = !adjustDir;
			if(adjustDir)
			{
				OLED_DrawF6x8String(10, 30, "*");
				OLED_DrawF6x8String(70, 30, " ");
			}
			else
			{
				OLED_DrawF6x8String(10, 30, " ");
				OLED_DrawF6x8String(70, 30, "*");
			}
		}
		
		if(adjustDir)
		{
			if(KeyStatus.trigger[0] == 1)
			{
				EulerAdjustFlag = 1;
			}
			else if(KeyStatus.trigger[1] == 1)
			{
				EulerAdjustFlag = 2;
			}
		}
		else
		{
			if(KeyStatus.trigger[0] == 1)
			{
				EulerAdjustFlag = 3;
			}
			else if(KeyStatus.trigger[1] == 1)
			{
				EulerAdjustFlag = 4;
			}
		}
		
		if(KeyStatus.trigger[6] == 1)
		{
			EulerAdjustFlag = 5;
		}
	}
	else
	{
		OLED_DrawF6x8String(20, 30, "-----");
		OLED_DrawF6x8String(80, 30, "-----");
	}
}
static void Status_LeaveCopterAdjust(void)
{
	
}


//摇杆调节
static void Status_RcokerAdjust(uint8_t frist)
{
	static uint32_t Param[4][3];
	static uint8_t Flag = 0;
	
	if(frist)
	{
		OLED_ClearGRAM(0);
		OLED_DrawF6x8String(0, 0, "rockerAdjust");
		OLED_DrawF6x8String(0, 11, "0S:");
		OLED_DrawF6x8String(0, 20, "0M:");
		OLED_DrawF6x8String(0, 29, "0L:");
		OLED_DrawF6x8String(0, 38, "1S:");
		OLED_DrawF6x8String(0, 47, "1M:");
		OLED_DrawF6x8String(0, 56, "1L:");
		OLED_DrawF6x8String(60, 11, "2S:");
		OLED_DrawF6x8String(60, 20, "2M:");
		OLED_DrawF6x8String(60, 29, "2L:");
		OLED_DrawF6x8String(60, 38, "3S:");
		OLED_DrawF6x8String(60, 47, "3M:");
		OLED_DrawF6x8String(60, 56, "3L:");
		
		for(uint8_t i = 0; i < 4; ++i)
		{
			for(uint8_t j = 0; j < 3; ++j)
			{
				Param[i][j] = 2048;
			}
		}
		Flag = 0;
	}
	
	uint32_t data[4];
	char temp[8];
	Rocker_GetOriginal(data);
	
	if(Flag == 0)
	{
		for(uint8_t i = 0; i < 4; ++i)
		{
			Param[i][0] = Param[i][0] > data[i] ? data[i] : Param[i][0];
			Param[i][2] = Param[i][2] < data[i] ? data[i] : Param[i][2];
			Param[i][1] = data[i];
		}
		
		OLED_DrawF6x8String(90, 0, "...");
		
		for(uint8_t i = 0; i < 3; ++i)
		{
			sprintf(temp, "%d  ", Param[1][i]);
			OLED_DrawF6x8String(19, 11 + 9 * i, "    ");
			OLED_DrawF6x8String(19, 11 + 9 * i, temp);
		}
		for(uint8_t i = 0; i < 3; ++i)
		{
			sprintf(temp, "%d  ", Param[0][i]);
			OLED_DrawF6x8String(19, 38 + 9 * i, "    ");
			OLED_DrawF6x8String(19, 38 + 9 * i, temp);
		}
		for(uint8_t i = 0; i < 3; ++i)
		{
			sprintf(temp, "%d  ", Param[3][i]);
			OLED_DrawF6x8String(79, 11 + 9 * i, "    ");
			OLED_DrawF6x8String(79, 11 + 9 * i, temp);
		}
		for(uint8_t i = 0; i < 3; ++i)
		{
			sprintf(temp, "%d  ", Param[2][i]);
			OLED_DrawF6x8String(79, 38 + 9 * i, "    ");
			OLED_DrawF6x8String(79, 38 + 9 * i, temp);
		}
		
		if(KeyStatus.trigger[0] == 1)
		{
			Flash_Write_Rocker(Param);
			Rocker_SetOffset(Param);
			Flag = 1;
		}
	}
	else
	{
		OLED_DrawF6x8String(90, 0, "OK ");
	}
	
	if(KeyStatus.trigger[1] == 1)
	{
		for(uint8_t i = 0; i < 4; ++i)
		{
			for(uint8_t j = 0; j < 3; ++j)
			{
				Param[i][j] = 2048;
			}
		}
		Flag = 0;
	}
}
static void Status_LeaveRcokerAdjust(void)
{
	
}



