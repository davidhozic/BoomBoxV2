#include <avr/interrupt.h>
#include "castimer/castimer.h"
#include "FreeRTOS.h"
#include "task.h"
/************************************************************************/
/*								PROTOTYPES                              */
/************************************************************************/
void bujenje();

/************************************************************************/
/*						INTERRUPT SERVICE ROUTINES		                */
/************************************************************************/

ISR (TIMER3_COMPA_vect){										// TIMER ISR
	taskENTER_CRITICAL();
	increment_timers();
	taskEXIT_CRITICAL();
}


ISR(PCINT2_vect)												// SLEEP ISR
{
	bujenje();
}