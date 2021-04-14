
#include "castimer/castimer.h"
#include "common/inc/global.h"
#include "FreeRTOS.h"
#include "avr/interrupt.h"
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "FreeRTOS_def_decl.h"
#include <vector>

std::vector <class_TIMER *> timer_list;

/************************************************************************/
/*						TIMER OBJECT FUNCTIONS                          */
/************************************************************************/
unsigned long class_TIMER::vrednost()
{
	this->timer_enabled = true;
	return this->timer_value;
}

void class_TIMER::ponastavi()
{
	this->timer_enabled = false;
	this->timer_value = 0;
}

class_TIMER::class_TIMER(){
		add_timer_to_list(this);
}


void class_TIMER::increment(){
	if (this->timer_enabled && timer_value < 65535) //Prevent overflow
		this->timer_value += 1;
}


/************************************************************************/
/*					 TIMER FRAMEWORK FUNCTIONS                          */
/************************************************************************/


void add_timer_to_list(class_TIMER* timer_to_add){
	timer_list.push_back(timer_to_add);										//Add timer to list and increase length
}

void increment_timers(){									
	for (class_TIMER* timer : timer_list){
		timer->increment();													// Increments timer value by one
	}
}


