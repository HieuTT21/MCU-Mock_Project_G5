/*
 * Flash.c
 *
 *  Created on: Oct 16, 2023
 *      Author: admin
 */

#include "Flash.h"


void Flash_WriteWord( uint32_t Address, uint32_t Data)
{
	// wait Previous command complete
	while ( (FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0U);

	// Check and Clear Error of Previous command
	if ( ((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0U) || \
			((FTFA->FSTAT & FTFA_FSTAT_FPVIOL_MASK) != 0U))
		{
			FTFA->FSTAT = 0x30;
		}

	// Write command and parameters
	FTFA->FCCOB0 = FCMD_WRITE_WORD;

	FTFA->FCCOB1 = (uint8_t)(Address >> 16);
	FTFA->FCCOB2 = (uint8_t)(Address >> 8);
	FTFA->FCCOB3 = (uint8_t)Address;
	FTFA->FCCOB4 = (uint8_t)Data;
	FTFA->FCCOB5 = (uint8_t)(Data >> 8);
	FTFA->FCCOB6 = (uint8_t)(Data >> 16);
	FTFA->FCCOB7 = (uint8_t)(Data >> 24);

	// Launch the Command
	FTFA->FSTAT = 0x80;
}

void Flash_WriteWord_Reverse(uint32_t Address, uint32_t Data)
{
	// wait Previous command complete
	while ( (FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0U);

	// Check and Clear Error of Previous command
	if ( ((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0U) || \
			((FTFA->FSTAT & FTFA_FSTAT_FPVIOL_MASK) != 0U))
		{
			FTFA->FSTAT = 0x30;
		}

	// Write command and parameters
	FTFA->FCCOB0 = FCMD_WRITE_WORD;

	FTFA->FCCOB1 = (uint8_t)(Address >> 16);
	FTFA->FCCOB2 = (uint8_t)(Address >> 8);
	FTFA->FCCOB3 = (uint8_t)Address;
	FTFA->FCCOB7 = (uint8_t)Data;
	FTFA->FCCOB6 = (uint8_t)(Data >> 8);
	FTFA->FCCOB5 = (uint8_t)(Data >> 16);
	FTFA->FCCOB4 = (uint8_t)(Data >> 24);

	// Launch the Command
	FTFA->FSTAT = 0x80;
}
bool Flash_EraseSectorNum( uint8_t startSec, uint8_t sectorNum)
{
	bool retVal = TRUE;
	uint8_t index;
	uint32_t Address;

	if ( startSec <= 1)
	{
		retVal = FALSE;
		return retVal;
	}
	for (index = startSec; index < (startSec + sectorNum); index++)
	{
		Address = index * 1024U;
		// wait Previous command complete
		while ( (FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0U);
		// Check and Clear Error of Previous command
		if ( ((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0U) || \
				((FTFA->FSTAT & FTFA_FSTAT_FPVIOL_MASK) != 0U))
			{
				FTFA->FSTAT = 0x30;
			}
		// Write command and parameters
		FTFA->FCCOB0 = FCMD_ERASE_SECTOR;
		FTFA->FCCOB1 = (uint8_t)(Address >> 16);
		FTFA->FCCOB2 = (uint8_t)(Address >> 8);
		FTFA->FCCOB3 = (uint8_t)Address;
		// Launch the Command
		FTFA->FSTAT = 0x80;
	}

	return retVal;
}

void Flash_EraseSector(uint8_t SectorNum)
{
	uint32_t address = SectorNum * 1024;

	if (SectorNum <= 1) return;

	while ((FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0);

	//Check and clear Error of previous command
	if ((FTFA->FSTAT & FTFA_FSTAT_ACCERR_MASK) != 0 ||
		(FTFA->FSTAT & FTFA_FSTAT_FPVIOL_MASK) != 0)
	{
		FTFA->FSTAT = 0x30u;
	}

	FTFA->FCCOB0 = FCMD_ERASE_SECTOR;

	FTFA->FCCOB1 = (uint8_t)(address >> 16);
	FTFA->FCCOB2 = (uint8_t)(address >> 8);
	FTFA->FCCOB3 = (uint8_t) address;

	FTFA->FSTAT |= 0x80u;
}

uint32_t Flash_ReadWord(uint32_t address)
{
	while ( (FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0U);
	return (*(uint32_t *)address);
}

bool IsFlashEraseSector(uint8_t startSec, uint8_t sectorNum)
{
	bool retVal = TRUE;
	uint8_t i;
	uint16_t k;

	while ( (FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0U);

	for( i = startSec; i < (startSec + sectorNum); i++)
	{
		for( k = 0; k < 1024; k += 4)
		{
			if( FLASH_DATA_ZERO != Flash_ReadWord((i * 1024) + k))
			{
				retVal = FALSE;
				return retVal;
			}
		}
	}
	return retVal;
}

bool IsFlashDataSector(uint8_t startSec)
{
	bool retVal = TRUE;
	uint8_t dataCount = 0U;
	uint16_t i;

	while ( (FTFA->FSTAT & FTFA_FSTAT_CCIF_MASK) == 0U);

	for( i = 0; i < 1024; i += 4)
	{
		if( FLASH_DATA_ZERO != Flash_ReadWord((startSec * 1024) + i))
		{
			dataCount++;
			if( FIRMWARE_CODE_EXIST <= dataCount)
			{
				return retVal;
			}
		}

	}
	retVal = FALSE;

	return retVal;
}
