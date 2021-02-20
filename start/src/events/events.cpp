#include "Vhod.h"
#include "D:\Documents\Arduino\libraries\castimer\castimer.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\PolnenjeZvoc\code\start\src\header\namespaces.h"
#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"

void audio_mode_change(char *ch);
void button2Events();
void Shutdown();
void external_power_switch_ev();
void internal_power_switch_ev();
extern VHOD napajalnik;

extern TaskHandle_t core_handle;

void events(void *paramOdTaska)
{

    while (true)
    {
        button2Events();

        if (napajalnik.vrednost() && Hardware::PSW == false)
        {
            vTaskSuspend(core_handle);
            external_power_switch_ev();
            vTaskResume(core_handle);
        }

        else if (napajalnik.vrednost() == 0 && Hardware::PSW)
        {
            vTaskSuspend(core_handle);
            internal_power_switch_ev();
            vTaskResume(core_handle);
        }
    }
}

void button2Events()
{
    static VHOD audioSW(4, 'B', 1);
    static castimer hold_TIMER; // Timer that times the time of button press
    static int hold_time = 0;
    /******************************************** SWITCH 2 EVENTS ****************************************/

    if (audioSW.vrednost())
    {
        hold_time = hold_TIMER.vrednost(); // Calls for value to start the timer

        if (hold_time >= 3000 && hold_time <= 3200)
        {
            audio_mode_change("off");
        }
    }

    else if (hold_time > 0)
    {
        hold_TIMER.ponastavi();
        if (hold_time < 500)
        {
            Hardware::display_enabled = !Hardware::display_enabled;
        }
        else if (hold_time < 1500)
        {
            audio_mode_change("");
        }
        hold_time = 0;
    }
}
/********************************************************************************************************/

void external_power_switch_ev()
{

    Shutdown();
    vTaskDelay(15 / portTICK_PERIOD_MS);
    PORTD |= (1 << 7);
    TIMERS_folder::stikaloCAS.ponastavi();
    Hardware::PSW = true;
}

void internal_power_switch_ev()
{
    Shutdown();
    vTaskDelay(15 / portTICK_PERIOD_MS);
    PORTD &= ~(1 << 7);
    TIMERS_folder::stikaloCAS.ponastavi();
    Hardware::PSW = false;
}