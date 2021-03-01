#include <Arduino_FreeRTOS.h>
#include "includes/audio.h"
#include "../includes/includes.h"
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\EEPROM\src\EEPROM.h"
/**************************************************************************************************************************
*                                                                                                                         *
*                                                           Pomozne funkcije                                              *
*                                                                                                                         *
**************************************************************************************************************************/

void delete_AVDIO_subTASK(int doNotDelete) // Zbrise obstojece taske ce obstajajo in niso oznacene kot da se jih ne sme brisati (da task ne izbrise samega sebe)
{
    if (fade_control != NULL && doNotDelete != dont_fade_delete)
    {
        vTaskDelete(fade_control);
        fade_control = NULL;
    }

    if (color_fade_control != NULL && doNotDelete != dont_color_fade_delete)
    {
        vTaskDelete(color_fade_control);
        color_fade_control = NULL;
    }
    if (Mixed_fade_control != NULL && doNotDelete != dont_Mixed_fade_delete)
    {
        vTaskDelete(Mixed_fade_control);
        Mixed_fade_control = NULL;
    }
    if (Breathe_control != NULL && doNotDelete != dont_Breathe_delete)
    {
        vTaskDelete(Breathe_control);
        Breathe_control = NULL;
    }
}

void turnOFFstrip()
{

    taskENTER_CRITICAL();
    delete_AVDIO_subTASK(Delete_ALL);
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
    analogWrite(r_trak, tr_r * tr_bright / 255);
    analogWrite(z_trak, tr_z * tr_bright / 255);
    analogWrite(m_trak, tr_m * tr_bright / 255);
    taskEXIT_CRITICAL();
}

void color_fade_funct(byte *B)
{

    float smer_r = (mozne_barve.barvni_ptr[BARVA][0] - tr_r) / (abs(mozne_barve.barvni_ptr[BARVA][0] - tr_r)); //barva.barvni_ptr ima shranjene naslove barvnih tabel
    float smer_g = (mozne_barve.barvni_ptr[BARVA][1] - tr_z) / (abs(mozne_barve.barvni_ptr[BARVA][1] - tr_z));
    float smer_b = (mozne_barve.barvni_ptr[BARVA][2] - tr_m) / (abs(mozne_barve.barvni_ptr[BARVA][2] - tr_m));

    smer_r = isnan(smer_r) ? 0 : smer_r;
    smer_g = isnan(smer_g) ? 0 : smer_g;
    smer_b = isnan(smer_b) ? 0 : smer_b;

    tr_r += 5 * smer_r;
    tr_z += 5 * smer_g;
    tr_m += 5 * smer_b;
    writeTRAK();
}

void svetlost_mod_funct(byte *svetlost, byte smer)
{
    *svetlost += 5 * smer;
    writeTRAK();
}

void mic_mode_change() // Switches audio mode ; 1s hold
{
    vTaskSuspend(audio_system_control);

    int ct = 0, delay_switch = 300;

    AUSYS_vars.mic_mode = (AUSYS_vars.mic_mode + 1) % mic_detection_mode::LENGHT_1;

    digitalWrite(r_trak, 0);
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
    vTaskResume(audio_system_control);
}

void audio_mode_change(char *ch) // Double click
{
    vTaskSuspend(audio_system_control);
    turnOFFstrip();

    vTaskDelay(500 / portTICK_PERIOD_MS);

    if (ch == "off")
    {
        trenutni_audio_mode = OFF_A;
    }
    else
    {
        trenutni_audio_mode = (trenutni_audio_mode + 1) % audio_mode::LENGTH_2;
    }
#if SHRANI_AUDIO_MODE
    EEPROM.update(audiomode_eeprom_addr, trenutni_audio_mode); // Zapise zadnje stanje
#endif
    vTaskDelay(500 / portTICK_PERIOD_MS);
    vTaskResume(audio_system_control);
}