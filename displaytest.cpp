// MCU Clock Speed - needed for delay.h
#define F_CPU	16000000UL
#define MCP4725_WRITE	0xC0

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
//#include "I2C/avr-i2c-master.h"
//#include "I2C/TWI_Master.h"
#include "I2C/I2C.h"
#include "SPI/SPI_implement_me.h"
#include "USART/USART_implement_me.h"
#include "display/ST7735_commands.h"
#include "display/graphic_shapes.h"

#include "PetitFatFilesystem/PetitFS.h"

#define NUMBER_OF_ALIENS	25 // 4x4 array
#define LIVES 				3
#define POS_OFFSET			7
#define KILLING_RANGE_Y	3
#define KILLING_RANGE_X	6
#define STARTING_ALIEN_SPEED	55
#define PLAYER_LIMIT_X_LEFT 	5
#define PLAYER_LIMIT_X_RIGHT 	122
#define GAME_OVER_LIMIT_Y		130
#define LEVEL_OFFSET			10

// UART configuration
#define BAUD	9600					// serial communication baud rate
#define UBRR_VALUE F_CPU/16/BAUD-1

/* ONLY FOR DEBUGGING!!! *
#include "USART/USART_implement_me.h"
//#include <stdlib.h>
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
/* END OF DEBUGGING */




/*	GLOBAL VARIABLES	*/
static uint8_t SpaceshipPos[2] = {TFT_WIDTH/2,TFT_HEIGHT-20};
Alien aliens[NUMBER_OF_ALIENS];
Shoot shootplayer;
volatile uint8_t lives = LIVES;
volatile uint8_t currentLevel = 1;
volatile int scoreboard = 0;
volatile int hi_score = (int)eeprom_read_word((uint16_t *)0);
volatile int aliveAliens = NUMBER_OF_ALIENS;
volatile uint16_t coun = 0;
volatile uint16_t alienspeed = STARTING_ALIEN_SPEED;
//volatile int 

// declared in sound.h!
extern volatile unsigned char *cursor;
extern volatile uint16_t currentLength;
extern volatile uint16_t soundCounter;

void initLevel(uint8_t level);
/* END OF DEFINITION OF GLOBAL VARIABLES */



void createAliens(uint8_t level){
	uint8_t type = 0;
	int counter = NUMBER_OF_ALIENS-1;
	for(int y=-2;y<3;y++){
		for(int x=-3;x<2;x++){
			//if (type == 2) aliens[counter--].initAlien(type, TFT_WIDTH/3+x*ALIEN_X_SEPARATION+POS_OFFSET, TFT_HEIGHT/3+y*ALIEN_Y_SEPARATION, ST7735_GREEN);
			//else 
			aliens[counter--].initAlien(type, TFT_WIDTH/3+x*ALIEN_X_SEPARATION+POS_OFFSET, TFT_HEIGHT/3+y*ALIEN_Y_SEPARATION+LEVEL_OFFSET*level, ST7735_WHITE);
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
/*
void dac_i2c(uint16_t sample) {
	uint8_t i2c_msg[4];
	i2c_msg[0] = MCP4725_WRITE;
	i2c_msg[1] = (0x01<<6);
	i2c_msg[2] = (uint8_t) ((sample & 0xFF0) >> 4);
	i2c_msg[3] = (uint8_t) ((sample & 0xF) << 4);
	//i2c_transmit(i2c_msg, 4, 1);
	TWI_Start_Transceiver_With_Data(i2c_msg, 4);
}
*/
void dac_i2c(uint16_t sample) {
	i2cSendStart();							// send start condition
	i2cWaitForComplete();
	
	i2cSendByte(MCP4725_WRITE); 			// send WRITE address of TMP102
	i2cWaitForComplete();
	i2cSendByte((0x01<<6)); 				// set TMP102 pointer register to 0 (read temperature)
	i2cWaitForComplete();
	i2cSendByte((sample & 0xFF0) >> 4); 	// upper data bits
	i2cWaitForComplete();
	i2cSendByte((sample & 0xF) << 4); 		// lower data bits
	i2cWaitForComplete();
	i2cSendStop();							// send stop condition
	TWCR = 0;								// stop everything
}

bool checkGameOver(){
	for(uint8_t i=0; i<NUMBER_OF_ALIENS; i++){
		if(aliens[i].initialized && !aliens[i].destroyed && !aliens[i].targeted){
			/* DEBUG SECTION *
			char t_str[30];
			sprintf(t_str, "alienY %d\n", aliens[i].getY());
			USART_Transmit_String(t_str);*/
			/* END DEBUG */
			if(aliens[i].getY()>=GAME_OVER_LIMIT_Y){
				return true;
			}
		}
	}
	return false;
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
/*
void errorHalt(char* msg) {
	USART_Transmit_String("Error: ");
	USART_Transmit_String(msg);
	USART_Transmit_String("\r\n");
	while(1);
}
*/
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

void saveHighScore(){
	if(scoreboard > hi_score){
		eeprom_update_word((uint16_t*) 0,scoreboard);
	}
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
			sprintf(t_str, "COMP IF: %d<=%d<=%d\n", coords[0]-KILLING_RANGE_X, aliens[i].getX(),coords[0]+KILLING_RANGE_X);
			USART_Transmit_String(t_str);
			sprintf(t_str, "COMP IF: %d<=%d<=%d\n", coords[1]-KILLING_RANGE_Y, aliens[i].getY(),coords[1]+KILLING_RANGE_Y);
			USART_Transmit_String(t_str);
			USART_Transmit_String("--------------------------\n");*/
			/* END DEBUG */

			if(aliens[i].getX()<=coords[0]+KILLING_RANGE_X && aliens[i].getX()>=coords[0]-KILLING_RANGE_X){
				if(aliens[i].getY()<=coords[1]+KILLING_RANGE_Y && aliens[i].getY()>=coords[1]-KILLING_RANGE_Y){
					aliens[i].destroyedAlien();
					shootplayer.setTargetReached();
					update_scoreboard(aliens[i].getType());
					playSound('k');
					alienspeed -= 2;
					if(aliveAliens <= 0){
						initLevel(++currentLevel);
					}
					//if(alienspeed<=20) alienspeed = 20;
					/* DEBUG SECTION */
					/*char t_str[30];
					sprintf(t_str, "alienspeed %d\n", alienspeed);
					USART_Transmit_String(t_str);*/
					/* END DEBUG */
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


void initLevel(uint8_t level){
	fillScreen(ST7735_BLACK);
	//splashDrawer();
	drawSpaceship(SpaceshipPos[0], SpaceshipPos[1],ST7735_WHITE);
	drawScore(0,0);
	init_hi_score();
	drawFastHLine(0,148,TFT_WIDTH,ST7735_GREEN);
	push_score(0,8,scoreboard);
	for(int i=0; i<LIVES; i++) {
		drawSpaceship(15+14*i,TFT_HEIGHT-6, ST7735_GREEN);	
	}
	drawNumber(2,TFT_HEIGHT-10,lives);
	stop_timer1();
	stopSound_TIMER0();
	stopSound_TIMER2();
	alienspeed = STARTING_ALIEN_SPEED;
	aliveAliens = NUMBER_OF_ALIENS;
	createAliens(level-1);
	_delay_ms(400);
	Timer_IO_Init();
	//stop_timer1();
	Timer_Sound0_Init();
	Timer_Sound2_Init();
}

/** The main function **/
int main(void) {
	FATFS fs;     			/* File system structure */
	uint8_t buf[24];		/* Buffer for card access */
	static UINT nr;			/* Used for various file access functions. */
	//eeprom_update_word(( uint16_t*) 0, 0);	//write code
	USART_Init(BAUD);
	//i2c_master_init();
	//TWI_Master_Initialise();

	I2C_Init();
	SPI_Master_Init();
	ST7735_init();
	if (pf_mount(&fs)) //errorHalt("pf_mount");
	//if (pf_open("test.bmp")) errorHalt("pf_open");
	if (pf_open("test.bmp")) //errorHalt("pf_open");
	if (pf_lseek(54)) //errorHalt("pf_lseek");
	for(int z=0; z<TFT_HEIGHT; z++) {
		for(int j=0; j<16; j++) {
			//for(int i=0; i<1; i++) {
				if (pf_read(buf, sizeof(buf), &nr)) //errorHalt("pf_read");
				//for(uint8_t z=0; z<nr; z++) {
					//USART_Transmit_char(buf[z]);
				//}
			//}
			for(int n=0; n<8;n++) {
				drawPixelRGB(n+j*8, TFT_HEIGHT-1-z, buf[2+3*n], buf[1+3*n], buf[0+3*n]);
			}
		}
	}
	initLevel(currentLevel);
	sei();		
	
	while(1);

}

ISR(TIMER1_COMPA_vect) {
	if(!(PINC & (1<<PINC0))) {
		//stop_timer1();
	if(SpaceshipPos[0]>PLAYER_LIMIT_X_LEFT){
			SpaceshipPos[0] -= 2;
			drawSpaceship(SpaceshipPos[0], SpaceshipPos[1],ST7735_WHITE);}
	}
	else if (!(PINC & (1<<PINC1))) {
		//stop_timer1();
		if(SpaceshipPos[0]<PLAYER_LIMIT_X_RIGHT){
			SpaceshipPos[0] += 2;
			drawSpaceship(SpaceshipPos[0], SpaceshipPos[1],ST7735_WHITE);}
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
	/* DEBUG SECTION */
	/*char t_str[30];
	sprintf(t_str, "ALIENSPEED %d\n", alienspeed);
	USART_Transmit_String(t_str);
	USART_Transmit_String("--------------------------\n"); */
	/* END DEBUG */
	if(coun>=alienspeed){
		coun = 0;
		moveAliens();
		if(checkGameOver()){
			fillScreen(ST7735_RED);
			saveHighScore();
			stop_timer1();
			stopSound_TIMER0();
			stopSound_TIMER2();}
	}
}

ISR(TIMER0_COMPA_vect){
	PORTB ^= (1<<PB0);
	/* DEBUG SECTION */
	/*char t_str[30];
	sprintf(t_str, "soundcounter %d\n", soundCounter);
	USART_Transmit_String(t_str); */
	/* END DEBUG */
	//PORTD = *(cursor_TIMER0+soundCounter_TIMER0);
	//uint16_t data_sound = *(cursor_TIMER0+soundCounter_TIMER0);
	uint16_t data_sound = cursor_TIMER0[soundCounter_TIMER0];
	data_sound = (data_sound<<4); 								//dac 12 bits
	soundCounter_TIMER0++;
	dac_i2c(data_sound);
	if(soundCounter_TIMER0 >= currentLength_TIMER0){ 
		stopSound_TIMER0();
		dac_i2c(0x0000);
		//_delay_ms(1000);
		//PORTD = 0;
	}
}

ISR(TIMER2_COMPA_vect){
	/* DEBUG SECTION  */
	/*char t_str[30];
	sprintf(t_str, "soundcounter %d\n", soundCounter);
	USART_Transmit_String(t_str);*/
	/* END DEBUG */
	//PORTD = *(cursor_TIMER2+soundCounter_TIMER2);
	uint16_t data_sound = *(cursor_TIMER2+soundCounter_TIMER2);
	data_sound = (data_sound<<4); 								//dac 12 bits
	soundCounter_TIMER2++;
	dac_i2c(data_sound);
	if(soundCounter_TIMER2 >= currentLength_TIMER2){ 
		stopSound_TIMER2();
		dac_i2c(0x0000);
	}
}