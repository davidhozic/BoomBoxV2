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

void watchdog_off()
{
	MCUSR &= ~(1<<WDRF);
	WDTCSR = 0x00;
}

void watchdog_on()
{
	MCUSR &= ~(1<<WDRF);
	WDTCSR = 0x00;
}

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


ISR(WDT_vect)
{
	cli();
	Shutdown();
	watchdog_off();	
	writeBIT(Hardware.error_reg, HARDWARE_ERROR_REG_WATCHDOG_FAIL, 1); // Watchdog got triggered.
	watchdog_on();
	sei();
}