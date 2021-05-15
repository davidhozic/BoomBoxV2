
#include <avr/io.h>
#include "outputs_inputs.h"

#include "bit_manipulation.h"

#include "util/atomic.h"

void writeOUTPUT(unsigned char pin, char port, bool vrednost)
{
	pwmOFF(pin, port); // izklopi pwm
	if (port <= 'G')
		writeBIT( *((unsigned char*)&PORTA + 3*(port - 'A')), pin, vrednost);	
	else if (port == 'H')
		writeBIT(PORTH, pin, vrednost);
	else
		writeBIT( *((unsigned char*)&PORTJ + 3*(port - 'J')), pin, vrednost);	
}


void toggleOUTPUT(unsigned char pin, char port)
{
	pwmOFF(pin, port); // izklopi pwm
	if (port <= 'G')
		*(	(unsigned char*)&PORTA + 3* (port - 'A')  )	^= (1 << pin);	// Move forwared by 3 * (length from A to reg) and XOR 
	else if (port == 'H')
		PORTH ^= (1 << pin);
	else
		*(	(unsigned char*)&PORTJ + 3* (port - 'J')  )	^= (1 << pin);
}

void pwmOFF(uint8_t pin, char port)
{
	switch (port)
	{
		case 'B':
			switch (pin)
			{	
				/*************** PIN SWITCH  ******************/
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
					break;
				case 6:
					TCCR1A &= ~(1 << COM1B1); //izklopi komparator
					if ( (TCCR1A & (1 << COM1A1)) == 0  ){ //Ce je tudi primerjalnik B izklopljen se izklopi
						TCCR1B = 0;
						TCNT1 = 0;
					}
					break;
				/**********************************************/
			}
		break;

		default:
		break;
	}
}




unsigned short readANALOG(uint8_t channel)
{
	while (ADCSRA & (1 << ADSC)); //Dokler se bit ne resetira
	ADMUX = (1 << REFS0);
						
	if (channel < 8)	//	Stevilka ADC pina ustreza binarni obliki ki jo je treba pisati za ADC0:7. Pri ADC8:15 je potreba binarna
	{					//	oblika enaka kot da bi sli steti od 0 naprej z izjemo da je bit 5 vedno postavljen (po dokumentaciji)
		ADCSRB &= ~(1 << MUX5);
		ADMUX |= channel;
	}
	else
	{
		ADCSRB |= (1 << MUX5);
		ADMUX  |=  channel%8;
	}
	
	ADCSRA |= (1 << ADSC); //Start konverzija			
	while (ADCSRA & (1 << ADSC)); //Dokler se bit ne resetira
	ADMUX &= ~(channel);
	return ADC;
}

void writePWM(uint8_t pin, char port, uint8_t vrednost)
{

	switch (port)
	{
		case 'B':
			DDRB = DDRB | (1 << pin); //Nastavi na output
			switch (pin)
			{
				case 4:
					TCCR2A |= (1 << WGM21) | (1 << WGM20) | (1 << COM2A1);  //fast pwm, clear on compare match
					TCCR2B = (1 << CS20); //brez prescalrja
					OCR2A = vrednost;		//vrednost kjer se izhod ugasne
				break;
			
				case 5:
					TCCR1A |= (1 << COM1A1) | (1 << WGM10);
					TCCR1B = (1 << CS10) | (1 << WGM12);
					OCR1A = vrednost;
				break;
			
				case 6:
					TCCR1A |= (1 << COM1B1) | (1 << WGM10);
					TCCR1B = (1 << CS10) | (1 << WGM12);
					OCR1B = vrednost;
				break;
			}
		break;
	}
}
