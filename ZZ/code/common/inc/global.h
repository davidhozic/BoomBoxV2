
#ifndef GLOBAL_STRUCTS_HEADER
#define GLOBAL_STRUCTS_HEADER
#include "settings.h"
#include "castimer/castimer.h"
#include "VHOD/Vhod.h"

/************************************************************************/
/*								GLOBAL OBJECTS	                        */
/************************************************************************/
extern VHOD napajalnik;
extern castimer stikaloCAS;


/************************************************************************/
/*							GLOBAL STRUCTS		                        */
/************************************************************************/
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
	float Hardware_temp;
	float napetost;
	unsigned long timeFROMboot;
};

extern Hardware_t Hardware;


#endif