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
 * This file will be given to you in an 'empty' state. The function bodies are
 * provided, but not their content. You are supposed to add the proper code
 * and complete these functions.
 *
 * Hint 1: RTFM! The Atmel device datasheets contain all information necessary.
 *
 * Hint 2: We do not expect you to write the most performant or exceptionally
 * well-engineered code, but it should work. If in doubt, sacrifice speed for
 * reliability. If you are bored, rewrite the entire library in assembler and
 * squeeze the last microsecond out of it.
 *
 ****************************************************/
#include <stdint.h>

#ifdef __AVR_ATmega328P__
    #define F_CPU 16000000UL
    #include <avr/io.h>
#elif __MSP430__
    #include <msp430.h>
    volatile float N_a3;
#endif

#include <string.h>

#include "USART_implement_me.h"



// The initialisation function. Call it once from your main() program before
// issuing any USART commands with the functions below!
void USART_Init(uint16_t ubrr){
    #ifdef __AVR_ATmega328P__
        uint16_t ubrrk = (((F_CPU / (ubrr * 16UL))) - 1); // formula written in the datasheet
        //The down-counter, running at system clock (fosc), is loaded with the UBRRn value each
        //time the counter has counted down to zero or when the UBRRnL Register is written. A clock is generated each time the counter reaches zero.

        // USART BAUD RATE 16bit register
        // gotta write nibbles independently 8bits a time.
        UBRR0H = (uint8_t)(ubrrk>>8);
        UBRR0L = (uint8_t)ubrrk;
        // Set frame format to 8 data bits, no parity, 1 stop bit (NO SETTING USBS0)
        // USART Modes: The UCSZ0[1:0] bits combined with the UCSZ02 bit in UCSR0B sets the number of data bits (Character Size) in a frame the Receiver and Transmitter use.
        // en este caso se activa UCSZ0[2:0] = 011 = operacion con 8bits
        UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
        //enable transmission The Transmitter will override normal port operation for the TxD0 pin when enabled.
        // The disabling of the Transmitter (writing TXEN0 to zero) will not become effective until ongoing and pending transmissions are completed
        UCSR0B |= (1<<TXEN0);
    #elif __MSP430__

        WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
          P4SEL = BIT4+BIT5;                        // P3.4,5 = USCI_A0 TXD/RXD
          UCA1CTL1 |= UCSWRST;                      // **Put state machine in reset** (1b = Enabled. USCI logic held in reset state.)
          UCA1CTL1 |= UCSSEL_1;                     // CLK = ACLK
          if(ubrr == 57600){
              UCA1CTL1 |= UCSSEL_2;
              UCA1BR0 = 18;                           // 32kHz/9600=3.41 (see User's Guide)
              UCA1BR1 = 0;
          }
          else{
          N_a3 = (float)(32000/ubrr);
          UCA1BR0 = (uint8_t)(N_a3);                           // 32kHz/9600=3.41 (see User's Guide)
          UCA1BR1 = (uint8_t)((uint16_t)N_a3>>8);
          }
          UCA1MCTL = UCBRS_3+UCBRF_0;               // Modulation UCBRSx=3, UCBRFx=0
          UCA1CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    #endif
}



// Transmits a single character
void USART_Transmit_char(uint8_t data){
#ifdef __AVR_ATmega328P__
    while(!(UCSR0A&(1<<UDRE0))){};
    // Transmit data
    UDR0 = data;  // Transmit data buffer, 8 bit long
#elif __MSP430__
    while (!(UCA1IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
    UCA1TXBUF = data;                  // TX -> RXed character
#endif
}

// Transmits a given string
void USART_Transmit_String(char* string)
{
  while(*string != '\0'){
  /* Wait for empty transmit buffer */
  USART_Transmit_char(*string);
  string++;}
}
