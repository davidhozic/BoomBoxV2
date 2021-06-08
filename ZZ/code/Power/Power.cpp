

#include "outputs_inputs.hh"
#include "global.hh"
#include "castimer/castimer.hh"
#include "input/input.hh"
#include "events.hh"


void power(void *paramOdTaska)
{
	/*		TIMER objects		*/
	class_TIMER power_up_delay_timer;  // Turns on the speaker if all conditions met for at least 2 seconds 
	class_TIMER voltage_read_timer	;

	/*	   	INPUT objects		*/
	class_INPUT stikalo(main_power_switch_pin, main_power_switch_port, 0);

	while (true)
	{ 
		/************************************************************************/
		/*							VOLTAGE READING                             */
		/************************************************************************/
		if (voltage_read_timer.value() > 200)
		{
			m_Hardware.battery_voltage = readANALOG(vDIV_pin) *	(double) adc_milivolt_ref/1023.00;
			voltage_read_timer.reset();
		}
		/************************************************************************/
		/*							POWER UP							        */
		/************************************************************************/
		
		/* No need to worry about the timer running because the first condition will always be false while speaker is enabled, meaning other elements won't execute in the if statement */
		if ( !m_Hardware.status_reg.powered_up && (BATTERY_VOLTAGE_PERCENT > 8 || m_Hardware.status_reg.external_power) && power_up_delay_timer.value() >= 2000 )
		{ // Elapsed 2000 ms, not overheated, enough power or (already switched to)external power and not already powered up
			system_event(POWER_UP);
			power_up_delay_timer.reset();
		}			
		
		
		/************************************************************************/
		/*							SHUT DOWN ( & SLEEP )	                    */
		/************************************************************************/
		
		if (!stikalo.value() || (m_Hardware.battery_voltage > 0 && BATTERY_VOLTAGE_PERCENT < 5 && !m_Hardware.status_reg.external_power) )
		{
			power_up_delay_timer.reset();
			if (m_Hardware.status_reg.powered_up)
			{
				system_event(SHUTDOWN);
			}
		}
		
	
		/************************************************************************/
		/*							POWER SWITCH						        */
		/************************************************************************/
		
		if (m_napajalnik.value() && !m_Hardware.status_reg.external_power)
		{
			system_event(POWER_SWITCH_EXTERNAL);
		}

		else if (m_napajalnik.value() == 0 && m_Hardware.status_reg.external_power)
		{
			system_event(POWER_SWITCH_INTERNAL);
		}
		/************************************************************************/
		delayFREERTOS(100);
	}
}

