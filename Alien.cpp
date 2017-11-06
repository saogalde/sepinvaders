#include "Alien.h"

Alien::Alien(uint8_t type, uint8_t x, uint8_t y){
	// this function initializes the alien position, and
	// also defines its type. At the moment of instantiation
	// the alien is automatically drawn on the screen.
	_x = x;
	_y = y;
	_type = type;
	direction = DIRECTION_LEFT;
	animationSprite = ANIM_0;
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
	drawAlien(_x,_y);
}

Alien::moveAlien(){
	// Moves the alien. Performs limits verification and changes
	// the motion direction in case of reaching the screen border.
	// Also, changes the animStatus field, in order to change the
	// sprite in every movement.
}

Alien::drawAlien(uint8_t x, uint8_t y){
	// Draws the alien in an specified position
}

Alien::eraseAlien(uint8_t x, uint8_t y){
	// Erases the alien in an specified position
}