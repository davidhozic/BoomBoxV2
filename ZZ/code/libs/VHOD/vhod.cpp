#include "VHOD/Vhod.h"
#include "avr/io.h"
#include "castimer/castimer.h"
#include "global.h"
#include <stdio.h>
#include "libs/povezan_seznam/povezan_seznam.h"
#include <util/atomic.h>

#define FILTER_TIME_MS				50

bool class_VHOD::vrednost()
{
	if (port < 'H')
		unfiltered_curr_state = readBIT( *((unsigned char*)&PINA + 3*(port-'A'))   , pin); // Write bit in status register read from PINA + (port - 'A')* 3 (start at port A and then move forward thru addresses to get to other PINs) (0x20 = PINA)	
	else if (port == 'H')
		unfiltered_curr_state =  readBIT( PINH, pin);
	else
		unfiltered_curr_state = readBIT( *((unsigned char*)&PINJ + 3*(port-'J'))   , pin); 	
	// END READING OF PIN
	/************************/
	
	/* Filter input with a timer */
	if (unfiltered_curr_state != filtered_curr_state && filter_timer.vrednost() >= FILTER_TIME_MS)
	{
		filtered_curr_state = unfiltered_curr_state;
	}
	else if(unfiltered_curr_state == filtered_curr_state)
	{
		filter_timer.ponastavi();
	}
	/* END OF FILTERING*/
	
	/* Filter input */
	if (prev_state != filtered_curr_state)
	{
		/* If state has changed and input is high -> rising edge*/
		if (filtered_curr_state)
		{
			rising_edge = 1;
			prev_state = filtered_curr_state;
		}
		else
		{
			falling_edge = 1;
			prev_state = filtered_curr_state;
		}
		
	}
	
	if (!filtered_curr_state)
	{
		rising_edge = 0;
	}
	else if(filtered_curr_state){
		falling_edge = 0;
	}
	
	/* Return inverted value if input is pull down to return logical 1 when pressed instead of 0*/
	if (default_state)
		return !filtered_curr_state;
	else	
		return filtered_curr_state;
}

bool class_VHOD::risingEdge()
{
	vrednost();
	if (rising_edge)
	{
		rising_edge = 0;			
		return true;
	}
	return false;
}

bool class_VHOD::fallingEdge()
{
	vrednost();
	if (falling_edge)
	{
		falling_edge = 0;
		return true;
	}
	return false;
}

class_VHOD::class_VHOD(unsigned char pin, char port, char default_state)
{
	this->port = port;
	this->pin = pin;
	this->default_state = default_state;
}


