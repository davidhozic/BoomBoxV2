


#include "castimer.hpp"
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
	LIST_t <TIMER_t *> TIMER_t::timer_list;
#endif

uint32_t TIMER_t::value()
{
#if (SOURCE_INTERUPT == 1)

	if (!initialized)	/* Must initialize post FreeRTOS start due to memory issues */
	{
		init();
	}

	timer_enabled = true;				
	uint32_t temp_timer_value;
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

void TIMER_t::reset()
{
#if (SOURCE_INTERUPT == 1)
	this->timer_value = 0;
#endif
	this->timer_enabled = false;
}

#if (SOURCE_INTERUPT == 1)

void TIMER_t::init()
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		TIMER_t::timer_list += this;
		initialized = true;
	}
}

#endif


#if (SOURCE_INTERUPT == 1)

	void TIMER_t::increment()
	{
		if (timer_enabled)
		{
			timer_value += TIMER_INCREMENT_VALUE_MS;
			if (function_ptr != NULL && timer_value >= function_call_period)
			{
				timer_value = 0;
				function_ptr(function_parameter);
			}
		}
	}

	ISR(TIMER_ISR_VECTOR)
	{
		for (uint16_t ind = 0, len = TIMER_t::timer_list.length() ; ind < len ; ind++)
		{
			TIMER_t::timer_list[ind]->increment();
		}
	}
	
	void TIMER_t::set_hook(void (*function_ptr)(void*), uint32_t call_period ,void* function_param_ptr)
	{
		this->function_ptr = function_ptr;
		this->function_call_period = call_period;
		this->function_parameter = function_param_ptr;
		this->timer_enabled = 1;
	}
	
#endif