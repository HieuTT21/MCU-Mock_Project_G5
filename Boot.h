/*
 * Boot.h
 *
 *  Created on: Oct 16, 2023
 *      Author: admin
 */

#ifndef BOOT_H_
#define BOOT_H_

#include "MKL46Z4.h"
#include "UART.h"
#include "Clock.h"
#include "Port.h"
#include "Flash.h"
#include "srec.h"


#define SCB_SHCSR_USGFAULTENA_Pos          (18U)
#define SCB_SHCSR_USGFAULTENA_Msk          (1UL << SCB_SHCSR_USGFAULTENA_Pos)
#define SCB_SHCSR_BUSFAULTENA_Pos          (17U)
#define SCB_SHCSR_BUSFAULTENA_Msk          (1UL << SCB_SHCSR_BUSFAULTENA_Pos)
#define SCB_SHCSR_MEMFAULTENA_Pos          (16U)
#define SCB_SHCSR_MEMFAULTENA_Msk          (1UL << SCB_SHCSR_MEMFAULTENA_Pos)

#define START_SECTOR_CHAR			(3U)
#define SECTOR_ERASE_NUM_DEFAULT	(10U)
#define LOAD_FIRMWARE_SUCCESS		(2U)
#define SECTOR_SIZE					(1024U)

#define DEFAULT_FIRMWARE			(192U)
#define NEW_FIRMWARE				(212U)


typedef enum
{
	NOMAL_MODE		= 0U,
	BOOTLOADER_MODE	= 1U
} BTN_State;

typedef enum
{
	ERR_FLASH_ERASE				= 0U,
	ERR_FLASH_SECTOR_EMPTY		= 1U,
	ERR_SREC_FILE				= 2U,
	ERR_SECTOR_SPECIAL			= 3U,

	FIRMWARE_LOAD_OK			= 5U,
	FIRMWARE_LOAD_NOT_OK		= 6U,
	RESTORE_FIRMWARE_OK			= 7U,
	RESTORE_FIRMWARE_NOT_OK		= 8U,
	MOVE_FIRMWARE_STATE			= 9U,
	RESTORE_STATE				= 10U,
	END_OF_FILE					= 11U,

	ERR_UNIDENTIFIED			= 20U,

	RESET_STATE					= 30U

} Status_Type;

typedef enum
{
	INVALID_COMMAND			= 0U,
	FIRST_COMMAND			= 1U,
	START_SECTOR			= 2U,
	SECTOR_NUMBER			= 3U,
//	COMAND_EXCUTE			= 4U,
	LOAD_COMMAND			= 5U,
	ERASE_COMMAND			= 6U,
	RESTORE_COMMAND			= 7U,
	SEND_FILE_COMMAND		= 8U,
	RESTORE_CONFIRM			= 9U,
	ERASE_SUCCESS			= 10U,
	LOAD_SUCCESS			= 11U,

	NO_FIRMWARE				= 20U

} Command_Type;


/*----------------GLOBAL - PROTOTYPE FUNCTION-----------------*/

// Function to initialize UART module, enable UART interrupt and direct flow of program
void BootLoaderMode_Handler(void);

// Function to check the existence of firmware and Jump to it
void NomalMode_Handler(void);

// Function to initialize push button
void Button_Init(void);

// Function to check if the push button is pressed or not
BTN_State IsButtonPressed(void);



#endif /* BOOT_H_ */
