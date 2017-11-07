/***************************************************
 * This is an example program on how to use the 
 * ST7735R based 1.8" color TFT LCD display from
 * Adafruit Industries with the Atmel ATmega328PB
 * Xplained mini developer board.
 * Note that this display also contains a micro SD
 * card socket on its back side.
 * 
 * See here for further information:
 * http://www.adafruit.com/products/358
 *
 * 
 * Note that a lot of this code is directly taken from
 * the C code examples found in the Atmel datasheets.
 * Other parts of the code are inspired by and based on
 * the Adafruit example programs for Arduino.
 * 
 * This code is under the MIT licence.
 * 
 ****************************************************/


// MCU Clock Speed - needed for delay.h
#define F_CPU	16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "Alien.h"
#include "sprite.h"
#include "SPI/SPI_implement_me.h"
#include "USART/USART_implement_me.h"

#include "display/ST7735_commands.h"
#include "display/graphic_shapes.h"

// UART configuration
#define BAUD	9600					// serial communication baud rate
#define UBRR_VALUE F_CPU/16/BAUD-1

static uint8_t SpaceshipPos[2] = {TFT_WIDTH/2,TFT_HEIGHT-20};

void stop_timer1(void) {
	TCCR1B &= ~((1<<CS10)|(1<<CS12));									/* CS => prescaler set to 64 */
}

void start_timer1(void) {
	TCCR1B |= (1<<CS10)|(1<<CS12);									/* CS => prescaler set to 64 */
}

void Timer_IO_Init(void) {
	TCCR1B |= (1 << WGM12);
	TIMSK1 |= (1 << OCIE1A);
	OCR1A   = 1562;
	TCCR1B |= (1<<CS10)|(1<<CS12);									/* CS => prescaler set to 64 */

	DDRC &= ~(1<<DDC0);												/* Input button */
	PORTC |= (1<<PORTC0);											/* pull-pu */

	DDRC &= ~(1<<DDC1);												/* Input button */
	PORTC |= (1<<PORTC1);											/* pull-pu */

	DDRC |= (1<<DDC2);												/* output button */
	PORTC &= ~(1<<PORTC2);											/* clear */
}

/** The main function */
int main(void)
{
	// Initialises the serial communication interface
	USART_Init(BAUD);
	SPI_Master_Init();
	ST7735_init();
	drawSpaceship(SpaceshipPos[0], SpaceshipPos[1]);
	Timer_IO_Init();
	sei();
	start_timer1();

	
	Alien only1alien(0, TFT_WIDTH/3, TFT_HEIGHT/3);
	Alien only2alien(0, TFT_WIDTH/3+15, TFT_HEIGHT/3);
	Alien only3alien(0, TFT_WIDTH/3-15, TFT_HEIGHT/3);
	Alien only4alien(0, TFT_WIDTH/3-30, TFT_HEIGHT/3);
	fillScreen(ST7735_BLACK);
	drawScore(0,0);
	drawNumber(0,8,0);
	drawNumber(6,8,0);
	drawNumber(12,8,0);
	drawNumber(18,8,0);
	for(int i=1;i<250;i++) {
		only1alien.moveAlien();
		only2alien.moveAlien();
		only3alien.moveAlien();
		only4alien.moveAlien();
		_delay_ms(100);
		//_delay_ms((int)(100/i));
	}
	while(1);
}// Test programs. The following sequence runs many different images over the display.


ISR(TIMER1_COMPA_vect) {
	if(!(PINC & (1<<PINC0))) {
		stop_timer1();
		SpaceshipPos[0] -= 2;
		drawSpaceship(SpaceshipPos[0], SpaceshipPos[1]);
	}
	else if (!(PINC & (1<<PINC1))) {
		stop_timer1();
		SpaceshipPos[0] += 2;
		drawSpaceship(SpaceshipPos[0], SpaceshipPos[1]);
	}
	PORTC ^= (1<<PC2);
}