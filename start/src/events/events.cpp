#include "Vhod.h"
#include "castimer.h"
#include "../includes/includes.h"
#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "../audio/includes/audio.h"

#define toggleLCD() Hardware.display_enabled = !(Hardware.display_enabled);
#define showSeek()                  \
    tr_bright = 255;                \
    deleteTask(color_fade_control); \
    colorSHIFT(&evnt_st.menu_seek); \
// Prikaze element v seeku ce je scroll aktiven

#define auto_exit()                                 \
    if (evnt_st.state_exit_timer.vrednost() > 6000) \
    {                                               \
        evnt_st.state = unset;                      \
        evnt_st.state_exit_timer.ponastavi();       \
                                                    \
        flash_strip(evnt_st.menu_seek);                              \
        brightDOWN();                               \
                                                    \
        resumeTASK(audio_system_control);           \
        delay_FRTOS(500);                           \
    }
// auto izhod iz scrolla

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
    STRIP_MD_CHG,
    STRIP_OFF,
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
    uint8_t state = unset;
    uint8_t menu_seek = TOGGLE_LCD;
    castimer state_exit_timer;
    castimer hold_timer;
    unsigned int hold_time = 0;
    bool longPRESS = false; // Po tem ko se neka stvar zaradi dolgega pritiska izvede, cakaj na izpust
} evnt_st;

void events(void *paramOdTaska)
{
    while (true)
    {

        /******************************************** SWITCH 2 EVENTS ****************************************/

        if (eventSW.vrednost())
        {
            Timers.SW2_off_timer.ponastavi(); // Filtrira lazne nepritiske
        }
        else if (Timers.SW2_off_timer.vrednost() > 100)
        {
            evnt_st.longPRESS = false;
        }

        //State machine
        if (Hardware.is_Powered_UP && !evnt_st.longPRESS)
        {
            switch (evnt_st.state)
            {
            case unset:
                if (eventSW.vrednost() && evnt_st.hold_timer.vrednost() > 1000)
                {
                    holdTASK(audio_system_control);
                    deleteALL_subAUDIO_tasks();
                    evnt_st.state = SCROLL;
                    evnt_st.menu_seek = TOGGLE_LCD;
                    evnt_st.state_exit_timer.ponastavi();
                    evnt_st.hold_timer.ponastavi();
                    turnOFFstrip();
                    flash_strip(evnt_st.menu_seek);
                    delay_FRTOS(200);
                    evnt_st.longPRESS = true;
                }

                else if (!eventSW.vrednost())
                    evnt_st.hold_timer.ponastavi();

                break;
            case SCROLL:
                auto_exit(); //Macro to auto exit timer
                if (eventSW.vrednost())
                {
                    evnt_st.hold_time = evnt_st.hold_timer.vrednost(); //stopa cas pritiska
                    evnt_st.state_exit_timer.ponastavi();

                    if (evnt_st.hold_time > 1000)
                    {

                        flash_strip(evnt_st.menu_seek);
                        brightDOWN(); // Zafada navzdol

                        switch (evnt_st.menu_seek) //Glede na trenutni menu seek nekaj izvede
                        {
                        case TOGGLE_LCD:
                            toggleLCD();
                            break;
                        case STRIP_MD_CHG:
                            audio_mode_change("");
                            break;

                        case STRIP_OFF:
                            audio_mode_change("off");
                            break;

                        case MIC_MODE_CH:
                            mic_mode_change();
                            break;
                        }
                        evnt_st.state = unset;
                        evnt_st.menu_seek = TOGGLE_LCD;
                        evnt_st.longPRESS = true;
                        delay_FRTOS(500);
                        resumeTASK(audio_system_control);
                    }
                }

                else if (evnt_st.hold_time > 0)
                {

                    if (evnt_st.hold_time < 500)
                    {
                        evnt_st.menu_seek = (evnt_st.menu_seek + 1) % LENGTH;
                    }
                    showSeek();
                    evnt_st.hold_timer.ponastavi();
                    evnt_st.hold_time = 0;
                }
                break;
            }
        }

        /******************************** POWER SWITCH EVENTS ********************************/
        if (napajalnik.vrednost() && Hardware.PSW == false)
        {
            delay(20);
            external_power_switch_ev();
            delay(20);
        }

        else if (napajalnik.vrednost() == 0 && Hardware.PSW)
        {
            delay(20);
            internal_power_switch_ev();
            delay(20);
        }
        delay_FRTOS(30);
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
