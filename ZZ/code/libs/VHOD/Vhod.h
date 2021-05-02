
#ifndef VHOD_H
#define	VHOD_H

#include "castimer/castimer.h"
#include <stdint.h>
#ifndef  SEZNAM_INC
#include "./povezan_seznam/povezan_seznam.h"
#endif




class class_VHOD // pin, port, stanje ko ni pritisnjen
{
private:
	uint8_t curr_state		:	1;
	uint8_t prev_state		:	1;
	uint8_t rising_edge		:	1;
	uint8_t falling_edge	:	1;
	uint8_t default_state	:	1;
    unsigned char pin;
    char port;

public:
    bool vrednost();
    bool risingEdge();
    bool fallingEdge();
	class_VHOD(unsigned char pin, char port, char default_state);
};

#endif


