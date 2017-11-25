#include <avr/pgmspace.h>
#include "explosion.h"
#include "shoot.h"
#include "invaderkilled.h"
#include "tone_inv0.h"
#include "tone_inv1.h"
#include "tone_inv2.h"
#include "tone_inv3.h"

volatile unsigned char *cursor_TIMER0;
volatile uint16_t currentLength_TIMER0;
volatile uint16_t soundCounter_TIMER0;
volatile unsigned char *cursor_TIMER2;
volatile uint16_t currentLength_TIMER2;
volatile uint16_t soundCounter_TIMER2;

void stopSound_TIMER0(){
	TCCR0B &= ~(1 << CS01);
	TCNT0 = 0;
}

void startSound_TIMER0(){
	TCCR0B |= (1 << CS01);
	TCNT0 = 0;
}

void stopSound_TIMER2(){
	TCCR2B &= ~(1 << CS01);
	TCNT2 = 0;
}

void startSound_TIMER2(){
	TCCR2B |= (1 << CS01);
	TCNT2 = 0;
}

void playSound(char opt){
	switch(opt){
		// timer 0 sounds
		case 's':
			cursor_TIMER0 = (unsigned char*)shoot_data;
			currentLength_TIMER0 = shoot_length;
			soundCounter_TIMER0 = 0;
			startSound_TIMER0();
			break;
		case 'k':
			cursor_TIMER0 = (unsigned char*)invaderkilled_data;
			currentLength_TIMER0 = invaderkilled_length;
			soundCounter_TIMER0 = 0;
			startSound_TIMER0();
			break;
		case 'e':
			cursor_TIMER0 = (unsigned char*)explosion_data;
			currentLength_TIMER0 = explosion_length;
			soundCounter_TIMER0 = 0;
			startSound_TIMER0();
			break;
		// timer 2 sounds
		case 0:
			cursor_TIMER2 = (unsigned char*)tone_inv0_data;
			currentLength_TIMER2 = tone_inv0_length;
			soundCounter_TIMER2 = 0;
			startSound_TIMER2();
			break;
		case 1:
			cursor_TIMER2 = (unsigned char*)tone_inv1_data;
			currentLength_TIMER2 = tone_inv1_length;
			soundCounter_TIMER2 = 0;
			startSound_TIMER2();
			break;
		case 2:
			cursor_TIMER2 = (unsigned char*)tone_inv2_data;
			currentLength_TIMER2 = tone_inv2_length;
			soundCounter_TIMER2 = 0;
			startSound_TIMER2();
			break;
		case 3:
			cursor_TIMER2 = (unsigned char*)tone_inv3_data;
			currentLength_TIMER2 = tone_inv3_length;
			soundCounter_TIMER2 = 0;
			startSound_TIMER2();
			break;

	}
}