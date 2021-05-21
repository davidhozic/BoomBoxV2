

#include "common/inc/global.hh"
#include "castimer/castimer.hh"
#include "VHOD/Vhod.hh"
#include "libs/outputs_inputs/outputs_inputs.hh"

#include "EEPROM.hh"
#include "audio-visual/includes/audio.hh"
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>
/************************************************************************/
/*							PROTOTYPES                                  */
/************************************************************************/
void Shutdown();
void Power_UP();
void audio_visual();
void spanje();
void power_switch_ev(uint8_t mode);
/************************************************************************/

enum enum_power_switch_modes
{
	INTERNAL = 0,
	EXTERNAL
};


/************************************************************************/

void power(void *paramOdTaska)
{
	
	/*		TIMER objects		*/
	class_TIMER power_up_delay_timer;  // Turns on the speaker if all conditions met for at least 2 seconds 
	class_TIMER sleep_timer;		   // Puts speaker to sleep after 500 ms ( prevents false immediate sleep due to 
	class_TIMER voltage_read_timer;
	
	/*		INPUT objects		*/
	class_VHOD stikalo(main_power_switch_pin, main_power_switch_port, 0);
	
	while (true)
	{ 
		/************************************************************************/
		/*							VOLTAGE READING                             */
		/************************************************************************/
		if (voltage_read_timer.vrednost() > 200)
		{
			m_Hardware.battery_voltage = readANALOG(vDIV_pin) *	(double) adc_milivolt_ref/1023.00;
			voltage_read_timer.ponastavi();
		}
		/************************************************************************/
		/*							POWER UP							        */
		/************************************************************************/
		
		if ( !m_Hardware.status_reg.powered_up && (BATTERY_VOLTAGE_PERCENT > 8 || m_Hardware.status_reg.external_power) && power_up_delay_timer.vrednost() >= 2000 )
		{ // Elapsed 2000 ms, not overheated, enough power or (already switched to)external power and not already powered up
			Power_UP();
			power_up_delay_timer.ponastavi();
		}			
		
		
		/************************************************************************/
		/*						SHUT DOWN ( & SLEEP )	                        */
		/************************************************************************/
		
		if (!stikalo.vrednost() || ( BATTERY_VOLTAGE_PERCENT < 5 && !m_Hardware.status_reg.external_power) )
		{
			power_up_delay_timer.ponastavi();
			if (m_Hardware.status_reg.powered_up)
			{
				Shutdown();
			}
			
			if (!m_Hardware.status_reg.external_power && m_Hardware.battery_voltage > 0 && sleep_timer.vrednost() > 500) // Prevents sleeping on startup due to input timed filtering
			{
				delayFREERTOS(500);
				sleep_timer.ponastavi();
				spanje();
			}
		}
		
	
		/************************************************************************/
		/*							POWER SWITCH						        */
		/************************************************************************/
		
		if (m_napajalnik.vrednost() && !m_Hardware.status_reg.external_power)
		{
			power_switch_ev(EXTERNAL);
		}

		else if (m_napajalnik.vrednost() == 0 && m_Hardware.status_reg.external_power)
		{
			power_switch_ev(INTERNAL);
		}
		/************************************************************************/
		delayFREERTOS(20);
	}
}

void Shutdown()
{
	m_audio_system.stripOFF();
	writeOUTPUT(_12V_line_pin, _12V_line_port, 0);					 
	writeOUTPUT(main_mosfet_pin, main_mosfet_port , 0);
	m_Hardware.status_reg.powered_up = 0;
}

void Power_UP()
{
	writeOUTPUT(_12V_line_pin, _12V_line_port, 1);				
	writeOUTPUT(main_mosfet_pin, main_mosfet_port, 1);
	m_audio_system.stripON();
	m_Hardware.status_reg.powered_up = 1; 
	delayFREERTOS(200);
}


void power_switch_ev(uint8_t mode)
{
	switch(mode)
	{
		case EXTERNAL:
			Shutdown();
			delayFREERTOS(25);
			writeOUTPUT(menjalnik_pin,menjalnik_port,1);
			m_Hardware.status_reg.external_power = 1;
			delayFREERTOS(200);
		break;
		
		case INTERNAL:
			Shutdown();
			delayFREERTOS(25);
			writeOUTPUT(menjalnik_pin,menjalnik_port, 0);
			m_Hardware.status_reg.external_power = 0;
			delayFREERTOS(200);
		break;
	}
}


void bujenje()
{ 
	sleep_disable();
	PCICR = 0;
	PCIFR = 0;
	PCMSK2 = 0;
	m_Hardware.status_reg.sleeping = 0;
	_delay_ms(50);
}

void spanje()
{
	asm ("sei");
	PCICR = (1 << PCIE2);	 // Vklopi PCINT interrupt
	PCIFR = (1 << PCIF2);	 // Vlopi ISR
	PCMSK2 = (1 << PCINT17); //Vklopi vector na 17
	m_Hardware.status_reg.sleeping = 1;
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_cpu();
}