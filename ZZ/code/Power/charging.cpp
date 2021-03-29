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


		if (Hardware.battery_voltage >= 4150 && readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN) == 0)
		{
			writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN, 1);

			EEPROM.pisi(battery_eeprom_addr, readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN)); //Posodobitev EEPROM-a na bajtu 1 z spremenljivko readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN); Na vsake 5000 pisanj zamenja bajt na katerega piše
		}

		else if (Hardware.battery_voltage <= 4000 && readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN))
		{ //Če se dokonca napolne, se bo polnjenje lahko nadaljevalo šele, ko se baterija izprazne za približno 10% (3V = 0%, 4.2V = 100%, 4.1V = 90% . 3.95V = 80% oz. 10% manj ;  napetost = 0.012 * procent + 3);
			writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN,0) ;

			EEPROM.pisi(battery_eeprom_addr, readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN));
		}



		if ((readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN) == 1  || napajalnik.vrednost() == 0) && readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN))
		{
			writeOUTPUT(bat_charge_pin, bat_charge_port,0);
			writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN, 0);
		}

		else if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN) == 0 && napajalnik.vrednost() && !readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN))
		{
			delayFREERTOS(1000);
			writeOUTPUT(bat_charge_pin, bat_charge_port,1);
			writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN, 1);
		}
		delayFREERTOS(100);
	}
}