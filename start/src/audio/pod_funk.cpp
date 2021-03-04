#include <Arduino_FreeRTOS.h>
#include "includes/audio.h"
#include "../includes/includes.h"
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\EEPROM\src\EEPROM.h"
/**************************************************************************************************************************
*                                                                                                                         *
*                                                           Pomozne funkcije                                              *
*                                                                                                                         *
**************************************************************************************************************************/

void holdALL_tasks() // Zbrise obstojece taske ce obstajajo
{
    holdTASK(fade_control);
    holdTASK(color_fade_control);
    holdTASK(Mixed_fade_control);
    holdTASK(Breathe_control);
}

void deleteALL_tasks()
{
    deleteTask(fade_control);
    deleteTask(color_fade_control);
    deleteTask(Mixed_fade_control);
    deleteTask(Breathe_control);
    delay_FRTOS(15);
}

void turnOFFstrip()
{
    AUSYS_vars.mikrofon_detect = false;
    deleteALL_tasks();
    digitalWrite(r_trak, 0);
    digitalWrite(z_trak, 0);
    digitalWrite(m_trak, 0);
    tr_bright = 0;
}

void writeTRAK()
{
    analogWrite(r_trak, (float)tr_r * (float)tr_bright / 255.00);
    analogWrite(z_trak, (float)tr_z * (float)tr_bright / 255.00);
    analogWrite(m_trak, (float)tr_m * (float)tr_bright / 255.00);
}

void color_fade_funct(byte *B)
{
    if (Mixed_fade_control == NULL)
        tr_bright = 255;
    while (barv_razlika_cond_true)
    {
        char smer[3] = {0, 0, 0};
        mozne_barve.barvni_ptr[*B][0] >= tr_r ? smer[0] = 1 : smer[0] = -1;
        mozne_barve.barvni_ptr[*B][1] >= tr_z ? smer[1] = 1 : smer[1] = -1;
        mozne_barve.barvni_ptr[*B][2] >= tr_m ? smer[2] = 1 : smer[2] = -1;

        tr_r = tr_r + (10 * smer[0]);
        tr_z = tr_z + (10 * smer[1]);
        tr_m = tr_m + (10 * smer[2]);

        tr_r = tr_r < 0 ? 0 : tr_r;
        tr_z = tr_z < 0 ? 0 : tr_z;
        tr_m = tr_m < 0 ? 0 : tr_m;

        tr_r = tr_r > 255 ? 255 : tr_r;
        tr_z = tr_z > 255 ? 255 : tr_z;
        tr_m = tr_m > 255 ? 255 : tr_m;

        writeTRAK();
        delay(12);
    }
}

void svetlost_mod_funct(int smer)
{
    #define while_cond (smer > 0 ? tr_bright < 255 : tr_bright > 0)

    while (while_cond)
    {
        tr_bright += 8 * smer; //8 stopenj = priblizno 500ms na fade oz 1000ms na breathe (za polni fade)
        tr_bright = tr_bright < 0 ? 0 : tr_bright;
        tr_bright = tr_bright > 255 ? 255 : tr_bright;
        writeTRAK();
    }
}

void mic_mode_change() // Switches audio mode ; 1s hold
{
    holdTASK(audio_system_control);
    int ct = 0, delay_switch = 300;

    AUSYS_vars.mic_mode = (AUSYS_vars.mic_mode + 1) % mic_detection_mode::LENGHT_1;

    digitalWrite(r_trak, 1);
    digitalWrite(z_trak, 0);
    digitalWrite(m_trak, 0);
    delay_FRTOS(700);

    while (ct < (AUSYS_vars.mic_mode + 1) * 2) // n+1 blink = n+1 audio_mode
    {
        PORTB ^= (1 << 1);                  //R
        PORTD ^= (1 << 3);                  //G
        PORTB ^= (1 << 3);                  //B
        delay_switch = 1000 - delay_switch; // Switches between 300ms and 700ms
        delay_FRTOS(delay_switch);
        ct++;
    }

    PORTB &= ~(1 << 1); //R
    PORTD &= ~(1 << 3); //G
    PORTB &= ~(1 << 3); //B
    create_audio_meritve(&AUSYS_vars.mic_mode);
    resumeTASK(audio_system_control);
}

void audio_mode_change(char *ch) // Double click
{
    holdTASK(audio_system_control);
    turnOFFstrip();

    delay_FRTOS(300);

    if (ch == "off")
        trenutni_audio_mode = OFF_A;

    else if (trenutni_audio_mode == OFF_A)
        trenutni_audio_mode = NORMAL_FADE;

    else
        trenutni_audio_mode = (trenutni_audio_mode + 1) % audio_mode::LENGTH_2;

    deleteALL_tasks();
    if (trenutni_audio_mode != OFF_A)
        resumeTASK(audio_system_control);
}