/*	Author: anguy589
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//timer
volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1; //start count, down to 0. default 1 ms
unsigned long _avr_timer_cntcurr = 0;// current interal count of 1 ms ticks


void TimerOn() {

	TCCR1B = 0x0B; //AVR timer/counter controller register
	OCR1A = 125; // AVR output compare register
	TIMSK1 = 0x02; // AVR timer interrupt mask register

	TCNT1 = 0; //initialize AVR counter

         //TimerISR will be called every _avr_timer_cntcurr milliseconds
	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;//enable global interrupts
}

void TimerOff() {
	TCCR1B = 0x00; //timer off
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;

	}
}

//set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

//state machines
unsigned char threeLEDs = 0x00;
unsigned char blinkingLED = 0x00;
unsigned char tempB = 0x00;

enum sm1{sm1start, light1, light2, light3} state1; //ThreeLEDsSM
enum sm2{sm2start, init, lighton} state2; //BlinkingLEDSM
enum sm3{sm3start, output} state3; //combineLEDsSM

void sm1tick() {
	switch(state1) {
		case sm1start:
			state1 = light1;
			break;
		case light1:
			state1 = light2;
			break;
		case light2:
			state1 = light3;
			break;
		case light3:
			state1 = light1;
			break;
		default:
			break;
	}

	switch(state1) {
		case sm1start:
			break;
		case light1:
			threeLEDs = 0x01;
			break;
		case light2:
			threeLEDs = 0x02;
			break;
		case light3:
			threeLEDs = 0x04;
			break;
		default:
			break;
	
	
	}
	
}

void sm2tick() {
	switch(state2) {
		case sm2start:
			state2 = init;
			break;
		case init:
			state2 = lighton;
			break;
		case lighton:
			state2 = init;
			break;
		default:
			break;
	
	}

	switch(state2) {
		case sm2start:
			break;
		case init:
			blinkingLED = 0x00;
			break;
		case lighton:
			blinkingLED = 0x08;
			break;
		default:
			break;
	
	}

}

void sm3tick() {
	switch(state3) {
		case sm3start:
			state3 = output;
			break;
		case output:
			state3 = output;
			break;
		default:
			break;
	
	}
	
	switch(state3) {
		case sm3start:
			tempB = 0x00;
			PORTB = tempB;
			break;
		case output:
			tempB =  blinkingLED | threeLEDs;
			PORTB = tempB;
			break;
		default:
			break;
	}



}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(1000);
	TimerOn();

	state1 = sm1start;
	state2 = sm2start;
	state3 = sm3start;
    /* Insert your solution below */
    while (1) {
	    sm1tick();
	    sm2tick();
	    sm3tick();
	
	    while(!TimerFlag);
	    TimerFlag = 0;
    }
    return 1;
}
