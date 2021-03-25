

#include "VHOD/Vhod.h"
#include "castimer/castimer.h"
#include "FreeRTOS.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "audio/includes/audio.h"
#include "libs/EEPROM/EEPROM.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/global.h"
#include <avr/io.h>
#include <avr/interrupt.h>



ISR (TIMER3_COMPA_vect){
	Hardware.timeFROMboot +=1;					// Belezi cas od zagona krmilnika
}

/************************************************************************/
/*						      TASK PROTOS                               */
/************************************************************************/
void power(void *paramOdTaska);
void thermal(void *paramOdTaska);
void zaslon(void *paramOdTaska);
void audio_visual(void *p);
void polnjenje(void *paramOdTaska);
void events(void *paramOdTaska);
/************************************************************************/

/*************************************************/
/*                                               */
/*            FreeRTOS main task control         */
/*                                               */
/*************************************************/
/**/ TaskHandle_t core_control = NULL;         /**/
/**/ TaskHandle_t event_control = NULL;        /**/
/**/ TaskHandle_t audio_system_control = NULL; /**/
/**/ TaskHandle_t zaslon_control = NULL;       /**/
/**/ TaskHandle_t chrg_control = NULL;         /**/
/**/ SemaphoreHandle_t voltage_SEM = NULL;	   /**/ // Preprecuje da bi dva taska dostopala do napetosti naenkrat
/*************************************************/


void init()
{
	/************************************************************************/
	/*						  SET DATA DIRECTION REGISTERS                  */
	/************************************************************************/
	DDRE = 1;
	DDRH = 1 << PH3 | 1 << PH4 | 1 << PH5;
	DDRB = 1 << PB7 | 1 << PB6 | 1 << PB5 | 1 << PB4;

	/************************************************************************/
	/*						SETUP TIMER FOR SYSTEM TIME                     */
	/************************************************************************/
	TCCR3A  = 0;
	TCCR3B	= 1 << WGM32 | 1 << CS31 | 1 << CS30;				// Clear timer on compare match
	OCR3A	= 249;												// Output compare match na 249 tickov (1ms)
	TIMSK3	= 1 << OCIE3A;										// Vklopi output compare match ISR

	/************************************************************************/
	/*								SETUP ADC                               */
	/************************************************************************/
	ADMUX   = (1 << REFS0);										// Izberi
	ADCSRA |= (1 << ADEN);				                        // Vklop adc in zacetek konverzije
	ADCSRA |= (1 << ADSC);

	/************************************************************************/
	/*							  SETUP OTHER                               */
	/************************************************************************/
	writeOUTPUT(red_button_pin, red_button_port, 1);			// PULL UP
	Hardware.POLKONC = EEPROM.beri(battery_eeprom_addr);
	voltage_SEM = xSemaphoreCreateMutex();
	xSemaphoreGive(voltage_SEM);								// GIVE = ostali lahko vzamejo dostop, TAKE = task ostalim taskom vzame dostop do semaforja

	/************************************************************************/
	/*							   SETUP TASKS                              */
	/************************************************************************/
	xTaskCreate(power, "power", 256, NULL, 1, &core_control);
	xTaskCreate(events, "Events task", 256, NULL, 3, &event_control);
	xTaskCreate(zaslon, "LVCHRG", 256, NULL, 1, &zaslon_control);
	xTaskCreate(polnjenje, "CHRG", 256, NULL, 1, &chrg_control);
	xTaskCreate(audio_visual, "AUSYS", 256, NULL, 2, &audio_system_control);
	vTaskStartScheduler();
}

int main()
{
init();
return 0;
}