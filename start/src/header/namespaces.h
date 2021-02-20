#include "castimer.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\PolnenjeZvoc\code\start\src\header\Errors.h"

#ifndef namespaces_H
#define namespaces_H
namespace Hardware
{
     extern bool is_Powered_UP;
     extern int napetost;
     extern bool PSW;
     extern bool POLKONC;
     extern bool polnjenjeON;
     extern Errors Napaka;
     extern float Amplifier_temp;
     extern bool display_enabled;
     struct evnT;
     extern evnT event;
} // namespace Hardware

namespace TIMERS_folder
{
     extern castimer stikaloCAS;
     extern castimer timerL1;
     extern castimer timerL2;
     extern castimer timerL3;
     extern castimer lucke_filter_time;
     extern castimer timer_frek_meri;
     extern castimer BATCHARGE_T;
} // namespace TIMERS_folder

#endif