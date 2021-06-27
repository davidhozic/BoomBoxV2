

#include "outputs_inputs.hh"
#include "global.hpp"
#include "castimer/castimer.hpp"
#include "input/input.hpp"
#include "events.hh"



struct POWER
{
	/*	   	INPUT_t objects		*/
	INPUT_t stikalo = INPUT_t(main_power_switch_pin, main_power_switch_port, 0);


	/*		TIMER_t objects		*/
	TIMER_t power_up_delay_timer;  // Turns on the speaker if all conditions met for at least 2 seconds
}m_power;



void power_task(void *p)
{
	while (true)
	{ 

		/************************************************************************/
		/*							POWER UP							        */
		/************************************************************************/
		
		/* No need to worry about the timer running because the first condition will always be false while speaker is enabled, meaning other elements won't execute in the if statement */
		if ( !m_Hardware.status_reg.powered_up && (BATTERY_VOLTAGE_PERCENT > 8 || m_Hardware.status_reg.external_power) && m_power.power_up_delay_timer.value() >= 2000 )
		{ // Elapsed 2000 ms, not overheated, enough power or (already switched to)external power and not already powered up
			system_event(EV_POWER_UP);
			 m_power.power_up_delay_timer.reset();
		}			
		
		/************************************************************************/
		/*							SHUT DOWN ( & SLEEP )	                    */
		/************************************************************************/
		
		if (!m_power.stikalo.value() || (m_Hardware.battery_voltage > 0 && BATTERY_VOLTAGE_PERCENT < 5 && !m_Hardware.status_reg.external_power) )
		{
			 m_power.power_up_delay_timer.reset();
			if (m_Hardware.status_reg.powered_up)
			{
				system_event(EV_SHUTDOWN);
			}
		}
		
	
		/************************************************************************/
		/*							POWER SWITCH						        */
		/************************************************************************/
		
		if (m_napajalnik.value() && !m_Hardware.status_reg.external_power)
		{
			system_event(EV_POWER_SWITCH_EXTERNAL);
		}

		else if (m_napajalnik.value() == 0 && m_Hardware.status_reg.external_power)
		{
			system_event(EV_POWER_SWITCH_INTERNAL);
		}
		/************************************************************************/
		delay_FreeRTOS_ms(100);
	}
}

