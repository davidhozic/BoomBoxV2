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
/*                                  ELEMENTI V STATE_SCROLL MENIJU                              */
/******************************************************************************************/
enum enum_EVENT_MENU_SEEK
{
	MENU_TOGGLE_LCD = 0,
	MENU_STRIP_MODE_CHANGE,
	MENU_STRIP_DISABLE,
	MENU_MIC_MODE_CHANGE,
	MENU_end															  //dolzina
};

/******************************************************************************************/
/*                                 RAZLICNI MENIJI oz. STANJA                             */
/******************************************************************************************/
enum enum_EVENT_STATES
{
	STATE_UNSET = 0,
	STATE_SCROLL,
	STATE_END //dolzina
};

/******************************************************************************************/
/*                                  SPREMENLJIVKE EVENTOV                                 */
/******************************************************************************************/
struct struct_settings_UI
{
	uint8_t state = STATE_UNSET;
	uint8_t menu_seek = MENU_TOGGLE_LCD;
	class_TIMER SW2_off_timer;
	class_TIMER state_exit_timer;
	class_TIMER hold_timer;
	unsigned short hold_time = 0;
	bool longPRESS = false; // Po tem ko se neka stvar zaradi dolgega pritiska izvede, cakaj na izpust
};

struct_settings_UI settings_ui;
class_VHOD eventSW(red_button_pin, red_button_port, 0);

/******************************************************************************************/
/*                                 FUNKCIJE | MAKRI EVENTOV                               */
/******************************************************************************************/
#define toggleLCD()                                         \
writeBIT(  Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN ,  !readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN)); \
if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN))                           \
resumeTASK(handle_capacity_display); //Ker se v zaslon tasku blocka v primeru da je display_enabled false

#define show_STATE_SCROLL_Seek() \
STRIP_CURRENT_BRIGHT = 255;       \
colorSHIFT(&settings_ui.menu_seek);
// Prikaze element v seeku ce je STATE_SCROLL aktiven

void exit()
{
	settings_ui.state = STATE_UNSET;
	settings_ui.menu_seek = MENU_TOGGLE_LCD;
	settings_ui.longPRESS = true;
	flash_strip();
	settings_ui.state_exit_timer.ponastavi();
	STRIP_CURRENT_BRIGHT = 255;
	brightDOWN(15);
	delayFREERTOS(100);
	resumeTASK(handle_audio_system);
}

#define check_auto_exit()                           \
if (settings_ui.state_exit_timer.vrednost() > 6000) \
{                                               \
	exit();                                     \
}
// auto izhod iz STATE_SCROLLa
/*******************************************************************************************/


void settings_UI(void *paramOdTaska)
{
	
	while (true)
	{
		if (eventSW.vrednost())
		{
			settings_ui.SW2_off_timer.ponastavi(); // Filtrira lazne nepritiske
		}
		else if (settings_ui.SW2_off_timer.vrednost() > 50)
		{
			settings_ui.longPRESS = false;
		}
		
		
		//State machine
		if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_POWERED_UP) && !settings_ui.longPRESS)
		{
			switch (settings_ui.state)
			{

				case STATE_UNSET:
				if (settings_ui.hold_timer.vrednost() > 1000)
				{
					settings_ui.state = STATE_SCROLL;
					settings_ui.menu_seek = MENU_TOGGLE_LCD;
					settings_ui.state_exit_timer.ponastavi();
					settings_ui.hold_timer.ponastavi();
					stripOFF();
					flash_strip();
					show_STATE_SCROLL_Seek();
					delayFREERTOS(200);
					settings_ui.longPRESS = true;
				}

				else if (!eventSW.vrednost())
				{
					settings_ui.hold_timer.ponastavi();
				}
				break;

				case STATE_SCROLL:
				check_auto_exit(); //Macro to auto exit timer
				if (eventSW.vrednost())
				{
					settings_ui.hold_time = settings_ui.hold_timer.vrednost(); //stopa cas pritiska
					settings_ui.state_exit_timer.ponastavi();

					if (settings_ui.hold_time > 1000)
					{
						settings_ui.longPRESS = true;
						switch (settings_ui.menu_seek) //Glede na trenutni menu seek nekaj izvede
						{
							case MENU_TOGGLE_LCD:
							toggleLCD(); //Task Zaslon se blocka v zaslon tasku
							exit();
							break;
							case MENU_STRIP_MODE_CHANGE:
							strip_mode_CHANGE("", strip_mode_handle_arr);
							exit();
							break;
							case MENU_STRIP_DISABLE:
							strip_mode_CHANGE("off", strip_mode_handle_arr);
							exit();
							break;
							
							case MENU_MIC_MODE_CHANGE:
							mic_mode_CHANGE(mic_mode_handle_arr);
							exit();
							break;
						}
						settings_ui.hold_timer.ponastavi();
					}
				}
				else if (settings_ui.hold_time > 0)
				{

					if (settings_ui.hold_time < 500) //Kratek pritisk
					{
						settings_ui.menu_seek = (settings_ui.menu_seek + 1) % MENU_end;
					}
					show_STATE_SCROLL_Seek();
					settings_ui.hold_timer.ponastavi();
					settings_ui.hold_time = 0;
				}
				break;
			}
		}
		delayFREERTOS(10);
		// END WHILE
	}
	// TASK END
}