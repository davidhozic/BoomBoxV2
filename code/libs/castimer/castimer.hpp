
#ifndef CASTIMER_H
#define CASTIMER_H
#include "timer_settings.hpp"

#if (	TIMER_CFG_USE_INTERRUPT == 1	)	
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
	void (*function_ptr)(void*) = nullptr;
	uint32_t function_call_period = 0;
	void *function_parameter = nullptr;

	#if (TIMER_CFG_USE_INTERRUPT)
		bool initialized = false;
        void init();
    #endif

public:
    uint32_t value();
    void reset();
	#if (TIMER_CFG_USE_INTERRUPT == 1)
        void increment();
		void set_hook(void (*function_ptr)(void*), uint32_t call_period, void* function_param_ptr);
		static LIST_t <TIMER_t *> timer_list;
        ~TIMER_t();
	#endif
};



#endif






