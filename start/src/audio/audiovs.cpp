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
TaskHandle_t Direct_signal_control = NULL;
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

castimer mic_sim_timer;

void audio_visual(void *p) //Funkcija avdio-vizualnega sistema
{
    bool mikrofon_detect = false;
    unsigned short prev_random = 0; //Za vecjo nakljucnost
    while (true)
    {
        /*         xSemaphoreTake(Meritveni_semafor, portMAX_DELAY); // Caka na tem mestu dokler ni semafor na zeleni luci
        
        switch (AUSYS_vars.mic_mode)
        {
        case Average_volume:
            mikrofon_detect = analogRead(mic_pin) >= povprecna_glasnost + 300 ? 1 : 0;
            break;

        case Frequency_mode:
            mikrofon_detect = frekvenca <= 65 ? 1 : 0;
            break;
        }
        xSemaphoreGive(Meritveni_semafor); */

        if (mic_sim_timer.vrednost() > 400)
        {
            mikrofon_detect = true;
            mic_sim_timer.ponastavi();
        }
        else
        {
            mikrofon_detect = false;
        }

        if (Timers.lucke_filter_time.vrednost() > 100 && mikrofon_detect) // AUDIO_M machine
        {

            uint8_t barva_selekt = static_cast<unsigned int>((random(0, 9113) * prev_random) % LENGHT); //Extra nakljucnost
            prev_random = random(0, analogRead(A4) + millis() % 5000);

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
            case Direct_signal: //Vijolicna barva glede na direktn signal iz mikrofon_detect = 1a
                deleteTask(Direct_signal_control);
                xTaskCreate(Direct_mic_Task, "Direkt na mic", 80, NULL, tskIDLE_PRIORITY, &Direct_signal_control);
                holdTASK(audio_system_control);
                break;
            case OFF_A:
                break;
            }
            Timers.lucke_filter_time.ponastavi();
        }
        vTaskDelay(2);
    }
}
