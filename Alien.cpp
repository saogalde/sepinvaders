#include "Alien.h"
#include "display/graphic_shapes.h"
#include "display/ST7735_commands.h"

void Alien::initAlien(uint8_t type, uint8_t x, uint8_t y){
	// this function initializes the alien position, and
	// also defines its type. At the moment of instantiation
	// the alien is automatically drawn on the screen.
	_x = x;
	_y = y;
	_type = type;
	direction = 1;
	animationSprite = 1;
	movCounter = 0;
	switch(_type){
		case 0:
			sprite_0= sprite_type0_0; // WARNING: ONLY THE POINTER IS BEING COPIED
			sprite_1= sprite_type0_1;
			break;
		case 1:
			sprite_0= sprite_type1_0; // WARNING: ONLY THE POINTER IS BEING COPIED
			sprite_1= sprite_type1_1;
			break;
		case 2:
			sprite_0= sprite_type2_0; // WARNING: ONLY THE POINTER IS BEING COPIED
			sprite_1= sprite_type2_1;
			break;
	}
	drawAlien();
	initialized=true;
	destroyed=false;
}

void Alien::moveAlien(){
	// Moves the alien. Performs limits verification and changes
	// the motion direction in case of reaching the screen border.
	// Also, changes the animStatus field, in order to change the
	// sprite in every movement.
	animationSprite ^= (1<<0);

	const uint8_t* alien;

	if(animationSprite) alien = sprite_0;
	else alien = sprite_1;
	uint8_t x = _x-6;
	uint8_t y = _y-4;

	if(movCounter != LATERAL_LIMIT) {
		for(int i=0; i<ALIEN_X_SPEED; i++) {
			for(int j=0; j<=8; j++) {
				if(direction) {
					drawPixel(x+i, y+j, ST7735_BLACK);
				}
				else {
					drawPixel(x+i+(12-ALIEN_X_SPEED), y+j, ST7735_BLACK);
				}
			}
		}
		for(int i=0; i<12; i++) {
			for(int j=0; j<=8; j++) {
				if(direction) {
					if(alien[i]&(0x80>>j)) drawPixel(x+i+ALIEN_X_SPEED, y+j, ST7735_WHITE);
					else drawPixel(x+i+ALIEN_X_SPEED, y+j, ST7735_BLACK);	
				}
				else {
					if(alien[i]&(0x80>>j)) drawPixel(x+i-ALIEN_X_SPEED, y+j, ST7735_WHITE);
					else drawPixel(x+i-ALIEN_X_SPEED, y+j, ST7735_BLACK);
				}
			}
		}
		if(direction) _x += ALIEN_X_SPEED;
		else _x -= ALIEN_X_SPEED;
		movCounter++;
	}
	else {
		for(int i=0; i<12; i++) {
			for(int j=0; j<ALIEN_Y_SPEED; j++) {
				drawPixel(x+i, y+j, ST7735_BLACK);
			}
		}
		for(int i=0; i<12; i++) {
			for(int j=0; j<=8; j++) {
				if(alien[i]&(0x80>>j)) drawPixel(x+i, y+j+ALIEN_Y_SPEED, ST7735_WHITE);
				else drawPixel(x+i, y+j+ALIEN_Y_SPEED, ST7735_BLACK);
			}
		}
		_y += ALIEN_Y_SPEED;
		movCounter = 0;
		direction ^= (1<<0);
	}
}

void Alien::drawAlien() {
	uint8_t x = _x-6;
	uint8_t y = _y+4;
	const uint8_t* alien;
	if(animationSprite) alien = sprite_0;
	else alien = sprite_1;
	
	for(int i=0; i<12; i++){
		for(int j=0; j<8; j++){
			if(alien[i]&(1<<j)) drawPixel(x+i, y-j, ST7735_WHITE);
			else drawPixel(x+i, y-j, ST7735_BLACK);
		}
	}
}