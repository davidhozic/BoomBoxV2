

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
#include <util/delay.h>



/************************************************************************/
/*						      TASK PROTOS                               */
/************************************************************************/
void power(void *paramOdTaska);
void thermal(void *paramOdTaska);
void zaslon(void *paramOdTaska);
void audio_visual(void *p);
void polnjenje(void *paramOdTaska);
void settings_UI(void *paramOdTaska);

/************************************************************************/
/*						Main thread handles			                    */
/************************************************************************/
TaskHandle_t handle_audio_system = NULL;
TaskHandle_t handle_capacity_display = NULL;
/************************************************************************/


void init()
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
	TCCR3B	= 1 << WGM32 | 1 << CS31 | 1 << CS30;				// Clear timer on compare match
	OCR3A	= 249;												// Output compare match na 249 tickov (1ms)
	TIMSK3	= 1 << OCIE3A;										// Vklopi output compare match ISR

	/************************************************************************/
	/*								SETUP ADC                               */
	/************************************************************************/
	ADMUX   = (1 << REFS0);										// Izberi
	ADCSRA |= (1 << ADEN);				                        // Vklop adc in zacetek konverzije
	ADCSRA |= (1 << ADSC);
	_delay_ms(4);

	/************************************************************************/
	/*							  SETUP OTHER                               */
	/************************************************************************/
	writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_FINISHED ,EEPROM.beri(battery_eeprom_addr));
	writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN ,1);
	/************************************************************************/
	/*							   SETUP TASKS                              */
	/************************************************************************/
	xTaskCreate(power, "power", 256, NULL, 1, NULL);
	xTaskCreate(settings_UI, "settings", 256, NULL, 3, NULL);
	xTaskCreate(zaslon, "LVCHRG", 256, NULL, 1, &handle_capacity_display);
	xTaskCreate(polnjenje, "CHRG", 256, NULL, 1, NULL);
	xTaskCreate(audio_visual, "AUSYS", 256, NULL, 3, &handle_audio_system);
	vTaskStartScheduler();
}



int main()
{	
	init();
	return 0;
}
