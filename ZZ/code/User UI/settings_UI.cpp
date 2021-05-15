#include "FreeRTOS.h"
#include "common/inc/global.h"
#include "../audio-visual/includes/audio.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include <util/delay.h>
#include <string.h>
#include "VHOD/Vhod.h"
#include "castimer/castimer.h"
#include "libs/EEPROM/EEPROM.h"
/******************************************************************************************/
/*                                  ELEMENTI V STATE_SCROLL MENIJU                        */
/******************************************************************************************/
enum enum_MAIN_SCROLL_MENU
{
	MENU_TOGGLE_LCD = 0,
	MENU_STRIP_MODE_CHANGE,
	MENU_STRIP_DISABLE,
	MENU_end															  //dolzina
};

/******************************************************************************************/
/*                                 RAZLICNI MENIJI oz. STANJA                             */
/******************************************************************************************/
enum enum_EVENT_STATES
{
	STATE_UNSET = 0,
	STATE_SCROLL,
	STATE_STRIP_SELECTION,
	STATE_END //dolzina
};

/******************************************************************************************/
/*                                  SPREMENLJIVKE EVENTOV                                 */
/******************************************************************************************/
struct struct_settings_UI
{
	uint8_t state : 4;
	uint8_t menu_seek : 4;
	class_VHOD SW2 = class_VHOD(red_button_pin, red_button_port, 0);
	unsigned short hold_time;
	bool long_press;
	class_TIMER SW2_off_timer = class_TIMER();
	class_TIMER state_exit_timer = class_TIMER();
	class_TIMER hold_timer = class_TIMER();
	
	void init()
	{
		state = STATE_UNSET;
		menu_seek =  MENU_TOGGLE_LCD;
		SW2 = class_VHOD(red_button_pin, red_button_port, 0);
		hold_time = 0;
		long_press = false; // Po tem ko se neka stvar zaradi dolgega pritiska izvede, cakaj na izpust
		hold_timer.ponastavi();
		state_exit_timer.ponastavi();
	}
	
	struct_settings_UI()
	{
		init();
	}
}settings_ui;

	/************************************************************************/
	/*							SETTINGS                                    */
	/************************************************************************/

	#define auto_exit_timeout			(20000)

/******************************************************************************************/
/*                                 FUNKCIJE | MAKRI EVENTOV                               */
/******************************************************************************************/
inline void toggleLCD()			
{																														
	Hardware.status_reg.capacity_lcd_en = !Hardware.status_reg.capacity_lcd_en;
	Hardware.status_reg.display_edge = false;
}


inline void showSEEK(struct_settings_UI *control_block)  // Prikaze element v seeku ce je STATE_SCROLL aktiven
{		
	switch(control_block->state)
	{
		case STATE_SCROLL:
			audio_system.current_brightness = 255;
			audio_system.colorSHIFT(control_block->menu_seek, 5);
		break;
		
		case STATE_STRIP_SELECTION:
			if (audio_system.handle_active_strip_mode == NULL)
			{
				xTaskCreate(audio_system.array_strip_modes[control_block->menu_seek], "seek", 128, NULL, 4,&audio_system.handle_active_strip_mode);
			}
		break;
	}
}
inline void exit_scroll()
{
	settings_ui.init();
	audio_system.flashSTRIP();
	audio_system.current_brightness = 255;
	brightDOWN(15);
	delayFREERTOS(500);
	audio_system.stripON();
}
/*******************************************************************************************/

void settings_UI(void *paramOdTaska)
{
	
	while (true)
	{
		/* RESET long press */
		if (settings_ui.SW2.fallingEdge())
		{
			settings_ui.long_press = false;
		}		
		
		//State machine
		if (!settings_ui.long_press)
		{
			switch (settings_ui.state)
			{
				case STATE_UNSET:
					if (Hardware.status_reg.powered_up && settings_ui.hold_timer.vrednost() > 1000)
					{
						audio_system.stripOFF();
						settings_ui.state = STATE_SCROLL;
						settings_ui.menu_seek = MENU_TOGGLE_LCD;
						audio_system.select_strip_color(BELA);
						settings_ui.state_exit_timer.ponastavi();
						settings_ui.hold_timer.ponastavi();
						audio_system.flashSTRIP();
						showSEEK(&settings_ui);
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
				
					if (settings_ui.state_exit_timer.vrednost() > auto_exit_timeout)	// Auto exit_scroll
					{
						exit_scroll();
					}
				
					else if (settings_ui.SW2.vrednost())
					{
						settings_ui.hold_time = settings_ui.hold_timer.vrednost();	// stopa cas pritiska
						settings_ui.state_exit_timer.ponastavi();
						if (settings_ui.hold_time > 1000)
						{
							settings_ui.long_press = true;
							settings_ui.hold_timer.ponastavi();
							settings_ui.hold_time = 0;
							switch (settings_ui.menu_seek)	// Glede na trenutni menu seek nekaj izvede
							{
								case MENU_TOGGLE_LCD:
									toggleLCD();	//Task Zaslon se blocka v zaslon tasku
								break;
							
								case MENU_STRIP_MODE_CHANGE:
									settings_ui.state = STATE_STRIP_SELECTION;
									settings_ui.menu_seek = NORMAL_FADE;
									audio_system.strip_loop_time = 25;
									brightDOWN(20);
									audio_system.select_strip_color(BELA);
									delayFREERTOS(100);
									continue;
								break;
							
								case MENU_STRIP_DISABLE:
									audio_system.strip_mode = STRIP_OFF;
									EEPROM.pisi(audio_system.strip_mode, eeprom_addr_strip_mode);
								break;
							}
							exit_scroll();
						}
					}
					else if (settings_ui.hold_time > 0)
					{

						if (settings_ui.hold_time > 20 && settings_ui.hold_time < 500) //Kratek pritisk
						{
							settings_ui.menu_seek = (settings_ui.menu_seek + 1) % MENU_end;
							showSEEK(&settings_ui);
						}
						settings_ui.hold_timer.ponastavi();
						settings_ui.hold_time = 0;
					}
				break;
				/*****	END CASE *****/
				
				case STATE_STRIP_SELECTION:
					if (settings_ui.state_exit_timer.vrednost() > auto_exit_timeout)
					{
						exit_scroll();
					}
				
					else if (settings_ui.SW2.vrednost())
					{
						settings_ui.state_exit_timer.ponastavi();
						settings_ui.hold_time = settings_ui.hold_timer.vrednost();

						if (settings_ui.hold_time >= 1000)
						{
							settings_ui.long_press = true;
							settings_ui.hold_timer.ponastavi();
							settings_ui.hold_time = 0;
							audio_system.strip_mode = settings_ui.menu_seek;
							EEPROM.pisi(audio_system.strip_mode, eeprom_addr_strip_mode);
							exit_scroll();			
						}
					}
					
					else if (settings_ui.hold_time > 0)
					{
						if (settings_ui.hold_time > 20 && settings_ui.hold_time < 500)
						{
							settings_ui.menu_seek = (settings_ui.menu_seek + 1);
						}
						settings_ui.hold_time = 0;
						settings_ui.hold_timer.ponastavi();
					}
											
					if (settings_ui.menu_seek >= STRIP_OFF)
					{
						settings_ui.menu_seek = NORMAL_FADE;
					}
											
						showSEEK(&settings_ui);
					
				break;
				/*****	END CASE *****/
			}
		}
		delayFREERTOS(5);
		// END WHILE
	}
	// TASK END
}