
#ifndef CASTIMER_H
#define CASTIMER_H
#include "timer_settings.hpp"

#if (	SOURCE_INTERUPT == 1	)	
	#ifndef SEZNAM_INC
	#include "linked_list.hpp"
	#endif	
	#include <avr/interrupt.h>
#endif

class TIMER_t
{
private:
	volatile bool timer_enabled = false;
	volatile uint32_t timer_value = 0;

	/* Hook Function */
	void (*function_ptr)(void*) = NULL;
	uint32_t function_call_period = 0;
	void *function_parameter = NULL;

	#if (SOURCE_INTERUPT)
		bool initialized = false;
	#endif

public:
    uint32_t value();
    void reset();
	void increment();
	#if (SOURCE_INTERUPT == 1)
		void set_hook(void (*function_ptr)(void*), uint32_t call_period, void* function_param_ptr);
		void init();
		static LIST_t <TIMER_t *> timer_list;
	#endif
};



#endif






