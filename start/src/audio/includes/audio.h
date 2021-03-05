
#ifndef AUDIO_H
#define AUDIO_H
#include "../../includes/settings.h"
#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "../BARVE/barve.h"
#define BARVA *((byte *)B)
#define r_trak 9
#define z_trak 3
#define m_trak 11
#define tr_r AUSYS_vars.TR_BARVA[0]
#define tr_z AUSYS_vars.TR_BARVA[1]
#define tr_m AUSYS_vars.TR_BARVA[2]
#define tr_bright AUSYS_vars.tr_svetlost
#define povprecna_glasnost AUSYS_vars.povprecna_glas
#define frekvenca AUSYS_vars.frek
#define trenutni_audio_mode AUSYS_vars.A_mode

#define brightUP() svetlost_mod_funct(1);
#define brightDOWN() svetlost_mod_funct(-1);
#define colorSHIFT(param) color_fade_funct((byte *)param);
#define flash_strip()          \
    free(AUSYS_vars.TR_BARVA); \
    memcpy(AUSYS_vars.TR_BARVA, mozne_barve.barvni_ptr[evnt_st.menu_seek], 3);\
for (uint8_t i = 0; i < 5; i++)\
{\
    digitalWrite(r_trak, 0);\
    digitalWrite(z_trak, 0);\
    digitalWrite(m_trak, 0);\
    delay(125);\
    digitalWrite(r_trak, 1);\
    digitalWrite(z_trak, 1);\
    digitalWrite(m_trak, 1);\
    delay(125);\
}
#define turnOFFstrip()                      \
    {                                       \
        AUSYS_vars.mikrofon_detect = false; \
        deleteALL_subAUDIO_tasks();         \
        brightDOWN();                       \
    }
extern TaskHandle_t fade_control;
extern TaskHandle_t color_fade_control;
extern TaskHandle_t Mixed_fade_control;
extern TaskHandle_t Breathe_control;

void holdALL_tasks();
void writeTRAK();
void color_fade_funct(byte *B);
void svetlost_mod_funct(int smer);
void mic_mode_change();
void audio_mode_change(char *ch);
void deleteALL_subAUDIO_tasks();
void create_audio_meritve(uint8_t *mode);

void fade_task(void *B);
void Color_Fade_task(void *B);
void Fade_Breathe_Task(void *B);
void Mesan_fade_task(void *b);

enum mic_detection_mode
{
    Average_volume, // Meri povprecno glasnost
    Frequency_mode, // Hardware.frekvenca
    LENGHT_1,
};

enum audio_mode
{

    NORMAL_FADE,
    COLOR_FADE,
    Fade_Breathe,
    Direct_signal, //Signal iz AUSYS_vars.mikrofon_detecta -> lucke
    LENGTH_2,
    OFF_A = 999
};

struct adsys
{
    uint8_t mic_mode = default_mic_mode;
    bool mikrofon_detect = false;
    byte A_mode = DEFAULT_Audio_Mode;
    short TR_BARVA[3] = {0, 0, 0}; //Trenutna barva traku RGB
    unsigned short povprecna_glas = 0;
    unsigned short frek = 0;
    short tr_svetlost = 0;
};
extern adsys AUSYS_vars;

#endif
