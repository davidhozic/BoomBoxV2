
#pragma once

#include "castimer/castimer.h"

enum enum_VHOD_STATUS_REGISTER
{
	VHOD_REG_TRENUTNO_STANJE=0,
	VHOD_REG_PREJSNJE_STANJE,
	VHOD_REG_RISING_EDGE,
	VHOD_REG_FALLING_EDGE,
	VHOD_REG_DEFAULT_STATE
};



class class_VHOD // pin, port, stanje ko ni pritisnjen
{
private:
	unsigned char status_register;
    unsigned char pin;
    char port;
	
public:
    bool vrednost();
    bool risingEdge();
    bool fallingEdge();
    class_VHOD(unsigned char pin, char port, char default_state);
};

