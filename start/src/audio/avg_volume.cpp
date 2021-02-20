
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\Arduino.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\PolnenjeZvoc\code\start\src\header\namespaces.h"



int povprecna_glasnost(int mic_pin)
{
  static int max_vrednost = 0;
  int trenutna_vrednost = analogRead(mic_pin);
  static int count_branj = 0;
  static int vsota_branj = 0;
  static bool zabelezeno = false;
  if (trenutna_vrednost > max_vrednost)
  {

    if (trenutna_vrednost - max_vrednost >= 3)
    {
      zabelezeno = false;
    }
    max_vrednost = trenutna_vrednost;
  }

  else if ((max_vrednost - trenutna_vrednost) >= 3)
  {
    if (zabelezeno == false)
    {
      vsota_branj += max_vrednost;
      count_branj += 1;
      zabelezeno = true;
    }
    max_vrednost = trenutna_vrednost;
  }

  if (count_branj == 50)
  {
    int tmp = vsota_branj / count_branj;
    count_branj = 0;
    vsota_branj = 0;
    return tmp;
  }

  else
  {
    return -1;
  }
}
