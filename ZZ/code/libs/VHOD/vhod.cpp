#include "Vhod.h"


bool class_VHOD::vrednost()
{

	switch (port)
	{
	#ifdef PINA
		case 'A':
			unfiltered_curr_state = readBIT(PINA, pin);
		break;
	#endif

	#ifdef PINB
		case 'B':
			unfiltered_curr_state = readBIT(PINB, pin);
		break;
	#endif

	#ifdef PINC
		case 'C':
			unfiltered_curr_state = readBIT(PINC, pin);
		break;
	#endif

	#ifdef PIND
		case 'D':
			unfiltered_curr_state = readBIT(PIND, pin);
		break;
	#endif

	#ifdef PINE
		case 'E':
			unfiltered_curr_state = readBIT(PINE, pin);
		break;
	#endif

	#ifdef PINF
		case 'F':
			unfiltered_curr_state = readBIT(PINF, pin);
		break;
	#endif

	#ifdef PING
		case 'G':
			unfiltered_curr_state = readBIT(PING, pin);
		break;
	#endif

	#ifdef PINH
		case 'H':
			unfiltered_curr_state = readBIT(PINH, pin);
		break;
	#endif

	#ifdef PINI
		case 'I':
			unfiltered_curr_state = readBIT(PINI, pin);
		break;
	#endif

	#ifdef PINJ
		case 'J':
			unfiltered_curr_state = readBIT(PINJ, pin);
		break;
	#endif


	#ifdef PINK
		case 'K':
			unfiltered_curr_state = readBIT(PINK, pin);
		break;
	#endif

	#ifdef PINL
		case 'L':
			unfiltered_curr_state = readBIT(PINL, pin);
		break;
	#endif

	#ifdef PINM
		case 'M':
			unfiltered_curr_state = readBIT(PINM, pin);
		break;
	#endif

		default:
		break;
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
	filtered_curr_state = 0;
	unfiltered_curr_state = 0;
	rising_edge = 0;
	falling_edge = 0;
}


