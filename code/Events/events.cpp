#include "events.hpp"
#include "common.hpp"
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


/**********************************************************************
 *  FUNCTION:    system_event
 *  PARAMETERS:  enum_system_event event
 *  DESCRIPTION: Performs a system event                         
 *  RETURN:      void
 **********************************************************************/
void system_event(enum_system_event event){
	switch(event)
	{
		case EV_POWER_SWITCH_EXTERNAL:
        	system_event(EV_SHUTDOWN);
			delay_FreeRTOS_ms(3);
			writeOUTPUT(GLOBAL_CFG_PIN_MENJALNIK,GLOBAL_CFG_PORT_MENJALNIK,1);
			m_hw_status.external_power = 1;
        break;

		case EV_POWER_SWITCH_INTERNAL:
			system_event(EV_SHUTDOWN);
			delay_FreeRTOS_ms(3);
			writeOUTPUT(GLOBAL_CFG_PIN_MENJALNIK,GLOBAL_CFG_PORT_MENJALNIK, 0);
			m_hw_status.external_power = 0;
		break;
		
        case EV_SLEEP:
            asm("cli");
            /* Set sleep mode to power down */
            SMCR = 0;
            SMCR |= (1 << SM1);
            SMCR |= (1 << SE);
            /* Setup PCINT interrupt */
            PCICR = (1 << PCIE2);
            PCMSK2 = (1 << PCINT16) | (1 << PCINT17);
            asm("sei");
            asm("SLEEP"); 
        break;

        case EV_WAKE:
            asm("cli");
            SMCR = 0;
            PCICR = 0;
            PCMSK2 = 0;
            asm("sei");
        break;

		case EV_POWER_UP:
			writeOUTPUT(GLOBAL_CFG_PIN_12V_LINE, GLOBAL_CFG_PORT_12V_LINE, 1);
			writeOUTPUT(GLOBAL_CFG_PIN_OUTPUT_MOSFET, GLOBAL_CFG_PORT_OUTPUT_MOSFET, 1);
			m_audio_system.strip_on();
			m_hw_status.powered_up = 1;
			delay_FreeRTOS_ms(10);
		break;
		
		case EV_SHUTDOWN:
			m_hw_status.powered_up = 0;
            writeOUTPUT(GLOBAL_CFG_PIN_12V_LINE, GLOBAL_CFG_PORT_12V_LINE, 0);
			writeOUTPUT(GLOBAL_CFG_PIN_OUTPUT_MOSFET, GLOBAL_CFG_PORT_OUTPUT_MOSFET , 0);
			m_audio_system.strip_off();
		break;
		
		case EV_INITIALIZATION:

			/************************************************************************/
			/*						SETUP TIMER3 FOR TIMER_t LIBRARY                */
			/************************************************************************/
			TCCR3A  = 0;
			OCR3A = 249;												// Output compare match na 249 tickov (1ms)
			TIMSK3	= (1 << OCIE3A);										// Vklopi output compare match ISR
			TCCR3B	= (1 << WGM32) | (1 << CS31) | (1 << CS30);				// Clear timer on compare match	, /64 prescaler
			/************************************************************************/
			/*								SETUP ADC                               */
			/************************************************************************/

			ADMUX   =	(1 << REFS0);
			ADCSRA  =	(1 << ADPS0) | (1 << ADPS2);
			ADCSRB  =	0;
			ADCSRA |=	(1 << ADEN);
			DIDR0   =	0xFF;
			ADMUX   =   1;
			ADCSRA |=	(1 << ADSC);		
            /************************************************************************/
			/*						SET DATA DIRECTION REGISTERS			        */
			/************************************************************************/
			DDRK = 0;
            DDRE = 1;
			DDRH = (1 << PH3) | (1 << PH4) | (1 << PH5);
			DDRB = (1 << PB4) | (1 << PB5) | (1 << PB6) | (1 << PB7);
            
			/************************************************************************/
			/*							   SETUP TASKS                              */
			/************************************************************************/
			xTaskCreate(power_task, "Power", TASK_CFG_TASK_DEFAULT_STACK, NULL, 1, NULL);
			xTaskCreate(user_ui_task, "User UI", TASK_CFG_TASK_DEFAULT_STACK, NULL, 2, NULL);
            xTaskCreate(audio_visual_task, "audio-visual", TASK_CFG_TASK_DEFAULT_STACK, NULL, 3, &m_audio_system.handle_audio_system);
            holdTASK(&m_audio_system.handle_audio_system); // <---- Gets resumed after strip .strip_on() function gets called
			/************************************************************************/
			/*								OTHER                                   */
			/************************************************************************/
			m_hw_status.charging_finished = EEPROM.beri(GLOBAL_CFG_EEPROM_ADDR_BATTERY_CHARGING_STATUS);
			/************************************************************************/
			/*								START TASKS                             */
			/************************************************************************/
			vTaskStartScheduler();
		break;
	}
}


