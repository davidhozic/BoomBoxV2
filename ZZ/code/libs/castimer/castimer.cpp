


#include "./castimer.hh"
#include <math.h>
#if (SOURCE_INTERUPT == 1)
	#include "util/atomic.h"
#endif

/************************************************************************/
/*							ERRORS AND WARNINGS                         */
/************************************************************************/
#if (	SOURCE_INTERUPT == 1	)
#pragma message "Timer library will use one of the hardware timers' interrupt as a source. Hardware timer must be configured manually to trigger interrupt every TIMER_INCREMENT_VALUE_MS"
#ifndef TIMER_ISR_VECTOR
#error TIMER_ISR_VECTOR "is not defined!"
#endif

#elif ( SOURCE_SYSTEM_TIME == 1 )
#ifndef SYSTEM_TIME_FUNCTION
#error "SYSTEM_TIME_FUNCTION is not defined!"
#endif
#pragma message "Timer library will use system time as a source."
#endif
/************************************************************************/

/* Initialization of timer list */
#if (SOURCE_INTERUPT == 1)
	Vozlisce_t <class_TIMER*> class_TIMER::timer_list;
#endif



unsigned long class_TIMER::vrednost()
{
#if (SOURCE_INTERUPT == 1)
	timer_enabled = true;				
	unsigned short temp_timer_value;
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		temp_timer_value = timer_value;		
	}
		
	return temp_timer_value; 

#elif (SOURCE_SYSTEM_TIME == 1)					// Use System time as source for the timer

	if (timer_enabled == false)
	{
		timer_enabled = true;
		timer_value = SYSTEM_TIME_FUNCTION;	 // Start value
		return 0;
	}

	return	SYSTEM_TIME_FUNCTION - timer_value;		
#endif
}

void class_TIMER::ponastavi()
{
#if (SOURCE_INTERUPT == 1)
	this->timer_enabled = false;
	this->timer_value = 0;

#elif (SOURCE_SYSTEM_TIME == 1)
	timer_enabled = false;
#endif
}

#if (SOURCE_INTERUPT == 1)

	void class_TIMER::increment()
	{
		if (timer_enabled)
		{
			#ifndef DEBUG
			timer_value += TIMER_INCREMENT_VALUE_MS;
			#else
			timer_value += 250;
			#endif
		}
	}


	class_TIMER::class_TIMER()
	{
		ATOMIC_BLOCK(ATOMIC_FORCEON)
		{
			class_TIMER::timer_list.dodaj_konec(this);
		}
	}

	ISR(TIMER_ISR_VECTOR)
	{
		for (uint16_t ind = 0, len = class_TIMER::timer_list.length() ; ind < len ; ind++)
		{
			class_TIMER::timer_list[ind]->increment();
		}
	}
	
	

#endif