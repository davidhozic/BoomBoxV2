#include "VHOD/Vhod.h"
#include "avr/io.h"
#include "castimer/castimer.h"

bool class_VHOD::vrednost()
{

	switch (port)
	{
		case 'D':
			trenutno_stanje = (PIND & (1 << pin)) > 0;
			break;

		case 'B':
			trenutno_stanje = (PINB & (1 << pin)) > 0;
			break;
			
		case 'K':
			trenutno_stanje = (PINK & (1 << pin)) > 0;
			break;	
		
		case 'G':
			trenutno_stanje = (PING & (1 << pin)) > 0;
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

	if (trenutno_stanje)
		off_timer.ponastavi();	//Resetira timer za filtriranje

	if (!trenutno_stanje && off_timer.vrednost() > 20) //Filtrira lazne izklope
		return 0;

	return 1;
	

}

bool class_VHOD::risingEdge()
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

bool class_VHOD::fallingEdge()
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
