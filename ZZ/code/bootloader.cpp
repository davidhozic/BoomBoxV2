#include "settings.hh"
#include "global.hh"
#include "libs/EEPROM/EEPROM.hh"
#include "libs/outputs_inputs/outputs_inputs.hh"
#include "audio-visual/includes/audio.hh"
#include <util/delay.h>
#include "libs/VHOD/Vhod.hh"
/************************************************************************/
/*						      TASK PROTOS                               */
/************************************************************************/
void power(void *paramOdTaska);
void zaslon(void *paramOdTaska);
void audio_visual(void *p);
void polnjenje(void *paramOdTaska);
void settings_UI(void *paramOdTaska);


int main()
 {	
	
	/************************************************************************/
	/*						  SET DATA DIRECTION REGISTERS                  */
	/************************************************************************/
	DDRE = 1;
	DDRH = 1 << PH3 | 1 << PH4 | 1 << PH5;
	DDRB = 1 << PB4 | 1 << PB5 | 1 << PB6 | 1 << PB7;
	/************************************************************************/
	/*						SETUP TIMER FOR TIMER OBJECS                    */
	/************************************************************************/
	TCCR3A  = 0;
	OCR3A = 249;												// Output compare match na 249 tickov (1ms)
	TIMSK3	= 1 << OCIE3A;										// Vklopi output compare match ISR
	TCCR3B	= 1 << WGM32 | 1 << CS31 | 1 << CS30;				// Clear timer on compare match	, /64 prescaler 
	/************************************************************************/
	/*								SETUP ADC                               */
	/************************************************************************/
	ADMUX   =	(1 << REFS0);																		
	ADCSRA  =	(1 << ADPS0) | (1 << ADPS2);				                      
	ADCSRB  =	 0;
	ADCSRA |=	(1 << ADEN);
	DIDR0   =	0xFF;
	ADMUX = 1;
	ADCSRA |=	(1 << ADSC);
	/************************************************************************/
	/*							  SETUP OTHER                               */ 
	/************************************************************************/
	 m_Hardware.status_reg.charging_finished = EEPROM.beri(EEPROM_ADDRESS_BATTERY_CHARGING_STATUS);	/* Read charging state */
	/************************************************************************/
	/*							   SETUP TASKS                              */
	/************************************************************************/
	xTaskCreate(power, "Power", 256, NULL, 1, NULL);
	xTaskCreate(zaslon, "display", 256, NULL, 1, NULL);
	xTaskCreate(polnjenje, "charing", 256, NULL, 1, NULL);
	xTaskCreate(settings_UI, "settings_ui", 256, NULL, 3, NULL);
	xTaskCreate(audio_visual, "m_audio_system", 256, NULL, 3, NULL);
	/************************************************************************/
	/*								START TASKS                             */
	/************************************************************************/
	vTaskStartScheduler();
	return 0;
}
