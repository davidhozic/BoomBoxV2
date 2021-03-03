
#include "castimer.h"
#include "../includes/includes.h"
#include "includes/audio.h"

void merjenje_frekvence()
{
    static int prejsnja_vrednost = 0;
    static unsigned long lokal_timer = 0;

    int trenutna_vrednost = analogRead(mic_pin);
    if (trenutna_vrednost > prejsnja_vrednost)
    {
        prejsnja_vrednost = trenutna_vrednost;
        lokal_timer = Timers.audio_timer.vrednost();
    }

    if (prejsnja_vrednost - trenutna_vrednost >= 50)
    {
        int cas_milisekunde = lokal_timer * 4;
        Timers.audio_timer.ponastavi();
        float cas_sekunde = (float)cas_milisekunde / 1000.00;
        prejsnja_vrednost = trenutna_vrednost;
        frekvenca = 1 / cas_sekunde;
    }
}

void pvp_glasnost()
{
    static unsigned int max_branja = 0;
    static bool first_read = false;
    static unsigned int max_vrednost = 0;
    static unsigned int count_branj = 0;
    static unsigned long vsota_branj = 0;
    static unsigned short count_above_average = 0;
    static bool previosly_risen = false;

    int trenutna_vrednost = analogRead(mic_pin);
    if (trenutna_vrednost > max_vrednost)
    {

        if (trenutna_vrednost - max_vrednost >= 50)
            previosly_risen = true;
        max_vrednost = trenutna_vrednost;
    }

    else if ((max_vrednost - trenutna_vrednost) >= 50)
    {
        if (previosly_risen)
        {

            if (abs(max_vrednost - povprecna_glasnost < 300) || count_above_average >= 100)
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

    if (!first_read)
    {
        max_branja = 100;
    }
    else
    {
        max_branja = 3000;
    }

    if (count_branj >= max_branja)
    {
        first_read = true;
        povprecna_glasnost = vsota_branj / count_branj;
        count_branj = 0;
        vsota_branj = 0;
        count_above_average = 0;
    }
}

/*                                  Povprecje maksimalnih vrednosti v razmaku 100ms
void pvp_glasnost()
{
  static unsigned int max_VR = 0;
  static unsigned long vsota = 0;
  static unsigned short ct = 0;


  int tr_vrednost = analogRead(A0);
  if (tr_vrednost > max_VR)
    max_VR = tr_vrednost;
  if (Timers.audio_timer.vrednost() >= 100) {
    vsota += max_VR;
    max_VR = 0;
    ct++;
    testt.ponastavi();
  }

  if (ct == 200){
    povprecna_glasnost = vsota/ct;
    ct = 0;
    vsota = 0;
  }
}*/




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