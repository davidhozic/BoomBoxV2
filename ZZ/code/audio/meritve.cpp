
#include "castimer/castimer.h"
#include "../includes/includes.h"
#include "includes/audio.h"

int AVG_Volume_Meri()
{
    static unsigned long vsota_branj = 0;
    static unsigned short st_branj = 0;
    static unsigned short max_izmerjeno = 0;


    unsigned short tr_vrednost = readANALOG(mic_pin);
    if (tr_vrednost > max_izmerjeno)
        max_izmerjeno = tr_vrednost;

    if (Timers.average_v_timer.vrednost() >= 20)
    {
        vsota_branj += max_izmerjeno;
        st_branj++;
        max_izmerjeno = 0;
        Timers.average_v_timer.ponastavi();
    }

    if (st_branj >= 50)
    {
        uint16_t tmp = vsota_branj / st_branj;
        vsota_branj = 0;
        st_branj = 0;
        max_izmerjeno = 0;
        return tmp;
    }
    return 0;
}
