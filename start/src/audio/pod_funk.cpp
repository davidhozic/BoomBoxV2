#include <Arduino_FreeRTOS.h>
#include "includes/audio.h"
#include "../includes/includes.h"
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\EEPROM\src\EEPROM.h"
/**************************************************************************************************************************
*                                                                                                                         *
*                                                           Pomozne funkcije                                              *
*                                                                                                                         *
**************************************************************************************************************************/

void holdALL_tasks() // Zbrise obstojece taske ce obstajajo
{
    holdTASK(fade_control);
    holdTASK(color_fade_control);
    holdTASK(Mixed_fade_control);
    holdTASK(Breathe_control);
}

void deleteALL_subAUDIO_tasks()
{
    deleteTask(fade_control);
    deleteTask(color_fade_control);
    deleteTask(Mixed_fade_control);
    deleteTask(Breathe_control);
    delay_FRTOS(15);
}

void writeTRAK()
{
    analogWrite(r_trak, (float)tr_r * (float)tr_bright / 255.00);
    analogWrite(z_trak, (float)tr_z * (float)tr_bright / 255.00);
    analogWrite(m_trak, (float)tr_m * (float)tr_bright / 255.00);
}

void flash_strip() //Utripanje (Izhod iz scroll stata / menjava mikrofona)
{
    free(AUSYS_vars.TR_BARVA);
    memcpy(AUSYS_vars.TR_BARVA, mozne_barve.barvni_ptr[BELA], 3);
    for (uint8_t i = 0; i < 5; i++)
    {
        digitalWrite(r_trak, 0);
        digitalWrite(z_trak, 0);
        digitalWrite(m_trak, 0);
        delay(125);
        digitalWrite(r_trak, 1);
        digitalWrite(z_trak, 1);
        digitalWrite(m_trak, 1);
        delay(125);
    }
}

void color_fade_funct(uint8_t *BARVA)
{
    while (tr_r != mozne_barve.barvni_ptr[*BARVA][0] || tr_z != mozne_barve.barvni_ptr[*BARVA][1] || tr_m != mozne_barve.barvni_ptr[*BARVA][2]) //Trenutna razlicna od zeljene
    {
        char smer[3] = {0, 0, 0};
        mozne_barve.barvni_ptr[*BARVA][0] >= tr_r ? smer[0] = 1 : smer[0] = -1;
        mozne_barve.barvni_ptr[*BARVA][1] >= tr_z ? smer[1] = 1 : smer[1] = -1;
        mozne_barve.barvni_ptr[*BARVA][2] >= tr_m ? smer[2] = 1 : smer[2] = -1;

        tr_r = tr_r + (10 * smer[0]);
        tr_z = tr_z + (10 * smer[1]);
        tr_m = tr_m + (10 * smer[2]);

        //Preveri prenihaj:

        smer[0] == 1 && tr_r > mozne_barve.barvni_ptr[*BARVA][0] ? tr_r = mozne_barve.barvni_ptr[*BARVA][0] : NULL; //Ce je bila trenutna barva pod zeljeno ali na zeljeni in je zdaj trenudna nad zeljeno, se nastavi na zeljeno (prenihaj)
        smer[1] == 1 && tr_z > mozne_barve.barvni_ptr[*BARVA][1] ? tr_z = mozne_barve.barvni_ptr[*BARVA][1] : NULL;
        smer[2] == 1 && tr_m > mozne_barve.barvni_ptr[*BARVA][2] ? tr_m = mozne_barve.barvni_ptr[*BARVA][2] : NULL;

        smer[0] == -1 && tr_r < mozne_barve.barvni_ptr[*BARVA][0] ? tr_r = mozne_barve.barvni_ptr[*BARVA][0] : NULL;
        smer[1] == -1 && tr_z < mozne_barve.barvni_ptr[*BARVA][1] ? tr_z = mozne_barve.barvni_ptr[*BARVA][1] : NULL;
        smer[2] == -1 && tr_m < mozne_barve.barvni_ptr[*BARVA][2] ? tr_m = mozne_barve.barvni_ptr[*BARVA][2] : NULL;

        tr_r = tr_r < 0 ? 0 : tr_r;
        tr_z = tr_z < 0 ? 0 : tr_z;
        tr_m = tr_m < 0 ? 0 : tr_m;

        tr_r = tr_r > 255 ? 255 : tr_r;
        tr_z = tr_z > 255 ? 255 : tr_z;
        tr_m = tr_m > 255 ? 255 : tr_m;

        writeTRAK();
        delay(5);
    }
}

void svetlost_mod_funct(char smer, uint8_t cas_krog)
{

    while (smer > 0 ? tr_bright < 255 : tr_bright > 0)
    {
        tr_bright += 8 * smer;
        tr_bright = tr_bright < 0 ? 0 : tr_bright;
        tr_bright = tr_bright > 255 ? 255 : tr_bright;
        writeTRAK();
        delay(cas_krog);
    }
}

void strip_mode_chg(char *ch)
{
    if (ch == "off")
        trenutni_audio_mode = OFF_A;

    else if (trenutni_audio_mode == OFF_A)
        trenutni_audio_mode = NORMAL_FADE;

    else
        trenutni_audio_mode = ++trenutni_audio_mode % audio_mode::LENGTH_2;
    EEPROM.write(audiomode_eeprom_addr, trenutni_audio_mode);
    deleteALL_subAUDIO_tasks();
}