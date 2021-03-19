/*
* CFile1.c
*
* Created: 11/03/2021 11:19:37
*  Author: D
*/
#include "avr/io.h"
#include "FreeRTOS.h"
#include "avr/interrupt.h"

#include "EEPROM.h"

void EEPROM_t::pisi(uint8_t podatek, uint16_t naslov){
	cli();
	while(EECR & (1<<EEPE));			// Cakaj da se prejsnje branje/pisanje zakljuci
	EEAR = naslov;					//Izberi index bajta na eepromu
	EEDR = podatek;					//Podatek na zacasen bajt
	EECR |= (1<<EEMPE);				//Vklopi interrupt
	EECR |= (1<<EEPE);				//Pisi
	
	sei();
}

uint8_t EEPROM_t::beri (uint16_t naslov){
	while(EECR & (1<<EEPE));			// Cakaj da se prejsnje branje/pisanje zakljuci
		cli();
	EEAR = naslov;					//Izberi index bajta na eepromu
	EECR |= (1<<EERE);				//Beri
	return EEDR;					//Vrni vrednost
	sei();
}





