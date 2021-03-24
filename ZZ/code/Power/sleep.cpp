#include <avr/sleep.h>
#include "avr/common.h"
#include "avr/interrupt.h"
#include "settings.h"
#include "util/delay.h"

void Shutdown();

void bujenje()
{ //Avtomaticno rising edge, saj se interrupt vklopi sele ob spanju
	sleep_disable();
	PCICR = 0;
	PCIFR = 0;
	PCMSK2 = 0;
	_delay_ms(200);
}

void spanje()
{	
	Shutdown();
	asm("sei");					 //vklop zunanjih interruptov
	PCICR = (1 << PCIE2);	 // Vklopi PCINT interrupt
	PCIFR = (1 << PCIF2);	 // Vlopi ISR
	PCMSK2 = (1 << PCINT17); //Vklopi vector na 17
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_cpu();
}

ISR(PCINT2_vect)
{
	bujenje();
}
