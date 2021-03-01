#include "includes/audio.h"
#include "../includes/includes.h"

/**************************************************************************************************************************
*                                                                                                                         *
*                                                  GLAVNI NACINI TASKI                                                    *
*                                                                                                                         *
**************************************************************************************************************************/
void fade_task(void *B) //Prizig na barbi in pocasen izklop
{
    tr_bright = 255;
    tr_r = mozne_barve.barvni_ptr[BARVA][0];
    tr_z = mozne_barve.barvni_ptr[BARVA][1];
    tr_m = mozne_barve.barvni_ptr[BARVA][2];

    while (tr_bright > 0)
    {
        if (Timers.brightness_timer.vrednost() > 15)
        {
            svetlost_mod_funct(&tr_bright, -1);
            Timers.brightness_timer.ponastavi();
        }
    }
    fade_control = NULL;
    vTaskDelete(NULL);
}

void Color_Fade_task(void *B) //Fade iz ene barve v drugo
{

    while (barv_razlika_cond_true)
    {
        if (Mixed_fade_control == NULL) // Dihalni nacin svetlosti se ne izvaja
            tr_bright = 255;

        if (Timers.color_timer.vrednost() > 15)
        {
            color_fade_funct(&BARVA);
            Timers.color_timer.ponastavi();
        }
    }
    color_fade_control = NULL;
    vTaskDelete(NULL);
}

void Fade_Breathe_Task(void *B)
{
    vTaskSuspend(audio_system_control);

    tr_r = mozne_barve.barvni_ptr[BARVA][0];
    tr_z = mozne_barve.barvni_ptr[BARVA][1];
    tr_m = mozne_barve.barvni_ptr[BARVA][2];
    tr_bright = 0;

    while (tr_bright < 255)
    {
        svetlost_mod_funct(&tr_bright, 1);
    }

    while (tr_bright > 0)
    {
        svetlost_mod_funct(&tr_bright, -1);
    }
    vTaskResume(audio_system_control);

    Breathe_control = NULL;
    vTaskDelete(NULL);
}

void Mesan_fade_task(void *b)
{
    delete_AVDIO_subTASK(dont_Mixed_fade_delete);
    xTaskCreate(Fade_Breathe_Task, "breathe fade", 45, b, 1, &Breathe_control);
    xTaskCreate(Color_Fade_task, "col_fade", 45, b, 1, &color_fade_control);
    delay(100);
    while (Breathe_control != NULL)
    {
    };

    Mixed_fade_control = NULL;
    vTaskDelete(NULL);
}