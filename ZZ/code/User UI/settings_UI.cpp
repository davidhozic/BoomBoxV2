#include "FreeRTOS.h"
#include "common/inc/global.hh"
#include "EEPROM.hh"	
#include "../audio-visual/includes/audio.hh"
#include "libs/outputs_inputs/outputs_inputs.hh"
#include <util/delay.h>
#include <string.h>
#include "input/input.hh"
#include "castimer/castimer.hh"
#include "libs/EEPROM/EEPROM.hh"
/******************************************************************************************/
/*                                  ELEMENTI V STATE_SCROLL MENIJU                        */
/******************************************************************************************/
enum enum_MAIN_SCROLL_MENU
{
	MENU_TOGGLE_LCD = 0,
	MENU_STRIP_MODE_CHANGE,
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
	enum_EVENT_STATES state = STATE_UNSET;
	uint8_t	menu_seek = 0;
	class_INPUT SW2 = class_INPUT(red_button_pin, red_button_port, 0);
	unsigned short hold_time;
	bool long_press;
	class_TIMER SW2_off_timer;
	class_TIMER state_exit_timer;
	class_TIMER hold_timer;
	
	void init()
	{
		state = STATE_UNSET;
		menu_seek =  MENU_TOGGLE_LCD;
		SW2 = class_INPUT(red_button_pin, red_button_port, 0);
		hold_time = 0;
		long_press = false; // Po tem ko se neka stvar zaradi dolgega pritiska izvede, cakaj na izpust
		hold_timer.reset();
		state_exit_timer.reset();
	}
	
	struct_settings_UI()
	{
		init();
	}
}settings_ui;

/*******************************************************************************************/
/*							SETTINGS						                               */
/*******************************************************************************************/
#define auto_exit_timeout			(20000)

/******************************************************************************************/
/*                                 FUNKCIJE | MAKRI EVENTOV                               */
/******************************************************************************************/
inline void toggleLCD()			
{																														
	m_Hardware.status_reg.capacity_lcd_en = !m_Hardware.status_reg.capacity_lcd_en;
}


void showSEEK(struct_settings_UI *control_block)  // Prikaze element v seeku ce je STATE_SCROLL aktiven
{		
	switch(control_block->state)
	{
		case STATE_SCROLL:
			m_audio_system.current_brightness = 255;
			m_audio_system.colorSHIFT(control_block->menu_seek, SLOW_ANIMATION_TIME_MS);
		break;
		
		case STATE_STRIP_SELECTION:
			if (m_audio_system.strip_mode == STRIP_OFF)
			{
				m_audio_system.colorSHIFT(RED, SLOW_ANIMATION_TIME_MS);				
			}
			else if (m_audio_system.handle_active_strip_mode == NULL)
			{
				xTaskCreate(m_audio_system.array_strip_modes[control_block->menu_seek], "seek", 128, NULL, 4,&m_audio_system.handle_active_strip_mode);
			}
		break;
		
		default:
		break;
	}
}

void exit_scroll()
{
	settings_ui.init();
	m_audio_system.flashSTRIP();
	m_audio_system.current_brightness = 255;
	brightDOWN(SLOW_ANIMATION_TIME_MS);
	delayFREERTOS(500);
	m_audio_system.stripON();
}
/*******************************************************************************************/

void settings_UI(void *paramOdTaska)
{
	while (true)
	{
		/* RESET long press */
		if (settings_ui.SW2.fallen_edge())
		{
			settings_ui.long_press = false;
		}		
		
		//State machine
		if (!settings_ui.long_press && m_Hardware.status_reg.powered_up)
		{
			switch (settings_ui.state)
			{
				case STATE_UNSET:
					if (m_Hardware.status_reg.powered_up && settings_ui.hold_timer.value() > 1000)
					{
						m_audio_system.stripOFF();
						settings_ui.state = STATE_SCROLL;
						settings_ui.menu_seek = MENU_TOGGLE_LCD;
						settings_ui.state_exit_timer.reset();
						settings_ui.hold_timer.reset();
						m_audio_system.flashSTRIP();
						showSEEK(&settings_ui);
						delayFREERTOS(200);
						settings_ui.long_press = true;
					}
					
					else if (!settings_ui.SW2.value())
					{
						settings_ui.hold_timer.reset();
					}
				break;	
		
				/*****	END CASE *****/
			
				case STATE_SCROLL:
				
					if (settings_ui.state_exit_timer.value() > auto_exit_timeout)	// Auto exit_scroll
					{
						exit_scroll();
					}
				
					else if (settings_ui.SW2.value())
					{
						settings_ui.hold_time = settings_ui.hold_timer.value();	// stopa cas pritiska
						settings_ui.state_exit_timer.reset();
						if (settings_ui.hold_time > 1000)
						{
							settings_ui.long_press = true;
							settings_ui.hold_timer.reset();
							settings_ui.hold_time = 0;
							switch (settings_ui.menu_seek)	// Glede na trenutni menu seek nekaj izvede
							{
								case MENU_TOGGLE_LCD:
									toggleLCD();	//Task Zaslon se blocka v zaslon tasku
								break;
							
								case MENU_STRIP_MODE_CHANGE:
									settings_ui.state = STATE_STRIP_SELECTION;
									settings_ui.menu_seek = NORMAL_FADE;
									brightDOWN(SLOW_ANIMATION_TIME_MS);
									m_audio_system.select_strip_color(BELA);
									delayFREERTOS(100);
									continue;
								break;
							}
							exit_scroll();
						}
					}
					else if (settings_ui.hold_time > 0)
					{

						if (settings_ui.hold_time < 500) //Kratek pritisk
						{
							settings_ui.menu_seek = (settings_ui.menu_seek + 1) % MENU_end;
							showSEEK(&settings_ui);
						}
						settings_ui.hold_timer.reset();
						settings_ui.hold_time = 0;
					}
				break;
				/*****	END CASE *****/
				
				case STATE_STRIP_SELECTION:
					if (settings_ui.state_exit_timer.value() > auto_exit_timeout)
					{
						exit_scroll();
					}
				
					else if (settings_ui.SW2.value())
					{
						settings_ui.state_exit_timer.reset();
						settings_ui.hold_time = settings_ui.hold_timer.value();

						if (settings_ui.hold_time >= 1000)
						{
							settings_ui.long_press = true;
							settings_ui.hold_timer.reset();
							settings_ui.hold_time = 0;
							m_audio_system.strip_mode = static_cast<enum_STRIP_MODES> (settings_ui.menu_seek);
							m_audio_system.save_strip_mode();
							exit_scroll();			
						}
					}
					
					else if (settings_ui.hold_time > 0)
					{
						if (settings_ui.hold_time > 20 && settings_ui.hold_time < 500)
						{
							settings_ui.menu_seek = (settings_ui.menu_seek + 1) % enum_STRIP_MODES::end_strip_modes;
						}
						settings_ui.hold_time = 0;
						settings_ui.hold_timer.reset();
					}
																	
					showSEEK(&settings_ui);
					
				break;
				/*****	END CASE *****/
				
				default:
				break;
			}
		}
		delayFREERTOS(20);
		// END WHILE
	}
	// TASK END
}