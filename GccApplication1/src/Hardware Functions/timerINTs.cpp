#include <avr/interrupt.h>
#include "../includes/includes.h"
#include <avr/io.h>


ISR (TIMER5_COMPA_vect){

Hardware.timeFROMboot +=1; //Pristeje cas od zacetka programa za 1ms
TCNT5 = 0;
}