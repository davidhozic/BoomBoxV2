
#include "castimer.h"
#include "../includes/includes.h"
#include "includes/audio.h"
#include "queue.h"

/*********************************************************************/
/*-------------------------------------------------------------------*/
/*                     Task kontrole za audio meritve                */
/*-------------------------------------------------------------------*/
TaskHandle_t avg_volume_control;  //Kontrola za povprecno glasnost
TaskHandle_t Frek_analyze_handle; //Kontrola za frekvenco
/*********************************************************************/

void AVG_Volume_Meri(void *povp)
{
    unsigned long vsota_branj = 0;
    unsigned short st_branj = 0;
    unsigned short max_izmerjeno = 0;
    //QueueHandle_t avg_volume_queue;
    while (1)
    {
        delay_FRTOS(50);
        vsota_branj += max_izmerjeno;
        st_branj++;
        max_izmerjeno = 0;
        Timers.average_v_timer.ponastavi();
    }
    if (st_branj >= 100)
    {
        unsigned short temp = (float)vsota_branj / st_branj;
        vsota_branj = 0;
        st_branj = 0;
        max_izmerjeno = 0;
    }
}

void frek_meri(void *frek)
{
    unsigned short prej_vrednost = 0;
    unsigned short tr_vrednost = 0;
    unsigned short tTime = 0;
    bool highMarkHit = false;
    while (1)
    {
        tr_vrednost = analogRead(mic_pin);

        if (tr_vrednost > 511 && highMarkHit && tr_vrednost >= prej_vrednost + 50)
        { // 511 ADC == 2.5V
            unsigned short temp = (float)1 / ((float)tTime / 1000.00);
            frekvenca = temp;
            tTime = 0;
            Timers.frek_meassurement_t.ponastavi();
        }
        else if (tr_vrednost >= prej_vrednost + 50)
        {

            tTime = Timers.frek_meassurement_t.vrednost();
            prej_vrednost = tr_vrednost;
        }
        else if (tr_vrednost <= prej_vrednost - 50)
        {
            highMarkHit = true;
            tTime = Timers.frek_meassurement_t.vrednost();
        }
     }
}

void create_audio_meritve(uint8_t *mode)
{

    deleteTask(avg_volume_control);
    deleteTask(Frek_analyze_handle);

    switch (*mode)
    {
    case Frequency_mode:
        xTaskCreate(AVG_Volume_Meri, "Mer. povp. glasnosti", 64, &AUSYS_vars.povprecna_glas, tskIDLE_PRIORITY, &avg_volume_control);
        break;

    case Average_volume:
        xTaskCreate(frek_meri, "Mer. povp. glasnosti", 64, &AUSYS_vars.frek, tskIDLE_PRIORITY, &Frek_analyze_handle);
        break;
    }
}