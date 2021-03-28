#include "VHOD/Vhod.h"
#include "castimer/castimer.h"
#include "FreeRTOS.h"
#include "../audio-visual/includes/audio.h"
#include "common/inc/global.h"
#include <util/delay.h>
#include "libs/outputs_inputs/outputs_inputs.h"
#include "task.h"
#include <string.h>
#include <stdio.h>

/******************************************************************************************/
/*                                     EXTERN DEKLARACIJE                                 */
/******************************************************************************************/
void Shutdown();
void external_power_switch_ev();
void internal_power_switch_ev();
/******************************************************************************************/
/*                                  ELEMENTI V SCROLL MENIJU                              */
/******************************************************************************************/
enum enum_EVENT_MENU_SEEK
{
    TOGGLE_LCD = 0,
    STRIP_MODE_CHANGE,
    STRIP_DISABLE,
	MIC_MODE_CHANGE,
    end_event_menu_seek //dolzina
};

/******************************************************************************************/
/*                                 RAZLICNI MENIJI oz. STANJA                             */
/******************************************************************************************/
enum enum_EVENT_STATES
{
    unset = 0,
    SCROLL,
    states_len //dolzina
};

/******************************************************************************************/
/*                                  SPREMENLJIVKE EVENTOV                                 */
/******************************************************************************************/
struct event_struct_t
{
    uint8_t state = unset;
    uint8_t menu_seek = TOGGLE_LCD;
	class_TIMER SW2_off_timer;
	class_TIMER state_exit_timer;
	class_TIMER hold_timer;
    unsigned short hold_time = 0;
    bool longPRESS = false; // Po tem ko se neka stvar zaradi dolgega pritiska izvede, cakaj na izpust
};

 event_struct_t event_struct;
 class_VHOD eventSW(red_button_pin, red_button_port, 0);
 
/******************************************************************************************/
/*                                 FUNKCIJE | MAKRI EVENTOV                               */
/******************************************************************************************/
#define toggleLCD()                                         \
    writeBIT(  Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN ,  !readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN)); \
    if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN))                           \
        resumeTASK(capacity_display_handle); //Ker se v zaslon tasku blocka v primeru da je display_enabled false

#define show_scroll_Seek() \
    STRIP_CURRENT_BRIGHT = 255;       \
    colorSHIFT(&event_struct.menu_seek);
// Prikaze element v seeku ce je scroll aktiven

void exit()
{
    event_struct.state = unset;
    event_struct.menu_seek = TOGGLE_LCD;
    event_struct.longPRESS = true;
    flash_strip();
    event_struct.state_exit_timer.ponastavi();
    STRIP_CURRENT_BRIGHT = 255;
    brightDOWN(15);
    delayFREERTOS(100);
    resumeTASK(audio_system_handle);
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
        if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_POWERED_UP) && !event_struct.longPRESS)
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
                    stripOFF();
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
                        case STRIP_MODE_CHANGE:
                            strip_mode_change("", strip_mode_handle_arr);
                            exit();
                            break;
                        case STRIP_DISABLE:
                            strip_mode_change("off", strip_mode_handle_arr);
                            exit();
                            break;
							
						case MIC_MODE_CHANGE:
							mic_mode_change(mic_mode_handle_arr);
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
                        event_struct.menu_seek = (event_struct.menu_seek + 1) % end_event_menu_seek;
                    }
                    show_scroll_Seek();
                    event_struct.hold_timer.ponastavi();
                    event_struct.hold_time = 0;
                }
                break;
            }
        }

        /******************************** POWER SWITCH EVENTS ********************************/
        if (napajalnik.vrednost() && readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_EXTERNAL_POWER) == false)
        {
            external_power_switch_ev();
        }

        else if (napajalnik.vrednost() == 0 && readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_EXTERNAL_POWER))
        {
            internal_power_switch_ev();
        }
        delayFREERTOS(15);
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
    writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_EXTERNAL_POWER, 1);
}

void internal_power_switch_ev()
{
    taskENTER_CRITICAL();
    Shutdown();
    _delay_ms(20);
     writeOUTPUT(menjalnik_pin,menjalnik_port, 0);
    stikaloCAS.ponastavi();
    _delay_ms(20);
    writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_EXTERNAL_POWER, 0);
    taskEXIT_CRITICAL();
}
