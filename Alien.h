#ifndef _ALIEN_H_
#define _ALIEN_H_

#define DIRECTION_LEFT 		true
#define DIRECTION_RIGHT 	false
#define ANIM_0				true
#define ANIM_1				false
#define ALIEN_PX_SPEED		2

const uint8_t sprite_type0_0[11] = {14,24,190,109,61,60,61,109,190,24,14};
const uint8_t sprite_type0_1[11] = {120,29,190,108,60,60,60,108,190,29,120};
const uint8_t sprite_type1_0[11] = {14,24,190,109,61,60,61,109,190,24,14};
const uint8_t sprite_type1_1[11] = {14,24,190,109,61,60,61,109,190,24,14};
const uint8_t sprite_type2_0[11] = {14,24,190,109,61,60,61,109,190,24,14};
const uint8_t sprite_type2_1[11] = {14,24,190,109,61,60,61,109,190,24,14};

class Alien{
	public:
		Alien(uint8_t type, uint8_t x, uint8_t y);
		void moveAlien();
		bool destroyedAlien(uint8_t x, uint8_t y);

	private:
		uint8_t _type, _x, _y; 	// type of alien, and position coordinates
		bool animationSprite;	// status of the animation
		bool direction; 		// direction of movement. Uses the DIRECTION_LEFT
								// and DIRECTION_RIGHT macros
		uint8_t *sprite_0, *sprite_1; // one for each different sprite of the animation
		void drawAlien(uint8_t x, uint8_t y);
		void eraseAlien(uint8_t x, uint8_t y);
}

#endif