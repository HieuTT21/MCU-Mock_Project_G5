/*
 * Port.c
 *
 *  Created on: Sep 11, 2023
 *      Author: WIN 10
 */

/* Params:
 * Port ? Pin ?
 * MUX (Analog, GPIO, Alt2->7)
 * Pull (Pullup / Pulldown / Pull-Disable)
 * Interrupt (Disable / Falling / Rasing / Both / High / Low)
 * */
#include "Port.h"

static CallbackType s_gCallback = NULL;

void PORT_Init(PORT_Type * PORTx, uint8_t pinnum, const PortConfigType * Config)
{
	/* Configure Multiplexing */
	PORTx->PCR[pinnum] &= ~PORT_PCR_MUX_MASK;
	PORTx->PCR[pinnum] |= PORT_PCR_MUX(Config->mux);

	/* Configure Pull Control */
	PORTx->PCR[pinnum] &= ~PORT_PCR_PE_MASK;
	PORTx->PCR[pinnum] &= ~PORT_PCR_PS_MASK;
	PORTx->PCR[pinnum] |= (Config->pull << 0U);

	/* Configure Interrupt Control */
	PORTx->PCR[pinnum] &= ~PORT_PCR_IRQC_MASK;
	PORTx->PCR[pinnum] |= PORT_PCR_IRQC(Config->interrupt);

	if (PORT_INTERRUPT_DISABLE != Config->interrupt)
	{
		// 2. NVIC - Enable Interrupt.
		if (PORTx == PORTA)
		{
			NVIC_EnableIRQ(PORTA_IRQn);
		}
		if (PORTx == PORTC || PORTx == PORTD)
		{
			NVIC_EnableIRQ(PORTC_PORTD_IRQn);
		}

		s_gCallback = Config->callback;
	}


}

void PORT_ReadInterruptFlag(PORT_Type * PORTx, uint8_t pinnum);
void PORT_ClearInterruptFlag(PORT_Type * PORTx, uint8_t pinnum);

void PORTC_PORTD_IRQHandler(void)
{
	// Call function - address? - function pointer
	uint8_t pinnum;

	// check interrupt flag ? -> flag = 1 -> clear
	if ( (PORTC->PCR[3] & PORT_PCR_ISF_MASK) != 0U)
	{
		// Clear flag
		PORTC->PCR[3] |= PORT_PCR_ISF_MASK;

		pinnum = 3;
	}
	if ( (PORTC->PCR[12] & PORT_PCR_ISF_MASK) != 0U)
	{
		// Clear flag
		PORTC->PCR[12] |= PORT_PCR_ISF_MASK;

		pinnum = 12;
	}

	(*s_gCallback)(pinnum);
}

