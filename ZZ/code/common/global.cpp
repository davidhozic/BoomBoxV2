#include "global.h"



/************************************************************************/
/*								GLOBAL OBJECTS	                        */
/************************************************************************/
VHOD napajalnik(0, 'K', 0);
castimer stikaloCAS;

/************************************************************************/
/*					          GLOBAL  STRUCTURES                        */
/************************************************************************/
Hardware_t Hardware = {
	5000.00f,
	5.00f,
	false,
	false,
	false,
	false,
	false,
	false,
	0.0f,
	0
};

/*
struct Hardware_t
{
	float REF_mVOLT; //Referencna napetost na ADC (4.999V - 5.002V)
	float REF_VOLT;
	bool is_Powered_UP;
	bool PSW;
	bool POLKONC;
	bool polnjenjeON;
	bool AMP_oheat;
	bool display_enabled;
	float napetost;
	unsigned long timeFROMboot;
};
*/