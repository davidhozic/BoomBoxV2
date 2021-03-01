
#ifndef AUDIO_H
#define AUDIO_H

#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "../BARVE/barve.h"
#define BARVA *((byte *)B)
#define r_trak 9
#define z_trak 3
#define m_trak 11
#define mic_pin A0
#define tr_r AUSYS_vars.TR_BARVA[0]
#define tr_z AUSYS_vars.TR_BARVA[1]
#define tr_m AUSYS_vars.TR_BARVA[2]
#define tr_bright AUSYS_vars.tr_svetlost
#define barv_razlika_cond_true abs(mozne_barve.barvni_ptr[BARVA][0] - tr_r) > 5 || abs(mozne_barve.barvni_ptr[BARVA][1] - tr_z) > 5 || abs(mozne_barve.barvni_ptr[BARVA][2] - tr_m) > 5
#define povprecna_glasnost AUSYS_vars.povprecna_glas
#define frekvenca AUSYS_vars.frek
#define trenutni_audio_mode AUSYS_vars.A_mode

void delete_AVDIO_subTASK(int doNotDelete);
void turnOFFstrip();
void writeTRAK();
void color_fade_funct(byte *B);
void svetlost_mod_funct(byte *svetlost, byte smer);
void mic_mode_change();
void audio_mode_change(char *ch);
extern TaskHandle_t fade_control;
extern TaskHandle_t color_fade_control;
extern TaskHandle_t Mixed_fade_control;
extern TaskHandle_t Breathe_control;

void fade_task(void *B);
void Color_Fade_task(void *B);
void Fade_Breathe_Task(void *B);
void Mesan_fade_task(void *b);

enum mic_detection_mode
{
    Average_volume, // Meri povprecno glasnost
    Frequency_mode, // Hardware.frekvenca
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
    OFF_A
};

enum control_blocks{ //doNotDelete
    dont_fade_delete,
    dont_color_fade_delete,
    dont_Mixed_fade_delete,
    dont_Breathe_delete,
    Delete_ALL
};


struct adsys
{
    int mic_mode = Frequency_mode;
    byte A_mode = OFF_A;
    float TR_BARVA[3] = {0, 0, 0}; //Trenutna barva traku RGB
    int povprecna_glas = 0;
    int frek = 0;
    byte tr_svetlost = 0;
};
extern adsys AUSYS_vars;




#endif