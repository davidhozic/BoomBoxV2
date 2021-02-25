#include "D:\Documents\Arduino\libraries\VHOD\Vhod.h"
#include "D:\Documents\Arduino\libraries\castimer\castimer.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\header\namespaces.h"
#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"

void mic_mode_change();
void button2Events();
void Shutdown();
void external_power_switch_ev();
void internal_power_switch_ev();
void audio_mode_change(char *ch);
extern VHOD napajalnik;

extern TaskHandle_t core_handle;

void events(void *paramOdTaska)
{
    VHOD audioSW(4, 'B', 1);
    castimer hold_TIMER; // Timer that times the time of button press
    castimer click_timer;
    unsigned int hold_time = 0;

    struct d02130404
    {
        bool double_click = false;
        bool click = true;
        uint8_t press_counter = 0;
    } click_event;

    while (true)
    {
        /******************************************** SWITCH 2 EVENTS ****************************************/

        if (audioSW.vrednost())
        {
            hold_time = hold_TIMER.vrednost(); // Calls for value to start the timer

            if (hold_time >= 3000 && hold_time <= 3500)
            {
                audio_mode_change("off");
            }
        }

        else if (hold_time > 0)
        {
            hold_TIMER.ponastavi();
            if (hold_time < 500)
            {
                click_event.press_counter++;
                click_timer.ponastavi(); //Podaljsa casovik
            }
            else if (hold_time < 1500)
            {
                audio_mode_change("");
            }
            hold_time = 0;
        }

        /********************** CLICK MACHINE **********************/

        if (click_event.press_counter > 0)
        {
            if (click_timer.vrednost() > 700)
            { //Start casovnika
                switch (click_event.press_counter)
                {
                case 1:
                    Hardware::display_enabled = !Hardware::display_enabled; // Toggles LCD
                    break;

                case 2:
                    mic_mode_change();
                    break;
                }
                click_event.press_counter = 0;
            }
        }

        /******************************** POWER SWITCH EVENTS ********************************/
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

        /*************************************************************************************/
    }
}

void external_power_switch_ev()
{

    Shutdown();
    vTaskDelay(20 / portTICK_PERIOD_MS);
    PORTD |= (1 << 7);
    TIMERS_folder::stikaloCAS.ponastavi();
    Hardware::PSW = true;
}

void internal_power_switch_ev()
{
    Shutdown();
    vTaskDelay(20 / portTICK_PERIOD_MS);
    PORTD &= ~(1 << 7);
    TIMERS_folder::stikaloCAS.ponastavi();
    Hardware::PSW = false;
}