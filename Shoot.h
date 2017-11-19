#ifndef _SHOOT_H_
#define _SHOOT_H_

#define _LENGTH_SHOOTENEMY		3
#define _LENGTH_SHOOTPLAYER		1
#define _MOVEMENT				2
#define _UPPER_LIM				10

#include <stdint.h>

const uint8_t shootPlayer[_LENGTH_SHOOTPLAYER] = {0x7}; // type 0
const uint8_t shootEnemy1[_LENGTH_SHOOTENEMY] = {0x02,0x15,0x08}; // type 1
const uint8_t shootEnemy2[_LENGTH_SHOOTENEMY] = {0x08,0x1F,0x08}; // type 2


class Shoot{
	public:
		Shoot(): movCounter(0), shooting(false){};
		void initShoot(uint8_t type, uint8_t x, uint8_t y, uint16_t color);
		void moveShoot();
		void drawShoot();
		void eraseShoot();
		void setTargetReached();
		uint8_t getX();
		uint8_t getY();
		/*bool ();
		uint8_t getType();
		bool initialized;
		bool destroyed;*/
		bool shooting;

	private:
		uint8_t _type, _x, _y, movCounter; 	// type of shoot (0: player, 1: enemy1, 2: enemy2)
											// position coordinates
		uint16_t _color;
		uint8_t direction; 		// direction of movement
		char xlim;
		const uint8_t *sprite; // one for each different sprite of the animation
		
};

#endif