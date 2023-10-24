// include guard
#ifndef _SREC_H_
#define _SREC_H_

#include <stdint.h>
#define  HEX_INVALID (0xFF)

typedef struct
{
    uint8_t SrecType[2];
    uint8_t ByteCount;
    uint32_t Address;
    uint32_t Data;
    uint8_t CheckSum;
} Srec_Type;

typedef enum
{
	SREC_FLAG_S 		= 0u,
	SREC_FLAG_TYPE 		= 1u,
	SREC_FLAG_BYTECOUNT,
	SREC_FLAG_ADDRESS,
	SREC_FLAG_DATA,
	SREC_FLAG_CHECKSUM,
	SREC_FLAG_NEXTLINE
}Srec_Flag_Type;

/*
 *Convert String to Decimal
 *param	- uint8_t * Input String
 *param	- uint8_t Length of input String
 *reval	- uint32_t Output Decimal
*/

uint8_t Sector_StrtoDec( int8_t * pString, uint8_t Len);
uint8_t ChartoDec( uint8_t Character);

#endif // _SERC_H_
