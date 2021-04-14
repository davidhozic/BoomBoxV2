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
/*                                  ELEMENTI V STATE_SCROLL MENIJU                        */
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
	class_VHOD SW2  = class_VHOD(red_button_pin, red_button_port, 0);
	class_TIMER SW2_off_timer;
	class_TIMER state_exit_timer;
	class_TIMER hold_timer;
	unsigned short hold_time = 0;
	bool long_press = false; // Po tem ko se neka stvar zaradi dolgega pritiska izvede, cakaj na izpust	
}settings_ui;




/******************************************************************************************/
/*                                 FUNKCIJE | MAKRI EVENTOV                               */
/******************************************************************************************/
#define toggleLCD()																																		\
writeBIT(  Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN ,  !readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN));		\
if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN))																		         \
	resumeTASK(&handle_capacity_display);	//Ker se v zaslon tasku blocka v primeru da je display_enabled false
	
#define showSEEK(current_el)			\
STRIP_CURRENT_BRIGHT = 255;				\
colorSHIFT(current_el, 2);		// ^^ Prikaze element v seeku ce je STATE_SCROLL aktiven


void exit()
{
	settings_ui.state = STATE_UNSET;
	settings_ui.menu_seek = MENU_TOGGLE_LCD;
	settings_ui.long_press = true;
	flashSTRIP();
	settings_ui.state_exit_timer.ponastavi();
	STRIP_CURRENT_BRIGHT = 255;
	brightDOWN(15);
	delayFREERTOS(100);
	resumeTASK(&handle_audio_system);
}
/*******************************************************************************************/

void settings_UI(void *paramOdTaska)
{
	
	while (true)
	{
		if (settings_ui.SW2.vrednost())
		{
			settings_ui.SW2_off_timer.ponastavi(); // Filtrira lazne nepritiske
		}
		else if (settings_ui.SW2_off_timer.vrednost() > 50)
		{
			settings_ui.long_press = false;
		}


		
		//State machine
		if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_POWERED_UP) && !settings_ui.long_press)
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
					stripOFF(&active_strip_mode);
					flashSTRIP();
					showSEEK(settings_ui.menu_seek);
					delayFREERTOS(200);
					settings_ui.long_press = true;
				}

				else if (!settings_ui.SW2.vrednost())
				{
					settings_ui.hold_timer.ponastavi();
				}
				break;	
		
				/*****	END CASE *****/
			
				case STATE_SCROLL:
				
				if (settings_ui.state_exit_timer.vrednost() > 6000)	// Auto exit
				{
					exit();
				}
				
				if (settings_ui.SW2.vrednost())
				{
					settings_ui.hold_time = settings_ui.hold_timer.vrednost();	// stopa cas pritiska
					settings_ui.state_exit_timer.ponastavi();

					if (settings_ui.hold_time > 1000)
					{
						settings_ui.long_press = true;
						switch (settings_ui.menu_seek)	// Glede na trenutni menu seek nekaj izvede
						{
							case MENU_TOGGLE_LCD:
							toggleLCD();	//Task Zaslon se blocka v zaslon tasku
							break;
							
							case MENU_STRIP_MODE_CHANGE:
							strip_mode_CHANGE("", &active_strip_mode);
							break;
							
							case MENU_STRIP_DISABLE:
							strip_mode_CHANGE("off", &active_strip_mode);
							break;
							
							case MENU_MIC_MODE_CHANGE:
							mic_mode_CHANGE();
							break;
						}
						exit();
						settings_ui.hold_timer.ponastavi();
					}
				}
				else if (settings_ui.hold_time > 0)
				{

					if (settings_ui.hold_time < 500) //Kratek pritisk
					{
						settings_ui.menu_seek = (settings_ui.menu_seek + 1) % MENU_end;
					}
					showSEEK(settings_ui.menu_seek);
					settings_ui.hold_timer.ponastavi();
					settings_ui.hold_time = 0;
				}
				break;
				/*****	END CASE *****/
			}
		}
		delayFREERTOS(2);
		// END WHILE
	}
	// TASK END
}