
#include "castimer/castimer.h"
#include "common/inc/global.h"
#include "FreeRTOS.h"
#include "avr/interrupt.h"
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "FreeRTOS_def_decl.h"
#include <util/delay.h>
#include "libs/povezan_seznam/povezan_seznam.h"



Vozlisce <class_TIMER*> timer_list;


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
	this->timer_enabled = false;
	this->timer_value = 0;
}


void class_TIMER::increment(){
	if (this->timer_enabled && timer_value < 65535) //Prevent overflow
		this->timer_value += 1;
}

class_TIMER::class_TIMER()
{
	timer_list.dodaj_konec(this);
}

/************************************************************************/
/*					 TIMER FRAMEWORK FUNCTIONS                          */
/************************************************************************/


void increment_timers(){									
	for (uint8_t i = 0, length = timer_list.length(); i < length ;i++)
	{
		timer_list[i]->increment();
	}
}


