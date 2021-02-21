
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\Arduino.h"
#include "castimer.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\PolnenjeZvoc\code\start\src\header\namespaces.h"


void merjenje_frekvence(void *input)
{
  const int mic_pin = *((int*)input);
  int prejsnja_vrednost;
  unsigned long lokal_timer;
  while (1)
  {

    int trenutna_vrednost = analogRead(mic_pin);
    if (trenutna_vrednost > prejsnja_vrednost)
    {
      prejsnja_vrednost = trenutna_vrednost;
      lokal_timer = TIMERS_folder::timer_frek_meri.vrednost();
    }

    if (trenutna_vrednost - prejsnja_vrednost <= -10)
    {
      int cas_milisekunde = lokal_timer * 4;
      TIMERS_folder::timer_frek_meri.ponastavi();
      int cas_sekunde = cas_milisekunde / 1000;
      prejsnja_vrednost = trenutna_vrednost;
      Hardware::frekvenca = 1 / cas_sekunde;
    }
  }
}