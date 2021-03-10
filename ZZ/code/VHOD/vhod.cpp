#include "VHOD/Vhod.h"
#include "avr/io.h"

bool VHOD::vrednost()
{

	switch (port)
	{
		case 'D':
			trenutno_stanje = (PIND & (1 << pin)) > 0;
			break;

		case 'B':
			trenutno_stanje = (PINB & (1 << pin)) > 0;
			break;
	}

	if (default_state)
		trenutno_stanje = !(trenutno_stanje);

	if (prejsnje_stanje != trenutno_stanje && trenutno_stanje)
	{
		rs_edge = true;
		prejsnje_stanje = trenutno_stanje;
	}
	else if (prejsnje_stanje != trenutno_stanje)
	{
		fl_edge = true;
		prejsnje_stanje = trenutno_stanje;
	}

	return trenutno_stanje;
}

bool VHOD::risingEdge()
{
	vrednost();
	if (trenutno_stanje == 0)
	{
		rs_edge = false;
	}
	else if (rs_edge)
	{
		rs_edge = false;
		return true;
	}
	return false;
}

bool VHOD::fallingEdge()
{
	vrednost();
	if (trenutno_stanje)
	{
		fl_edge = false;
	}
	else if (fl_edge)
	{
		fl_edge = false;
		return true;
	}
	return false;
}
