#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\Arduino.h"
#include "C:\Program Files (x86)\Arduino\hardware\tools\avr\avr\include\avr/sleep.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\header\namespaces.h"
void bujenje()
{
  detachInterrupt(0);
  sleep_disable();
  _delay_ms(20);
}

void spanje()
{                                      //Ko je zvočnik premalo napolnjen, gre v spanje da prepreči škodo kapaciteti baterij
  attachInterrupt(0, bujenje, RISING); //prekinitvena funkcija v procesorju št. 0 (pin 2)
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
}
