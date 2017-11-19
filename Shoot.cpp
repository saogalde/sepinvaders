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


/***************** TO BE OPTIMIZED! **************/
/***************** TO BE OPTIMIZED! **************/
/***************** TO BE OPTIMIZED! **************/
/***************** TO BE OPTIMIZED! **************/
/***************** TO BE OPTIMIZED! **************/
/***************** TO BE OPTIMIZED! **************/


void Shoot::eraseShoot(){
	uint8_t x = _x-1;
	uint8_t y = _y-5;
	/*if(xlim==1) USART_Transmit_String("VALOR CORRECTO\n");
	char t_str[6];
	sprintf(t_str, "x:%d\n", xlim);
	USART_Transmit_String(t_str);*/
	for(int i=0; i<xlim; i++){
		for(int j=0; j<8; j++){
			if(sprite[i]&(0x80>>j)) drawPixel(x+i, y+j, ST7735_BLACK);
		}
	}
}

void Shoot::moveShoot(){
	eraseShoot();
	_y+=direction;
	if(_y<=_UPPER_LIM) setTargetReached();
	drawShoot();
}

void Shoot::drawShoot() {
	uint8_t x = _x-1;
	uint8_t y = _y-8;
	/*if(xlim==1) USART_Transmit_String("VALOR CORRECTO\n");
	char t_str[6];
	sprintf(t_str, "x:%d\n", xlim);
	USART_Transmit_String(t_str);*/
	for(int i=0; i<xlim; i++){
		for(int j=0; j<8; j++){
			if(sprite[i]&(0x80>>j)) drawPixel(x+i, y+j, _color);
			else drawPixel(x+i, y+j, ST7735_BLACK);
		}
	}
}