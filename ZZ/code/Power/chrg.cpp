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
		xSemaphoreTake(voltage_SEM, portMAX_DELAY);

		if (Hardware.napetost >= 4150 && Hardware.POLKONC == 0)
		{
			Hardware.POLKONC = 1;

			EEPROM.pisi(battery_eeprom_addr, Hardware.POLKONC); //Posodobitev EEPROM-a na bajtu 1 z spremenljivko Hardware.POLKONC; Na vsake 5000 pisanj zamenja bajt na katerega piše
		}

		else if (Hardware.napetost <= 4000 && Hardware.POLKONC)
		{ //Če se dokonca napolne, se bo polnjenje lahko nadaljevalo šele, ko se baterija izprazne za približno 10% (3V = 0%, 4.2V = 100%, 4.1V = 90% . 3.95V = 80% oz. 10% manj ;  napetost = 0.012 * procent + 3);
			Hardware.POLKONC = 0;

			EEPROM.pisi(battery_eeprom_addr, Hardware.POLKONC);
		}

		xSemaphoreGive(voltage_SEM);
		

		if ((Hardware.POLKONC == 1 || Hardware.AMP_oheat || napajalnik.vrednost() == 0) && Hardware.polnjenjeON)
		{
			writeOUTPUT(bat_charge_pin, bat_charge_port,0);
			Hardware.polnjenjeON = false;
		}

		else if (Hardware.POLKONC == 0 && napajalnik.vrednost() && Hardware.AMP_oheat == false && !Hardware.polnjenjeON)
		{
			delayFREERTOS(1000);
			writeOUTPUT(bat_charge_pin, bat_charge_port,1);
			Hardware.polnjenjeON = true;
		}
		delayFREERTOS(100);
	}
}