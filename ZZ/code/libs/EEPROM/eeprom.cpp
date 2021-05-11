
#include "avr/io.h"
#include "FreeRTOS.h"
#include "avr/interrupt.h"
#include "libs/EEPROM/EEPROM.h"
#include "common/inc/FreeRTOS_def_decl.h"

void EEPROM_t::pisi(uint8_t podatek, uint16_t naslov){
	if (EEPROM_t::beri(naslov) == podatek)
		return;
	while(EECR & (1<<EEPE));			// Cakaj da se prejsnje branje/pisanje zakljuci
	EEAR = naslov;					//Izberi index bajta na eepromu
	EEDR = podatek;					//Podatek na zacasen bajt
	EECR |= (1<<EEMPE);				//Vklopi interrupt
	EECR |= (1<<EEPE);				//Pisi
}

uint8_t EEPROM_t::beri (uint16_t naslov){
	while(EECR & (1<<EEPE)) delayFREERTOS(1);			// Cakaj da se prejsnje branje/pisanje zakljuci
	EEAR = naslov;										//Izberi index bajta na eepromu
	EECR |= (1<<EERE);									//Beri
	while(EECR & 1<<EERE);		
	return EEDR;										//Vrni vrednost
}
EEPROM_t EEPROM;



