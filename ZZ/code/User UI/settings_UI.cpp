#include "VHOD/Vhod.h"
#include "castimer/castimer.h"
#include "FreeRTOS.h"
#include "../audio-visual/includes/audio.h"
#include "common/inc/global.h"
#include <util/delay.h>
#include "libs/outputs_inputs/outputs_inputs.h"
#include "task.h"
#include <string.h>

/******************************************************************************************/
/*                                  ELEMENTI V STATE_SCROLL MENIJU                        */
/******************************************************************************************/
enum enum_MAIN_SCROLL_MENU
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
	STATE_STRIP_SELECTION,
	STATE_END //dolzina
};

/******************************************************************************************/
/*                                  SPREMENLJIVKE EVENTOV                                 */
/******************************************************************************************/
struct struct_settings_UI
{
	uint8_t state;
	uint8_t menu_seek;
	class_VHOD SW2;
	unsigned short hold_time;
	bool long_press;
	class_TIMER SW2_off_timer;
	class_TIMER state_exit_timer;
	class_TIMER hold_timer;
};

	
struct_settings_UI settings_ui = {
	.state = STATE_UNSET,
	.menu_seek =  MENU_TOGGLE_LCD,
	.SW2 = class_VHOD(red_button_pin, red_button_port, 0),
	.hold_time = 0,
	.long_press = false // Po tem ko se neka stvar zaradi dolgega pritiska izvede, cakaj na izpust
};

/******************************************************************************************/
/*                                 FUNKCIJE | MAKRI EVENTOV                               */
/******************************************************************************************/
inline void toggleLCD()			
{																														
	writeBIT(  Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN ,  !readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN));	
}


inline void showSEEK(struct_settings_UI *control_block)  // Prikaze element v seeku ce je STATE_SCROLL aktiven
{		
	switch(control_block->state)
	{
		case STATE_SCROLL:
			STRIP_CURRENT_BRIGHT = 255;
			colorSHIFT(control_block->menu_seek, 5);
		break;
		
		case STATE_STRIP_SELECTION:
			if (audio_system.handle_active_strip_mode == NULL)
			{
				deleteTASK(&audio_system.handle_active_strip_mode);
				xTaskCreate(audio_system.array_strip_modes[control_block->menu_seek], "seek", 128, &audio_system.barva_selekt,4,&audio_system.handle_active_strip_mode);
			}
		break;
	}
}
inline void exit_scroll()
{
	settings_ui.state = STATE_UNSET;
	settings_ui.menu_seek = MENU_TOGGLE_LCD;
	settings_ui.long_press = true;
	flashSTRIP();
	settings_ui.state_exit_timer.ponastavi();
	STRIP_CURRENT_BRIGHT = 255;
	brightDOWN(15);
	delayFREERTOS(1000);
	stripON();
}
/*******************************************************************************************/

void settings_UI(void *paramOdTaska)
{
	
	while (true)
	{
		if (settings_ui.SW2.vrednost())
		{
			settings_ui.SW2_off_timer.ponastavi();							// Filtrira lazne nepritiske
		}
		else if (settings_ui.SW2_off_timer.vrednost() > 50)
		{
			settings_ui.long_press = false;
		}	
		
		//State machine
		if (!settings_ui.long_press)
		{
			switch (settings_ui.state)
			{
				case STATE_UNSET:
				if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_POWERED_UP) && settings_ui.hold_timer.vrednost() > 1000)
				{
					stripOFF();
					settings_ui.state = STATE_SCROLL;
					settings_ui.menu_seek = MENU_TOGGLE_LCD;
					settings_ui.state_exit_timer.ponastavi();
					settings_ui.hold_timer.ponastavi();
					flashSTRIP();
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
				
				if (settings_ui.state_exit_timer.vrednost() > 6000)	// Auto exit_scroll
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
						switch (settings_ui.menu_seek)	// Glede na trenutni menu seek nekaj izvede
						{
							case MENU_TOGGLE_LCD:
								toggleLCD();	//Task Zaslon se blocka v zaslon tasku
							break;
							
							case MENU_STRIP_MODE_CHANGE:
								settings_ui.state = STATE_STRIP_SELECTION;
								 audio_system.strip_mode == STRIP_OFF ? settings_ui.menu_seek = 0 : settings_ui.menu_seek = audio_system.strip_mode;
								brightDOWN(20);
								delayFREERTOS(500);
								continue;
							break;
							
							case MENU_STRIP_DISABLE:
								STRIP_MODE = STRIP_OFF;
							break;
							
							case MENU_MIC_MODE_CHANGE:
								MIC_MODE = (MIC_MODE + 1) %	end_mic_modes;
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
				
					if (settings_ui.state_exit_timer.vrednost() > 6000)
					{
						exit_scroll();
					}
				
					else if (settings_ui.SW2.vrednost() && !settings_ui.long_press)
					{
						settings_ui.state_exit_timer.ponastavi();
						settings_ui.hold_time = settings_ui.hold_timer.vrednost();
						
						if (settings_ui.hold_time >= 1000)
						{
							settings_ui.long_press = true;
							audio_system.strip_mode = settings_ui.menu_seek;
							exit_scroll();			
						}
						
						else if (settings_ui.hold_time > 0)
						{
							if (settings_ui.hold_time > 50 && settings_ui.hold_time < 500)
							{
								settings_ui.menu_seek = (settings_ui.menu_seek + 1) % enum_STRIP_MODES::end_strip_modes;
								deleteTASK(&audio_system.handle_active_strip_mode);
							}
							settings_ui.hold_time = 0;
							settings_ui.hold_timer.ponastavi();
						}
				
						showSEEK(&settings_ui);		
					}
					
				break;
			}
		}
		delayFREERTOS(5);
		// END WHILE
	}
	// TASK END
}