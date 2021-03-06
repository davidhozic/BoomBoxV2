
#ifndef AUDIO_H
#define AUDIO_H
#include "../../includes/settings.h"
#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "../BARVE/barve.h"
#include <semphr.h>


/*********************************/
/*             Makro             */
/*********************************/
#define BARVA *((uint8_t *)B)
#define r_trak 9
#define z_trak 3
#define m_trak 11
#define tr_r AUSYS_vars.TR_BARVA[0]
#define tr_z AUSYS_vars.TR_BARVA[1]
#define tr_m AUSYS_vars.TR_BARVA[2]
#define tr_bright AUSYS_vars.tr_svetlost
#define povprecna_glasnost AUSYS_vars.meritve.povprecna_glas
#define frekvenca AUSYS_vars.meritve.frek
#define trenutni_audio_mode AUSYS_vars.STRIP_MODE
#define brightUP() svetlost_mod_funct(1);
#define brightDOWN() svetlost_mod_funct(-1);
#define colorSHIFT(param) color_fade_funct((uint8_t *)param);
#define turnOFFstrip()          \
    deleteALL_subAUDIO_tasks(); \
    brightDOWN();
/********************************************************************/

extern TaskHandle_t fade_control;
extern TaskHandle_t color_fade_control;
extern TaskHandle_t Mixed_fade_control;
extern TaskHandle_t Breathe_control;
extern SemaphoreHandle_t Meritveni_semafor;
extern TaskHandle_t Direct_signal_control;
/*********************************************/
/*         Prototipi pomoznih funkcij        */
/*********************************************/
void holdALL_tasks();
void writeTRAK();
void color_fade_funct(uint8_t *B);
void svetlost_mod_funct(int smer);
void mic_mode_change();
void audio_mode_change(char *ch);
void deleteALL_subAUDIO_tasks();
void flash_strip(uint8_t x);
/*********************************************/

/*********************************************/
/*             Prototipi taskov              */
/*********************************************/
void fade_task(void *B);
void Color_Fade_task(void *B);
void Fade_Breathe_Task(void *B);
void Direct_mic_Task(void*);
/*********************************************/

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
    Direct_signal, //Signal iz mikrofon_detecta -> lucke
    LENGTH_2,
    OFF_A
};

struct adsys_t
{
    mic_detection_mode mic_mode = Average_volume;
    audio_mode STRIP_MODE = NORMAL_FADE;
    short TR_BARVA[3] = {0, 0, 0}; //Trenutna barva traku RGB

    struct Meritve_t
    {
        unsigned short povprecna_glas = 0;
        unsigned short frek = 0;
    } meritve;

    short tr_svetlost = 0;
};
extern adsys_t AUSYS_vars;

#endif
