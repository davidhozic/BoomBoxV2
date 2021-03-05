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
        /*         if (AUSYS_vars.mikrofon_detect == NULL)
        {
            AUSYS_vars.mikrofon_detect = Average_volume;
            create_audio_meritve(&AUSYS_vars.mic_mode);
        } */

        if (mic_sim_timer.vrednost() > 1000)
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
                deleteTask(fade_control);
                xTaskCreate(fade_task, "normalni fade_create", 70, &barva_selekt, 2, &fade_control);
                break;

            case COLOR_FADE: //Prehod iz trenutne barve v zeljeno
                deleteTask(color_fade_control);
                xTaskCreate(Color_Fade_task, "col_fade", 70, &barva_selekt, 2, &color_fade_control);
                break;
            case Fade_Breathe:
                deleteTask(Breathe_control);
                xTaskCreate(Fade_Breathe_Task, "breathe fade", 70, &barva_selekt, 2, &Breathe_control);
                break;
            case Direct_signal: //Vijolicna barva glede na direktn signal iz AUSYS_vars.mikrofon_detect = 1a
                unsigned short Signal_level = (float)analogRead(mic_pin) * 255.00 / 1023.00;
                analogWrite(r_trak, Signal_level); // Direktna povezava AUSYS_vars.mikrofon_detect = 1a na izhod vijolicne barve
                analogWrite(m_trak, (Signal_level - 50) >= 0 ? Signal_level - 50 : 0);
                break;

            case OFF_A:
                break;
            }
            Timers.lucke_filter_time.ponastavi();
        }
        delay_FRTOS(30);
    }
}
