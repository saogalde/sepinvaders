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
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "Alien.h"
#include "SPI/SPI_implement_me.h"
#include "USART/USART_implement_me.h"

#include "display/ST7735_commands.h"
#include "display/graphic_shapes.h"

// UART configuration
#define BAUD	9600					// serial communication baud rate
#define UBRR_VALUE F_CPU/16/BAUD-1


const uint8_t sprite_player[11] = {0x07,0x0F,0x0F,0x0F,0x3F,0x7F,0x3F,0x0F,0x0F,0x0F,0x07};
const uint8_t score[24] = 	{0x32,0x49,0x49,0x49,0x26,0x00,	0x3E,0x41,0x41,0x41,0x22,0x00,0x3E,0x41,0x41,0x41,0x3E,0x00,0x7F,0x48,0x4C,0x4A,0x31,0x00,0x7F,0x49,0x49,0x49,0x41,0x00 };
/*const uint8_t score[11] = 	{0x32,0x49,0x49,0x49,0x26,0x00,	\
							0x3E,0x41,0x41,0x41,0x22,0x00, \
							0x3E,0x41,0x41,0x41,0x3E,0x00, \
							0x7F,0x48,0x4C,0x4A,0x31,0x00 \ 
							0x7F,0x49,0x49,0x49,0x41,0x00 };
							//0x,0x,0x,0x,0x,0x00 \}*/
const uint8_t number0[5] = {0x3E,0x45,0x49,0x51,0x3E};
const uint8_t number1[5] = {0x00,0x00,0x21,0x7F,0x01};
const uint8_t number2[5] = {0x21,0x43,0x45,0x49,0x31};
const uint8_t number3[5] = {0x22,0x49,0x49,0x49,0x36};
const uint8_t number4[5] = {0x0C,0x14,0x24,0x7F,0x04};
const uint8_t number5[5] = {0x7A,0x49,0x49,0x49,0x46};
const uint8_t number6[5] = {0x3E,0x49,0x49,0x49,0x26};
const uint8_t number7[5] = {0x60,0x40,0x47,0x48,0x70};
const uint8_t number8[5] = {0x36,0x49,0x49,0x49,0x36};
const uint8_t number9[5] = {0x32,0x49,0x49,0x49,0x3E};

void drawAlien(uint8_t x, uint8_t y) {
	uint8_t alien[13] = {14,24,190,109,61,60,61,109,190,24,14};
	x = x-6;
	y = y+4;
	for(int i=0; i<13; i++){
		for(int j=0; j<8; j++){
			if(alien[i]&(1<<j)) drawPixel(x+i, y-j, ST7735_WHITE);
			else drawPixel(x+i, y-j, ST7735_BLACK);
		}
	}
}

void drawNumber(uint8_t x, uint8_t y){
	const uint8_t *number = number0;
	for(int i=0; i<5; i++){
		for(int j=0; j<8; j++){
			if(number[i]&(1<<j)) drawPixel(x+i, y-j, ST7735_WHITE);
		}
	}
}

void eraseAlien(uint8_t x, uint8_t y) {
	uint8_t alien[13] = {0,14,24,190,109,61,60,61,109,190,24,14,0};
	x = x-6;
	y = y+4;
	for(int i=0; i<13; i++){
		for(int j=0; j<8; j++){
			if(alien[i]&(1<<j)) drawPixel(x+i, y-j, ST7735_BLACK);
		}
	}
}

/** The main function */
int main(void)
{
	// Initialises the serial communication interface
	USART_Init(BAUD);
	SPI_Master_Init();
	ST7735_init();


	USART_Transmit_String("success.\n");
	fillScreen(ST7735_BLACK);
	for(int i = 0; i<50; i++){
	drawAlien(TFT_WIDTH/2, 10+i*2);
	drawNumber(20,20);
	_delay_ms(400);
	eraseAlien(TFT_WIDTH/2, 10+i*2);
}// Test programs. The following sequence runs many different images over the display.
	// Showing this full working sequence to your superviros is mandatory for passing the lab.
	// Apart from that, feel free to adapt and explore this code and the functions called by it.
	while(true)
	{
	}
}


