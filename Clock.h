/*
 * Clock.h
 *
 *  Created on: Oct 9, 2023
 *      Author: admin
 */

#ifndef CLOCK_H_
#define CLOCK_H_

#include"MKL46Z4.h"

typedef enum
{
	FLL_PLL_CLOCK		= 0U,
	INTERNAL_CLOCK		= 1U,
	EXTERNAL_CLODK		= 2U
}Clock_Source;

typedef enum
{
	CLOCK_FREQ_1MHZ		= 0U,
	CLOCK_FREQ_2MHZ		= 1U,
	CLOCK_FREQ_4MHZ		= 3U,
	CLOCK_FREQ_8MHZ		= 4U,
	CLOCK_FREQ_16MHZ	= 5U,
	CLOCK_FREQ_32MHZ	= 6U,
	CLOCK_FREQ_48MHZ	= 7U
}Clock_Freq_Sel;


void ClockSourceSelection(uint8_t);
void UpdateClockFreqForSystem(uint8_t);
void UpdateClockFreqForBus(uint8_t);

void Clock_Init(PORT_Type *);


#endif /* CLOCK_H_ */
