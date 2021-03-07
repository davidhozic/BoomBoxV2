#include "C:\Program Files (x86)\Arduino\hardware\tools\avr\avr\include\avr\io.h"

void writeOUTPUT(unsigned char pin, char port, bool vrednost)
{
    switch (port)
    {

    case 'B':
        vrednost == 1 ? PORTB |= (1 << pin) : PORTB &= ~(1 << pin);
        break;

    case 'D':
        vrednost == 1 ? PORTD |= (1 << pin) : PORTD &= ~(1 << pin);
        break;
    case 'H':
        vrednost == 1 ? PORTH |= (1 << pin) : PORTH &= ~(1 << pin);
        break;
    }
}
