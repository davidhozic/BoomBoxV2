#include "Vhod.h"
#include "castimer.h"
#include "../includes/includes.h"
#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include "../audio/includes/audio.h"

#define toggleLCD() Hardware.display_enabled = !(Hardware.display_enabled);
#define showSeek()   \
    tr_bright = 255; \
    colorSHIFT(&evnt_st.menu_seek);
// Prikaze element v seeku ce je scroll aktiven

#define auto_exit()                                 \
    if (evnt_st.state_exit_timer.vrednost() > 6000) \
    {                                               \
        evnt_st.state = unset;                      \
        evnt_st.state_exit_timer.ponastavi();       \
                                                    \
        flash_strip();                              \
        tr_bright = 255;                            \
        brightDOWN(15);                             \
                                                    \
        resumeTASK(audio_system_control);           \
        delay_FRTOS(500);                           \
    }
// auto izhod iz scrolla

void Shutdown();
void external_power_switch_ev();
void internal_power_switch_ev();
void strip_mode_chg(char *ch);
extern VHOD napajalnik;
VHOD eventSW(4, 'B', 0);

enum menu_seek_t
{
    TOGGLE_LCD,
    STRIP_MD_CHG,
    STRIP_OFF,
    menu_seek_LEN //dolzina
};

enum states_t
{
    unset,
    SCROLL,
    states_len //dolzina
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
                /***************************************************************************/
                /*                           NE-NASTAVLJEN STATE                           */
                /***************************************************************************/

            case unset:
                if (evnt_st.hold_timer.vrednost() > 1000)
                {
                    evnt_st.state = SCROLL;
                    evnt_st.menu_seek = TOGGLE_LCD;
                    evnt_st.state_exit_timer.ponastavi();
                    evnt_st.hold_timer.ponastavi();
                    turnOFFstrip();
                    flash_strip();
                    showSeek();
                    delay_FRTOS(200);
                    evnt_st.longPRESS = true;
                }

                else if (!eventSW.vrednost())
                    evnt_st.hold_timer.ponastavi();

                break;

                /***************************************************************************/
                /*                               SCROLL STATE                              */
                /***************************************************************************/

            case SCROLL:
                auto_exit(); //Macro to auto exit timer
                if (eventSW.vrednost())
                {
                    evnt_st.hold_time = evnt_st.hold_timer.vrednost(); //stopa cas pritiska
                    evnt_st.state_exit_timer.ponastavi();

                    if (evnt_st.hold_time > 1000)
                    {
                        flash_strip();
                        switch (evnt_st.menu_seek) //Glede na trenutni menu seek nekaj izvede
                        {
                        case TOGGLE_LCD:
                            toggleLCD(); //Task se blocka v zaslon tasku
                            if (Hardware.display_enabled)
                                resumeTASK(zaslon_control);
                            break;
                        case STRIP_MD_CHG:
                            strip_mode_chg("");
                            break;
                        case STRIP_OFF:
                            strip_mode_chg("off");
                            break;
                            evnt_st.state = unset;
                            evnt_st.menu_seek = TOGGLE_LCD;
                            evnt_st.longPRESS = true;
                            tr_bright = 255;
                            brightDOWN(15); // Zafada navzdol
                            delay_FRTOS(500);
                            resumeTASK(audio_system_control);
                        }
                    }

                    else if (evnt_st.hold_time > 0)
                    {

                        if (evnt_st.hold_time < 500) //Kratek pritisk
                        {
                            evnt_st.menu_seek = (evnt_st.menu_seek + 1) % menu_seek_LEN;
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
