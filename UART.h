/*
 * UART.h
 *
 *  Created on: Oct 16, 2023
 *      Author: admin
 */

#ifndef UART_H_
#define UART_H_

#include "MKL46Z4.h"
#include "Port.h"

void UART0_Init(void);

void UART_SendString(uint8_t * pData, uint8_t Length);

void UART_ReceiveINT(void);

#endif /* UART_H_ */
