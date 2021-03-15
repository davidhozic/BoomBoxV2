#include <avr/sleep.h>
#include "avr/common.h"
#include "avr/interrupt.h"
#include "includes/settings.h"
#include "util/delay.h"

void bujenje()
{ //Avtomaticno rising edge, saj se interrupt vklopi sele ob spanju
	SMCR &= 0;
	PCICR = 0;
	PCIFR = 0;
	PCMSK2 = 0;
	_delay_ms(200);
}

void spanje()
{
	sei();					 //vklop zunanjih interruptov
	PCICR = (1 << PCIE2);	 // Vklopi PCINT interrupt
	PCIFR = (1 << PCIF2);	 // Vlopi ISR
	PCMSK2 = (1 << PCINT17); //Vklopi vector na 17
	SMCR = 0b010;
	SMCR |= 1;
}

ISR(PCINT2_vect)
{
	bujenje();
}
