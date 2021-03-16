
#include "avr/io.h"
#include "includes/includes.h"
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
				TCCR2A &= ~(1 << COM2A1); //izklopi komparator
				if ( (TCCR2A & (1 << COM2B1)) == 0  ){ //Ce je tudi primerjalnik B izklopljen se izklopi
					TCCR2B = 0;
					TCNT2 = 0;
				}
			break;
			
			case 5:
				TCCR1A &= ~(1 << COM1A1); //izklopi komparator
				if ( (TCCR1A & (1 << COM1B1)) == 0  ){ //Ce je tudi primerjalnik B izklopljen se izklopi
					TCCR1B = 0;
					TCNT1 = 0;
				}
			case 6:
				TCCR1A &= ~(1 << COM1B1); //izklopi komparator
				if ( (TCCR1A & (1 << COM1A1)) == 0  ){ //Ce je tudi primerjalnik B izklopljen se izklopi
					TCCR1B = 0;
					TCNT1 = 0;
				}
			break;
			
		}
		break;

		default:
		break;
	}
}




uint16_t readANALOG(uint8_t pin)
{
	ADSCSRA |= (1 << ADSC); //Start konverzija
	while (ADCSRA & (1 << ADSC)); //Dokler se bit ne resetira
	/* DODAJ */
	return ADCH;
}

void writePWM(uint8_t pin, char port, uint8_t vrednost)
{

	switch (port)
	{
		case 'B':
		DDRB = DDRB | (0b00000001 << pin); //Nastavi na output
		switch (pin){
			case 4:
				TCCR2A = (1 << WGM21) | (1 << WGM20) | (1 << COM2A1); //fast pwm non inverted
				TCCR2B = (1 << CS20); //brez prescalrja
				OCR2A = vrednost;		//vrednost kjer se izhod ugasne
			break;
			
			case 5:
				TCCR1A = (1 << COM1A1) | (1 << WGM10) | (1 << WGM12);
				TCCR1B = (1 << CS10);
				OCR1A = vrednost;
			break;
			
			case 6:
				TCCR1A = (1 << COM1B1) | (1 << WGM10) | (1 << WGM12);
				TCCR1B = (1 << CS10);
				OCR1B = vrednost;
			break;
			
		}

		break;

		default:
		break;
	}
}
