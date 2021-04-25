#include <avr/interrupt.h>
#include "castimer/castimer.h"
#include "FreeRTOS.h"
#include "task.h"
#include <avr/wdt.h>

#include "common/inc/global.h"
/************************************************************************/
/*								PROTOTYPES                              */
/************************************************************************/
void bujenje();
void Shutdown();


/************************************************************************/
/*						INTERRUPT SERVICE ROUTINES		                */
/************************************************************************/

ISR (TIMER3_COMPA_vect){										// TIMER ISR
	increment_timers();
}


ISR(PCINT2_vect)												// SLEEP ISR
{
	bujenje();
}


ISR(WDT_vect)
{
	cli();
	wdt_disable();
	Shutdown();
	writeBIT(Hardware.error_reg, HARDWARE_ERROR_REG_WATCHDOG_FAIL, 1); // Watchdog got triggered.
	wdt_enable(WDTO_2S);
	sei();
}