

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
	
	ATOMIC_BLOCK(ATOMIC_FORCEON)	// Prevents an interrupt from corrupting 16-bit variable (8-bit cpu)
	{
	#ifdef DEBUG
		return timer_value*500; 
	#else
		return timer_value;
	#endif
	}
}


void class_TIMER::ponastavi()
{
	this->timer_enabled = false;
	this->timer_value = 0;
}


void class_TIMER::increment()
{
	if (this->timer_enabled && timer_value < 65535) //Prevent overflow
		this->timer_value += 1;
}

class_TIMER::class_TIMER(Vozlisce <class_TIMER*> &timer_list)
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		timer_list.dodaj_konec(this);
	}
}




