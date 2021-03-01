
#include "castimer.h"
#include "../includes/includes.h"
#include "includes/audio.h"

void merjenje_frekvence()
{
    static int prejsnja_vrednost;
    static unsigned long lokal_timer;

    int trenutna_vrednost = analogRead(mic_pin);
    if (trenutna_vrednost > prejsnja_vrednost)
    {
        prejsnja_vrednost = trenutna_vrednost;
        lokal_timer = Timers.timer_frek_meri.vrednost();
    }

    if (trenutna_vrednost - prejsnja_vrednost <= -10)
    {
        int cas_milisekunde = lokal_timer * 4;
        Timers.timer_frek_meri.ponastavi();
        int cas_sekunde = cas_milisekunde / 1000;
        prejsnja_vrednost = trenutna_vrednost;
        frekvenca = 1 / cas_sekunde;
    }
}

void pvp_glasnost()
{
    static unsigned int max_vrednost = 0;
    static unsigned short count_branj = 0;
    static unsigned int vsota_branj = 0;
    static unsigned short count_above_average = 0;
    static bool previosly_risen = false;

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

void audio_meritve(void *p)
{
    while (true)
    {
        switch (AUSYS_vars.mic_mode)
        {
        case Frequency_mode:
            merjenje_frekvence();
            break;

        case Average_volume:
            pvp_glasnost();
            break;
        }
    }
}