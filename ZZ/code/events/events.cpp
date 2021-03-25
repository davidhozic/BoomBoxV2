#include "VHOD/Vhod.h"
#include "castimer/castimer.h"
#include "FreeRTOS.h"
#include "../audio/includes/audio.h"
#include "common/inc/global.h"
#include <util/delay.h>
#include "libs/outputs_inputs/outputs_inputs.h"
/******************************************************************************************/
/*                                     EXTERN DEKLARACIJE                                 */
/******************************************************************************************/
void Shutdown();
void external_power_switch_ev();
void internal_power_switch_ev();
void strip_mode_chg(char *ch);
/******************************************************************************************/
/*                                  ELEMENTI V SCROLL MENIJU                              */
/******************************************************************************************/
enum menu_seek_scroll_t
{
    TOGGLE_LCD=0,
    STRIP_MD_CHG=1,
    STRIP_OFF=2,
	MIC_MD_CHG=3,
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
struct event_struct_t
{
    uint8_t state;
    uint8_t menu_seek;
	castimer SW2_off_timer;
	castimer state_exit_timer;
	castimer hold_timer;
    unsigned int hold_time;
    bool longPRESS; // Po tem ko se neka stvar zaradi dolgega pritiska izvede, cakaj na izpust
	
	event_struct_t(){
	    state = unset;
	    menu_seek = TOGGLE_LCD;
	    hold_time = 0;
	    longPRESS = false; // Po tem ko se neka stvar zaradi dolgega pritiska izvede, cakaj na izpust
	}
};

 event_struct_t event_struct;
 VHOD eventSW(red_button_pin, red_button_port, 0);
 
/******************************************************************************************/
/*                                 FUNKCIJE | MAKRI EVENTOV                               */
/******************************************************************************************/
#define toggleLCD()                                         \
    Hardware.display_enabled = !(Hardware.display_enabled); \
    if (Hardware.display_enabled)                           \
        resumeTASK(zaslon_control); //Ker se v zaslon tasku blocka v primeru da je display_enabled false

#define show_scroll_Seek() \
    tr_bright = 255;       \
    colorSHIFT(&event_struct.menu_seek);
// Prikaze element v seeku ce je scroll aktiven

void exit()
{
    event_struct.state = unset;
    event_struct.menu_seek = TOGGLE_LCD;
    event_struct.longPRESS = true;
    flash_strip();
    event_struct.state_exit_timer.ponastavi();
    tr_bright = 255;
    brightDOWN(15);
    delayFREERTOS(100);
    resumeTASK(audio_system_control);
}

#define check_auto_exit()                           \
    if (event_struct.state_exit_timer.vrednost() > 6000) \
    {                                               \
        exit();                                     \
    }
// auto izhod iz scrolla
/*******************************************************************************************/

void events(void *paramOdTaska)
{
	
    while (true)
    {
        /************************************************************************/
        /*					 SWITCH 2 (RED BUTTON) EVENTS                       */
        /************************************************************************/
		
        if (eventSW.vrednost())
        {
           event_struct.SW2_off_timer.ponastavi(); // Filtrira lazne nepritiske
        }
        else if (event_struct.SW2_off_timer.vrednost() > 50)
        {
            event_struct.longPRESS = false;
        }
		
		
        //State machine
        if (Hardware.is_Powered_UP && !event_struct.longPRESS)
        {
            switch (event_struct.state)
            {

            case unset:
                if (event_struct.hold_timer.vrednost() > 1000)
                {
                    event_struct.state = SCROLL;
                    event_struct.menu_seek = TOGGLE_LCD;
                    event_struct.state_exit_timer.ponastavi();
                    event_struct.hold_timer.ponastavi();
                    turnOFFstrip();
                    flash_strip();
                    show_scroll_Seek();
                    delayFREERTOS(200);
                    event_struct.longPRESS = true;
                }

                else if (!eventSW.vrednost())
                {
                    event_struct.hold_timer.ponastavi();
                }
                break;

            case SCROLL:
                check_auto_exit(); //Macro to auto exit timer
                if (eventSW.vrednost())
                {
                    event_struct.hold_time = event_struct.hold_timer.vrednost(); //stopa cas pritiska
                    event_struct.state_exit_timer.ponastavi();

                    if (event_struct.hold_time > 1000)
                    {
                        event_struct.longPRESS = true;
                        switch (event_struct.menu_seek) //Glede na trenutni menu seek nekaj izvede
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
							
						case MIC_MD_CHG:
							Audio_vars.MIC_MODE = (Audio_vars.MIC_MODE + 1) % mic_enum_len;
							exit();
							break;	
                        }
                        event_struct.hold_timer.ponastavi();
                    }
                }

                else if (event_struct.hold_time > 0)
                {

                    if (event_struct.hold_time < 500) //Kratek pritisk
                    {
                        event_struct.menu_seek = (event_struct.menu_seek + 1) % menu_seek_LEN;
                    }
                    show_scroll_Seek();
                    event_struct.hold_timer.ponastavi();
                    event_struct.hold_time = 0;
                }
                break;
            }
        }

        /******************************** POWER SWITCH EVENTS ********************************/
        if (napajalnik.vrednost() && Hardware.PSW == false)
        {
            external_power_switch_ev();
        }

        else if (napajalnik.vrednost() == 0 && Hardware.PSW)
        {
            internal_power_switch_ev();
        }
        delayFREERTOS(5);
        /*************************************************************************************/
		// END WHILE
	}
	// TASK END
}

void external_power_switch_ev()
{
    taskENTER_CRITICAL();
    Shutdown();
    _delay_ms(20);
    writeOUTPUT(menjalnik_pin,menjalnik_port,1);
    stikaloCAS.ponastavi();
	taskEXIT_CRITICAL();
    Hardware.PSW = true;
}

void internal_power_switch_ev()
{
    taskENTER_CRITICAL();
    Shutdown();
    _delay_ms(20);
     writeOUTPUT(menjalnik_pin,menjalnik_port, 0);
    stikaloCAS.ponastavi();
    _delay_ms(20);
    Hardware.PSW = false;
    taskEXIT_CRITICAL();
}
