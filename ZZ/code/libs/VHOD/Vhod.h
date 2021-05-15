
#ifndef VHOD_H
#define	VHOD_H

typedef unsigned char uint8_t;

/************************************************************/
					/*		 SETTINGS		*/
					
#define FILTER_TIME_MS									50
#define USE_FILTERING									0
/************************************************************/


#if (USE_FILTERING == 1)
	#include "povezan_seznam.h"
	#include "castimer.h"
#endif


class class_VHOD // pin, port, stanje ko ni pritisnjen
{
private:
	uint8_t filtered_curr_state		:	1;
	uint8_t unfiltered_curr_state	:	1;	
	uint8_t prev_state				:	1;
	uint8_t rising_edge				:	1;
	uint8_t falling_edge			:	1;
	uint8_t default_state			:	1;
	
    uint8_t pin;
    char port;
#if (USE_FILTERING == 1)
	class_TIMER filter_timer;
#endif

public:
    bool vrednost();
    bool risingEdge();
    bool fallingEdge();
	class_VHOD(uint8_t pin, char port, char default_state);
};


#endif


