#include "D:\Documents\Arduino\libraries\castimer\castimer.h"
#include "Arduino.h"

#ifndef namespaces_H
#define namespaces_H
#define r_trak 9
#define z_trak 3
#define m_trak 11
#define mic_pin A0
#define tr_r Hardware.AUSYS_vars.TR_BARVA[0]
#define tr_z Hardware.AUSYS_vars.TR_BARVA[1]
#define tr_m Hardware.AUSYS_vars.TR_BARVA[2]
#define tr_bright Hardware.AUSYS_vars.tr_svetlost
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
        float TR_BARVA[3] = {0, 0, 0}; //Trenutna barva traku RGB
        int povprecna_glasnost;
        int frekvenca;
        byte tr_svetlost = 0;
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
    castimer brightness_timer;
    castimer color_timer;
};
extern timer_struct Timers;

#endif
