#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "D:\Documents\Arduino\libraries\castimer\castimer.h"
#include "D:\Documents\Arduino\libraries\VHOD\Vhod.h"
#include "../includes/includes.h"
#include "includes/audio.h"

/*************************** task nadzor  **************************/
TaskHandle_t fade_control = NULL;
TaskHandle_t color_fade_control = NULL;
TaskHandle_t Mixed_fade_control = NULL;
TaskHandle_t Breathe_control = NULL;
/*******************************************************************/

/**************************************************************************************************************************
*                                                                                                                         *
*                                                                                                                         *
*                                                       AUDIO                                                             *
*                                                       VIZUALNI                                                          *
*                                                       SISTEM                                                            *
*                                                                                                                         *
*                                                                                                                         *
**************************************************************************************************************************/
/**************************************************************************************************************************
*                                                                                                                         *
*                                            GLAVNI UPRAVLJALNI SISTEM (TASK)                                             *
*                                                                                                                         *
**************************************************************************************************************************/
c mozne_barve;
castimer mic_sim_timer;
void audio_visual(void *p) //Funkcija avdio-vizualnega sistema
{
    while (true)
    {
        /* switch (AUSYS_vars.mic_mode) //MIC machine
        {
        case Average_volume:
            int trenutna_vrednost = analogRead(mic_pin);
            if ((trenutna_vrednost - povprecna_glasnost) > 150 && povprecna_glasnost != 0)
            {
                AUSYS_vars.mikrofon_detect = 1;
            }
            else
            {
                AUSYS_vars.mikrofon_detect = 0;
            }
            break;

        case Frequency_mode:
            if (frekvenca > 0 && frekvenca <= 60)
            {
                AUSYS_vars.mikrofon_detect = 1;
            }
            else
            {
                AUSYS_vars.mikrofon_detect = 0;
            }
            break;
        } */

        if (mic_sim_timer.vrednost() > 2000)
        {
            AUSYS_vars.mikrofon_detect = true;
            mic_sim_timer.ponastavi();
        }
        else
        {
            AUSYS_vars.mikrofon_detect = false;
        }

        if (Timers.lucke_filter_time.vrednost() > 100 && AUSYS_vars.mikrofon_detect) // AUDIO_M machine
        {
            byte barva_selekt = random(0, LENGHT);
            switch (trenutni_audio_mode)
            {
            case NORMAL_FADE: //Prizig in fade izklop
                delete_AVDIO_subTASK();
                xTaskCreate(fade_task, "normalni fade_create", 70, (void *)&barva_selekt, tskIDLE_PRIORITY, &fade_control);
                break;

            case COLOR_FADE: //Prehod iz trenutne barve v zeljeno
                delete_AVDIO_subTASK();
                xTaskCreate(Color_Fade_task, "col_fade", 70, (void *)&barva_selekt, tskIDLE_PRIORITY, &color_fade_control);
                break;

            case MIXED_FADE:
                delete_AVDIO_subTASK();
                xTaskCreate(Mesan_fade_task, "color fade with off fade", 70, (void *)&barva_selekt, tskIDLE_PRIORITY, &Mixed_fade_control);
                break;

            case Fade_Breathe:
                delete_AVDIO_subTASK();
                xTaskCreate(Fade_Breathe_Task, "breathe fade", 70, (void *)&barva_selekt, tskIDLE_PRIORITY, &Breathe_control);
                break;

            case Direct_signal: //Vijolicna barva glede na direktn signal iz AUSYS_vars.mikrofon_detect = 1a
                unsigned short Signal_level = analogRead(mic_pin) * 255 / 1023;
                analogWrite(r_trak, Signal_level); // Direktna povezava AUSYS_vars.mikrofon_detect = 1a na izhod vijolicne barve
                analogWrite(m_trak, (Signal_level - 50) >= 0 ? Signal_level - 50 : 0);
                break;

            case OFF_A:
                break;
            }
            Timers.lucke_filter_time.ponastavi();
        }
    }
}