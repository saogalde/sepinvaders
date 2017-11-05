/*
 * USART_implement_me.h
 *
 *  Created on: Sep 9, 2017
 *      Author: saogalde
 */

#ifndef USART_USART_IMPLEMENT_ME_H_
#define USART_USART_IMPLEMENT_ME_H_

/***************************************************
 * This is a USART library for the ATMega328P.
 *
 * It can provide basic USART (serial) communication for any application.
 *
 * This code is in the public domain. Feel free to do with it whatever you want.
 *
 *
 *
 * FOR STUDENTS:
 *
 * This file is complete, but the according .c file will be given to you in an
 * 'empty' state. See there for further information.
 *
 ****************************************************/

#ifndef _USART_IMPLEMENT_ME_H_
#define _USART_IMPLEMENT_ME_H_

#include <stdint.h>

struct USART_configuration
{
	uint16_t baudrate;
	uint8_t databits;
	uint8_t parity; // 
	uint8_t stopbits;
};


// Call once to initialize USART communication
void USART_Init(uint16_t ubrr);

// Transmits a single character
void USART_Transmit_char(uint8_t data );

void USART_Transmit_String(char* string);
#endif // _USART_IMPLEMENT_ME_H_

#endif /* USART_USART_IMPLEMENT_ME_H_ */
