#include "Alien.h"
#include "display/graphic_shapes.h"
#include "display/ST7735_commands.h"

void Alien::initAlien(uint8_t type, uint8_t x, uint8_t y, uint16_t col){
	// this function initializes the alien position, and
	// also defines its type. At the moment of instantiation
	// the alien is automatically drawn on the screen.
	_x = x;
	_y = y;
	_type = type;
	color = col;
	direction = 1;
	animationSprite = 0;
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

	const uint8_t* Animations[2] = {sprite_0,sprite_1};

	if(targeted){
		uint8_t x = _x-6;
		uint8_t y = _y-4;
		for(int i = 0; i < 12; i++){
			for(int j=0; j<8; j++){
				if(sprite_0[i]&(0x80>>j)) drawPixel(x+i, y+j, ST7735_BLACK);
			}
		}
		destroyed = true;
	}
	else{
		const uint8_t* alien;
		const uint8_t* prev_alien;
		prev_alien = Animations[animationSprite];
		animationSprite ^= (1<<0);
		alien = Animations[animationSprite];

		uint8_t x = _x-6;
		uint8_t y = _y-4;

		if(movCounter != LATERAL_LIMIT) {
			for(int i=0; i<ALIEN_X_SPEED; i++) {
				for(int j=0; j<=8; j++) {
					if(direction) {
						if(prev_alien[i]&(0x80>>j)) drawPixel(x+i, y+j, ST7735_BLACK);
					}
					else {
						if(prev_alien[11-i]&(0x80>>j)) drawPixel(x+11-i, y+j, ST7735_BLACK);
					}
				}
			}
			for(int i=0; i<12; i++) {
				for(int j=0; j<=8; j++) {
					if (i<12-ALIEN_X_SPEED) {
						if(direction) {
							if((alien[i]&(0x80>>j)) && ( (prev_alien[i+ALIEN_X_SPEED]&(0x80>>j)) ==0 )) drawPixel(x+i+ALIEN_X_SPEED, y+j, color);
							else if ( ((alien[i]&(0x80>>j)) ==0 ) && prev_alien[i+ALIEN_X_SPEED]&(0x80>>j) ) drawPixel(x+i+ALIEN_X_SPEED, y+j, ST7735_BLACK);	
						}
						else {
							if((alien[11-i]&(0x80>>j)) && ( (prev_alien[11-i-ALIEN_X_SPEED]&(0x80>>j)) ==0 )) drawPixel(x+11-i-ALIEN_X_SPEED, y+j, color);
							else if ( ((alien[11-i]&(0x80>>j)) ==0 ) && prev_alien[11-i-ALIEN_X_SPEED]&(0x80>>j) ) drawPixel(x+11-i-ALIEN_X_SPEED, y+j, ST7735_BLACK);	
						}
					}
					else {
						if(direction) {
							if(alien[i]&(0x80>>j)) drawPixel(x+i+ALIEN_X_SPEED, y+j, color);
						}
						else {
							if(alien[11-i]&(0x80>>j)) drawPixel(x+11-i-ALIEN_X_SPEED, y+j, color);
						}
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
					if(prev_alien[i]&(0x80>>j)) drawPixel(x+i, y+j, ST7735_BLACK);
				}
			}
			for(int i=0; i<12; i++) {
				for(int j=0; j<8; j++) {
					if(j<8-ALIEN_Y_SPEED) {
						if((alien[i]&(0x80>>j)) && ( (prev_alien[i]&(0x80>>(j+ALIEN_Y_SPEED))) ==0 )) drawPixel(x+i, y+j+ALIEN_Y_SPEED, color);
						else if ( ((alien[i]&(0x80>>j)) ==0 ) && prev_alien[i]&(0x80>>(j+ALIEN_Y_SPEED)) ) drawPixel(x+i, y+j+ALIEN_Y_SPEED, ST7735_BLACK);	
					}
					else {
						if(alien[i]&(0x80>>j)) drawPixel(x+i, y+j+ALIEN_Y_SPEED, color);
					}
				}
			}
			_y += ALIEN_Y_SPEED;
			movCounter = 0;
			direction ^= (1<<0);
		}
	}
}

void Alien::drawAlien() {
	uint8_t x = _x-6;
	uint8_t y = _y-4;
	const uint8_t* alien;
	if(animationSprite) alien = sprite_1;
	else alien = sprite_0;
	
	for(int i=0; i<12; i++){
		for(int j=0; j<8; j++){
			if(alien[i]&(0x80>>j)) drawPixel(x+i, y+j, color);
			else drawPixel(x+i, y+j, ST7735_BLACK);
		}
	}
}

bool Alien::destroyedAlien(){ // until now, this is a 'destroyAlien' method
	sprite_0= sprite_destroy; // WARNING: ONLY THE POINTER IS BEING COPIED
	sprite_1= sprite_destroy;
	color = ST7735_RED;
	drawAlien();
	targeted = true;
	return true;
}

uint8_t Alien::getType() {
	return _type;
}