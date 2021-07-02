
#include "common.hpp"
#include "events.hpp"
#include "input.hpp"
#include "castimer.hpp"
#include "outputs_inputs.hpp"
#include "EEPROM.hpp"
#include "power.hpp"


/**    Structs    **/
struct POWER_t
{  
	/*	   	INPUT_t objects		*/
	INPUT_t switch_pwr = INPUT_t(GLOBAL_CFG_PIN_MAIN_POWER_SWITCH, GLOBAL_CFG_PORT_MAIN_POWER_SWITCH, 0);
    INPUT_t napajalnik = INPUT_t(GLOBAL_CFG_PIN_21V_PSU, GLOBAL_CFG_PORT_21V_PSU, 0);

	/*		TIMER_t objects		*/
	TIMER_t power_up_delay_timer;  // Turns on the speaker if all conditions met for at least 2 seconds
    
    /*      Other        */
    uint16_t battery_voltage;   // Battery voltage in milivolts
}m_power;



void power_task(void *p)
{
    for (;;)
    {
        m_power.battery_voltage = readANALOG(GLOBAL_CFG_PIN_VOLTAGE_DIV) * (5000.00/1023);
        /* No need to worry about the timer running because the first condition will always be false while speaker is enabled, meaning other elements won't execute in the if statement */
        if ( !m_hw_status.powered_up && (BATTERY_VOLTAGE_PERCENT(m_power.battery_voltage) > 8 || m_hw_status.external_power) && m_power.power_up_delay_timer.value() >= 2000 )
        { // Elapsed 2000 ms, not overheated, enough power or (already switched to)external power and not already powered up
            system_event(EV_POWER_UP);
            m_power.power_up_delay_timer.reset();
        }			
        
        
        if (!m_power.switch_pwr.value() || (BATTERY_VOLTAGE_PERCENT(m_power.battery_voltage) > 0 && BATTERY_VOLTAGE_PERCENT(m_power.battery_voltage) < 5 && !m_hw_status.external_power) )
        {
            m_power.power_up_delay_timer.reset();
            if (m_hw_status.powered_up)
            {
                system_event(EV_SHUTDOWN);
            }
        }
        

        if (m_power.napajalnik.value() && !m_hw_status.external_power)
        {
            system_event(EV_POWER_SWITCH_EXTERNAL);
        }

        else if (m_power.napajalnik.value() == 0 && m_hw_status.external_power)
        {
            system_event(EV_POWER_SWITCH_INTERNAL);
        }


        
        if (m_hw_status.charging_finished == 0 && BATTERY_VOLTAGE_PERCENT(m_power.battery_voltage) >= 100)
        {
            m_hw_status.charging_finished = 1;
            EEPROM.pisi(m_hw_status.charging_finished, GLOBAL_CFG_EEPROM_ADDR_BATTERY_CHARGING_STATUS); //Posodobitev EEPROM-a na bajtu 1 z spremenljivko readBIT(m_hw_status, m_Hardware_STATUS_REG_CHARGING_EN); Na vsake 5000 pisanj zamenja bajt na katerega piše
        }

        else if (m_hw_status.charging_finished && BATTERY_VOLTAGE_PERCENT(m_power.battery_voltage) <= 100 - GLOBAL_CFG_CHARGE_HYSTERESIS)
        {	/* Histereza */
            m_hw_status.charging_finished = 0;
            EEPROM.pisi(m_hw_status.charging_finished, GLOBAL_CFG_EEPROM_ADDR_BATTERY_CHARGING_STATUS);
        }


        if (m_hw_status.charging_enabled && (m_hw_status.charging_finished || m_power.napajalnik.value() == 0))
        {
            writeOUTPUT(GLOBAL_CFG_PIN_CHARGE, GLOBAL_CFG_PORT_CHARGE,0);
            m_hw_status.charging_enabled = 0;
        }

        else if (m_hw_status.charging_enabled == 0 && m_power.napajalnik.value() && !m_hw_status.charging_finished)
        {
            writeOUTPUT(GLOBAL_CFG_PIN_CHARGE, GLOBAL_CFG_PORT_CHARGE,1);
            m_hw_status.charging_enabled = 1;
        }
        delay_FreeRTOS_ms(200);
    } 
}