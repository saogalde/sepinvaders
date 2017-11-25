#define F_CPU	16000000UL

#include "Shoot.h"
#include "display/graphic_shapes.h"
#include "display/ST7735_commands.h"
#include <util/delay.h>
/*#include "USART/USART_implement_me.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
  char fmt[20];

  int whole = val;
  float mantissa = val - whole;

  int32_t frac = mantissa * powf(10, prec);
  if(frac < 0) frac = -frac;

  sprintf(fmt, "%%0%dd.%%0%dd", width, prec);
  sprintf(sout, fmt, whole, frac);
  return sout;
}*/

void Shoot::initShoot(uint8_t type, uint8_t x, uint8_t y, uint16_t color){
	_type = type;
	_x = x;
	_y = y;
	_color = color;
	switch(_type){
		case 0:
			sprite = shootPlayer;
			xlim = _LENGTH_SHOOTPLAYER;
			direction = -_MOVEMENT;
			break;
		case 1:
			sprite = shootEnemy1;
			xlim = _LENGTH_SHOOTENEMY;
			direction = _MOVEMENT;
			break;
		case 2:
			sprite = shootEnemy2;
			xlim = _LENGTH_SHOOTENEMY;
			direction = _MOVEMENT;
			break;
	}
	drawShoot();
	shooting = true;
}

void Shoot::setTargetReached(){
	shooting = false;
	eraseShoot();
}

uint8_t Shoot::getX(){
	return _x;
}

uint8_t Shoot::getY(){
	return _y;
}

void Shoot::drawShoot() {
	uint8_t x = _x;
	uint8_t y = _y-7;
	for(int i=0; i<xlim; i++) {
		for(int j=0; j<_HEIHG_SHOOTPLAYER; j++) {
			if(sprite[i]&(0x01<<j)) drawPixel(x+i, y+j, _color);
		}
	}
}

void Shoot::moveShoot(){
	_y+=direction;
	uint8_t x = _x;
	uint8_t y = _y-7;
	if(_y<=_UPPER_LIM) {
		//setTargetReached();
	}
	else {
		for(int i=0; i<xlim; i++) {
			for(int j=0; j<_MOVEMENT; j++) {
				if(sprite[i]&(0x01<<j)) drawPixel(x+i, y+j, _color);
			}
			for(int j=0; j<_MOVEMENT; j++) {
				if(sprite[i]&(0x01<<j)) drawPixel(x+i, y+j+_HEIHG_SHOOTPLAYER, ST7735_BLACK);
			}
		}
	}
}

void Shoot::eraseShoot(){
	uint8_t x = _x;
	uint8_t y = _y-7;
	for(int i=0; i<xlim; i++) {
		for(int j=0; j<_HEIHG_SHOOTPLAYER; j++) {
			if(sprite[i]&(0x01<<j)) drawPixel(x+i, y+j, ST7735_BLACK);
		}
	}
}