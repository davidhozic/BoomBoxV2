#include "events.hh"
#include "global.hh"
#include "outputs_inputs.hh"
#include "EEPROM.hh"
#include "audio.hh"


/************************************************************************/
/*								 PROTOS						            */
/************************************************************************/
void power(void *paramOdTaska);
void zaslon(void *paramOdTaska);
void audio_visual(void *p);
void polnjenje(void *paramOdTaska);
void settings_UI(void *paramOdTaska);
void power_switch_ev(uint8_t mode);

void system_event(enum_system_event eventt){
	
	switch(eventt)
	{
		case POWER_SWITCH_EXTERNAL:
		case POWER_SWITCH_INTERNAL:
			power_switch_ev(eventt);
		break;
		
		case POWER_UP:
			writeOUTPUT(_12V_line_pin, _12V_line_port, 1);
			writeOUTPUT(main_mosfet_pin, main_mosfet_port, 1);
			m_audio_system.stripON();
			m_Hardware.status_reg.powered_up = 1;
			delayFREERTOS(10);
		break;
		
		case SHUTDOWN:
			m_audio_system.stripOFF();
			writeOUTPUT(_12V_line_pin, _12V_line_port, 0);
			writeOUTPUT(main_mosfet_pin, main_mosfet_port , 0);
			m_Hardware.status_reg.powered_up = 0;
		break;
		
		case INITIALIZATION:
			/************************************************************************/
			/*						SET DATA DIRECTION REGISTERS			        */
			/************************************************************************/
			DDRE = 1;
			DDRH = 1 << PH3 | 1 << PH4 | 1 << PH5;
			DDRB = 1 << PB4 | 1 << PB5 | 1 << PB6 | 1 << PB7;
			/************************************************************************/
			/*						SETUP TIMER3 FOR TIMER LIBRARY                  */
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
		break;
	}
}


void power_switch_ev(uint8_t mode)
{
	switch(mode)
	{
		case POWER_SWITCH_EXTERNAL:
			system_event(SHUTDOWN);
			delayFREERTOS(3);
			writeOUTPUT(menjalnik_pin,menjalnik_port,1);
			m_Hardware.status_reg.external_power = 1;
		break;
		
		case POWER_SWITCH_INTERNAL:
			system_event(SHUTDOWN);
			delayFREERTOS(3);
			writeOUTPUT(menjalnik_pin,menjalnik_port, 0);
			m_Hardware.status_reg.external_power = 0;
		break;
	}
}



