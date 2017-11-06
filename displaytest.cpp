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
#include "sprite.h"
#include "SPI/SPI_implement_me.h"
#include "USART/USART_implement_me.h"

#include "display/ST7735_commands.h"
#include "display/graphic_shapes.h"

// UART configuration
#define BAUD	9600					// serial communication baud rate
#define UBRR_VALUE F_CPU/16/BAUD-1

void drawAlien(uint8_t x, uint8_t y) {
	const uint8_t *alien = sprite_player;
	x = x-6;
	y = y+4;
	for(int i=0; i<11; i++){
		for(int j=0; j<8; j++){
			if(alien[i]&(1<<j)) drawPixel(x+i, y-j, ST7735_WHITE);
			else drawPixel(x+i, y-j, ST7735_BLACK);
		}
	}
}
/*
void drawGenericSprite(uint8_t x, uint8_t y, uint8_t x_length, const uint8_t *sprite){
	x = x-6;
	y = y+4;
	USART_Transmit_String("GENERIC\n");
	if(sprite == number4) USART_Transmit_String("es un 4\n");
	for(int i=0; i<5; i++){
		for(int j=0; j<8; j++){
			if(sprite[i]&(1<<j)) drawPixel(x+i, y-j, ST7735_WHITE);
		}
	}
}


void drawNumber(uint8_t x, uint8_t y, uint8_t number){
	const uint8_t *sprite;
	switch(number){
		case 0:
			sprite = number0;
			break;
		case 1:
			sprite = number1;
			break;
		case 2:
			sprite = number2;
			break;
		case 3:
			sprite = number3;
			break;
		case 4:
			sprite = number4;
			break;
		case 5:
			sprite = number5;
			break;
		case 6:
			sprite = number6;
			break;
		case 7:
			sprite = number7;
			break;
		case 8:
			sprite = number8;
			break;
		case 9:
			sprite = number9;
			break;
	}
	x = x-6;
	y = y+4;
	//if(sprite == number4) USART_Transmit_String("es un 4\n");
	drawGenericSprite(x,y,X_LENGTH_NUMBER,sprite);
	/*for(int i=0; i<5; i++){
		for(int j=0; j<8; j++){
			if(sprite[i]&(1<<j)) drawPixel(x+i, y-j, ST7735_WHITE);
		}
	}
}*/



void eraseAlien(uint8_t x, uint8_t y) {
	const uint8_t *alien = sprite_player;
	x = x-6;
	y = y+4;
	for(int i=0; i<11; i++){
		for(int j=0; j<8; j++){
			if(alien[i]&(1<<j)) drawPixel(x+i, y-j, ST7735_BLACK);
		}
	}
}
/*
void drawScore(uint8_t x, uint8_t y){
	const uint8_t *number = score;
	x = x-6;
	y = y+4;
	for(int i=0; i<30; i++){
		for(int j=0; j<8; j++){
			if(number[i]&(1<<j)) drawPixel(x+i, y-j, ST7735_WHITE);
		}
	}
}*/

/** The main function */
int main(void)
{
	// Initialises the serial communication interface
	USART_Init(BAUD);
	SPI_Master_Init();
	ST7735_init();


	USART_Transmit_String("success.\n");
	fillScreen(ST7735_BLACK);
	drawScore(0,0);
	for(int i = 0; i<10; i++){
	drawAlien(TFT_WIDTH/2, 10+i*2);
	drawNumber(10,10*i,4);
	_delay_ms(400);
	eraseAlien(TFT_WIDTH/2, 10+i*2);
}// Test programs. The following sequence runs many different images over the display.
	// Showing this full working sequence to your superviros is mandatory for passing the lab.
	// Apart from that, feel free to adapt and explore this code and the functions called by it.
	while(true)
	{
	}
}


