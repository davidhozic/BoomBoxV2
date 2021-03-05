

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

        if (xSemaphoreTake(voltage_SEM, portMAX_DELAY) == pdTRUE) // Vzame dostop do napetostnega semaforja -> ostali taski ne morajo brati napetosti
        {
            unsigned short vReadDelay;
            Hardware.napetost == 0 ? vReadDelay = 500 : 3000; //Ce napetost ni prebrana cakaj le 500ms, drugace pa 3000ms

            if (Timers.VOLT_timer.vrednost() > vReadDelay)
            {
                Timers.VOLT_timer.ponastavi();
                Hardware.napetost = analogRead(vDIV_pin) * (float)Hardware.REF_mVOLT / 1023.00f;
            }

            xSemaphoreGive(voltage_SEM); // Da zeleno luc ostalim taskom
        }

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
        delay_FRTOS(100);   
    }
}

void Shutdown()
{
    PORTB &= ~1;
    PORTD &= ~1; //Izklop
    Hardware.is_Powered_UP = false;
    trenutni_audio_mode = OFF_A;
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
