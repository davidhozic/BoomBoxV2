#include <Arduino_FreeRTOS.h>
#include "includes/audio.h"
#include "../includes/includes.h"
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\EEPROM\src\EEPROM.h"
/**************************************************************************************************************************
*                                                                                                                         *
*                                                           Pomozne funkcije                                              *
*                                                                                                                         *
**************************************************************************************************************************/

void delete_AVDIO_subTASK() // Zbrise obstojece taske ce obstajajo
{
    if (fade_control != NULL)
    {
        vTaskDelete(fade_control);
        fade_control = NULL;
    }

    if (color_fade_control != NULL)
    {
        vTaskDelete(color_fade_control);
        color_fade_control = NULL;
    }
    if (Mixed_fade_control != NULL)
    {
        vTaskDelete(Mixed_fade_control);
        Mixed_fade_control = NULL;
    }
    if (Breathe_control != NULL)
    {
        vTaskDelete(Breathe_control);
        Breathe_control = NULL;
    }
}

void turnOFFstrip()
{
    taskENTER_CRITICAL();
    AUSYS_vars.mikrofon_detect = false;
    delete_AVDIO_subTASK();
    digitalWrite(r_trak, 0);
    digitalWrite(z_trak, 0);
    digitalWrite(m_trak, 0);
    tr_r = 0;
    tr_z = 0;
    tr_m = 0;
    tr_bright = 0;
    taskEXIT_CRITICAL();
}

void writeTRAK()
{
    taskENTER_CRITICAL();
    analogWrite(r_trak, (float)tr_r * tr_bright / 255.00);
    analogWrite(z_trak, (float)tr_z * tr_bright / 255.00);
    analogWrite(m_trak, (float)tr_m * tr_bright / 255.00);
    taskEXIT_CRITICAL();
}

void color_fade_funct(byte *B)
{

    char smer_r = (mozne_barve.barvni_ptr[*B][0] - tr_r) / (abs(mozne_barve.barvni_ptr[*B][0] - tr_r)); //barva.barvni_ptr ima shranjene naslove barvnih tabel
    char smer_g = (mozne_barve.barvni_ptr[*B][1] - tr_z) / (abs(mozne_barve.barvni_ptr[*B][1] - tr_z));
    char smer_b = (mozne_barve.barvni_ptr[*B][2] - tr_m) / (abs(mozne_barve.barvni_ptr[*B][2] - tr_m));

    smer_r = isnan(smer_r) ? 0 : smer_r;
    smer_g = isnan(smer_g) ? 0 : smer_g;
    smer_b = isnan(smer_b) ? 0 : smer_b;

    tr_r += 10 * smer_r;
    tr_z += 10 * smer_g;
    tr_m += 10 * smer_b;

    writeTRAK();
}

void svetlost_mod_funct(int smer)
{
    tr_bright += 12 * smer; //8 stopenj = priblizno 500ms na fade oz 1000ms na breathe (za polni fade)
    writeTRAK();
}

void mic_mode_change() // Switches audio mode ; 1s hold
{
    holdAUDIOSYS();
    int ct = 0, delay_switch = 300;

    AUSYS_vars.mic_mode = (AUSYS_vars.mic_mode + 1) % mic_detection_mode::LENGHT_1;

    digitalWrite(r_trak, 1);
    digitalWrite(z_trak, 0);
    digitalWrite(m_trak, 0);
    delay(700);

    while (ct < (AUSYS_vars.mic_mode + 1) * 2) // n+1 blink = n+1 audio_mode
    {
        PORTB ^= (1 << 1);                  //R
        PORTD ^= (1 << 3);                  //G
        PORTB ^= (1 << 3);                  //B
        delay_switch = 1000 - delay_switch; // Switches between 300ms and 700ms
        delay(delay_switch);
        ct++;
    }

    PORTB &= ~(1 << 1); //R
    PORTD &= ~(1 << 3); //G
    PORTB &= ~(1 << 3); //B
    resumeAUDIOSYS();
}

void audio_mode_change(char *ch) // Double click
{
    holdAUDIOSYS();
    turnOFFstrip();

    delay(300);

    if (ch == "off")
    {
        trenutni_audio_mode = OFF_A;
    }
    else
    {
        trenutni_audio_mode = (trenutni_audio_mode + 1) % audio_mode::LENGTH_2;
    }

    resumeAUDIOSYS();
}