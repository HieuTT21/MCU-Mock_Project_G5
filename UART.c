/*
 * UART.c
 *
 *  Created on: Oct 16, 2023
 *      Author: admin
 */
#include "UART.h"

void UART0_Init()
{
		//Choose Fast Internal Clock
		MCG->C2 |= MCG_C2_IRCS(1u);

		//Enable MCGIRCLK
		MCG->C1 |= MCG_C1_IRCLKEN(1u);

		//Enable Clock for UART0 and PortA
		SIM->SCGC4 |= SIM_SCGC4_UART0(1u);
		SIM->SCGC5 |= SIM_SCGC5_PORTA(1u);

		//Choose Clock Source of UART0 as MCGIRCLK
		SIM->SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
		SIM->SOPT2 |= SIM_SOPT2_UART0SRC(3u);

		//Set PortA1 and PortA2 MUX to ALT2 and Enable Pull-up
		PORTA->PCR[1] &= ~PORT_PCR_MUX_MASK;
		PORTA->PCR[1] |= PORT_PCR_MUX(2u) | PORT_PCR_PE(1u) | PORT_PCR_PS(1u);

		PORTA->PCR[2] &= ~PORT_PCR_MUX_MASK;
		PORTA->PCR[2] |= PORT_PCR_MUX(2u) | PORT_PCR_PE(1u) | PORT_PCR_PS(1u);

		//Set Baud Rate Divider
		UART0->BDL = 41u;

		//Set OSR Divider
		UART0->C4 = (UART0->C4 & ~UART0_C4_OSR_MASK) | UART0_C4_OSR(9u);
}

void UART_SendString(uint8_t * pData, uint8_t Length)
{
	uint8_t count;

	// Enable Transmitter
	UART0->C2 |= UART0_C2_TE_MASK;

	for (count = 0; count < Length; count++)
	{
		// 1. wait Tx Buffer is Empty (TDRE = 1)
		while ((UART0->S1 & UART0_S1_TDRE_MASK) == 0U);
		// 2. write data to Tx Buffer
		UART0->D = pData[count];
	}

	// 3. wait Transmit is complete (TC = 1)
	while( (UART0->S1 & UART0_S1_TC_MASK) == 0U);

	// Disable Transmitter
	UART0->C2 &= ~UART0_C2_TE_MASK;
}

void UART_ReceiveINT(void)
{
	// Enable Receive Interrupt
	UART0->C2 |= UART0_C2_RIE_MASK;
	NVIC_EnableIRQ(UART0_IRQn);

	// Enable Receiver
	UART0->C2 |= UART0_C2_RE_MASK;
}

