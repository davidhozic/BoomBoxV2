
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\header\stuff.h"
#include "D:\Documents\Arduino\libraries\castimer\castimer.h"

bool is_Powered_UP = false;
float napetost = 0.0;
bool PSW = false;
bool POLKONC = false;
bool polnjenjeON = false;
bool AMP_oheat;
bool display_enabled = true;
int povprecna_glasnost = 0;
int frekvenca = 0;
float Amplifier_temp = 0;


castimer stikaloCAS;
castimer timerL1;
castimer timerL2;
castimer timerL3;
castimer lucke_filter_time;
castimer timer_frek_meri;
castimer BATCHARGE_T;
castimer stikaloOFFtime;