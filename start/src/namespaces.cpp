
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\header\namespaces.h"
#include "D:\Documents\Arduino\libraries\castimer\castimer.h"

bool Hardware::is_Powered_UP = false;
int Hardware::napetost = 0;
bool Hardware::PSW = false;
bool Hardware::POLKONC = false;
bool Hardware::polnjenjeON = false;
Errors Hardware::Napaka;
float Hardware::Amplifier_temp = 0.0f;
bool Hardware::display_enabled = true;
int Hardware::povprecna_glasnost = 0;
int Hardware::frekvenca = 0;

castimer TIMERS_folder::stikaloCAS;
castimer TIMERS_folder::timerL1;
castimer TIMERS_folder::timerL2;
castimer TIMERS_folder::timerL3;
castimer TIMERS_folder::lucke_filter_time;
castimer TIMERS_folder::timer_frek_meri;
castimer TIMERS_folder::BATCHARGE_T;
