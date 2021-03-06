#include "includes/audio.h"
#include "../includes/includes.h"

/**************************************************************************************************************************
*                                                                                                                         *
*                                                         FADE TASKI                                                      *
*                                                                                                                         *
**************************************************************************************************************************/
void fade_task(void *B) //Prizig na barbi in pocasen izklop
{

    tr_bright = 255;
    tr_r = mozne_barve.barvni_ptr[BARVA][0];
    tr_z = mozne_barve.barvni_ptr[BARVA][1];
    tr_m = mozne_barve.barvni_ptr[BARVA][2];

    brightDOWN();

    fade_control = NULL;
    vTaskDelete(NULL);
}

void Color_Fade_task(void *BA) //Fade iz ene barve v drugo
{

    if (Breathe_control == NULL)
    {
        tr_bright = 255;
    }

    colorSHIFT(BA);
    color_fade_control = NULL;
    vTaskDelete(NULL);
}

void Fade_Breathe_Task(void *B)
{
    if (color_fade_control == NULL)
    {
        tr_r = mozne_barve.barvni_ptr[BARVA][0];
        tr_z = mozne_barve.barvni_ptr[BARVA][1];
        tr_m = mozne_barve.barvni_ptr[BARVA][2];
    }
    brightUP();
    brightDOWN();

    tr_bright = 0;
    Breathe_control = NULL;
    vTaskDelete(NULL);
}

void Direct_mic_Task(void *)
{
    tr_bright = 255;
    while (1)
    {
        tr_bright = static_cast<float>(analogRead(mic_pin) * 255 / 1023);
        writeTRAK();
        delay(5);
    }
}
