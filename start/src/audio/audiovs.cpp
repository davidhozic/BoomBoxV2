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

castimer mic_sim_timer;

void audio_visual(void *p) //Funkcija avdio-vizualnega sistema
{
    bool mikrofon_detect = false;
    bool mikrofon_det_prejsnja = false;
    unsigned short prev_random = 0; //Za vecjo nakljucnost
    uint8_t povprecna_glasnost = 0;

    while (true)
    {
        uint16_t checkPVP = AVG_Volume_Meri(); // Izmeri povprecje
        povprecna_glasnost = checkPVP != 0 ? checkPVP : povprecna_glasnost;
        mikrofon_detect = analogRead(mic_pin) >= (povprecna_glasnost + 80) && povprecna_glasnost != 0 ? true : false;

        if (mikrofon_detect && mikrofon_detect != mikrofon_det_prejsnja) // AUDIO_M machine
        {
            mikrofon_det_prejsnja = mikrofon_detect;
            uint8_t barva_selekt = (random(0, 55) * prev_random) % LENGHT; //Extra nakljucnost
            prev_random = random(0, analogRead(A4) + millis() % 5000);

            switch (trenutni_audio_mode)
            {

            case NORMAL_FADE: //Prizig in fade izklop
                deleteTask(fade_control);
                xTaskCreate(fade_task, "normalni fade_create", 46, &barva_selekt, 2, &fade_control);
                break;

            case COLOR_FADE: //Prehod iz trenutne barve v zeljeno
                deleteTask(color_fade_control);
                xTaskCreate(Color_Fade_task, "col_fade", 46, &barva_selekt, 2, &color_fade_control);
                break;

            case Fade_Breathe:  //Dihalni nacin
                deleteTask(Breathe_control);
                xTaskCreate(Fade_Breathe_Task, "breathe fade", 46, &barva_selekt, 3, &Breathe_control);
                break;

            case OFF_A:
                holdTASK(audio_system_control); //Ne rabi hoditi v task ce je izkljucen
                break;
            }
        }

        vTaskDelay(1);
    }
}
