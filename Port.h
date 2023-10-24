/*
 * Port.h
 *
 *  Created on: Sep 11, 2023
 *      Author: WIN 10
 */

#ifndef PORT_H_
#define PORT_H_

#include "MKL46Z4.h"

#define NULL ((void *)0)

typedef enum {
	PORT_MUX_ANALOG		= 0x0U,
	PORT_MUX_GPIO		= 0x1U,
	PORT_MUX_ALT2		= 0x2U,
	PORT_MUX_ALT3		= 0x3U,
	PORT_MUX_ALT4		= 0x4U,
	PORT_MUX_ALT5		= 0x5U,
	PORT_MUX_ALT6		= 0x6U,
	PORT_MUX_ALT7		= 0x7U
} PortMuxType;

typedef enum
{
	PORT_PULL_DISABLE	= 0x0U,
	PORT_PULL_DOWN		= 0x2U,
	PORT_PULL_UP		= 0x3U,
} PortPullType;

typedef enum
{
	PORT_INTERRUPT_DISABLE		= 0x0U,
	PORT_INTERRUPT_LOW			= 0x8U,
	PORT_INTERRUPT_RISING		= 0x9U,
	PORT_INTERRUPT_FALLING		= 0xAU,
	PORT_INTERRUPT_EITHER		= 0xBU,
	PORT_INTERRUPT_HIGH			= 0xCU
} PortInterruptType;

typedef void (*CallbackType)(uint8_t pinnum);

typedef struct
{
	PortMuxType 		mux;
	PortPullType 		pull;
	PortInterruptType 	interrupt;
	CallbackType 		callback;
} PortConfigType;

void PORT_Init(PORT_Type * PORTx, uint8_t pinnum, const PortConfigType * Config);

#endif /* PORT_H_ */
