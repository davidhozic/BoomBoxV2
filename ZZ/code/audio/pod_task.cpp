#include "includes/audio.h"

/**************************************************************************************************************************
*                                                                                                                         *
*                                                         FADE TASKI                                                      *
*                                                                                                                         *
**************************************************************************************************************************/
void fade_task(void *BARVA) //Prizig na barbi in pocasen izklop
{
    tr_bright = 255;
    nastavi_barve(BARVA);

    brightDOWN(15);

    fade_control = NULL;
    vTaskDelete(NULL);
}

void Color_Fade_task(void *BARVA) //Fade iz ene barve v drugo
{

    if (Breathe_control == NULL) // Ce diha on ne sme nastaviti svetlosti,
    {                            // saj jo nastavlja dihalni task
        tr_bright = 255;
    }

    colorSHIFT(BARVA); //prehod iz ene barve v drugo
    color_fade_control = NULL;
    vTaskDelete(NULL);
}

void Fade_Breathe_task(void *BARVA)
{
    if (color_fade_control == NULL)
    {

        nastavi_barve(BARVA);
    }
    brightUP(7);
    brightDOWN(7);
    Breathe_control = NULL;
    vTaskDelete(NULL);
}
