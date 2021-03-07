#include <Arduino.h>
#include "C:\Program Files (x86)\Arduino\hardware\tools\avr\avr\include\avr\sleep.h"
#include "../includes/includes.h"
#include <Arduino_FreeRTOS.h>
#include <PinChangeInterrupt.h>
#include <PinChangeInterruptBoards.h>
#include <PinChangeInterruptPins.h>
#include <PinChangeInterruptSettings.h>

void bujenje()
{
  detachPinChangeInterrupt(16);
  sleep_disable();
  taskEXIT_CRITICAL();
}

void spanje()
{
  taskENTER_CRITICAL();
  //Ko je zvočnik premalo napolnjen, gre v spanje da prepreči škodo kapaciteti baterij
  attachPinChangeInterrupt(16, &bujenje, HIGH);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
}
