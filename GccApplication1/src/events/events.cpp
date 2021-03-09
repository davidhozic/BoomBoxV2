#include "VHOD/Vhod.h"
#include "castimer/castimer.h"
#include "../includes/includes.h"
#include "Arduino_FreeRTOS.h"
#include "../audio/includes/audio.h"

/******************************************************************************************/
/*                                     EXTERN DEKLARACIJE                                 */
/******************************************************************************************/
void Shutdown();
void external_power_switch_ev();
void internal_power_switch_ev();
void strip_mode_chg(char *ch);
extern VHOD napajalnik;

/******************************************************************************************/
/*                                  ELEMENTI V SCROLL MENIJU                              */
/******************************************************************************************/
enum menu_seek_scroll_t
{
    TOGGLE_LCD,
    STRIP_MD_CHG,
    STRIP_OFF,
    menu_seek_LEN //dolzina
};

/******************************************************************************************/
/*                                 RAZLICNI MENIJI oz. STANJA                             */
/******************************************************************************************/
enum states_t
{
    unset,
    SCROLL,
    states_len //dolzina
};

/******************************************************************************************/
/*                                  SPREMENLJIVKE EVENTOV                                 */
/******************************************************************************************/
struct sw2_state_machine_strct
{
    uint8_t state = unset;
    uint8_t menu_seek = TOGGLE_LCD;
    castimer state_exit_timer;
    castimer hold_timer;
    unsigned int hold_time = 0;
    bool longPRESS = false; // Po tem ko se neka stvar zaradi dolgega pritiska izvede, cakaj na izpust
} evnt_st;

VHOD eventSW(4, 'B', 0);
/******************************************************************************************/
/*                                 FUNKCIJE | MAKRI EVENTOV                               */
/******************************************************************************************/
#define toggleLCD()                                         \
    Hardware.display_enabled = !(Hardware.display_enabled); \
    if (Hardware.display_enabled)                           \
        resumeTASK(zaslon_control); //Ker se v zaslon tasku blocka v primeru da je display_enabled false

#define show_scroll_Seek() \
    tr_bright = 255;       \
    colorSHIFT(&evnt_st.menu_seek);
// Prikaze element v seeku ce je scroll aktiven

void exit()
{
    evnt_st.state = unset;
    evnt_st.menu_seek = TOGGLE_LCD;
    evnt_st.longPRESS = true;
    flash_strip();
    evnt_st.state_exit_timer.ponastavi();
    tr_bright = 255;
    brightDOWN(15);
    delay_FRTOS(500);
    resumeTASK(audio_system_control);
}

#define check_auto_exit()                           \
    if (evnt_st.state_exit_timer.vrednost() > 6000) \
    {                                               \
        exit();                                     \
    }
// auto izhod iz scrolla
/*******************************************************************************************/

void events(void *paramOdTaska)
{
    while (true)
    {

        /******************************************** SWITCH 2 EVENTS ****************************************/

        if (eventSW.vrednost())
        {
            Timers.SW2_off_timer.ponastavi(); // Filtrira lazne nepritiske
        }
        else if (Timers.SW2_off_timer.vrednost() > 50)
        {
            evnt_st.longPRESS = false;
        }
        //
        //State machine
        if (Hardware.is_Powered_UP && !evnt_st.longPRESS)
        {
            switch (evnt_st.state)
            {

            case unset:
                if (evnt_st.hold_timer.vrednost() > 1000)
                {
                    evnt_st.state = SCROLL;
                    evnt_st.menu_seek = TOGGLE_LCD;
                    evnt_st.state_exit_timer.ponastavi();
                    evnt_st.hold_timer.ponastavi();
                    turnOFFstrip();
                    flash_strip();
                    show_scroll_Seek();
                    delay_FRTOS(200);
                    evnt_st.longPRESS = true;
                }

                else if (!eventSW.vrednost())
                {
                    evnt_st.hold_timer.ponastavi();
                }
                break;

            case SCROLL:
                check_auto_exit(); //Macro to auto exit timer
                if (eventSW.vrednost())
                {
                    evnt_st.hold_time = evnt_st.hold_timer.vrednost(); //stopa cas pritiska
                    evnt_st.state_exit_timer.ponastavi();

                    if (evnt_st.hold_time > 1000)
                    {
                        evnt_st.longPRESS = true;
                        switch (evnt_st.menu_seek) //Glede na trenutni menu seek nekaj izvede
                        {
                        case TOGGLE_LCD:
                            toggleLCD(); //Task Zaslon se blocka v zaslon tasku
                            exit();
                            break;
                        case STRIP_MD_CHG:
                            strip_mode_chg("");
                            exit();
                            break;
                        case STRIP_OFF:
                            strip_mode_chg("off");
                            exit();
                            break;
                        }
                        evnt_st.hold_timer.ponastavi();
                    }
                }

                else if (evnt_st.hold_time > 0)
                {

                    if (evnt_st.hold_time < 500) //Kratek pritisk
                    {
                        evnt_st.menu_seek = (evnt_st.menu_seek + 1) % menu_seek_LEN;
                    }
                    show_scroll_Seek();
                    evnt_st.hold_timer.ponastavi();
                    evnt_st.hold_time = 0;
                }
                break;
            }
        }

        /******************************** POWER SWITCH EVENTS ********************************/
        if (napajalnik.vrednost() && Hardware.PSW == false)
        {
            _delay_ms(20);
            external_power_switch_ev();
            _delay_ms(20);
        }

        else if (napajalnik.vrednost() == 0 && Hardware.PSW)
        {
            _delay_ms(20);
            internal_power_switch_ev();
            _delay_ms(20);
        }
        delay_FRTOS(30);
        /*************************************************************************************/
    }
}

void external_power_switch_ev()
{
    taskENTER_CRITICAL();
    Shutdown();
    _delay_ms(20);
    PORTD |= (1 << 7);
    Timers.stikaloCAS.ponastavi();
    _delay_ms(20);
    Hardware.PSW = true;
    taskEXIT_CRITICAL();
}

void internal_power_switch_ev()
{
    taskENTER_CRITICAL();
    Shutdown();
    _delay_ms(20);
    PORTD &= ~(1 << 7);
    Timers.stikaloCAS.ponastavi();
    _delay_ms(20);
    Hardware.PSW = false;
    taskEXIT_CRITICAL();
}
