#include "VHOD/Vhod.h"
#include "avr/io.h"
#include "castimer/castimer.h"
#include "global.h"

bool class_VHOD::vrednost()
{
	
	writeBIT(status_register, VHOD_REG_TRENUTNO_STANJE, readBIT( *((unsigned char*)&PINA + 3*(port-'A'))   , pin)   ); // Write bit in status register read from PINA + (port - 'A')* 3 (start at port A and then move forward thru addresses to get to other PINs) (0x20 = PINA)	

	if (readBIT(status_register, VHOD_REG_DEFAULT_STATE))																								// If unpressed state is 1, invert to return 0 if unpressed
		writeBIT(status_register, VHOD_REG_TRENUTNO_STANJE, !readBIT(status_register, VHOD_REG_TRENUTNO_STANJE));	//Inverts current state

	if (readBIT(status_register, VHOD_REG_PREJSNJE_STANJE) != readBIT(status_register, VHOD_REG_TRENUTNO_STANJE) && readBIT(status_register, VHOD_REG_TRENUTNO_STANJE))
	{
 		writeBIT(status_register, VHOD_REG_RISING_EDGE, 1);															// Rising edge = 1	
 		writeBIT(status_register, VHOD_REG_PREJSNJE_STANJE, readBIT(status_register, VHOD_REG_TRENUTNO_STANJE));	// Previous value = current value
	}
	else if (readBIT(status_register, VHOD_REG_PREJSNJE_STANJE) != readBIT(status_register, VHOD_REG_TRENUTNO_STANJE))
	{
		writeBIT(status_register, VHOD_REG_FALLING_EDGE, 1);
		writeBIT(status_register, VHOD_REG_PREJSNJE_STANJE, readBIT(status_register, VHOD_REG_TRENUTNO_STANJE));
	}
	
	if (readBIT(status_register, VHOD_REG_TRENUTNO_STANJE) == 0)
	{
		writeBIT(status_register, VHOD_REG_RISING_EDGE, 0);			
	}
	else if(readBIT(status_register, VHOD_REG_TRENUTNO_STANJE)){
		writeBIT(status_register, VHOD_REG_FALLING_EDGE, 0);
	}

	return readBIT(status_register, VHOD_REG_TRENUTNO_STANJE);

}

bool class_VHOD::risingEdge()
{
	vrednost();
	if (readBIT(status_register, VHOD_REG_RISING_EDGE))
	{
		writeBIT(status_register, VHOD_REG_RISING_EDGE, 0);			
		return true;
	}
	return false;
}

bool class_VHOD::fallingEdge()
{
	vrednost();
	if (readBIT(status_register, VHOD_REG_FALLING_EDGE))
	{
		writeBIT(status_register, VHOD_REG_FALLING_EDGE, 0);
		return true;
	}
	return false;
}

class_VHOD::class_VHOD(unsigned char pin, char port, char default_state)
{
	this->port = port;
	this->pin = pin;
	writeBIT(status_register, VHOD_REG_DEFAULT_STATE, default_state);
	status_register = 0;
}