
#include <avr/interrupt.h>
#include "global.h"
#include "castimer/castimer.h"
#include "FreeRTOS.h"
void bujenje();

class_TIMER test;
ISR (TIMER3_COMPA_vect){
	for (uint8_t i = 0 ; i < timer_num ; i++){
		timer_list[i]->increment();
	}
}


ISR(PCINT2_vect)
{
	bujenje();
}


