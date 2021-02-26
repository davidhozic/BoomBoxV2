#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\header\stuff.h"
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\EEPROM\src\EEPROM.h"
#include "Vhod.h"
#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "castimer.h"
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\Arduino.h"

extern VHOD napajalnik;

void polnjenje(void *paramOdTaska)
{
  while (true)
  {
    if (napetost >= 4015 && POLKONC == 0)
    {
      POLKONC = 1;
      EEPROM.update(5, POLKONC); //Posodobitev EEPROM-a na bajtu 1 z spremenljivko POLKONC; Na vsake 5000 pisanj zamenja bajt na katerega piše
    }

    else if (napetost <= 4000 && POLKONC) // For charging to continue it needs to discharge atleast 4% after full charge
    {                                                         //Če se dokonca napolne, se bo polnjenje lahko nadaljevalo šele, ko se baterija izprazne za približno 10% (3V = 0%, 4.2V = 100%, 4.1V = 90% . 3.95V = 80% oz. 10% manj ;  napetost = 0.012 * procent + 3);
      POLKONC = 0;                                  //Poenostavi se spremenljivka, zato da se v zgornjem pogoju vključi polnenje.
      EEPROM.update(5, POLKONC);
    }

    if ((POLKONC == 1 || AMP_oheat || napajalnik.vrednost() == 0) && polnjenjeON)
    {
      PORTD &= ~(1 << PD6);
      polnjenjeON = false;
    }

    else if (POLKONC == 0 && napajalnik.vrednost() && AMP_oheat == false && !polnjenjeON)
    {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      PORTD |= (1 << PD6);
      polnjenjeON = true;
    }
  }
}
