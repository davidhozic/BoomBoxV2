
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\Arduino.h"
#include "castimer.h"
#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\header\stuff.h"
extern const int mic_pin;
castimer nap_read;

void merjenje_frekvence()
{
    int prejsnja_vrednost;
    unsigned long lokal_timer;

    int trenutna_vrednost = analogRead(mic_pin);
    if (trenutna_vrednost > prejsnja_vrednost)
    {
        prejsnja_vrednost = trenutna_vrednost;
        lokal_timer = timer_frek_meri.vrednost();
    }

    if (trenutna_vrednost - prejsnja_vrednost <= -10)
    {
        int cas_milisekunde = lokal_timer * 4;
        timer_frek_meri.ponastavi();
        int cas_sekunde = cas_milisekunde / 1000;
        prejsnja_vrednost = trenutna_vrednost;
        frekvenca = 1 / cas_sekunde;
    }
}

void pvp_glasnost()
{
    unsigned int max_vrednost = 0;
    unsigned short count_branj = 0;
    unsigned int vsota_branj = 0;
    unsigned short count_above_average = 0;
    bool previosly_risen = false;

    int trenutna_vrednost = analogRead(mic_pin);
    if (trenutna_vrednost > max_vrednost)
    {
        max_vrednost = trenutna_vrednost;
        if (trenutna_vrednost - max_vrednost >= 3)
            previosly_risen = true;
    }

    else if ((max_vrednost - trenutna_vrednost) >= 3)
    {
        if (previosly_risen)
        {

            if (max_vrednost - povprecna_glasnost < 300 || count_above_average >= 10)
            { /* Vrednost se ni bila zabelezena in glasnost ni porastla za vec kot 300 ADC oz. je porastla vec kot 10x */
                vsota_branj += max_vrednost;
                count_branj += 1;
            }
            else
            {
                count_above_average++; //Filtrira spice v glasnosti
            }

            previosly_risen = false;
        }

        max_vrednost = trenutna_vrednost;
    }
    if (count_branj == 200)
    {

        povprecna_glasnost = vsota_branj / count_branj;
        count_branj = 0;
        vsota_branj = 0;
        count_above_average = 0;
    }
}

void meritve(void *p)
{
    while (true)
    {
        unsigned short mode = *((int *)p);
        switch (mode)
        {
        case Frequency_mode:
            merjenje_frekvence();
            break;

        case Average_volume:
            pvp_glasnost();
            break;
        }
    }

    if (nap_read.vrednost() > 500)
    {
        taskENTER_CRITICAL();
        napetost = (float)analogRead(A2) * (float)5.0 / 1023.00;
        nap_read.ponastavi();
        taskEXIT_CRITICAL();
    }
}