
#ifndef CASTIMER_H
#define CASTIMER_H

/*********************************************	SETTINGS	*************************************************/
#define SOURCE_INTERUPT								( 1 )			// Use ISR as timing source
	#define TIMER_ISR_VECTOR						( TIMER3_COMPA_vect )	
	#define TIMER_INCREMENT_VALUE_MS				( 1 )

#define SOURCE_SYSTEM_TIME							( 0 )			// Use System time as source
#if (SOURCE_SYSTEM_TIME == 1)
	#include "path_to_system_time_function_prototype"	
	#define SYSTEM_TIME_FUNCTION				   	(millis())
#endif
/************************************************************************************************************/


#if (	SOURCE_INTERUPT == 1	)	
	#ifndef SEZNAM_INC
	#include "povezan_seznam.h"
	#endif	
	#include <avr/interrupt.h>
#endif



class class_TIMER
{
private:
	bool timer_enabled = false;
	unsigned long timer_value = 0;
	
public:
    unsigned long vrednost();
    void ponastavi();
	void increment();
	#if (SOURCE_INTERUPT == 1)
		class_TIMER();
		static Vozlisce_t <class_TIMER*> timer_list;
	#endif
};





#endif






