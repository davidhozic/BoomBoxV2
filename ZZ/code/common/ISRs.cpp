
#include <avr/interrupt.h>
#include "global.h"
#include "libs/castimer/inc/castimer.h"

void bujenje();
void cas_timer_increment();

ISR (TIMER3_COMPA_vect){
	Hardware.sys_time +=1;					// Belezi cas od zagona krmilnika
	cas_timer_increment();
}


ISR(PCINT2_vect)
{
	bujenje();
}