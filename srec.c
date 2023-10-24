#include "srec.h"
#include <math.h>

// Convert a Charater to Decimal 
uint8_t ChartoDec( uint8_t Character)
{
	uint8_t retVal;

	if( Character >= '0' && Character <= '9')
	{
		retVal = Character - '0';
	}
	else if( Character >= 'A' && Character <= 'F')
	{
		retVal = Character - 'A' + 10;
	}
	else
	{
		retVal = HEX_INVALID;
	}
	return retVal;
}

uint8_t Sector_StrtoDec( int8_t * pString, uint8_t Len)
{
	uint8_t retVal = 0;
	uint8_t 	i;
    uint8_t 	dec;

	for( i = 0; i < Len; ++i)
	{
		dec = ChartoDec( pString[i]);
		if ((int8_t)dec < 0)
		{
			break;
		}
		else
		{
			retVal += dec * pow(10, Len-1-i);
		}
	}
	return retVal;
}
