


#include "castimer.hpp"
#if  (TIMER_CFG_USE_INTERRUPT == 1)
	#include "util/atomic.h"
#elif (TIMER_CFG_USE_SYSTEM_TIME == 1)
    #include TIMER_CFG_SYSTEM_TIME_HEADER
#endif





/* Initialization of timer list */
#if (TIMER_CFG_USE_INTERRUPT == 1)
	LIST_t <TIMER_t *> TIMER_t::timer_list;
    #warning "LITL timer: ISR will be used to increment timer value! Hardware timer MUST be configured MANUALLY! ISR is already configured."
#endif

uint32_t TIMER_t::value()
{
#if (TIMER_CFG_USE_INTERRUPT == 1)

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

#elif (TIMER_CFG_USE_SYSTEM_TIME == 1)					// Use System time as source for the timer

	if (timer_enabled == false)
	{
		timer_enabled = true;
		timer_value = TIMER_CFG_SYSTEM_TIME_FUNCTION;	 // Start value
		return 0;
	}

	return	TIMER_CFG_SYSTEM_TIME_FUNCTION - timer_value;		
#endif
}

void TIMER_t::reset()
{
	this->timer_value = 0;
	this->timer_enabled = false;
}

#if (TIMER_CFG_USE_INTERRUPT == 1)

void TIMER_t::init()
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		TIMER_t::timer_list += this;
		initialized = true;
	}
}

void TIMER_t::increment()
{
    if (timer_enabled)
    {
        timer_value += TIMER_CFG_INCREMENT_VALUE;
        if (function_ptr != nullptr && timer_value >= function_call_period)
        {
            timer_value = 0;
            function_ptr(function_parameter);
        }
    }
}

ISR(TIMER_CFG_ISR_VECTOR)
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