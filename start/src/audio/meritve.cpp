
#include "castimer.h"
#include "../includes/includes.h"
#include "includes/audio.h"
#include "queue.h"

/*********************************************************************/
/*-------------------------------------------------------------------*/
/*                     Task kontrole za audio meritve                */
/*-------------------------------------------------------------------*/
TaskHandle_t avg_volume_control;     //Kontrola za povprecno glasnost
TaskHandle_t Frek_analyze_handle;    //Kontrola za frekvenco
SemaphoreHandle_t Meritveni_semafor; // Kontrolira dostop do merjene velecine
/*********************************************************************/

void AVG_Volume_Meri()
{
    static unsigned long vsota_branj = 0;
    static unsigned short st_branj = 0;
    static unsigned short max_izmerjeno = 0;

    delay_FRTOS(50);
    vsota_branj += max_izmerjeno;
    st_branj++;
    max_izmerjeno = 0;
    Timers.average_v_timer.ponastavi();

    if (st_branj >= 100)
    {
        xSemaphoreTake(Meritveni_semafor, portMAX_DELAY); //Semafor na rdeco luc da se ostali taski ne morejo izvajati
        povprecna_glasnost = (float)vsota_branj / st_branj;
        xSemaphoreGive(Meritveni_semafor);
        vsota_branj = 0;
        st_branj = 0;
        max_izmerjeno = 0;
    }
}

void frek_meri()
{
    static unsigned short prej_vrednost = 0;
    static unsigned short tr_vrednost = 0;
    static unsigned short tTime = 0;
    static bool highMarkHit = false; //Belezi ce je perida dosegla predhodno zgornjo mejo

    tr_vrednost = analogRead(mic_pin);

    if (tr_vrednost > 511 && highMarkHit && tr_vrednost >= prej_vrednost + 50)
    { // Ce je dosegel napetost DC offseta in narasca in je predhnodno ze dosegel mag vrednost -> potem je izvedel eno periodo
        xSemaphoreTake(Meritveni_semafor, portMAX_DELAY);
        frekvenca = (float)1 / ((float)tTime / 1000.00);
        xSemaphoreGive(Meritveni_semafor);
        tTime = 0;
        highMarkHit = false;
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
