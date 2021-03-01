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

void audio_visual(void *paramOdTaska) //Funkcija avdio-vizualnega sistema
{
    bool mikrofon_detect = 0;
    while (true)
    {
        switch (AUSYS_vars.mic_mode) //MIC machine
        {
        case Average_volume:
            int trenutna_vrednost = analogRead(mic_pin);
            if ((trenutna_vrednost - povprecna_glasnost) > 150 && povprecna_glasnost != 0)
            {
                mikrofon_detect = 1;
            }
            else
            {
                mikrofon_detect = 0;
            }
            break;

        case Frequency_mode:
            if (frekvenca > 0 && frekvenca <= 60)
            {
                mikrofon_detect = 1;
            }
            else
            {
                mikrofon_detect = 0;
            }
            break;
        }

        if (Timers.lucke_filter_time.vrednost() > 100 && mikrofon_detect == 1) // AUDIO_M machine
        {
            Timers.lucke_filter_time.ponastavi();
            byte barva_selekt = random(0, barve::LENGHT);

            switch (trenutni_audio_mode)
            {
            case NORMAL_FADE: //Prizig in fade izklop
                delete_AVDIO_subTASK(Delete_ALL);
                xTaskCreate(fade_task, "normalni fade_create", 45, (void *)&barva_selekt, 1, &fade_control);
                break;

            case COLOR_FADE: //Prehod iz trenutne barve v zeljeno
                delete_AVDIO_subTASK(Delete_ALL);
                xTaskCreate(Color_Fade_task, "col_fade", 45, (void *)&barva_selekt, 1, &color_fade_control);
                break;

            case MIXED_FADE:
                delete_AVDIO_subTASK(Delete_ALL);
                xTaskCreate(Mesan_fade_task, "color fade with off fade", 20, (void *)&barva_selekt, 1, &Mixed_fade_control);
                break;

            case Fade_Breathe:
                delete_AVDIO_subTASK(Delete_ALL);
                xTaskCreate(Fade_Breathe_Task, "breathe fade", 45, (void *)&barva_selekt, 1, &Breathe_control);
                break;

            case Direct_signal: //Vijolicna barva glede na direktn signal iz mikrofon_detect = 1a
                mikrofon_detect = 0;
                unsigned short Signal_level = analogRead(mic_pin) * 255 / 1023;
                analogWrite(r_trak, Signal_level); // Direktna povezava mikrofon_detect = 1a na izhod vijolicne barve
                analogWrite(m_trak, (Signal_level - 50) >= 0 ? Signal_level - 50 : 0);
                break;

            case OFF_A:
                turnOFFstrip();
            }
        }
    }
}
