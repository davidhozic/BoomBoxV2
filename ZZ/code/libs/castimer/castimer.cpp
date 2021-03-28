

#include "castimer/castimer.h"
#include "common/inc/global.h"
#include "FreeRTOS.h"
#include "avr/interrupt.h"

class_TIMER_framework timer_control;										// Control object for all the timers


/************************************************************************/
/*						TIMER OBJECT FUNCTIONS                          */
/************************************************************************/
unsigned long class_TIMER::vrednost()
{
	timer_enabled = true;
	return timer_value;
}

void class_TIMER::ponastavi()
{
	timer_enabled = false;
	timer_value = 0;
}


class_TIMER::class_TIMER(){
	timer_control.add_timer_to_list(this);	
}

void class_TIMER::increment(){
	this->timer_value += 1;
}


/************************************************************************/
/*					 TIMER FRAMEWORK FUNCTIONS                          */
/************************************************************************/


void class_TIMER_framework::add_timer_to_list(class_TIMER* timer_to_add){
	timer_list[timer_num++] = timer_to_add;									//Add timer to list and increase length
}

void class_TIMER_framework::increment_timers(){									
	for (uint8_t index = 0; timer_list[index] != NULL; index++){
		timer_list[index]->increment();										// Increments timer value by one
	}
}

class_TIMER_framework::class_TIMER_framework(){
	for (class_TIMER* &element : class_TIMER_framework::timer_list){		//Setups the list to be NULL
		element = NULL;		
	}
}


