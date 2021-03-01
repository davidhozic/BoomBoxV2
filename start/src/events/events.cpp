#include "Vhod.h"
#include "castimer.h"
#include "../includes/includes.h"
#include <Arduino_FreeRTOS.h>
#include <Arduino.h>

void mic_mode_change();
void button2Events();
void Shutdown();
void external_power_switch_ev();
void internal_power_switch_ev();
void audio_mode_change(char *ch);
extern VHOD napajalnik;
VHOD audioSW(4, 'B', 1);
castimer hold_TIMER; // Timer that times the time of button press
castimer click_timer;

struct click_event_structure
{
    unsigned int hold_time = 0;
    bool double_click = false;
    bool click = true;
    unsigned short press_counter = 0;
} click_event;

void events(void *paramOdTaska)
{
    while (true)
    {
        /******************************************** SWITCH 2 EVENTS ****************************************/

        if (audioSW.vrednost())
        {
            click_event.hold_time = hold_TIMER.vrednost(); // Calls for value to start the timer

            if (click_event.hold_time >= 3000 && click_event.hold_time <= 3500)
            {
                audio_mode_change("off");
            }
        }

        else if (click_event.hold_time > 0)
        {
            hold_TIMER.ponastavi();
            if (click_event.hold_time < 500)
            {
                click_event.press_counter++;
                click_timer.ponastavi(); //Podaljsa casovik
            }
            else if (click_event.hold_time < 1500)
            {
                audio_mode_change("");
            }
            click_event.hold_time = 0;
        }

        /********************** CLICK MACHINE **********************/

        if (click_event.press_counter > 0)
        {
            if (click_timer.vrednost() > 700)
            { //Start casovnika
                switch (click_event.press_counter)
                {
                case 1:
                    Hardware.display_enabled = !Hardware.display_enabled; // Toggles LCD
                    break;

                case 2:
                    mic_mode_change();
                    break;
                }
                click_event.press_counter = 0;
            }
        }

        /******************************** POWER SWITCH EVENTS ********************************/
        if (napajalnik.vrednost() && Hardware.PSW == false)
        {
            vTaskSuspend(core_control);
            delay(20);
            external_power_switch_ev();
            vTaskResume(core_control);
            delay(20);
        }

        else if (napajalnik.vrednost() == 0 && Hardware.PSW)
        {
            vTaskSuspend(core_control);
            delay(20);
            internal_power_switch_ev();
            vTaskResume(core_control);
            delay(20);
        }

        /*************************************************************************************/
    }
}

void external_power_switch_ev()
{
    taskENTER_CRITICAL();
    Shutdown();
    delay(20);
    PORTD |= (1 << 7);
    Timers.stikaloCAS.ponastavi();
    delay(20);
    Hardware.PSW = true;
    taskEXIT_CRITICAL();
}

void internal_power_switch_ev()
{
    taskENTER_CRITICAL();
    Shutdown();
    delay(20);
    PORTD &= ~(1 << 7);
    Timers.stikaloCAS.ponastavi();
    delay(20);
    Hardware.PSW = false;
    taskEXIT_CRITICAL();
}
