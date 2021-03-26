
#include <avr/interrupt.h>
#include "global.h"

void bujenje();


ISR (TIMER3_COMPA_vect){
	Hardware.sys_time +=1;					// Belezi cas od zagona krmilnika
}


ISR(PCINT2_vect)
{
	bujenje();
}