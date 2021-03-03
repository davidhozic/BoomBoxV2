

#include <Arduino.h>
#include "castimer.h"
#include "Vhod.h"
#include <Arduino_FreeRTOS.h>
#include "includes/includes.h"
#include "audio/includes/audio.h"
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\EEPROM\src\EEPROM.h"

VHOD napajalnik(2, 'D', 0);
VHOD stikalo(4, 'D', 0);

/* ************************** Extenal ************************************ */
void spanje();
void Shutdown();
void Power_UP();
void turnOFFstrip();
void events(void *paramOdTaska);
void audio_visual();
/* *********************************************************************** */

void core(void *paramOdTaska)
{

    while (true)
    {
        if (stikalo.vrednost() == 0 && Timers.stikaloOFFtime.vrednost() > 30)
        {
            if (Hardware.is_Powered_UP)
            {
                Shutdown();
            }
            Timers.stikaloCAS.ponastavi();
            if (stikalo.fallingEdge())
            {
                Hardware.AMP_oheat = false;
            }
        }
        else if (stikalo.vrednost() == 1)
        {
            Timers.stikaloOFFtime.ponastavi();
        }

        /***********************************************************************************************
        *                                                                                             *
        *                                                                                             *
        *                                      BRANJE VOLT                                            *
        *                                                                                             *
        *                                                                                             * 
        ***********************************************************************************************/

        taskENTER_CRITICAL();
        Hardware.napetost = analogRead(vDIV_pin) * (float)Hardware.REF_VOLT / 1023.00;
        taskEXIT_CRITICAL();

       
        //----------------------------------------------------------------------------------------------------------------------------------
        //                                               Power UP
        //----------------------------------------------------------------------------------------------------------------------------------
        if (Timers.stikaloCAS.vrednost() >= 2000 && !Hardware.AMP_oheat && (Hardware.napetost > sleep_voltage + 50 || Hardware.PSW) && !Hardware.is_Powered_UP)
        { // Elapsed 2000 ms, not overheated, enough power or (already switched to)external power and not already powered up
            Power_UP();
        }

        if (Hardware.napetost <= sleep_voltage && napajalnik.vrednost() == 0 && Hardware.napetost != 0) //Če je napetost 0V, to pomeni da baterij še ni prebral ; V spanje gre pri 8% napolnjenosti
        {
            Shutdown();
            spanje();
        }
    }
}

void Shutdown()
{
    taskENTER_CRITICAL();
    PORTB &= ~1;
    PORTD &= ~1; //Izklop
    Hardware.is_Powered_UP = false;
    trenutni_audio_mode = OFF_A;
    taskEXIT_CRITICAL();
}

void Power_UP()
{
    delay(20);
    PORTB |= 1;
    Hardware.is_Powered_UP = true;
    PORTD |= 1;
    delay(210);
    Hardware.display_enabled = true;
}
