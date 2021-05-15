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

/************************************************************************/
/*						INTERRUPT SERVICE ROUTINES		                */
/************************************************************************/
ISR(PCINT2_vect)												// SLEEP ISR
{
	bujenje();
}



