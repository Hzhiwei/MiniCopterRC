#ifndef __LS12S_H
#define __LS12S_H


#include "stm32f1xx_hal.h"


//ͨ��Э��ĸ�����־λ�����ݷֿ���ϸ��
/*
ͨ��Э�飺

0xFFΪת���־�������൱���ַ����е�'\'
ת���ַ���
	0xFF 0x00		0xFF
	0xFF 0x01		֡ͷ
	0xFF 0x02		֡β

byte0 ��־λ��
		bit7 : 0 lock,1 unlock
		bit6 : 0 ����, 1 ��ͷģʽ
		bit5 : 
		bit4 : 
		bit3 : 
		bit2 : 
		bit1 : 
		bit0 : 

byte1 ���ţ�	0 ~ 100
byte2 ����ƫת�� -100 ~ 100��ʵ�ʽǶ�Ϊ����ֵ/4
byte3 ǰ��ƫת�� -100 ~ 100��ʵ�ʽǶ�Ϊ����ֵ/4
byte4 ƫ�Ƶ��ڣ� 
		bit7 : 0 �����ã�1 ����ƫ��
		bit6 : 0 ��ƫ0.1�ȣ�1 ��ƫ0.1��
		bit5 : 0 �����ã�1 ǰ��ƫ��
		bit4 : 0 ǰƫ0.1�ȣ�1 ��ƫ0.1��
		bit3 : 0 �����ã�1 ����ƫ�ƹ���
		bit2 : 0 �����ã�1 ǰ��ƫ�ƹ���
		bit1 : 0 �����ã�1 ƫ��д��flash
		bit0 : 
byte5 У���
*/
typedef struct 
{
	//1 ����	2 ����
	uint8_t locked;
	//1 ��ͷģʽ	2 ����ģʽ
	uint8_t headMode;
	//�������
	uint8_t power;
	//������б	������
	int8_t  LR;
	//ǰ����б	ǰ����
	int8_t  FB;
	//������бƫ�Ƶ���
	/*
	0��������
	1����ƫ0.1��
	2����ƫ0.1��
	3��ǰƫ0.1��
	4����ƫ0.1��
	5��д��Flash
	*/
	int8_t adjust;			
}protocolDetail;


uint8_t LC12S_Init(uint16_t Net);
void LC12S_Send(const protocolDetail *pd);


#endif
