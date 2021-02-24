
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\Arduino.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\PolnenjeZvoc\code\start\src\header\namespaces.h"

void povprecna_glasnost(void *input)
{
  const int mic_pin = *((int *)input);
  int max_vrednost = 0;
  int count_branj = 0;
  int vsota_branj = 0;
  uint8_t count_above_average = 0;
  bool previosly_risen = false;

  while (1)
  {
    int trenutna_vrednost = analogRead(mic_pin);
    if (trenutna_vrednost > max_vrednost)
    {
      max_vrednost = trenutna_vrednost;
    }

    else if ((max_vrednost - trenutna_vrednost) >= 3)
    {
      if (previosly_risen)
      {

        if (max_vrednost - Hardware::povprecna_glasnost < 300 || count_above_average >= 10)
        { /* Vrednost se ni bila zabelezena in glasnost ni porastla za vec kot 300 ADC oz. je porastla vec kot 10x */
          vsota_branj += max_vrednost;
          count_branj += 1;
        }
        else
        {
          count_above_average++; //Filtrira spice v glasnosti
        }

        previosly_risen = false;
      }

      max_vrednost = trenutna_vrednost;
    }

    if (count_branj == 200)
    {
      Hardware::povprecna_glasnost = vsota_branj / count_branj;
      count_branj = 0;
      vsota_branj = 0;
      count_above_average = 0;
    }
  }
}
