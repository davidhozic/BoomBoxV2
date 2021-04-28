#include "VHOD/Vhod.h"
#include "avr/io.h"
#include "castimer/castimer.h"
#include "global.h"
#include <stdio.h>
#include "libs/povezan_seznam/povezan_seznam.h"






bool class_VHOD::vrednost()
{
	if (port < 'H')
		curr_state = readBIT( *((unsigned char*)&PINA + 3*(port-'A'))   , pin); // Write bit in status register read from PINA + (port - 'A')* 3 (start at port A and then move forward thru addresses to get to other PINs) (0x20 = PINA)	
	else if (port == 'H')
		curr_state =  readBIT( PINH, pin);
	else
		curr_state = readBIT( *((unsigned char*)&PINJ + 3*(port-'J'))   , pin); 	
	// END READING OF PORT
	/************************/
	
	if (default_state)																// If unpressed state is 1, invert to return 0 if unpressed
		curr_state =  !curr_state;			//Inverts current state

	if (prev_state != curr_state && curr_state)
	{
 		rising_edge = 1;		
 		prev_state = curr_state;
	}
	else if (curr_state != prev_state)
	{
		falling_edge = 1;
		prev_state = curr_state;
	}
	
	if (!curr_state)
	{
		rising_edge = 0;
	}
	else if(curr_state){
		falling_edge = 0;
	}

	return curr_state;
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

class_VHOD::class_VHOD(unsigned char pin, char port, char default_state, Vozlisce <class_VHOD *> &input_objects_list)
{
	this->port = port;
	this->pin = pin;
	default_state = default_state;
	input_objects_list.dodaj_konec(this);
}


