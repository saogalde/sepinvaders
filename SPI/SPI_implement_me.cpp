/***************************************************
 * This is a SPI library for the ATMega328PB.
 *
 * It can provide basic SPI master functionality for any application, but is
 * directly targeted at the Adafruit 1.8" SPI display and its SD card socket.
 * 
 * It is based on example codes from the Atmel device datasheet. The code is
 * kept simple, but is not always efficient. It is an easy-to-understand example
 * for studies and not intended for productive use.
 *
 * This code is in the public domain. Feel free to do with it whatever you want.
 * 
 * 
 * 
 * FOR STUDENTS:
 * 
 * This file will be given to you in an 'empty' state. The function bodies are
 * provided, but not their content. You are supposed to add the proper setup
 * code and complete these functions.
 * 
 * Hint 1: RTFM! The Atmel device datasheets contain all information necessary.
 * 
 * Hint 2: This code is intended for an Atmel ATMega328PB microcontroller. But
 * at the time of writing the according avr-gcc toolchain does not support this
 * model yet. Hence, the code *will be compiled for an ATMega328P instead*.
 * These chips are not identical! Check both datasheets and compare the
 * registers if in doubt.
 * 
 ****************************************************/

#include <avr/io.h>

#include "SPI_implement_me.h"


// The initialisation function. Call it once from your main() program before
// issuing any SPI commands with the functions below!
//
// Hint: It shoud activate the according SPI module in the microcontroller and
// probably set some register values. SPI has many options to configure! Read
// about them and find out, which ones are to be used here. Understanding the
// datasheets will be much faster than a simple try and error approach.
//
// Also note that <avr/io.h> and other files provide convenient macros for most
// registers and their bits. Stuff like
//    EXPL = (1<<POS)|(1<<FOO);
// is much nicer to read than
//    EXPL = 0b10010;
// This is not a requirement, but it will make life easier for you and your
// supervisors.
void SPI_Master_Init(void)
{/* Set MOSI and SCK output, all others input */
	DDRB |= (1<<DD_MOSI)|(1<<DD_SCK)|(1<<PB2);
	// enable CS
	DDRD |= (1<<DD_SS_TFT)|(1<<DC_PIN);
	/* Enable SPI, Master, set clock rate fck/4 */
	SPCR = (1<<SPE)|(1<<MSTR);
	CS_HIGH();
	DC_HIGH();
}


// This function transfers a single byte over the SPI bus.
//
// It is relatively easy and short, but you will need to call it *many* times,
// so think twice. A 160*128 pixe display requires already 20.480 calls to this
// function just to fill the display *once*. But you surely want more than one
// static image on it. Also, do not forget the display control commands.
//
// Keep in mind that the ST7735 display controller uses an additional, not SPI
// conformant D/C line to switch between command and data mode. Here would be
// a *very* good place to control it if you want it rather easy than efficient.
// To speed things up, you probably might want to move the D/C line control
// elsewhere at a later stage.
// In this context, do not overlook the two additional macro definitions in
// the header file.
void SPI_Master_transmit_char(uint8_t data, bool commandmode)
{/* Start transmission */
	if(commandmode){DC_LOW();}
	else{DC_HIGH();}
	CS_LOW();
	SPDR = data;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	CS_HIGH();
}

void SPI_Master_transmit_char_fast(uint8_t data)
{/* Start transmission */
	CS_LOW();
	SPDR = data;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	CS_HIGH();
}