#include "events.hpp"
#include "global.hpp"
#include "outputs_inputs.hpp"
#include "EEPROM.hpp"
#include "audio.hpp"

/************************************************************************/
/*								 PROTOS						            */
/************************************************************************/
void power_task(void *);
void zaslon_task(void *);
void audio_visual_task(void *);
void charging_task(void *);
void user_ui_task(void *);
void power_switch_ev(uint8_t mode);


void system_event(enum_system_event eventt){
	switch(eventt)
	{
		case EV_POWER_SWITCH_EXTERNAL:
		case EV_POWER_SWITCH_INTERNAL:
			power_switch_ev(eventt);
		break;
		
		case EV_POWER_UP:
			writeOUTPUT(_12V_line_pin, _12V_line_port, 1);
			writeOUTPUT(main_mosfet_pin, main_mosfet_port, 1);
			m_audio_system.strip_on();
			m_Hardware.status_reg.powered_up = 1;
			delay_FreeRTOS_ms(10);
		break;
		
		case EV_SHUTDOWN:
			m_Hardware.status_reg.powered_up = 0;
			m_audio_system.strip_off();
			writeOUTPUT(_12V_line_pin, _12V_line_port, 0);
			writeOUTPUT(main_mosfet_pin, main_mosfet_port , 0);

		break;
		
		case EV_INITIALIZATION:
			/************************************************************************/
			/*						SET DATA DIRECTION REGISTERS			        */
			/************************************************************************/
			DDRE = 1;
			DDRH = 1 << PH3 | 1 << PH4 | 1 << PH5;
			DDRB = 1 << PB4 | 1 << PB5 | 1 << PB6 | 1 << PB7;
			/************************************************************************/
			/*						SETUP TIMER3 FOR TIMER_t LIBRARY                  */
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
			/*							   SETUP TASKS                              */
			/************************************************************************/
			xTaskCreate(power_task, "Power", 256, NULL, 1, NULL);
			xTaskCreate(charging_task, "charging", 256, NULL, 1, NULL);
			xTaskCreate(user_ui_task, "User UI", 256, NULL, 3, NULL);
			xTaskCreate(audio_visual_task, "m_audio_system", 256, NULL, 2, NULL);
			
			/************************************************************************/
			/*								OTHER                                   */
			/************************************************************************/
			m_Hardware.status_reg.charging_finished = EEPROM.beri(EEPROM_ADDRESS_BATTERY_CHARGING_STATUS);
			
			/************************************************************************/
			/*								START TASKS                             */
			/************************************************************************/
			vTaskStartScheduler();
		break;
	}
}


void power_switch_ev(uint8_t mode)
{
	switch(mode)
	{
		case EV_POWER_SWITCH_EXTERNAL:
			system_event(EV_SHUTDOWN);
			delay_FreeRTOS_ms(3);
			writeOUTPUT(menjalnik_pin,menjalnik_port,1);
			m_Hardware.status_reg.external_power = 1;
		break;
		
		case EV_POWER_SWITCH_INTERNAL:
			system_event(EV_SHUTDOWN);
			delay_FreeRTOS_ms(3);
			writeOUTPUT(menjalnik_pin,menjalnik_port, 0);
			m_Hardware.status_reg.external_power = 0;
		break;
	}
}



