
#include "castimer/castimer.h"
#include "common/inc/global.h"
#include "FreeRTOS.h"
#include "avr/interrupt.h"
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "FreeRTOS_def_decl.h"
#include <util/delay.h>


class_TIMER *timer_list[15];
uint8_t el_count = 0;

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

class_TIMER::class_TIMER()
{
	timer_list[el_count++] =  this;								//Add timer to list and increase length
}


void class_TIMER::increment(){
	if (this->timer_enabled && timer_value < 65535) //Prevent overflow
		this->timer_value += 1;
}


/************************************************************************/
/*					 TIMER FRAMEWORK FUNCTIONS                          */
/************************************************************************/


void increment_timers(){									
	for (class_TIMER *element : timer_list)
	{
		element->increment();
	}
}


