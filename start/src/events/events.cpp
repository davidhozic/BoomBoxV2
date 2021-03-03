#include "Vhod.h"
#include "castimer.h"
#include "../includes/includes.h"
#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "../audio/includes/audio.h"

#define toggleLCD() Hardware.display_enabled = !(Hardware.display_enabled);
#define showSeek()                                       \
    tr_r = mozne_barve.barvni_ptr[evnt_st.menu_seek][0]; \
    tr_z = mozne_barve.barvni_ptr[evnt_st.menu_seek][1]; \
    tr_m = mozne_barve.barvni_ptr[evnt_st.menu_seek][2]; \
    tr_bright = 255;                                     \
    writeTRAK(); // Prikaze element v seeku ce je scroll aktiven

void mic_mode_change();
void button2Events();
void Shutdown();
void external_power_switch_ev();
void internal_power_switch_ev();
void audio_mode_change(char *ch);
extern VHOD napajalnik;
VHOD eventSW(4, 'B', 0);

/* struct click_event_structure
{
    unsigned int hold_time = 0;
    bool double_click = false;
    bool click = true;
    unsigned short press_counter = 0;
} click_event; */

enum menu_seek
{
    TOGGLE_LCD,
    MIC_MODE_CH,
    A_MODE_CH,
    A_MODE_OFF,
    LENGTH
};

enum states
{
    unset,
    SCROLL,
    LLENGTH
};

struct sw2_state_machine_strct
{
    unsigned short state = unset;
    unsigned short menu_seek = TOGGLE_LCD;
    castimer state_exit_timer;
    castimer hold_timer;
    unsigned int hold_time = 0;

} evnt_st;

void events(void *paramOdTaska)
{
    while (true)
    {
        /******************************************** SWITCH 2 EVENTS ****************************************/

        //State machine
        if (Hardware.is_Powered_UP)
        {

            switch (evnt_st.state)
            {
            case unset:
                if (eventSW.vrednost() && evnt_st.hold_timer.vrednost() > 2000)
                {

                    evnt_st.state = SCROLL;
                    evnt_st.state_exit_timer.ponastavi();
                    evnt_st.hold_timer.ponastavi();
                    showSeek(); //Prikaze element v seeku
                    while (eventSW.vrednost())
                    {
                        vTaskDelay(200 / portTICK_PERIOD_MS);
                    }
                }

                else if (!eventSW.vrednost())
                    evnt_st.hold_timer.ponastavi();

                break;
            case SCROLL:

                if (eTaskGetState(audio_system_control) != eSuspended)
                    vTaskSuspend(audio_system_control);

                showSeek();

                if (evnt_st.state_exit_timer.vrednost() > 5000) // auto izhod iz scrolla
                {
                    evnt_st.state = unset;
                    turnOFFstrip();
                    evnt_st.state_exit_timer.ponastavi();
                }

                else if (eventSW.vrednost())
                {
                    evnt_st.hold_time = evnt_st.hold_timer.vrednost(); //stopa cas pritiska
                    evnt_st.state_exit_timer.ponastavi();
                }

                else if (evnt_st.hold_time > 0)
                {

                    if (evnt_st.hold_time < 500)
                        evnt_st.menu_seek += 1 % LENGTH;

                    else if (evnt_st.hold_time > 1000)
                    {
                        turnOFFstrip();
                        switch (evnt_st.menu_seek) //Glede na trenutni menu seek nekaj izvede
                        {
                        case TOGGLE_LCD:
                            toggleLCD();
                            break;
                        case A_MODE_CH:
                            audio_mode_change("");
                            break;

                        case A_MODE_OFF:
                            audio_mode_change("off");
                            break;

                        case MIC_MODE_CH:
                            mic_mode_change();
                            break;
                        }
                        if (eTaskGetState(audio_system_control) == eSuspended) //Resuma se v scrollu saj se izven SCROLL lahko
                            vTaskResume(audio_system_control);                 //izvajajo podtaski AU-sistema, ki suspendajo ta task in takrat se ne sme resumati
                        evnt_st.state = unset;
                        evnt_st.menu_seek = TOGGLE_LCD;
                    }

                    evnt_st.hold_timer.ponastavi();
                    evnt_st.hold_time = 0;
                }
                break;
            }
        }

        //////////////////////////////////////    DELETE?   /////////////////////////////////////////////////
        // {    if (eventSW.vrednost())
        //     {
        //         click_event.hold_time = hold_TIMER.vrednost(); // Calls for value to start the timer

        //         if (click_event.hold_time >= 3000 && click_event.hold_time <= 4000)
        //         {
        //             audio_mode_change("off");
        //         }
        //         Timers.sw2_not_pressed_timer.ponastavi();
        //     }

        //     else if (click_event.hold_time > 0 && Timers.sw2_not_pressed_timer.vrednost() > 50) //Cas nad 0 in filtriranj preklopov
        //     {
        //         hold_TIMER.ponastavi();
        //         if (click_event.hold_time < 500)
        //         {
        //             click_event.press_counter++;
        //             click_timer.ponastavi(); //Podaljsa casovik
        //         }
        //         else if (click_event.hold_time < 2000)
        //         {
        //             audio_mode_change("");
        //         }
        //         click_event.hold_time = 0;
        //     }

        //     /********************** CLICK MACHINE **********************/

        //     if (click_event.press_counter > 0)
        //     {
        //         if (click_timer.vrednost() > 700)
        //         { //Start casovnika
        //             switch (click_event.press_counter)
        //             {
        //             case 1:
        //                 toggleLCD(); // Toggles LCD
        //                 break;

        //             default:
        //                 mic_mode_change();
        //                 break;
        //             }
        //             click_event.press_counter = 0;
        //         }
        //     }
        // }
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
