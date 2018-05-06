#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H


#include "stm32f1xx_hal.h"


//通信协议的各个标志位，数据分开的细节
typedef struct 
{
	//1 加锁	2 解锁
	uint8_t locked;
	//1 无头模式	2 正常模式
	uint8_t headMode;
	//电机油门
	uint8_t power;
	//左右倾斜	左负右正
	int8_t  LR;
	//前后倾斜	后负前正
	int8_t  FB;
	//自旋	左负右正
	int8_t  SP;
	//左右倾斜偏移调节
	/*
	0：无作用
	1：左偏0.1度
	2：右偏0.1度
	3：前偏0.1度
	4：后偏0.1度
	5：写入Flash
	*/
	int8_t adjust;			
}SendProtocolDetail;


//通信协议的各个标志位，数据分开的细节
typedef struct 
{
	//模式 
	/*
	bit[1:0]	00 停止		01 飞行	02 等待抛飞
	bit[2]		0 欧拉角偏移已写入Flash		1 欧拉角偏移未写入Flash
	*/
	uint8_t mode;
	//左右偏移调节
	int8_t LRoffset;
	//前后偏移调节	
	int8_t FBoffset;			
}ReceiveProtocolDetail;


void Bluetooth_Start(void);
uint8_t Bluetooth_ReceiveAnalyze(void);
uint8_t Bluetooth_ReceiveAnalyzeAndGetData(ReceiveProtocolDetail *pd);
void Bluetooth_GetData(ReceiveProtocolDetail *pd);

uint8_t Bluetooth_Init(uint16_t Net);
void Bluetooth_Send(const SendProtocolDetail *pd);
HAL_StatusTypeDef Bluetooth_Pair(void);


#endif
