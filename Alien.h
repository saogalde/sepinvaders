#ifndef _ALIEN_H_
#define _ALIEN_H_

#include <stdint.h>

#define DIRECTION_LEFT 		1
#define DIRECTION_RIGHT 	0
#define ANIM_0				0
#define ANIM_1				1
#define ALIEN_X_SPEED		2
#define ALIEN_Y_SPEED		4
#define LATERAL_LIMIT		30

const uint8_t sprite_type0_0[11] = {14,24,190,109,61,60,61,109,190,24,14};
const uint8_t sprite_type0_1[11] = {120,29,190,108,60,60,60,108,190,29,120};
const uint8_t sprite_type1_0[11] = {14,24,190,109,61,60,61,109,190,24,14};
const uint8_t sprite_type1_1[11] = {14,24,190,109,61,60,61,109,190,24,14};
const uint8_t sprite_type2_0[11] = {14,24,190,109,61,60,61,109,190,24,14};
const uint8_t sprite_type2_1[11] = {14,24,190,109,61,60,61,109,190,24,14};
//const uint8_t sprite_destroy[11];

class Alien{
	public:
		Alien(uint8_t type, uint8_t x, uint8_t y);
		void moveAlien();
		bool destroyedAlien(uint8_t x, uint8_t y);

	private:
		uint8_t _type, _x, _y, movCounter; 	// type of alien, and position coordinates
		uint16_t color;
		uint8_t animationSprite;	// status of the animation
		uint8_t direction; 		// direction of movement. Uses the DIRECTION_LEFT
								// and DIRECTION_RIGHT macros
		const uint8_t *sprite_0, *sprite_1; // one for each different sprite of the animation
		void drawAlien(uint8_t x, uint8_t y);
};

#endif