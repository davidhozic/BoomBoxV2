
#pragma once

#include "castimer/castimer.h"

class class_VHOD // pin, port, stanje ko ni pritisnjen
{
private:
    bool prejsnje_stanje;
    bool trenutno_stanje;
    bool rs_edge;
    bool fl_edge;
    int pin;
    char port;
    int default_state;
	class_TIMER off_timer;

public:
    bool vrednost();
    bool risingEdge();
    bool fallingEdge();

    class_VHOD(int pin, char port, int default_state)
    {
        this->port = port;
        this->pin = pin;
        this->default_state = default_state;
    }
};

