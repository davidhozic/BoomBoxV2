

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

        static unsigned long vsota_br = 0;
        static unsigned short st_br = 0;
        taskENTER_CRITICAL();
        vsota_br += analogRead(vDIV_pin) * (float)Hardware.REF_VOLT / 1023.00;
        st_br++;
        taskEXIT_CRITICAL();

        if (st_br >= 10)
        {
            taskENTER_CRITICAL();
            Hardware.napetost = (float)vsota_br / st_br;
            taskEXIT_CRITICAL();
            vsota_br = 0;
            st_br = 0;
        }
        vTaskDelay(30 / portTICK_PERIOD_MS);

        //----------------------------------------------------------------------------------------------------------------------------------
        //                                               Power UP
        //----------------------------------------------------------------------------------------------------------------------------------
        if (Timers.stikaloCAS.vrednost() >= 2000 && !Hardware.AMP_oheat && (Hardware.napetost > sleep_voltage + 50 || Hardware.PSW) && !Hardware.is_Powered_UP)
        { // Elapsed 2000 ms, not overheated, enough power or (already switched to)external power and not already powered up
            if (eTaskGetState(audio_system_control) != eSuspended)
                vTaskSuspend(event_control);
            Power_UP();
            if (eTaskGetState(audio_system_control) == eSuspended)
                vTaskResume(event_control);
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
