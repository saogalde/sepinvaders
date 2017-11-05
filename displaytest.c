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


#include "SPI/SPI_implement_me.h"
#include "USART/USART_implement_me.h"

#include "display/ST7735_commands.h"
#include "display/graphic_shapes.h"

// UART configuration
#define BAUD	9600					// serial communication baud rate
#define UBRR_VALUE F_CPU/16/BAUD-1

void drawAlien(uint8_t x, uint8_t y) {
	uint8_t alien[13] = {0,14,24,190,109,61,60,61,109,190,24,14,0};
	x = x-6;
	y = y+4;
	for(int i=0; i<13; i++){
		for(int j=0; j<8; j++){
			if(alien[i]&(1<<j)) drawPixel(x+i, y-j, ST7735_WHITE);
			else drawPixel(x+i, y-j, ST7735_BLACK);
		}
	}
}

/** The main function */
int main(void)
{
	// Initialises the serial communication interface
	USART_Init(BAUD);
	
	
	// Welcome message
	USART_Transmit_String("Hello! This is the SPI TFT display test. This program will print a few test graphics in a loop.\r\n\n");
		
	//DDRC |= (1<<PC5);
	// Hardware initialisation
	USART_Transmit_String("Initialising hardware... ");
	//PORTC ^= (1<<PC5);

	SPI_Master_Init();
	ST7735_init();


	USART_Transmit_String("success.\n");
	fillScreen(ST7735_BLACK);
	drawAlien(TFT_WIDTH/2, TFT_HEIGHT/2);
	drawPixel(0, 0, ST7735_WHITE);
	// Test programs. The following sequence runs many different images over the display.
	// Showing this full working sequence to your superviros is mandatory for passing the lab.
	// Apart from that, feel free to adapt and explore this code and the functions called by it.
	while(true)
	{	
	/*	
		USART_Transmit_String("Starting next run of test graphics: ");	
		//drawPixel(10,10,ST7735_GREEN);
		fillScreen(ST7735_BLACK);
		drawCircle(TFT_WIDTH/2, TFT_HEIGHT/2, 50, ST7735_RED);
		_delay_ms(1000);
		/*uint8_t pixel_y = 4;
			for(uint8_t pixel_x=0; pixel_x<TFT_WIDTH; pixel_x++)
			{
				int16_t green = 255-(pixel_x+pixel_y)/2;
				if (green < 0)
					green = 0;
				drawPixelRGB(pixel_x, pixel_y, pixel_x+95, green, pixel_y+127);
			}*/
		/*
		// RGB test
		fillScreen(ST7735_RED);		// Fully red screen.
		_delay_ms(1000);
		fillScreen(ST7735_GREEN);	// Fully green screen.
		_delay_ms(1000);
		fillScreen(ST7735_BLUE);	// Fully blue screen.
		_delay_ms(1000);
		USART_Transmit_String("#");	
		
		// Color test: Starts with green at (0,0) and fades to red along the x axis and
		// to blue along the z axis. Gives a good overview of the color capabilities
		// of the display.
		for(uint8_t pixel_y=0; pixel_y<TFT_HEIGHT; pixel_y++)
		{
			for(uint8_t pixel_x=0; pixel_x<TFT_WIDTH; pixel_x++)
			{
				int16_t green = 255-(pixel_x+pixel_y)/2;
				if (green < 0)
					green = 0;
				drawPixelRGB(pixel_x, pixel_y, pixel_x+95, green, pixel_y+127);
			}
		}
		_delay_ms(1000);
		USART_Transmit_String("#");
		
		
		// Adds a circle to the previous image. This shows the imperfections of the
		// used algorithm very well!
		drawCircle(TFT_WIDTH/2, TFT_HEIGHT/2, 50, ST7735_RED);
		drawCircle(TFT_WIDTH/2, TFT_HEIGHT/2, 49, ST7735_RED);
		drawCircle(TFT_WIDTH/2, TFT_HEIGHT/2, 51, ST7735_RED);
		_delay_ms(1000);
		USART_Transmit_String("#");
		
		
		// Draws a small 2x2 square in each corner and a green pixel in the center.
		// Useful to identify the display rotation.
		fillScreen(ST7735_BLACK);
		testcorners();
		_delay_ms(1000);
		USART_Transmit_String("#");
		
		
		// Draws rays of yellow lines originating in each of the corners.
		testlines(ST7735_YELLOW);
		_delay_ms(1000);
		USART_Transmit_String("#");
		
		
		// Draws many boxed triangles.
		testtriangles();
		_delay_ms(1000);
		USART_Transmit_String("#");
		
		
		// Draws many rectangles.
		testdrawrects(ST7735_GREEN);
		_delay_ms(1000);
		USART_Transmit_String("#");
		
		
		// Draws many *filled* rectangles.
		testfillrects(ST7735_MAGENTA, ST7735_YELLOW);
		_delay_ms(1000);
		USART_Transmit_String("#");
		
		// Draws discs and circles.
		fillScreen(ST7735_BLACK);
		testfillcircles(10, ST7735_BLUE);
		testdrawcircles(7, ST7735_WHITE);
		_delay_ms(1000);
		USART_Transmit_String("#");
		
		
		USART_Transmit_String(" finished!\n");
		
		// Here would be a nice place for drawing a bitmap, wouldn't it?*/
		
	}
}


