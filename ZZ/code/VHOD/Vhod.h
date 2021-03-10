#ifndef VHOD_H
#define VHOD_H

class VHOD // pin, port, stanje ko ni pritisnjen
{
private:
    bool prejsnje_stanje;
    bool trenutno_stanje;
    bool rs_edge;
    bool fl_edge;
    int pin;
    char port;
    int default_state;

public:
    bool vrednost();
    bool risingEdge();
    bool fallingEdge();

    VHOD(int pin, char port, int default_state)
    {
        this->port = port;
        this->pin = pin;
        this->default_state = default_state;
    }
};

#endif
