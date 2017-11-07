#include "sprite.h"
#include "display/ST7735_commands.h"
#include "display/graphic_shapes.h"
#include "USART/USART_implement_me.h"
#include <avr/io.h>

void drawGenericSprite(uint8_t x, uint8_t y, uint8_t x_length, const uint8_t *sprite, uint16_t color){
	//x = x-6;
	y = y+7;
	for(int i=0; i<x_length; i++){
		for(int j=0; j<8; j++){
			if(sprite[i]&(1<<j)) drawPixel(x+i, y-j, color);
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
	drawGenericSprite(x,y,X_LENGTH_NUMBER,sprite, ST7735_WHITE);
	/*for(int i=0; i<5; i++){
		for(int j=0; j<8; j++){
			if(sprite[i]&(1<<j)) drawPixel(x+i, y-j, ST7735_BLACK);
		}
	}*/
}

void drawScore(uint8_t x, uint8_t y){
	const uint8_t *sprite = score;
	drawGenericSprite(x,y,X_LENGTH_SCORE,sprite, ST7735_GREEN);
}

void drawPlayer(uint8_t x, uint8_t y, uint8_t whichPlayer){
	const uint8_t *sprite = score;
	drawGenericSprite(x,y,X_LENGTH_SCORE,sprite, ST7735_WHITE);
}

void drawSpaceship(uint8_t x, uint8_t y) {
	uint8_t ship[15] = {0x00,0x00,0x07,0x0F,0x0F,0x0F,0x3F,0x7F,0x3F,0x0F,0x0F,0x0F,0x07,0x00,0x00};
	x = x-7;
	y = y-4;
	for(int i=0; i<15; i++) {
		for(int j=0; j<8; j++) {
			if(ship[i]&(0x80>>j)) drawPixel(x+i, y+j, ST7735_WHITE);
			else drawPixel(x+i, y+j, ST7735_BLACK);
		}
	}
	TCCR1B |= (1<<CS10)|(1<<CS12);	
	
}