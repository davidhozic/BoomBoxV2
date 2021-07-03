#include "input.hpp"


bool INPUT_t::value()
{
	/* Switch is used for compatibility with ALL boards */
	switch (port)
	{
	#ifdef PINA
		case 'A':
			register_value = readBIT(PINA, pin);
		break;
	#endif

	#ifdef PINB
		case 'B':
			register_value = readBIT(PINB, pin);
		break;
	#endif

	#ifdef PINC
		case 'C':
			register_value = readBIT(PINC, pin);
		break;
	#endif

	#ifdef PIND
		case 'D':
			register_value = readBIT(PIND, pin);
		break;
	#endif

	#ifdef PINE
		case 'E':
			register_value = readBIT(PINE, pin);
		break;
	#endif

	#ifdef PINF
		case 'F':
			register_value = readBIT(PINF, pin);
		break;
	#endif

	#ifdef PING
		case 'G':
			register_value = readBIT(PING, pin);
		break;
	#endif

	#ifdef PINH
		case 'H':
			register_value = readBIT(PINH, pin);
		break;
	#endif

	#ifdef PINI
		case 'I':
			register_value = readBIT(PINI, pin);
		break;
	#endif

	#ifdef PINJ
		case 'J':
			register_value = readBIT(PINJ, pin);
		break;
	#endif


	#ifdef PINK
		case 'K':
			register_value = readBIT(PINK, pin);
		break;
	#endif

	#ifdef PINL
		case 'L':
			register_value = readBIT(PINL, pin);
		break;
	#endif

	#ifdef PINM
		case 'M':
			register_value = readBIT(PINM, pin);
		break;
	#endif

		default:
		break;
	}

	// END OF REGISTER READING
	/************************/
	
	/*		Invert output if unpressed input state is 1		*/
	if (default_state)
	{
		register_value = !register_value;
	}	
	
#if (USE_FILTERING == 1)
	/* Filter input with a timer */
	if (register_value != filtered_curr_state && filter_timer.value() >= AUVS_CFG_MEASS_FILTER_TIME_MS)
	{
		filtered_curr_state = register_value;
	}
	else if(register_value == filtered_curr_state)
	{
		filter_timer.reset();
	}
#else
	filtered_curr_state = register_value;
#endif
	/* END OF FILTERING*/

	
	/* Edge detection */
	if (prev_state != filtered_curr_state)
	{
		/* If state has changed and input is high -> rising edge*/
		if (filtered_curr_state)
		{
			rising_edge_var = 1;
		}
		else
		{
			falling_edge_var = 1;
		}	
		prev_state = filtered_curr_state;
	}
	
	/* Clear edges if input state doesn't match */
	if (!filtered_curr_state)
	{
		rising_edge_var = 0;
	}
	else if(filtered_curr_state){
		falling_edge_var = 0;
	}
	

	return filtered_curr_state;
}

#if (INCLUDE_risen_edge == 1)
bool INPUT_t::risen_edge()
{
	value();
	if (rising_edge_var)
	{
		rising_edge_var = 0;			
		return true;
	}
	return false;
}
#endif

#if (INCLUDE_fallen_edge == 1)
bool INPUT_t::fallen_edge()
{
	value();
	if (falling_edge_var)
	{
		falling_edge_var = 0;
		return true;
	}
	return false;
}
#endif

INPUT_t::INPUT_t(unsigned char pin, char port, char default_state)
{
	this->port = port;
	this->pin = pin;
	this->default_state	  = default_state;
	this->filtered_curr_state   = 0;
	this->register_value = 0;
	this->rising_edge_var  = 0;
	this->falling_edge_var = 0;
}




/************************************************************************/
/*							WARNINGS/ERRORS                             */
/************************************************************************/
#if (USE_FILTERING == 1)
	#warning "LITL: USE_FILTERING is enabled, LITL's timer will be used"
#endif