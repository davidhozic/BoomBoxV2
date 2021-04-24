
#include "FreeRTOS.h"
#include "task.h"
#include "settings.h"
#include "global.h"
#include "libs/EEPROM/EEPROM.h"
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
	ADCSRA |= (1 << ADEN);				                        // Vklop adc in zacetek konverzije
	ADCSRA |= (1 << ADSC);
	/************************************************************************/
	/*							  SETUP OTHER                               */
	/************************************************************************/
	writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN, 1);
	writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_FINISHED, EEPROM.beri(battery_eeprom_addr));
	
	/************************************************************************/
	/*							  SETUP WATCHDOG                            */
	/************************************************************************/
//	WDTCSR = 0x1 << WDIE | 0x1 << WDP2 | 0x1 << WDP1;
	
	/************************************************************************/
	/*							   SETUP TASKS                              */
	/************************************************************************/

	xTaskCreate(power, "Power", 64, NULL, 1, NULL);
	xTaskCreate(zaslon, "display", 64, NULL, 1, NULL);
	xTaskCreate(polnjenje, "charing", 64, NULL, 1, NULL);
	xTaskCreate(settings_UI, "settings_ui", 64, NULL, 2, NULL);
	xTaskCreate(audio_visual, "audio_system", 64, NULL, 1, NULL);
	
	vTaskStartScheduler();
	return 0;
}
