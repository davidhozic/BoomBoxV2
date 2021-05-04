#include "settings.h"
#include "global.h"
#include "libs/EEPROM/EEPROM.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "audio-visual/includes/audio.h"
#include "common/inc/watchdog_functions.h"
#include <util/delay.h>
#include "libs/VHOD/Vhod.h"
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
	OCR3A	= 249;												// Output compare match na 249 tickov (1ms)
	TIMSK3	= 1 << OCIE3A;										// Vklopi output compare match ISR
	TCCR3B	= 1 << WGM32 | 1 << CS31 | 1 << CS30;				// Clear timer on compare match	
	/************************************************************************/
	/*								SETUP ADC                               */
	/************************************************************************/
	ADMUX   =	(1 << REFS0);																		
	ADCSRA  =	(1 << ADPS0) | (1 << ADPS2);				                      
	ADCSRB  =	 0;
	ADCSRA |=	(1 << ADEN);
	DIDR0   =	0xFF;
	ADCSRA |=	(1 << ADSC);
	/************************************************************************/
	/*							  SETUP OTHER                               */ 
	/************************************************************************/
	 Hardware.status_reg.charging_finished = EEPROM.beri(eeprom_addr_battery_stat);		/* Read charging state */

	/************************************************************************/
	/*							   SETUP TASKS                              */
	/************************************************************************/
	xTaskCreate(power, "Power", 256, NULL, 1, NULL);
	xTaskCreate(zaslon, "display", 256, NULL, 1, NULL);
	xTaskCreate(polnjenje, "charing", 256, NULL, 1, NULL);
	xTaskCreate(settings_UI, "settings_ui", 256, NULL, 3, NULL);
	xTaskCreate(audio_visual, "audio_system", 256, NULL, 3, NULL);

#ifndef DEBUG
	_delay_ms(25);
#endif


	/************************************************************************/
	/*								START TASKS                             */
	/************************************************************************/
	vTaskStartScheduler();
	return 0;
}
