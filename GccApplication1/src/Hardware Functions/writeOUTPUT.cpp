
#include "avr/io.h"
#include "src/includes/includes.h"
#define ADSCSRA _SFR_MEM8(0xE0)

void writeOUTPUT(unsigned char pin, char port, bool vrednost)
{

    pwmOFF(pin, port); // izklopi pwm
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

void pwmOFF(uint8_t pin, char port)
{
    switch (port)
    {
    case 'B':
        switch (pin)
        {
        case 4:
            TCCR2A &= ~(1 << COM2A1) & ~(1 << COM2A0); //Izklop primerjalnika
            break;
        case 5:
            TCCR1A &= ~(1 << COM1A1) & ~(1 << COM1A0);
            break;
        case 6:
            TCCR1A &= ~(1 << COM1B1) & ~(1 << COM1B0);
            break;
        }
        break;

    default:
        break;
    }
}

uint16_t readANALOG(uint8_t pin)
{
    taskENTER_CRITICAL(); //Dokler bere ta pin, ne sme brati ostalih
    ADSCSRA |= (1 << ADSC); //Start konverzija
    while (ADCSRA & (1 << ADSC)); //Dokler se bit ne resetira
    taskEXIT_CRITICAL();
    return ADCH;
}

void writePWM(uint8_t pin, char port, uint8_t vrednost)
{

    switch (port)
    {
    case 'B':
        DDRB = DDRB | (0b00000001 << pin); //Nastavi na output
        switch (pin)
        {
        case 4:
            TCCR2A = 1 << WGM22 || 1 << WGM20 || 1 << COM2A1; //Fast PWM, izklopi izhod po komparatorju
            TCCR2B = (1 << CS20);                                       // Delilnik frekvence
            OCR2A = vrednost;                                 //Nastavi se primerjalnik
            break;
        case 5:
            TCCR1A = 1 << WGM12 || 1 << WGM10 || 1 << COM1A1;
            TCCR2B = (1 << CS20); 
            OCR1A = vrednost; //Nastavi se primerjalnik
            break;
        case 6:
            TCCR1A = (1 << 0) | (1 << 3) | (1 << COM1B1);
            TCCR2B = (1 << CS20); 
            OCR1B = vrednost;
            break;
        }

        break;

    default:
        break;
    }
}
