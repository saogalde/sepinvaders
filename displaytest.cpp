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

#include <avr/eeprom.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "Alien.h"
#include "Shoot.h"
#include "sprite.h"
#include "sound/sound.h"
//#include "splashscreen.h"

#include "SPI/SPI_implement_me.h"
#include "USART/USART_implement_me.h"
#include "display/ST7735_commands.h"
#include "display/graphic_shapes.h"

#define NUMBER_OF_ALIENS	25 // 4x4 array
#define LIVES 				3
#define POS_OFFSET			7
#define KILLING_OFFSET_Y	3
#define KILLING_OFFSET_X	6

// UART configuration
#define BAUD	9600					// serial communication baud rate
#define UBRR_VALUE F_CPU/16/BAUD-1

/* 
ONLY FOR DEBUGGING!!!
#include "USART/USART_implement_me.h"
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
}
*/
/* END OF DEBUGGING */




/*	GLOBAL VARIABLES	*/
static uint8_t SpaceshipPos[2] = {TFT_WIDTH/2,TFT_HEIGHT-20};
Alien aliens[NUMBER_OF_ALIENS];
Shoot shootplayer;
volatile uint8_t lives = LIVES;
volatile uint8_t currentLevel = 1;
volatile int scoreboard = 0;
//eeprom_update_word (( uint16_t *) 0, 986 );	//write code
volatile int hi_score = (int) eeprom_read_word (( uint16_t *) 0);
volatile int aliveAliens = NUMBER_OF_ALIENS;
volatile uint16_t alienspeed = 15;
//volatile int 

// declared in sound.h!
extern volatile unsigned char *cursor;
extern volatile uint16_t currentLength;
extern volatile uint16_t soundCounter;
/* END OF DEFINITION OF GLOBAL VARIABLES */



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
	TCCR1B &= ~(1<<CS11);								// CS => prescaler set to 64 
}

void start_timer1(void) {
	TCCR1B |= (1<<CS11);									// CS => prescaler set to 64 
}


void Timer_IO_Init(void) {
	TCCR1B |= (1 << WGM12);
	TIMSK1 |= (1 << OCIE1A);
	//OCR1A   = 1562;

	OCR1A   = 0x57FF;  //<-------------- deleted for testing
	//OCR1A = 1000;
	
	//TCCR1B |= (1<<CS10)|(1<<CS12);		//NO ACTIVAR!!! CS => prescaler set to 64
	TCCR1B |= (1<<CS11);									// CS => prescaler set to 64

	DDRC &= ~(1<<DDC0);												// Input button 
	PORTC |= (1<<PORTC0);											// pull-pu 

	DDRC &= ~(1<<DDC1);												// Input button 
	PORTC |= (1<<PORTC1);											// pull-pu 

	DDRC &= ~(1<<DDC2);												// Input button 
	PORTC |= (1<<PORTC2);											// pull-pu 

	DDRC |= (1<<DDC3);												// output button 
	PORTC &= ~(1<<PORTC3);											// clear 
}

void Timer_Sound0_Init(void){
	DDRD |= (1<<DDD0);												// output button 
	PORTD &= ~(1<<PD0);											// clear 
	OCR0A = 180; // approx 11kHz
	TCCR0A |= (1 << COM0A1); 
	TCCR0A |= (1 << WGM01);
	TIMSK0 |= (1 << OCIE0A); // timer interrupt
	//TCCR0B |= (1 << CS01); // prescaler /8
}

void Timer_Sound2_Init(void){
	OCR0A = 180; // approx 11kHz
	TCCR0A |= (1 << COM0A1); 
	TCCR0A |= (1 << WGM01);
	TIMSK0 |= (1 << OCIE0A); // timer interrupt
	//TCCR0B |= (1 << CS01); // prescaler /8
}


void push_score(int x,int y,int scoreboard) {
	drawNumber(x+18,y,scoreboard%10);
	scoreboard /= 10;
	drawNumber(x+12,y,scoreboard%10);
	scoreboard /= 10;
	drawNumber(x+6,y,scoreboard%10);
	scoreboard /= 10;
	drawNumber(x+0,y,scoreboard%10);
}

void update_scoreboard(uint8_t type){
	if (type==ALIEN_0) scoreboard += 20;
	else if (type==ALIEN_1) scoreboard += 10;
	else if (type==ALIEN_2) scoreboard += 40;
	push_score(0,8,scoreboard);
	aliveAliens--;
}

void init_hi_score(void) {
	drawGenericSprite(TFT_WIDTH/2-20,0, 17, hi, ST7735_GREEN);
	drawScore(TFT_WIDTH/2,0);
	push_score(TFT_WIDTH/2-10,8,hi_score);		//update hi score
}
void update_lives(uint8_t player) {
	if(player) {									//player 1
		lives--;
		deleteSpaceship(15+14*lives,TFT_HEIGHT-6);
		drawNumber(2,TFT_HEIGHT-10,lives);
	}
	else {											//player 2

	}
	if(!lives) {									//GAME OVER							
		//
	}
}

void checkDeadAlien(uint8_t x, uint8_t y){
	uint8_t coords[2] = {x,y};
	for(uint8_t i=0; i<NUMBER_OF_ALIENS; i++){
		if(aliens[i].initialized && !aliens[i].destroyed && !aliens[i].targeted){
			/* DEBUG SECTION */
			/* char t_str[30];
			sprintf(t_str, "COMP IF: %d<=%d<=%d\n", coords[0]-KILLING_OFFSET_X, aliens[i].getX(),coords[0]+KILLING_OFFSET_X);
			USART_Transmit_String(t_str);
			sprintf(t_str, "COMP IF: %d<=%d<=%d\n", coords[1]-KILLING_OFFSET_Y, aliens[i].getY(),coords[1]+KILLING_OFFSET_Y);
			USART_Transmit_String(t_str);
			USART_Transmit_String("--------------------------\n"); */
			/* END DEBUG */

			if(aliens[i].getX()<=coords[0]+KILLING_OFFSET_X && aliens[i].getX()>=coords[0]-KILLING_OFFSET_X){
				if(aliens[i].getY()<=coords[1]+KILLING_OFFSET_Y && aliens[i].getY()>=coords[1]-KILLING_OFFSET_Y){
					aliens[i].destroyedAlien();
					shootplayer.setTargetReached();
					update_scoreboard(aliens[i].getType());
					playSound('k');
					if(alienspeed>40) alienspeed = aliveAliens*10;
					else alienspeed = 40; 
					//alienspeed = aliveAliens*10;
					//return 0;
				}
			}
		}
	}
	if (coords[1] < 25) {
		shootplayer.setTargetReached();
		shootplayer.eraseShoot();
	}
}

/** The main function **/
int main(void)
{
	// Initialises the serial communication interface
	USART_Init(BAUD);
	SPI_Master_Init();
	ST7735_init();
	fillScreen(ST7735_BLACK);
	drawSpaceship(SpaceshipPos[0], SpaceshipPos[1], ST7735_WHITE);
	drawScore(0,0);
	push_score(0,8,scoreboard);
	init_hi_score();
	drawFastHLine(0,148,TFT_WIDTH,ST7735_GREEN);

	for(int i=0; i<LIVES; i++) {
		drawSpaceship(15+14*i,TFT_HEIGHT-6, ST7735_GREEN);	
	}
	drawNumber(2,TFT_HEIGHT-10,lives);

	createAliens();
	_delay_ms(400);
	Timer_IO_Init();

	Timer_Sound0_Init();
	Timer_Sound2_Init();
	sei();		
	
	while(1);

}
volatile uint16_t coun = 0;
ISR(TIMER1_COMPA_vect) {
	if(!(PINC & (1<<PINC0))) {
		//stop_timer1();
		SpaceshipPos[0] -= 2;
		drawSpaceship(SpaceshipPos[0], SpaceshipPos[1], ST7735_WHITE);
	}
	else if (!(PINC & (1<<PINC1))) {
		//stop_timer1();
		SpaceshipPos[0] += 2;
		drawSpaceship(SpaceshipPos[0], SpaceshipPos[1], ST7735_WHITE);
	}
	else if (!(PINC & (1<<PINC2))) {
		if(!shootplayer.shooting){
			shootplayer.initShoot(0,SpaceshipPos[0], SpaceshipPos[1]+1,ST7735_WHITE);
			playSound('s');
		}
	}
	if(shootplayer.shooting){
		shootplayer.moveShoot();
		checkDeadAlien(shootplayer.getX(), shootplayer.getY());
	}
	coun++;
	/* DEBUG SECTION 
	char t_str[30];
	sprintf(t_str, "ALIENSPEED %d\n", alienspeed);
	USART_Transmit_String(t_str);
	USART_Transmit_String("--------------------------\n"); */
	/* END DEBUG */
	if(coun==alienspeed){
		coun = 0;
		moveAliens();
	}
}

ISR(TIMER0_COMPA_vect){
	PORTB ^= (1<<PB0);
	/* DEBUG SECTION 
	char t_str[30];
	sprintf(t_str, "soundcounter %d\n", soundCounter);
	USART_Transmit_String(t_str); */
	/* END DEBUG */
	PORTD = *(cursor_TIMER0+soundCounter_TIMER0);
	soundCounter_TIMER0++;
	
	if(soundCounter_TIMER0 >= currentLength_TIMER0){ 
		stopSound_TIMER0();
		PORTD = 0;
	}
}

ISR(TIMER2_COMPA_vect){
	/* DEBUG SECTION  */
	/*char t_str[30];
	sprintf(t_str, "soundcounter %d\n", soundCounter);
	USART_Transmit_String(t_str);*/
	/* END DEBUG */
	PORTD = *(cursor_TIMER2+soundCounter_TIMER2);
	soundCounter_TIMER2++;
	
	if(soundCounter_TIMER2 >= currentLength_TIMER2){ 
		stopSound_TIMER2();
		PORTD = 0;
	}
}