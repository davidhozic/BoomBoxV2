#include "includes/audio.h"
#include "../includes/includes.h"

/**************************************************************************************************************************
*                                                                                                                         *
*                                                         FADE TASKI                                                      *
*                                                                                                                         *
**************************************************************************************************************************/
void fade_task(void *B) //Prizig na barbi in pocasen izklop
{
    taskENTER_CRITICAL();
    tr_bright = 255;
    tr_r = mozne_barve.barvni_ptr[BARVA][0];
    tr_z = mozne_barve.barvni_ptr[BARVA][1];
    tr_m = mozne_barve.barvni_ptr[BARVA][2];
    taskEXIT_CRITICAL();
    while (tr_bright > 0)
    {
        svetlost_mod_funct(-1);
        vTaskDelay(1);
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

        color_fade_funct((byte *)B);
        vTaskDelay(1);
    }
    color_fade_control = NULL;
    vTaskDelete(NULL);
}

void Fade_Breathe_Task(void *B)
{
    if (Mixed_fade_control == NULL)
    {
        tr_r = mozne_barve.barvni_ptr[BARVA][0];
        tr_z = mozne_barve.barvni_ptr[BARVA][1];
        tr_m = mozne_barve.barvni_ptr[BARVA][2];
    }
    while (tr_bright < 255)
    {
        svetlost_mod_funct(1);
        vTaskDelay(1);
    }

    while (tr_bright > 0)
    {
        svetlost_mod_funct(-1);
        vTaskDelay(1);
    }
    tr_bright = 0;
    Breathe_control = NULL;
    vTaskDelete(NULL);
}

void Mesan_fade_task(void *b)
{

    xTaskCreate(Fade_Breathe_Task, "breathe fade", 64, b, 1, &Breathe_control);
    xTaskCreate(Color_Fade_task, "col_fade", 64, b, 1, &color_fade_control);

    while (Breathe_control != NULL)
    {
        vTaskDelay(1);
    }

    Mixed_fade_control = NULL;
    deleteTask(color_fade_control);
    vTaskDelete(NULL);
}