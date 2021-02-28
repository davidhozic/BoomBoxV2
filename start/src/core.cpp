

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
        static float firstRead = 0;
        if (firstRead == 0)
        {
            taskENTER_CRITICAL();
            firstRead = analogRead(A2) * 5.0 / 1023.0;
            taskEXIT_CRITICAL();
        }
        else if (Timers.VOLT_timer.vrednost() > 500)
        {
            taskENTER_CRITICAL();
            float secondRead = analogRead(A2) * 5.0 / 1023.0;

            if (abs(secondRead - firstRead) < 0.15)
            {
                Hardware.napetost = secondRead;
            }
            firstRead = 0;
            Timers.VOLT_timer.ponastavi();
            taskEXIT_CRITICAL();
        }

        //----------------------------------------------------------------------------------------------------------------------------------
        //                                               Power UP
        //----------------------------------------------------------------------------------------------------------------------------------
        if (Timers.stikaloCAS.vrednost() >= 2000 && !Hardware.AMP_oheat && (Hardware.napetost >= 3.1 || Hardware.PSW) && !Hardware.is_Powered_UP)
        { // Elapsed 2000 ms, not overheated, enough power or (already switched to)external power and not already powered up
            vTaskSuspend(event_control);
            Power_UP();
            vTaskResume(event_control);
        }

        if (Hardware.napetost < 3.07 && napajalnik.vrednost() == 0 && Hardware.napetost != 0) //Če je napetost 0V, to pomeni da baterij še ni prebral ; V spanje gre pri 8% napolnjenosti
        {
            spanje();
        }
    }
}

void Shutdown()
{
    vTaskSuspend(audio_system_control);
    taskENTER_CRITICAL();
    PORTB &= ~1;
    PORTD &= ~1; //Izklop
    turnOFFstrip();
    Hardware.is_Powered_UP = false;
    Hardware.display_enabled = false;
    AUSYS_vars.A_mode = OFF_A;
    taskEXIT_CRITICAL();
}

void Power_UP()
{
    delay(20);
    PORTB |= 1;
    Hardware.is_Powered_UP = true;
    PORTD |= 1;
    delay(210);
    taskENTER_CRITICAL();
    AUSYS_vars.A_mode = EEPROM.read(audiomode_eeprom_addr); //Prebere zadnje stanje audia
    taskEXIT_CRITICAL();
    vTaskResume(audio_system_control);
}
