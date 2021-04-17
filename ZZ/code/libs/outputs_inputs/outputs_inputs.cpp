
#include <avr/io.h>
#include "libs/outputs_inputs/outputs_inputs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "common/inc/global.h"

void writeOUTPUT(unsigned char pin, char port, bool vrednost)
{
	pwmOFF(pin, port); // izklopi pwm
	writeBIT( *((unsigned char*)&PORTA + 3*(port - 'A')), pin, vrednost);	
}


void toggleOUTPUT(unsigned char pin, char port)
{
	pwmOFF(pin, port); // izklopi pwm
	*(	(unsigned char*)&PORTA + 3* (port - 'A')  )	^= (1 << pin);	// Move forwared by 3 * (length from A to reg) and XOR 
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




unsigned short readANALOG(uint8_t pin)
{
	taskENTER_CRITICAL();	
	ADMUX = (1 << REFS0) | pin;
	ADCSRA |= (1 << ADSC); //Start konverzija
	while (ADCSRA & (1 << ADSC)); //Dokler se bit ne resetira
	ADMUX &= ~(pin);
	taskEXIT_CRITICAL();
	return ADC;
}

void writePWM(uint8_t pin, char port, uint8_t vrednost)
{

	switch (port)
	{
		case 'B':
		DDRB = DDRB | (1 << pin); //Nastavi na output
		switch (pin){
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

		default:
		break;
	}
}
