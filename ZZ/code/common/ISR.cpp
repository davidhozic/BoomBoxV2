#include <avr/interrupt.h>
#include "castimer/castimer.h"

/************************************************************************/
/*								PROTOTYPES                              */
/************************************************************************/
void bujenje();

/************************************************************************/
/*						INTERRUPT SERVICE ROUTINES		                */
/************************************************************************/

ISR (TIMER3_COMPA_vect){										// TIMER ISR
	timer_control.increment_timers();
}


ISR(PCINT2_vect)												// SLEEP ISR
{
	bujenje();
}