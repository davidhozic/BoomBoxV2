#include "../includes/includes.h"
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\EEPROM\src\EEPROM.h"
#include "Vhod.h"
#include <Arduino_FreeRTOS.h>
#include "castimer.h"
#include <Arduino.h>

extern VHOD napajalnik;

void polnjenje(void *paramOdTaska)
{

  while (true)
  {
    if (Hardware.napetost >= 4.15 && Hardware.POLKONC == 0)
    {
      Hardware.POLKONC = 1;
      EEPROM.update(battery_eeprom_addr, Hardware.POLKONC); //Posodobitev EEPROM-a na bajtu 1 z spremenljivko Hardware.POLKONC; Na vsake 5000 pisanj zamenja bajt na katerega piše
    }

    else if (Hardware.napetost <= 4.0 && Hardware.POLKONC) // For charging to continue it needs to discharge atleast 4% after full charge
    {                                                   //Če se dokonca napolne, se bo polnjenje lahko nadaljevalo šele, ko se baterija izprazne za približno 10% (3V = 0%, 4.2V = 100%, 4.1V = 90% . 3.95V = 80% oz. 10% manj ;  napetost = 0.012 * procent + 3);
      Hardware.POLKONC = 0;                             //Poenostavi se spremenljivka, zato da se v zgornjem pogoju vključi polnenje.
      EEPROM.update(battery_eeprom_addr, Hardware.POLKONC);
    }

    if ((Hardware.POLKONC == 1 || Hardware.AMP_oheat || napajalnik.vrednost() == 0) && Hardware.polnjenjeON)
    {
      PORTD &= ~(1 << PD6);
      Hardware.polnjenjeON = false;
    }

    else if (Hardware.POLKONC == 0 && napajalnik.vrednost() && Hardware.AMP_oheat == false && !Hardware.polnjenjeON)
    {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      PORTD |= (1 << PD6);
      Hardware.polnjenjeON = true;
    }

  }
}
