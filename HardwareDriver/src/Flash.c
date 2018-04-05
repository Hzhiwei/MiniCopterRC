#include "Flash.h"
#include "Rocker.h"


#define FLASH_PAGE_127_START	0x0801FC00
#define FLASH_PAGE_127_END		0x0801FFFF

#define FLASH_ROCKER_START		0
#define FLASH_ROCKER_LENGTH		48
#define FLASH_END				((FLASH_ROCKER_START + FLASH_ROCKER_LENGTH) + (FLASH_ROCKER_START + FLASH_ROCKER_LENGTH) % 2)


void Flash_BackupAndEarse(uint8_t *addr);
static uint8_t Flash_Write(uint32_t addr, uint8_t *src, uint32_t length);


void Flash_Read_Rocker(uint32_t Offset[4][3])
{
	for(uint32_t i = 0; i < FLASH_ROCKER_LENGTH; ++i)
	{
		*((uint8_t *)(&Offset[0][0]) + i) = *((uint8_t *)(FLASH_PAGE_127_START + FLASH_ROCKER_START + i));
	}
}


void Flash_Write_Rocker(uint32_t Offset[4][3])
{
    HAL_FLASH_Unlock(); 
	Flash_Write(FLASH_ROCKER_START, (uint8_t *)(&(Offset[0][0])), FLASH_ROCKER_LENGTH);
    HAL_FLASH_Lock();
}


static uint8_t Flash_Write(uint32_t addr, uint8_t *src, uint32_t length)
{
	uint8_t temp[FLASH_END];
	//��������
	for(uint32_t i = 0; i < FLASH_END; ++i)
	{
		temp[i] = *((uint8_t *)(FLASH_PAGE_127_START + i));
	}
	
	//������
	for(uint32_t i = addr; i < length; ++i)
	{
		temp[i] = *(src++);
	}
	
	//����
	FLASH_EraseInitTypeDef eitd;
	eitd.TypeErase = FLASH_TYPEERASE_PAGES;
	eitd.Banks = FLASH_BANK_1;
	eitd.NbPages = 1;
	eitd.PageAddress = FLASH_PAGE_127_START;
	uint32_t error;
	HAL_FLASHEx_Erase(&eitd, &error);
	
	//д����FLASH_TYPEPROGRAM_HALFWORD
	for(uint32_t i = 0; i < FLASH_END; i = i + 2)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_PAGE_127_START + i, *((uint16_t *)&temp[i]));
	}
	
	return 0;
}





