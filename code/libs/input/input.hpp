
#ifndef VHOD_H
#define	VHOD_H
#include "input_settings.hpp"
#include <stdint.h>
#include "avr/io.h"

#if (USE_FILTERING)
	#include "castimer.hpp"
#endif

class INPUT_t // pin, port, stanje ko ni pritisnjen
{
private:
/* Bitfield variables */
	uint8_t filtered_curr_state		:	1;
	uint8_t register_value			:	1;	
	uint8_t prev_state				:	1;
	uint8_t rising_edge_var			:	1;
	uint8_t falling_edge_var		:	1;
	uint8_t default_state			:	1;
/* Setting variables */
	uint8_t pin;
    uint8_t port;

#if (USE_FILTERING == 1)
	TIMER_t filter_timer;
#endif

public:
    bool value();
    bool risen_edge();
    bool fallen_edge();
	INPUT_t(uint8_t pin, char port, char default_state);
};

/* Easier readings in the value function */
#define readBIT(reg, bit)				(						(	(reg >> bit) & 0x1	)								)
#define writeBIT(reg, bit, val)			(		(reg = val ? ( reg | (0x1 << bit) ) : ( reg  &  ~(0x1 << bit)))			)


#endif


