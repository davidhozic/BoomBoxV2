


#include "./castimer.h"
#include "util/atomic.h"

/************************************************************************/
/*							ERRORS AND WARNINGS                         */
/************************************************************************/
#if (	SOURCE_INTERUPT == 1	)
#warning "Timer must be set manually to trigger interrupt every TIMER_INCREMENT_VALUE_MS"
#warning "Timer library will use interrupt as a source."
#ifndef TIMER_VECTOR
#error TIMER_VECTOR "is not defined!"
#endif

#elif ( SOURCE_SYSTEM_TIME == 1 )
#ifndef (SYSTEM_TIME_FUNCTION)
#error "SYSTEM_TIME_FUNCTION is not defined!"
#endif
#warning "Timer library will use system time as a source."
#endif
/************************************************************************/

/* Initialization of timer list */
Vozlisce_t <class_TIMER*> class_TIMER::timer_list;

unsigned short class_TIMER::vrednost()
{
#if (SOURCE_INTERUPT == 1)
	timer_enabled = true;
	unsigned short temp_timer_value;
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		temp_timer_value = timer_value;			// 16-bit variable gets incremented during interrupt -> this disables interrupts temporarily
	}
	return temp_timer_value; 

#elif (SOURCE_SYSTEM_TIME == 1)

	if (timer_enabled == false)
	{
		timer_enabled = true;
		timer_value = SYSTEM_TIME_FUNCTION;	 // Old Timer Value
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
		if (timer_enabled && timer_value < 65535) //Prevent overflow
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
		asm("cli");
		class_TIMER::timer_list.dodaj_konec(this);
		asm("sei");
	}
		
	ISR(TIMER_VECTOR)
	{

		for (uint16_t ind = 0, len = class_TIMER::timer_list.length(); ind < len ; ind++)
		{
			class_TIMER::timer_list[ind]->increment();
		}
	}
	
	

#endif