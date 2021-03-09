#include "../includes/includes.h"
#include "VHOD/Vhod.h"
#include "Arduino_FreeRTOS.h"
#include "castimer/castimer.h"

extern VHOD napajalnik;

void polnjenje(void *paramOdTaska)
{

	while (true)
	{
		xSemaphoreTake(voltage_SEM, portMAX_DELAY);

		if (Hardware.napetost >= 4150 && Hardware.POLKONC == 0)
		{
			Hardware.POLKONC = 1;

			//EEPROM.update(battery_eeprom_addr, Hardware.POLKONC); //Posodobitev EEPROM-a na bajtu 1 z spremenljivko Hardware.POLKONC; Na vsake 5000 pisanj zamenja bajt na katerega piše
		}

		else if (Hardware.napetost <= 4000 && Hardware.POLKONC)
		{ //Če se dokonca napolne, se bo polnjenje lahko nadaljevalo šele, ko se baterija izprazne za približno 10% (3V = 0%, 4.2V = 100%, 4.1V = 90% . 3.95V = 80% oz. 10% manj ;  napetost = 0.012 * procent + 3);
			Hardware.POLKONC = 0;

			//EEPROM.update(battery_eeprom_addr, Hardware.POLKONC);
		}

		xSemaphoreGive(voltage_SEM);
		

		if ((Hardware.POLKONC == 1 || Hardware.AMP_oheat || napajalnik.vrednost() == 0) && Hardware.polnjenjeON)
		{
			writeOUTPUT(PIN6,'D',0);
			Hardware.polnjenjeON = false;
		}

		else if (Hardware.POLKONC == 0 && napajalnik.vrednost() && Hardware.AMP_oheat == false && !Hardware.polnjenjeON)
		{
			delay_FRTOS(1000);
			writeOUTPUT(PIN6,'D',1);
			Hardware.polnjenjeON = true;
		}
		delay_FRTOS(100);
	}
}