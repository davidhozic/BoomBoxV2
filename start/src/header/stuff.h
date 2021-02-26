#include "castimer.h"

#ifndef namespaces_H
#define namespaces_H


extern bool is_Powered_UP;
extern float napetost;
extern bool PSW;
extern bool POLKONC;
extern bool polnjenjeON;
extern bool AMP_oheat;
extern bool display_enabled;
extern int povprecna_glasnost;
extern int frekvenca;
extern float Amplifier_temp;


extern castimer stikaloCAS;
extern castimer timerL1;
extern castimer timerL2;
extern castimer timerL3;
extern castimer lucke_filter_time;
extern castimer timer_frek_meri;
extern castimer BATCHARGE_T;
extern castimer stikaloOFFtime;

enum mic_detection_mode
{
    Average_volume, // Meri povprecno glasnost
    Frequency_mode, // frekvenca
    LENGHT_1
};

enum audio_mode
{

    NORMAL_FADE,
    COLOR_FADE,
    MIXED_FADE,
    Direct_signal, //Signal iz mikrofona -> lucke
    LENGTH_2,
    OFF
};

#endif