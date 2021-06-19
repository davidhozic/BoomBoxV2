#include "EEPROM.hh"
#include "FreeRTOS.h"
#include "common/inc/global.hh"
#include "../audio-visual/includes/audio.hh"
#include "libs/outputs_inputs/outputs_inputs.hh"
#include "input/input.hh"
#include "castimer/castimer.hh"
#include <settings_ui.hh>


/******************************************************************************************/
/*                                      STATES                                            */
/******************************************************************************************/

enum SETTINGS_UI_STATES
{
	STATE_UNSET = 0,
	STATE_SCROLL,
	STATE_STRIP_SELECTION,
};

/******************************************************************************************/
/*                                  MENU ENUMS                                            */
/******************************************************************************************/
enum SETTINGS_UI_MENU_SCROLL
{
	MENU_TOGGLE_LCD = 0,
	MENU_STRIP_MODE_CHANGE,
};

enum SETTINGS_UI_MENU_STRIP_SELECTION
{
	MENU_NORMAL_FADE = 0,
	MENU_INVERTED_FADE,
	MENU_BREATHE_FADE,
	MENU_STRIP_OFF
};

/******************************************************************************************/
/*									MENU STRUCTS										  */
/******************************************************************************************/


SETTINGS_UI_MENU_LIST settings_ui_menu_scroll[] =
{
	{ MENU_TOGGLE_LCD, 			STATE_SCROLL },
	{ MENU_STRIP_MODE_CHANGE, 	STATE_SCROLL }
};

SETTINGS_UI_MENU_LIST settings_ui_menu_strip_select[] =
{
	{ MENU_NORMAL_FADE, 		STATE_STRIP_SELECTION },
	{ MENU_INVERTED_FADE,		STATE_STRIP_SELECTION },
	{ MENU_BREATHE_FADE,		STATE_STRIP_SELECTION },
	{ MENU_STRIP_OFF,			STATE_STRIP_SELECTION }
};

/******************************************************************************************/
/*                                 RAZLICNI MENIJI oz. STANJA                             */
/******************************************************************************************/

struct SETTINGS_UI
{
	SETTINGS_UI_STATES state = STATE_UNSET;
	uint8_t	menu_seek = 0;
	INPUT_t SW2 = INPUT_t(red_button_pin, red_button_port, 0);
	unsigned short hold_time;
	bool long_press;
	TIMER_t SW2_off_timer;
	TIMER_t state_exit_timer;
	TIMER_t hold_timer;
	
	void init()
	{
		state = STATE_UNSET;
		menu_seek =  MENU_TOGGLE_LCD;
		SW2 = INPUT_t(red_button_pin, red_button_port, 0);
		hold_time = 0;
		long_press = false; // Po tem ko se neka stvar zaradi dolgega pritiska izvede, cakaj na izpust
		hold_timer.reset();
		state_exit_timer.reset();
	}
	
	SETTINGS_UI()
	{
		init();
	}
}m_settings_ui;


/******************************************************************************************/
/*                                 FUNKCIJE | MAKRI EVENTOV                               */
/******************************************************************************************/
inline void toggleLCD()			
{																														
	m_Hardware.status_reg.capacity_lcd_en = !m_Hardware.status_reg.capacity_lcd_en;
}


void showSEEK(SETTINGS_UI *control_block)  // Prikaze element v seeku ce je STATE_SCROLL aktiven
{		
	switch(control_block->state)
	{
		case STATE_SCROLL:
			m_audio_system.current_brightness = 255;
			m_audio_system.colorSHIFT(control_block->menu_seek, SLOW_ANIMATION_TIME_MS);
		break;
		
		case STATE_STRIP_SELECTION:
			if (control_block->menu_seek == MENU_STRIP_OFF)
			{
				m_audio_system.colorSHIFT(RED, SLOW_ANIMATION_TIME_MS);				
			}
			else if (m_audio_system.handle_active_strip_mode == NULL)
			{
				xTaskCreate(m_audio_system.list_strip_modes[control_block->menu_seek], "seek", 128, NULL, 4, &m_audio_system.handle_active_strip_mode);
			}
		break;
		
		default:
		break;
	}
}

void exit_scroll()
{
	m_settings_ui.init();
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
		if (m_settings_ui.SW2.fallen_edge())
		{
			m_settings_ui.long_press = false;
		}		
		
		//State machine
		if (!m_settings_ui.long_press && m_Hardware.status_reg.powered_up)
		{
			switch (m_settings_ui.state)
			{
				case STATE_UNSET:
					if (m_Hardware.status_reg.powered_up && m_settings_ui.hold_timer.value() > 1000)
					{
						m_audio_system.stripOFF();
						m_settings_ui.state = STATE_SCROLL;
						m_settings_ui.menu_seek = MENU_TOGGLE_LCD;
						m_settings_ui.state_exit_timer.reset();
						m_settings_ui.hold_timer.reset();
						m_audio_system.flashSTRIP();
						showSEEK(&m_settings_ui);
						m_settings_ui.long_press = true;
					}
					
					else if (!m_settings_ui.SW2.value())
					{
						m_settings_ui.hold_timer.reset();
					}
				break;	
		
				/*****	END CASE *****/
			
				case STATE_SCROLL:
				
					if (m_settings_ui.state_exit_timer.value() > auto_exit_timeout)	// Auto exit_scroll
					{
						exit_scroll();
					}
				
					else if (m_settings_ui.SW2.value())
					{
						m_settings_ui.hold_time = m_settings_ui.hold_timer.value();	// stopa cas pritiska
						m_settings_ui.state_exit_timer.reset();
						if (m_settings_ui.hold_time > 1000)
						{
							m_settings_ui.long_press = true;
							m_settings_ui.hold_timer.reset();
							m_settings_ui.hold_time = 0;
							switch (m_settings_ui.menu_seek)	// Glede na trenutni menu seek nekaj izvede
							{
								case MENU_TOGGLE_LCD:
									toggleLCD();	//Task Zaslon se blocka v zaslon tasku
								break;
							
								case MENU_STRIP_MODE_CHANGE:
									m_settings_ui.state = STATE_STRIP_SELECTION;
									m_settings_ui.menu_seek = NORMAL_FADE;
									brightDOWN(SLOW_ANIMATION_TIME_MS);
									m_audio_system.select_strip_color(BELA);
									delayFREERTOS(100);
									continue;
								break;
							}
							exit_scroll();
						}
					}
					else if (m_settings_ui.hold_time > 0)
					{

						if (m_settings_ui.hold_time < 500) //Kratek pritisk
						{
							MENU_NEXT(m_settings_ui.menu_seek, settings_ui_menu_scroll);
							showSEEK(&m_settings_ui);
						}
						m_settings_ui.hold_timer.reset();
						m_settings_ui.hold_time = 0;
					}
				break;
				/*****	END CASE *****/
				
				case STATE_STRIP_SELECTION:
					if (m_settings_ui.state_exit_timer.value() > auto_exit_timeout)
					{
						exit_scroll();
					}
				
					else if (m_settings_ui.SW2.value())
					{
						m_settings_ui.state_exit_timer.reset();
						m_settings_ui.hold_time = m_settings_ui.hold_timer.value();

						if (m_settings_ui.hold_time >= 1000)
						{
							m_settings_ui.long_press = true;
							m_settings_ui.hold_timer.reset();
							m_settings_ui.hold_time = 0;
							m_audio_system.strip_mode = static_cast <enum_STRIP_MODES> (m_settings_ui.menu_seek);
							m_audio_system.save_strip_mode();
							exit_scroll();			
						}
					}
					
					else if (m_settings_ui.hold_time > 0)
					{
						if (m_settings_ui.hold_time > 20 && m_settings_ui.hold_time < 500)
						{
							MENU_NEXT(m_settings_ui.menu_seek, settings_ui_menu_strip_select);
						}
						m_settings_ui.hold_time = 0;
						m_settings_ui.hold_timer.reset();
					}
																	
					showSEEK(&m_settings_ui);
					
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