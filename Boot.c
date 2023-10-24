/*
 * Boot.c
 *
 *  Created on: Oct 16, 2023
 *      Author: admin
 */
#include "Boot.h"

static volatile uint8_t UART_data;
static volatile uint8_t startSector		= DEFAULT_FIRMWARE;
static volatile uint8_t sectorNum		= 1;
static volatile uint8_t statusFlag		= RESET_STATE;

static volatile uint8_t GeneralFlag		= FIRST_COMMAND;
static volatile uint8_t commandFlag		= 0;
static volatile uint8_t index			= 0;
static volatile uint8_t Load_EOF_Flag	= 0;
static int8_t 	sectorString[3] = {-1,-1,-1};

static volatile Srec_Type Srec;
static volatile Srec_Flag_Type status;
static volatile uint8_t byteShift;
static volatile uint8_t numData;
static volatile uint8_t AddrBytesNum;
static volatile uint8_t wordAddr;
static volatile uint8_t checkSum;

/*-------------------------------------STATIC - FUNCTION PROTOTYPE--------------------------------------------*/

// Function to Load data from file S-recode and Write it into Flash memory
void Load_Firmware(void);

// Function to restore to default firmware
void Restore_Firmware(void);

// Function to Offset vector table and call main function  of application
void JumpToApplication(uint8_t);

// Function to send instructions and messages to the PC
void SendStringToPC(Command_Type);

// Function to send errors to PC and reset variables
void SendErrorToPC(Status_Type);

// Function to Handler all of character be received in process of transmit - receiver from  the PC
void UART0_IRQHandler()
{
	if( (UART0->S1 & UART0_S1_RDRF_MASK) != 0U)
	{
		UART_data = UART0->D;
	}

	switch(GeneralFlag)
	{
	case FIRST_COMMAND:
		switch(UART_data)
		{
		case 'L':
			SendStringToPC(LOAD_COMMAND);
			commandFlag = LOAD_COMMAND;
			GeneralFlag = START_SECTOR;
			break;
		case 'E':
			SendStringToPC(ERASE_COMMAND);
			commandFlag = ERASE_COMMAND;
			GeneralFlag = START_SECTOR;
			break;
		case 'R':
			SendStringToPC(RESTORE_COMMAND);
			GeneralFlag = RESTORE_COMMAND;
			break;
		default:
			SendStringToPC(INVALID_COMMAND);
		}
		break;
	case START_SECTOR:
		sectorString[index] = UART_data;
		index++;

		if(index == START_SECTOR_CHAR)
		{
			startSector = Sector_StrtoDec( sectorString, START_SECTOR_CHAR);
			index = 0;
			switch(commandFlag)
			{
			case LOAD_COMMAND:
				if(TRUE == IsFlashEraseSector(startSector, sectorNum))
				{
					SendStringToPC(SEND_FILE_COMMAND);
					GeneralFlag = LOAD_COMMAND;
				}
				else
				{
					SendErrorToPC(ERR_FLASH_ERASE);
				}
				break;
			case ERASE_COMMAND:
				SendStringToPC(SECTOR_NUMBER);
				GeneralFlag = SECTOR_NUMBER;
				break;
			}
		}
		break;
	case SECTOR_NUMBER:
		sectorNum = ChartoDec(UART_data);
		if(TRUE == Flash_EraseSectorNum(startSector, sectorNum))
		{
			SendStringToPC(ERASE_SUCCESS);
			SendStringToPC(FIRST_COMMAND);
		}
		else
		{
			SendErrorToPC(ERR_SECTOR_SPECIAL);
		}

		GeneralFlag 	= FIRST_COMMAND;
		index			= 0U;
		break;
	case LOAD_COMMAND:
		Load_Firmware();
		break;
	case RESTORE_COMMAND:
		if('R' == UART_data)
		{
			statusFlag = RESTORE_STATE;
		}
		else
		{
			SendStringToPC(INVALID_COMMAND);
		}
		break;
	}
}

void Load_Firmware()
{
	switch (status)
	{
	case SREC_FLAG_S:
		if (UART_data == 'S')
		{
			Srec.SrecType[0] = UART_data;
			status = SREC_FLAG_TYPE;
		}
		else
		{
			SendErrorToPC(ERR_SREC_FILE);
		}
		break;
	case SREC_FLAG_TYPE:
		if (UART_data >= '0' && UART_data <= '9')
		{
			Srec.SrecType[1] = UART_data;
			
			if (UART_data >= '1' && UART_data <= '3')
			{
				//Set byte shift for Byte Count and move to ByteCount region
				byteShift = 4;
				status = SREC_FLAG_BYTECOUNT;
			}
			else
			{
				status = SREC_FLAG_NEXTLINE;
			}
		}
		else
		{
			SendErrorToPC(ERR_SREC_FILE);
		}
		break;
	case SREC_FLAG_BYTECOUNT:
		Srec.ByteCount += ChartoDec(UART_data) << byteShift;

		if (byteShift == 0)
		{
			if (Srec.ByteCount >= 3)
			{
				//Check number of bytes of Address base on S Type
				switch (Srec.SrecType[1])
				{
					case '1':
						byteShift = 12;
						AddrBytesNum = 2;
						break;
					case '2':
						byteShift = 20;
						AddrBytesNum = 3;
						break;
					case '3':
						byteShift = 28;
						AddrBytesNum = 4;
						break;
				}
				checkSum += Srec.ByteCount;
				//Move to Address Region
				status = SREC_FLAG_ADDRESS;
				break;
			}
			else
			{
				SendErrorToPC(ERR_SREC_FILE);
			}
		}

		byteShift -= 4;
		break;
	case SREC_FLAG_ADDRESS:
		Srec.Address += ChartoDec(UART_data) << byteShift;

		if (byteShift % 8 == 0)
		{
			checkSum += (Srec.Address >> byteShift) & 0xFFu;
		}

		if (byteShift == 0)
		{
			//Move to Data Region and set up byte shift for Data
			byteShift = 28;
			numData = Srec.ByteCount - AddrBytesNum - 1;
			status = SREC_FLAG_DATA;
			break;
		}

		byteShift -= 4;
		break;
	case SREC_FLAG_DATA:
		Srec.Data += ChartoDec(UART_data) << byteShift;

		if (byteShift % 8 == 0)
		{
			checkSum += (Srec.Data >> byteShift) & 0xFFu;
		}

		if (byteShift == 0)
		{
			Flash_WriteWord((Srec.Address + wordAddr), Srec.Data);
			wordAddr += 4;

			Srec.Data = 0;
			byteShift = 28;

			if (wordAddr == numData)
			{
				byteShift = 4;
				status = SREC_FLAG_CHECKSUM;
			}

			break;
		}

		byteShift -= 4;
		break;
	case SREC_FLAG_CHECKSUM:
		Srec.CheckSum += ChartoDec(UART_data) << byteShift;

		if (byteShift == 0)
		{
			if ((~checkSum & 0xFFu) == Srec.CheckSum)
			{
				status = SREC_FLAG_NEXTLINE;
			}
			else
			{
				SendErrorToPC(ERR_SREC_FILE);
			}
			break;
		}

		byteShift -= 4;
		break;
	case SREC_FLAG_NEXTLINE:
		if (UART_data == '\n')
		{
			Srec.ByteCount = 0;
			Srec.Address = 0;
			Srec.CheckSum = 0;
			status = 0;
			wordAddr = 0;
			numData = 0;
			AddrBytesNum = 0;
			checkSum = 0;
			
			if (Srec.SrecType[1] >= '7' && Srec.SrecType[1] <= '9')
			{
				Srec.SrecType[0] 	= 0;
				Srec.SrecType[1] 	= 0;
				
				statusFlag = END_OF_FILE;
				// Disable Receive Interrupt
				UART0->C2 &= ~UART0_C2_RIE_MASK;
			}
		}
		break;
	}
}

void Restore_Firmware()
{
	if(TRUE == Flash_EraseSectorNum(NEW_FIRMWARE, SECTOR_ERASE_NUM_DEFAULT))
	{
		JumpToApplication(DEFAULT_FIRMWARE);
	}
	else
	{
		SendErrorToPC(ERR_UNIDENTIFIED);
	}
}

void JumpToApplication(uint8_t startSec)
{
	uint32_t appAddress = startSec * SECTOR_SIZE;
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	__disable_irq();
	SCB->SHCSR &= ~(SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk);
	__set_MSP(*((volatile uint32_t*)appAddress));
	__DMB();
	SCB->VTOR = appAddress;
	__DSB();

	void(*reset_handler)(void) = (void(*)(void))(*((volatile uint32_t *)(appAddress +4)));

	reset_handler();
}

void BootLoaderMode_Handler()
{
	UART0_Init();
	UART_ReceiveINT();
	SendStringToPC(FIRST_COMMAND);

	while(1)
	{
		switch(statusFlag)
		{
		case RESTORE_STATE:
			statusFlag = RESET_STATE;
			Restore_Firmware();
			break;
		case END_OF_FILE:
			statusFlag = RESET_STATE;
			SendStringToPC(LOAD_SUCCESS);
			JumpToApplication(startSector);
			break;
		}
	}
}

void NomalMode_Handler()
{
	if(TRUE == IsFlashDataSector(NEW_FIRMWARE))
	{
		JumpToApplication(NEW_FIRMWARE);
	}
	else
	{
		if(TRUE == IsFlashDataSector(DEFAULT_FIRMWARE))
		{
			JumpToApplication(DEFAULT_FIRMWARE);
		}
		else
		{
			UART0_Init();
			UART_ReceiveINT();
			SendStringToPC(NO_FIRMWARE);
			// Disable Receive Interrupt
			UART0->C2 &= ~UART0_C2_RIE_MASK;
		}
	}
}

void Button_Init()
{
	Clock_Init(PORTC);

	const PortConfigType Port_Config =
	{
			.mux 	 	= PORT_MUX_GPIO,
			.pull	 	= PORT_PULL_UP
	};
	PORT_Init(PORTC, 3, &Port_Config);
}

BTN_State IsButtonPressed()
{
	BTN_State retVal = NOMAL_MODE;

	if((GPIOC->PDIR & (1U << 3)) == 0)
	{
		retVal = BOOTLOADER_MODE;
	}
	return retVal;
}

void SendErrorToPC(Status_Type errType)
{
	switch(errType)
	{
	case ERR_FLASH_ERASE:
		UART_SendString("\nErase Flash Memory Error !! Please retry !\n", 44);
		break;
	case ERR_FLASH_SECTOR_EMPTY:
		UART_SendString("\nAddress region have not data !! Please retry !\n", 49);
		break;
	case ERR_SREC_FILE:
		UART_SendString("\nFILE SREC Error - ", 19);
		break;
	case ERR_SECTOR_SPECIAL:
		UART_SendString("\nDo not Erase this Sector !!\n", 29);
		break;
	case FIRMWARE_LOAD_NOT_OK:
		UART_SendString("\nLoad firmware unsuccessfully !\n", 32);
		break;
	default :
		UART_SendString("\nHave Error !! Please retry !\n", 30);
	}
	
	if (ERR_SREC_FILE == errType)
	{	
		UART0->C2 &= ~UART0_C2_RE_MASK;

		switch (status)
		{
		case SREC_FLAG_S:
			UART_SendString("UNDEFINED RECORD TYPE\n", 22);
			break;
		case SREC_FLAG_TYPE:
			UART_SendString("ERROR STYPE\n", 12);
			break;
		case SREC_FLAG_BYTECOUNT:
			UART_SendString("ERROR BYTECOUNT\n", 17);
			break;
		case SREC_FLAG_CHECKSUM:
			UART_SendString("ERROR CHECKSUM\n", 15);
			break;
		}

		Flash_EraseSectorNum(startSector, 5);

		Srec.SrecType[0] 	= 0;
		Srec.SrecType[1] 	= 0;
		Srec.ByteCount 		= 0;
		Srec.Address 		= 0;
		Srec.CheckSum 		= 0;
		status 				= 0;
		wordAddr 			= 0;
		numData 			= 0;
		AddrBytesNum 		= 0;
		checkSum 			= 0;
	}

	GeneralFlag		= FIRST_COMMAND;
	startSector 	= 0U;
	sectorNum	 	= 1U;
	commandFlag 	= 0U;
	index			= 0U;
	SendStringToPC(FIRST_COMMAND);
}

void SendStringToPC(Command_Type String)
{
	switch (String)
	{
	case FIRST_COMMAND:
		UART_SendString("\nBOOT MODE\nL - load firmware\nE - erase flash memory\nR - restore old firmware\n", 77);
		break;
	case LOAD_COMMAND:
		UART_SendString("\nLOAD-COMMAND received !\n Enter start sector\n", 46);
		break;
	case ERASE_COMMAND:
		UART_SendString("\nERASE-COMMAND received !\n Enter start sector\n", 47);
		break;
	case RESTORE_COMMAND:
		UART_SendString("\nRESTORE-COMMAND received !\nEnter R key again to commit !\n", 58);
		break;
	case INVALID_COMMAND:
		UART_SendString("\nCharacter Invalid ! Please enter again.\n", 41);
		break;
	case SEND_FILE_COMMAND:
		UART_SendString("\nSend file ...\n", 15);
		break;
	case SECTOR_NUMBER:
		UART_SendString("\nEnter number of sector (1->9) to erase\n", 40);
		break;
	case RESTORE_CONFIRM:
		UART_SendString("\nEnter any a character to start process ...\n", 44);
		break;
	case ERASE_SUCCESS:
		UART_SendString("\nErased successfully!\n", 22);
		break;
	case NO_FIRMWARE:
		UART_SendString("\nMemmory have no any firmware to run !\n", 40);
		break;
	case LOAD_SUCCESS:
		UART_SendString("\nLoad firmware successfully !!\n", 31);
		break;
	}
}
