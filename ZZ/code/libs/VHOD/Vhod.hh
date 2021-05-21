
#ifndef VHOD_H
#define	VHOD_H

#include <stdint.h>
#include "avr/io.h"

/************************************************************/
					/*		 SETTINGS		*/
					
#define FILTER_TIME_MS									50
#define USE_FILTERING									1
/************************************************************/


#if (USE_FILTERING == 1)
	#include "castimer.hh"
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

#define readBIT(reg, bit)				(		(	(reg >> bit) & 0x1	)												)
#define writeBIT(reg, bit, val)			(		(reg = val ? ( reg | (0x1 << bit) ) : ( reg  &  ~(0x1 << bit)))			)

#endif


