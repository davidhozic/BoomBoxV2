

#include "VHOD/Vhod.h"
#include "castimer/castimer.h"
#include "FreeRTOS.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "audio-visual/includes/audio.h"
#include "libs/EEPROM/EEPROM.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/global.h"
#include <avr/io.h>
#include <avr/interrupt.h>




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
/**/ TaskHandle_t audio_system_handle = NULL; /**/
/**/ TaskHandle_t capacity_display_handle = NULL;       /**/
/**/ TaskHandle_t chrg_control = NULL;         /**/
/**/ SemaphoreHandle_t voltage_semaphore = NULL;	   /**/ // Preprecuje da bi dva taska dostopala do napetosti naenkrat
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
	writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_FINISHED ,EEPROM.beri(battery_eeprom_addr));
	voltage_semaphore = xSemaphoreCreateMutex();
	xSemaphoreGive(voltage_semaphore);								// GIVE = ostali lahko vzamejo dostop, TAKE = task ostalim taskom vzame dostop do semaforja
		

	/************************************************************************/
	/*							   SETUP TASKS                              */
	/************************************************************************/
	xTaskCreate(power, "power", 256, NULL, 1, NULL);
	xTaskCreate(events, "Events task", 256, NULL, 3, NULL);
	xTaskCreate(zaslon, "LVCHRG", 256, NULL, 1, &capacity_display_handle);
	xTaskCreate(polnjenje, "CHRG", 256, NULL, 1, NULL);
	xTaskCreate(audio_visual, "AUSYS", 256, NULL, 2, &audio_system_handle);
	vTaskStartScheduler();
}

int main()
{
	init();
	return 0;
}
