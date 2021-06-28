#include "input/input.hpp"
#include "FreeRTOS.h"
#include "castimer/castimer.hpp"
#include "common/inc/global.hpp"
#include "libs/outputs_inputs/outputs_inputs.hpp"
#include "libs/EEPROM/EEPROM.hpp"


void charging_task(void *p)
{
	while (true)
	{
	
		/************************************************************************/
		/*							VOLTAGE READING                             */
		/************************************************************************/
	
		m_Hardware.battery_voltage = readANALOG(vDIV_pin) *	(double) adc_milivolt_ref/1023.00;
		
		if (m_Hardware.status_reg.charging_finished == 0 && BATTERY_VOLTAGE_PERCENT >= 100)
		{
			m_Hardware.status_reg.charging_finished = 1;
			EEPROM.pisi(m_Hardware.status_reg.charging_finished, EEPROM_ADDRESS_BATTERY_CHARGING_STATUS); //Posodobitev EEPROM-a na bajtu 1 z spremenljivko readBIT(m_Hardware.status_reg, m_Hardware_STATUS_REG_CHARGING_EN); Na vsake 5000 pisanj zamenja bajt na katerega piše
		}

		else if (m_Hardware.status_reg.charging_finished && BATTERY_VOLTAGE_PERCENT <= 100 - polnilna_histereza)
		{	/* Histereza */
			m_Hardware.status_reg.charging_finished = 0;
			EEPROM.pisi(m_Hardware.status_reg.charging_finished, EEPROM_ADDRESS_BATTERY_CHARGING_STATUS);
		}


		if (m_Hardware.status_reg.charging_enabled && (m_Hardware.status_reg.charging_finished || m_napajalnik.value() == 0))
		{
			writeOUTPUT(bat_charge_pin, bat_charge_port,0);
			m_Hardware.status_reg.charging_enabled = 0;
		}

		else if (m_Hardware.status_reg.charging_enabled == 0 && m_napajalnik.value() && !m_Hardware.status_reg.charging_finished)
		{
			writeOUTPUT(bat_charge_pin, bat_charge_port,1);
			m_Hardware.status_reg.charging_enabled = 1;
		}
		delay_FreeRTOS_ms(250);
	}
}
