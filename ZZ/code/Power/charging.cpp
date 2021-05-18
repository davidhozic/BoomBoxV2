#include "VHOD/Vhod.h"
#include "FreeRTOS.h"
#include "castimer/castimer.h"
#include "common/inc/global.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "libs/EEPROM/EEPROM.h"
#include "common/inc/global_inputs.h"

void polnjenje(void *paramOdTaska)
{
	while (true)
	{
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


		if (m_Hardware.status_reg.charging_enabled && (m_Hardware.status_reg.charging_finished || m_napajalnik.vrednost() == 0))
		{
			writeOUTPUT(bat_charge_pin, bat_charge_port,0);
			m_Hardware.status_reg.charging_enabled = 0;
		}

		else if (m_Hardware.status_reg.charging_enabled == 0 && m_napajalnik.vrednost() && !m_Hardware.status_reg.charging_finished)
		{
			writeOUTPUT(bat_charge_pin, bat_charge_port,1);
			m_Hardware.status_reg.charging_enabled = 1;
		}
		delayFREERTOS(1);
	}
}