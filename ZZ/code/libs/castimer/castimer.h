
#ifndef CASTIMER_H
#define CASTIMER_H

/********************	SETTINGS	************************/
#define SOURCE_INTERUPT					( 1 )
#define TIMER_VECTOR					( TIMER3_COMPA_vect )
#define TIMER_INCREMENT_VALUE_MS		( 1 )

#define SOURCE_SYSTEM_TIME				( 0 )
#define SYSTEM_TIME_FUNCTION			( millis() )
/***********************************************************/


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
	unsigned short timer_value = 0;
	
public:
    unsigned short vrednost();
    void ponastavi();
	void increment();
	#if (SOURCE_INTERUPT == 1)
		class_TIMER();
		static Vozlisce_t <class_TIMER*> timer_list;
	#endif
};





#endif






