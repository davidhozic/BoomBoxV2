#include "D:\Documents\Arduino\libraries\castimer\castimer.h"
#include "Arduino.h"

#ifndef namespaces_H
#define namespaces_H

enum mic_detection_mode
{
    Average_volume, // Meri povprecno glasnost
    Frequency_mode, // Hardware.AUSYS_vars.frekvenca
    LENGHT_1
};

enum audio_mode
{

    NORMAL_FADE,
    COLOR_FADE,
    MIXED_FADE,
    Fade_Breathe,
    Direct_signal, //Signal iz mikrofon_detecta -> lucke
    LENGTH_2,
    OFF
};

struct hardware_struct
{
    struct adsys
    {
        int mic_mode = Frequency_mode;
        int A_mode = OFF;
        const int mic_pin = A0;
        const int rdeca_LED_trak = 9;  //Rdeča lučka je na pinu 9
        const int zelena_LED_trak = 3; //Zelena lučka je na pinu 3
        const int modra_LED_trak = 11; //Modra lučka je na pinu 11
        float TR_BARVA[3] = {0, 0, 0}; //Trenutna barva traku RGB
        int povprecna_glasnost;
        int frekvenca;
    } AUSYS_vars;

    const short REF_VOLT = 5000; //Referenčna napetost na ADC (4.999V - 5.002V)
    bool is_Powered_UP = 0;
    bool PSW = 0;
    bool POLKONC;
    bool polnjenjeON;
    bool AMP_oheat = 0;
    bool display_enabled = 1;
    float Amplifier_temp = 0;
    float napetost = 0;
};
extern hardware_struct Hardware;

struct timer_struct
{
    castimer stikaloCAS;
    castimer lucke_filter_time;
    castimer timer_frek_meri;
    castimer LCD_timer;
    castimer stikaloOFFtime;
    castimer VOLT_timer;
};
extern timer_struct Timers;

#endif