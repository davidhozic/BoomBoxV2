
#include "FreeRTOS.h"
#include "task.h"
#include "settings.h"
#include "global.h"
#include "libs/EEPROM/EEPROM.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "audio-visual/includes/audio.h"
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
	ADMUX   = (1 << REFS0);										// Izberi
	ADCSRA |= (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1);				                        // Vklop adc in zacetek konverzije
	ADCSRB = 0;
	DIDR0 = 0xFF;
	ADCSRA |= (1 << ADSC);
	/************************************************************************/
	/*							  SETUP OTHER                               */
	/************************************************************************/
	writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN, 1);
	writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_FINISHED, EEPROM.beri(battery_eeprom_addr));
	
	/************************************************************************/
	/*							  SETUP WATCHDOG                            */
	/************************************************************************/
	/************************************************************************/
	/*							   SETUP TASKS                              */
	/************************************************************************/
	
	readANALOG(0);
	
	xTaskCreate(power, "Power", 128, NULL, 1, NULL);
	xTaskCreate(zaslon, "display", 128, NULL, 1, &handle_capacity_display);
	xTaskCreate(polnjenje, "charing", 128, NULL, 1, NULL);
	xTaskCreate(settings_UI, "settings_ui", 128, NULL, 3, NULL);
	xTaskCreate(audio_visual, "audio_system", 128, NULL, 3, &audio_system.handle_audio_system);
	
	vTaskStartScheduler();
	return 0;
}
