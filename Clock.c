/*
 * Clock.c
 *
 *  Created on: Oct 9, 2023
 *      Author: admin
 */

#include"Clock.h"



void ClockSourceSelection(uint8_t ClockSource)
{

}

void UpdateClockFreqForSystem(uint8_t ClockFreq)
{

}

void UpdateClockFreqForBus(uint8_t ClockFreq)
{

}

void Clock_Init(PORT_Type * PORTx)
{
    if (PORTx == PORTA)
    {
        SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    }
    else if (PORTx == PORTB)
    {
        SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    }
    else if (PORTx == PORTC)
    {
        SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    }
    else if (PORTx == PORTD)
    {
        SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    }
    else if (PORTx == PORTE)
    {
        SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    }
}



