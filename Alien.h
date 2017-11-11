#ifndef _ALIEN_H_
#define _ALIEN_H_

#include <stdint.h>

#define DIRECTION_LEFT 		1
#define DIRECTION_RIGHT 	0
#define ANIM_0				0
#define ANIM_1				1
#define ALIEN_X_SPEED		2
#define ALIEN_Y_SPEED		4
#define ALIEN_X_SEPARATION	14
#define ALIEN_Y_SEPARATION	10
#define LATERAL_LIMIT		28

#define ALIEN_0				0
#define ALIEN_1				1
#define ALIEN_2				2


const uint8_t sprite_type0_0[12] = {0,14,24,190,109,61,60,61,109,190,24,14};
const uint8_t sprite_type0_1[12] = {0,120,29,190,108,60,60,60,108,190,29,120};
const uint8_t sprite_type1_0[12] = {0x39,0x79,0x7A,0x6E,0xEC,0xFA,0xFA,0xEC,0x6E,0x7A,0x79,0x39};
const uint8_t sprite_type1_1[12] = {0x38,0x7A,0x7B,0x6D,0xEC,0xFA,0xFA,0xEC,0x6D,0x7B,0x7A,0x38};
const uint8_t sprite_type2_0[12] = {0x00,0x00,0x19,0x3A,0x6D,0xFA,0xFA,0x6D,0x3A,0x19,0x00,0x00};
const uint8_t sprite_type2_1[12] = {0x00,0x00,0x1A,0x3A,0x68,0xFC,0xFC,0x68,0x3A,0x1A,0x00,0x00};
//const uint8_t sprite_destroy[13] = {0x08,0x49,0x22,0x14,0x81,0x42,0x00,0x42,0x81,0x14,0x22,0x49,0x08};
const uint8_t sprite_destroy[12] = {0x49,0x22,0x14,0x81,0x42,0x00,0x42,0x81,0x14,0x22,0x49,0x08};
const uint8_t sprite_ufo[18] ={0x04,0x0C,0x1E,0x3D,0x3E,0x3C,0x7C,0x74,0x7E,0x7E,0x74,0x7C,0x3C,0x3E,0x37,0x1E,0x0C,0x04};		

class Alien{
	public:
		Alien(): initialized(false), destroyed(false), targeted(false){};
		void initAlien(uint8_t type, uint8_t x, uint8_t y, uint16_t col);
		void moveAlien();
		bool destroyedAlien();
		void drawAlien();
		uint8_t getType();
		bool initialized;
		bool destroyed;
		bool targeted;

	private:
		uint8_t _type, _x, _y, movCounter; 	// type of alien, and position coordinates
		uint16_t color;
		uint8_t animationSprite;	// status of the animation
		uint8_t direction; 		// direction of movement. Uses the DIRECTION_LEFT
								// and DIRECTION_RIGHT macros
		const uint8_t *sprite_0, *sprite_1; // one for each different sprite of the animation
		
};

#endif