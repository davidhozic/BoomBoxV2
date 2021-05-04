#include <avr/interrupt.h>
#include "FreeRTOS.h"
#include "task.h"
#include <avr/wdt.h>
#include "common/inc/global.h"
#include <util/delay.h>
#include "castimer/castimer.h"
#include "libs/VHOD/Vhod.h"
/************************************************************************/
/*								PROTOTYPES                              */
/************************************************************************/
void bujenje();
void Shutdown();


/************************************************************************/
/*						 HARDWARE RELATED FUNCTIONS		                */
/************************************************************************/

void increment_timers(Vozlisce <class_TIMER*> &timers)
{
	for (uint8_t i = 0, length = timers.length(); i < length ;i++)
	{
		timers[i]->increment();
	}
}

/************************************************************************/
/*						INTERRUPT SERVICE ROUTINES		                */
/************************************************************************/






ISR (TIMER3_COMPA_vect){										// TIMER ISR
	increment_timers(Hardware.timer_list);
}


ISR(PCINT2_vect)												// SLEEP ISR
{
	bujenje();
}



