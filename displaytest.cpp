/***************************************************
 * This is an example program on how to use the 
 * ST7735R based 1.8" color TFT LCD display from
 * Adafruit Industries with the Atmel ATmega328PB
 * Xplained mini developer board.
 * Note that this display also contains a micro SD
 * card socket on its back side.
 * 
 * See here for further information:
 * http://www.adafruit.com/products/358
 *
 * 
 * Note that a lot of this code is directly taken from
 * the C code examples found in the Atmel datasheets.
 * Other parts of the code are inspired by and based on
 * the Adafruit example programs for Arduino.
 * 
 * This code is under the MIT licence.
 * 
 ****************************************************/


// MCU Clock Speed - needed for delay.h
#define F_CPU	16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "Alien.h"
#include "sprite.h"
#include "SPI/SPI_implement_me.h"
#include "USART/USART_implement_me.h"
#include "display/ST7735_commands.h"
#include "display/graphic_shapes.h"

#define NUMBER_OF_ALIENS	25 // 4x4 array
#define POS_OFFSET			7

// UART configuration
#define BAUD	9600					// serial communication baud rate
#define UBRR_VALUE F_CPU/16/BAUD-1

static uint8_t SpaceshipPos[2] = {TFT_WIDTH/2,TFT_HEIGHT-20};

Alien aliens[NUMBER_OF_ALIENS];
volatile uint8_t currentLevel = 1;
volatile int scoreboard = 0;
volatile int aliveAlians = NUMBER_OF_ALIENS;

void createAliens(){
	uint8_t type = 0;
	int counter = NUMBER_OF_ALIENS-1;
	for(int y=-2;y<3;y++){
		for(int x=-3;x<2;x++){
			//if (type == 2) aliens[counter--].initAlien(type, TFT_WIDTH/3+x*ALIEN_X_SEPARATION+POS_OFFSET, TFT_HEIGHT/3+y*ALIEN_Y_SEPARATION, ST7735_GREEN);
			//else 
			aliens[counter--].initAlien(type, TFT_WIDTH/3+x*ALIEN_X_SEPARATION+POS_OFFSET, TFT_HEIGHT/3+y*ALIEN_Y_SEPARATION, ST7735_WHITE);
		}
		type++;
		if(type>2) type=0;
	}
}

void moveAliens(){
	for(uint8_t i=0; i<NUMBER_OF_ALIENS; i++){
		if(aliens[i].initialized && !aliens[i].destroyed){
			aliens[i].moveAlien();
		}
	}
}

void delay_ms(int count) {
	while(count--) {
		_delay_ms(1);
	}
}


void stop_timer1(void) {
	TCCR1B &= ~((1<<CS10)|(1<<CS12));									/* CS => prescaler set to 64 */
}

void start_timer1(void) {
	TCCR1B |= (1<<CS10)|(1<<CS12);									/* CS => prescaler set to 64 */
}


void Timer_IO_Init(void) {
	TCCR1B |= (1 << WGM12);
	TIMSK1 |= (1 << OCIE1A);
	//OCR1A   = 1562;
	OCR1A   = 0xAFFF;
	//TCCR1B |= (1<<CS10)|(1<<CS12);									/* CS => prescaler set to 64 */
	TCCR1B |= (1<<CS11);									/* CS => prescaler set to 64 */

	DDRC &= ~(1<<DDC0);												/* Input button */
	PORTC |= (1<<PORTC0);											/* pull-pu */

	DDRC &= ~(1<<DDC1);												/* Input button */
	PORTC |= (1<<PORTC1);											/* pull-pu */

	DDRC |= (1<<DDC2);												/* output button */
	PORTC &= ~(1<<PORTC2);											/* clear */
}

void push_score(int scoreboard) {
	drawNumber(18,8,scoreboard%10);
	scoreboard /= 10;
	drawNumber(12,8,scoreboard%10);
	scoreboard /= 10;
	drawNumber(6,8,scoreboard%10);
	scoreboard /= 10;
	drawNumber(0,8,scoreboard%10);
}

void update_scoreboard(uint8_t type){
	if (type==ALIEN_0) scoreboard += 20;
	else if (type==ALIEN_1) scoreboard += 10;
	else if (type==ALIEN_2) scoreboard += 40;
	push_score(scoreboard);
	aliveAlians--;
}

/** The main function **/
int main(void)
{
	// Initialises the serial communication interface
	USART_Init(BAUD);
	SPI_Master_Init();
	ST7735_init();
	fillScreen(ST7735_BLACK);
	drawSpaceship(SpaceshipPos[0], SpaceshipPos[1]);
	Timer_IO_Init();
	sei();													// Enable global interruptions for timer
	start_timer1();

	drawScore(0,0);
	push_score(scoreboard);
	
	createAliens();
	_delay_ms(400);
	for(int i=1;i<500;i++) {
		moveAliens();
		if(i==5) {
			aliens[2].destroyedAlien();
			update_scoreboard(aliens[2].getType());
		}
		if(i==10) {
			aliens[12].destroyedAlien();
			update_scoreboard(aliens[12].getType());
		}
		if(i==20) {
			aliens[4].destroyedAlien();
			update_scoreboard(aliens[4].getType());
		}
		if(i==30) {
			aliens[9].destroyedAlien();
			update_scoreboard(aliens[9].getType());
		}
		if(i==40) {
			aliens[10].destroyedAlien();
			update_scoreboard(aliens[10].getType());
			aliens[0].destroyedAlien();
			update_scoreboard(aliens[0].getType());
			aliens[5].destroyedAlien();
			update_scoreboard(aliens[5].getType());
			aliens[20].destroyedAlien();
			update_scoreboard(aliens[20].getType());
		}
		if(i==50) {
			aliens[15].destroyedAlien();
			update_scoreboard(aliens[15].getType());
			aliens[16].destroyedAlien();
			update_scoreboard(aliens[16].getType());
			aliens[17].destroyedAlien();
			update_scoreboard(aliens[17].getType());
			aliens[18].destroyedAlien();
			update_scoreboard(aliens[18].getType());
			aliens[19].destroyedAlien();
			update_scoreboard(aliens[19].getType());
		}
		if(i==60) {
			aliens[21].destroyedAlien();
			update_scoreboard(aliens[21].getType());
			aliens[22].destroyedAlien();
			update_scoreboard(aliens[22].getType());
			aliens[23].destroyedAlien();
			update_scoreboard(aliens[23].getType());
			aliens[24].destroyedAlien();
			update_scoreboard(aliens[24].getType());
		}
		if(i==70) {
			for(uint8_t i=0; i<14; i++){
				if (!aliens[i].destroyed) {
					aliens[i].destroyedAlien();
					update_scoreboard(aliens[i].getType());
				}
			}
		}
		delay_ms(aliveAlians*10);
	}
	while(1);
}// Test programs. The following sequence runs many different images over the display.


ISR(TIMER1_COMPA_vect) {
	if(!(PINC & (1<<PINC0))) {
		stop_timer1();
		SpaceshipPos[0] -= 2;
		drawSpaceship(SpaceshipPos[0], SpaceshipPos[1]);
	}
	else if (!(PINC & (1<<PINC1))) {
		stop_timer1();
		SpaceshipPos[0] += 2;
		drawSpaceship(SpaceshipPos[0], SpaceshipPos[1]);
	}
	PORTC ^= (1<<PC2);
}