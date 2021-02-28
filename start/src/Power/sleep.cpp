#include <Arduino.h>
#include "C:\Program Files (x86)\Arduino\hardware\tools\avr\avr\include\avr\sleep.h"
#include "../includes/includes.h"
#include <Arduino_FreeRTOS.h>

void bujenje()
{
  detachInterrupt(0);
  sleep_disable();
  taskEXIT_CRITICAL();
}

void spanje()
{
  taskENTER_CRITICAL();
  //Ko je zvočnik premalo napolnjen, gre v spanje da prepreči škodo kapaciteti baterij
  attachInterrupt(0, bujenje, RISING); //prekinitvena funkcija v procesorju št. 0 (pin 2)
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
}