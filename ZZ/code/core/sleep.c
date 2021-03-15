#include <avr/sleep.h>


void spanje(uint8_t PCINT){
	SMCR = 0b010;
	SMCR |= 1; 
	
}



void bujenje(uint8_t PCINT){
	SMCR &= ~(1);
	asm("sei");
}