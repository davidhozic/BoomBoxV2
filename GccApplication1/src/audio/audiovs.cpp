#include "castimer/castimer.h"
#include "VHOD/Vhod.h"
#include "../includes/includes.h"
#include "includes/audio.h"
/*************************** task nadzor  **************************/
TaskHandle_t fade_control ;
TaskHandle_t color_fade_control;
TaskHandle_t Breathe_control ;
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
    uint16_t povprecna_glasnost = 0;

    while (true)
    {
       
        int checkPVP = AVG_Volume_Meri(); // Izmeri povprecje
        povprecna_glasnost = checkPVP != 0 ? checkPVP : povprecna_glasnost;
        mikrofon_detect = readANALOG(mic_pin) >= (povprecna_glasnost + 80) && povprecna_glasnost != 0 ? true : false;
     

        if (Timers.lucke_filter_time.vrednost() > 100 && mikrofon_detect ) // AUDIO_M machine
        {
            Timers.lucke_filter_time.ponastavi();
            static uint8_t barva_selekt = 0;
			barva_selekt += 1 % LENGHT; 
            switch (trenutni_audio_mode)
            {

            case NORMAL_FADE: //Prizig in fade izklop
			    deleteTask(fade_control);                        
                cr_fade_tsk(fade_task, "Normal Fade", barva_selekt, fade_control);
                break;

            case COLOR_FADE: //Prehod iz trenutne barve v zeljeno
                cr_fade_tsk(Color_Fade_task, "Color shift", barva_selekt, color_fade_control);
                break;

            case Fade_Breathe: //Dihalni nacin
                cr_fade_tsk(Fade_Breathe_Task, "Breathe Fade", barva_selekt, Breathe_control);
                break;

            case OFF_A:
                holdTASK(audio_system_control); //Ne rabi hoditi v task ce je izkljucen
                break;
            }
        }

        vTaskDelay(1);
    }
}
