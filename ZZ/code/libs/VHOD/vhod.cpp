#include "VHOD/Vhod.h"
#include "bit_manipulation.h"
#include "avr/io.h"

bool class_VHOD::vrednost()
{
	if (port < 'H')
	{
		unfiltered_curr_state =	readBIT( *((unsigned char*)&PINA + 3*(port-'A'))   , pin); // Write bit in status register read from PINA + (port - 'A')* 3 (start at port A and then move forward thru addresses to get to other PINs) (0x20 = PINA)	
	}
	else if (port == 'H')
	{
		unfiltered_curr_state = readBIT(PINH, pin);
	}
	else
	{
		unfiltered_curr_state =	readBIT( *((unsigned char*)&PINJ + 3*(port-'J'))   , pin); 	
	}
	// END READING OF PIN
	/************************/
	
	/*		Invert output if unpressed input state is 1		*/
	if (default_state)
	{
		unfiltered_curr_state = !unfiltered_curr_state;
	}	
	
#if (USE_FILTERING == 1)
	/* Filter input with a timer */
	if (unfiltered_curr_state != filtered_curr_state
		&& filter_timer.vrednost() >= FILTER_TIME_MS)
	{
		filtered_curr_state = unfiltered_curr_state;
	}
	else if(unfiltered_curr_state == filtered_curr_state)
	{
		filter_timer.ponastavi();
	}
#else
	filtered_curr_state = unfiltered_curr_state;
#endif
	/* END OF FILTERING*/
	
	/* Edge detection */
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
	
	/* Clear edges if input state doesn't match */
	if (!filtered_curr_state)
	{
		rising_edge = 0;
	}
	else if(filtered_curr_state){
		falling_edge = 0;
	}
	

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


