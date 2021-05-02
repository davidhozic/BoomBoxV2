

#include "common/inc/global.h"
#include "FreeRTOS.h"
#include "avr/interrupt.h"
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "FreeRTOS_def_decl.h"
#include <util/delay.h>
#include "castimer/castimer.h"
#include "libs/povezan_seznam/povezan_seznam.h"
#include <util/atomic.h>


/************************************************************************/
/*						TIMER OBJECT FUNCTIONS                          */
/************************************************************************/

unsigned short class_TIMER::vrednost()
{
	timer_enabled = true;
	unsigned short temp_timer_value;
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		temp_timer_value = timer_value;	// 16-bit variable gets incremented during interrupt -> this disables interrupts temporarily
	}
	return temp_timer_value; 
	
}

void class_TIMER::ponastavi()
{
	this->timer_enabled = false;
	this->timer_value = 0;
}


void class_TIMER::increment()
{
	if (timer_enabled && timer_value < 65535) //Prevent overflow
	{
		#ifndef DEBUG
		timer_value += 1;
		#else
		timer_value += 250;
		#endif
	}
}

class_TIMER::class_TIMER(Vozlisce <class_TIMER*> &timer_list)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		timer_list.dodaj_konec(this);
	}
}




