#include <avr/interrupt.h>
#include "castimer/castimer.h"
#include "FreeRTOS.h"
#include "task.h"
#include <avr/wdt.h>
#include "common/inc/global.h"
#include <util/delay.h>

/************************************************************************/
/*								PROTOTYPES                              */
/************************************************************************/
void bujenje();
void Shutdown();


/************************************************************************/
/*						INTERRUPT SERVICE ROUTINES		                */
/************************************************************************/


void enable_watchdog(uint8_t setting)
{
#ifndef DEBUG
	asm("wdr");
	MCUSR &= ~(0x1 << WDRF); 
	WDTCSR = (1 << WDCE);
	WDTCSR |= (0x1 << WDIE) | setting;
#endif
}

void disable_watchdog()
{
	asm("wdr");
	MCUSR &= ~(0x1 << WDRF);
	WDTCSR = (0x1 << WDCE);
	WDTCSR = WDTCSR & ~(0x1 << WDIE) & ~(0x1 << WDE);
	WDTCSR &= ~(0x1 << WDCE);
}


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
	disable_watchdog();
	Shutdown();
#ifndef DEBUG
	_delay_ms(5);
#endif
	enable_watchdog(watchdog_time);
	sei();
}


