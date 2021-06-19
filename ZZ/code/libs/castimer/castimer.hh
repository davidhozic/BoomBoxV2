
#ifndef CASTIMER_H
#define CASTIMER_H
#include "settings.hh"

#if (	SOURCE_INTERUPT == 1	)	
	#ifndef SEZNAM_INC
	#include "linked_list.hh"
	#endif	
	#include <avr/interrupt.h>
#endif

class TIMER_t
{
private:
	bool timer_enabled = false;
	uint32_t timer_value = 0;

	/* Hook Function */
	void (*function_ptr)(void*) = NULL;
	uint32_t function_call_period = 0;
	void *function_parameter = NULL;
		
public:
    uint32_t value();
    void reset();
	void increment();
	#if (SOURCE_INTERUPT == 1)
	void set_hook(void (*function_ptr)(void*), uint32_t call_period, void* function_param_ptr);
		TIMER_t();
		static LIST_t <TIMER_t *> timer_list;
	#endif
};




#endif






