/*
 * Flash.h
 *
 *  Created on: Oct 16, 2023
 *      Author: admin
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "MKL46Z4.h"

#define FCMD_WRITE_WORD 	0x06U
#define FCMD_ERASE_SECTOR	0x09u
#define GET_SECTOR_NUMBER(ADDR) 		(((uint32_t)ADDR))

#define SECTOR_SIZE			(1024U)
#define FLASH_DATA_ZERO		(0xFFFFFFFF)
#define FIRMWARE_CODE_EXIST	(10U)

typedef enum
{
	FALSE	= 0U,
	TRUE	= 1U
} bool;


/*-------------------------------------------------------------*/

uint32_t Flash_ReadWord(uint32_t Address);

void Flash_WriteWord(uint32_t Address, uint32_t Data);

void Flash_WriteWord_Reverse(uint32_t Address, uint32_t Data);

bool Flash_EraseSectorNum(uint8_t SectorNum, uint8_t sectorNum);

bool IsFlashEraseSector(uint8_t startSec, uint8_t sectorNum);

bool IsFlashDataSector(uint8_t startSec);

#endif /* FLASH_H_ */
