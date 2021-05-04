#include "VHOD/Vhod.h"
#include "FreeRTOS.h"
#include "castimer/castimer.h"
#include "common/inc/global.h"
#include "semphr.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "libs/EEPROM/EEPROM.h"


void polnjenje(void *paramOdTaska)
{
	while (true)
	{
		if (Hardware.battery_voltage >= 4150 && Hardware.status_reg.charging_finished == 0)
		{
			Hardware.status_reg.charging_finished = 1;
			EEPROM.pisi(Hardware.status_reg.charging_finished, eeprom_addr_battery_stat); //Posodobitev EEPROM-a na bajtu 1 z spremenljivko readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN); Na vsake 5000 pisanj zamenja bajt na katerega piše
		}

		else if (Hardware.battery_voltage <= 4000 && Hardware.status_reg.charging_finished)
		{ //Če se dokonca napolne, se bo polnjenje lahko nadaljevalo šele, ko se baterija izprazne za približno 10% (3V = 0%, 4.2V = 100%, 4.1V = 90% . 3.95V = 80% oz. 10% manj ;  napetost = 0.012 * procent + 3);
			Hardware.status_reg.charging_finished = 0;
			EEPROM.pisi(Hardware.status_reg.charging_finished, eeprom_addr_battery_stat);
		}



		if ((Hardware.status_reg.charging_finished || napajalnik.vrednost() == 0) && Hardware.status_reg.charging_enabled)
		{
			writeOUTPUT(bat_charge_pin, bat_charge_port,0);
			Hardware.status_reg.charging_enabled = false;
		}

		else if (Hardware.status_reg.charging_enabled == 0 && napajalnik.vrednost() && !Hardware.status_reg.charging_finished)
		{
			writeOUTPUT(bat_charge_pin, bat_charge_port,1);
			Hardware.status_reg.charging_enabled = true;
		}
		delayFREERTOS(1);
	}
}