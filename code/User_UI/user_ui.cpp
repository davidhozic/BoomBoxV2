#include "EEPROM.hpp"
#include "FreeRTOS.h"
#include "common/inc/global.hpp"
#include "../audio-visual/includes/audio.hpp"
#include "libs/outputs_inputs/outputs_inputs.hpp"
#include "input/input.hpp"
#include "castimer/castimer.hpp"
#include "user_ui.hpp"
#include "events.hpp"

/******************************************************************************************/
/*                                      STATES                                            */
/******************************************************************************************/

enum SETTINGS_UI_STATES
{
	SU_STATE_UNSET = 0,
	SU_STATE_SCROLL,
	SU_STATE_STRIP_SELECTION,
};

/******************************************************************************************/
/*                                  MENU ENUMS                                            */
/******************************************************************************************/
enum SETTINGS_UI_MENU_SCROLL
{
	SU_MENU_SCROLL_TOGGLE_LCD = 0,
	SU_MENU_SCROLL_STRIP_ANIMATION,
};

enum SETTINGS_UI_MENU_STRIP_SELECTION
{
	SU_MENU_STRIP_ANIMATION_NORMAL_FADE = 0,
	SU_MENU_STRIP_ANIMATION_INVERTED_FADE,
	SU_MENU_STRIP_ANIMATION_BREATHE_FADE,
	SU_MENU_STRIP_ANIMATION_STRIP_OFF
};

/******************************************************************************************/
/*									MENU STRUCTS										  */
/******************************************************************************************/


static SETTINGS_UI_MENU_LIST su_menu_scroll[] =
{
		{ SU_MENU_SCROLL_TOGGLE_LCD, 			SU_STATE_SCROLL },
		{ SU_MENU_SCROLL_STRIP_ANIMATION, 		SU_STATE_SCROLL }
};

static SETTINGS_UI_MENU_LIST su_menu_strip_animation[] =
{
		{ SU_MENU_STRIP_ANIMATION_NORMAL_FADE, 			SU_STATE_STRIP_SELECTION },
		{ SU_MENU_STRIP_ANIMATION_INVERTED_FADE,		SU_STATE_STRIP_SELECTION },
		{ SU_MENU_STRIP_ANIMATION_BREATHE_FADE,			SU_STATE_STRIP_SELECTION },
		{ SU_MENU_STRIP_ANIMATION_STRIP_OFF,			SU_STATE_STRIP_SELECTION }
};

/************************************************************************/
/*							VARIABLE STRUCT	                            */
/************************************************************************/

struct USER_UI
{
	/* Settings ui module variables */
	SETTINGS_UI_STATES state = SU_STATE_UNSET;
	SETTINGS_UI_KEY_EVENT key_event = SU_KEY_CLEAR;
	SETTINGS_UI_KEY_EVENT prev_key_event = SU_KEY_CLEAR;

	unsigned short hold_time;
	uint8_t	menu_seek = 0;
	INPUT_t SW2 = INPUT_t(red_button_pin, red_button_port, 0);
	TIMER_t state_exit_timer;
	TIMER_t hold_timer;
	
	/* LCD charge display module variables */
	TIMER_t lcd_call_timer;
	TIMER_t LCD_timer;
	uint8_t capacity_lcd_en		: 1;	/* This bit is set if it was set in settings ui */
	
	void init()
	{
		state = SU_STATE_UNSET;
		menu_seek =  SU_MENU_SCROLL_TOGGLE_LCD;
		SW2 = INPUT_t(red_button_pin, red_button_port, 0);
		hold_time = 0;
		hold_timer.reset();
		state_exit_timer.reset();
	}

	USER_UI()
	{
		init();
	}
};

USER_UI m_user_ui;

/******************************************************************************************/
/*                                 FUNKCIJE | MAKRI EVENTOV                               */
/******************************************************************************************/

void showSEEK(SETTINGS_UI_MENU_LIST element)  // Prikaze element v seeku ce je SU_STATE_SCROLL aktiven
{		
	switch(element.state)
	{
	case SU_STATE_SCROLL:
		m_audio_system.color_shift(element.index, AUVS_CONFIG_FAST_ANIMATION_TIME_MS);
		brightUP(AUVS_CONFIG_FAST_ANIMATION_TIME_MS);
	break;

	case SU_STATE_STRIP_SELECTION:
		if (element.index == SU_MENU_STRIP_ANIMATION_STRIP_OFF)
		{
			deleteTASK(&m_audio_system.handle_active_strip_mode);
			m_audio_system.color_shift(COLOR_RED, AUVS_CONFIG_FAST_ANIMATION_TIME_MS);
			brightUP(AUVS_CONFIG_FAST_ANIMATION_TIME_MS);
		}
		else if (m_audio_system.handle_active_strip_mode == NULL)
		{
			xTaskCreate(m_audio_system.strip_animations[element.index].f_ptr, "seek", 256, &AUVS::strip_colors[COLOR_WHITE].color_index, 4, &m_audio_system.handle_active_strip_mode);
		}
	break;

	default:
	break;
	}
}

void exit_scroll()
{
	m_audio_system.flash_strip();
	brightDOWN(AUVS_CONFIG_SLOW_ANIMATION_TIME_MS);
	delay_FreeRTOS_ms(1000);
	m_audio_system.strip_on();
	m_user_ui.init();
}
/*******************************************************************************************/



/* user ui task */
void user_ui_task(void *p)
{
	for(;;)
	{
		/* Settigns (scroll) menu */
		settings_UI();
		zaslon();
		delay_FreeRTOS_ms(20);
	}

}


void settings_UI()
{
	/******************************		KEY EVENTS DESCRIPTION 	*******************************
	 * 
	 *	The key events section tracks key press/hold and triggers an event. 
	 *  In the next iteration, after the event has been triggered, the key events
	 *  will set their state to IGNORE all keys until the button has been unpressed.
	 *****************************************************************************************/ 			

	/* Key machine */
	switch(m_user_ui.key_event)
	{
		/* No previous press -> scan for press*/
		case SU_KEY_CLEAR:
			if (m_user_ui.SW2.value())
			{
				m_user_ui.hold_time = m_user_ui.hold_timer.value();

				if (m_user_ui.hold_time >= SU_LONG_PRESS_PERIOD)
				{
					m_user_ui.key_event = SU_KEY_LONG_PRESS;
				}
			}
			else 
			{
				if (m_user_ui.hold_time > 0 && m_user_ui.hold_time < SU_SHORT_PRESS_PERIOD)
				{
					m_user_ui.key_event = SU_KEY_SHORT_PRESS;
				}
				m_user_ui.hold_timer.reset();
				m_user_ui.hold_time = 0;
			}
		break;


		/* Previous press detected -> ignore presses */
		case SU_KEY_LONG_PRESS:
		case SU_KEY_SHORT_PRESS:
			m_user_ui.key_event = SU_KEY_IGNORE_KEY;
		break;

		/* Ignore presses 'till button release */
		case SU_KEY_IGNORE_KEY:
			if (m_user_ui.SW2.value() == 0)
			{
				m_user_ui.key_event = SU_KEY_CLEAR;
				m_user_ui.hold_timer.reset();
				m_user_ui.hold_time = 0;
			}	
		break;
	}


	/**************************************************************************/
	/*		State machine	    */

	if (m_Hardware.status_reg.powered_up)
	{
		switch (m_user_ui.state)
		{
		case SU_STATE_UNSET:
			if (m_user_ui.key_event == SU_KEY_LONG_PRESS)	/* Long press -> Enter into settings scroll menu */
			{
				m_audio_system.strip_off();
				m_user_ui.state = SU_STATE_SCROLL;
				m_user_ui.menu_seek = SU_MENU_SCROLL_TOGGLE_LCD;
				m_audio_system.flash_strip();
			}
		break;
			/*****	END CASE *****/

		case SU_STATE_SCROLL:

			if (m_user_ui.state_exit_timer.value() > SU_AUTO_EXIT_SCROLL_PERIOD)
			{
				exit_scroll();
			}

			showSEEK(su_menu_scroll[m_user_ui.menu_seek]);
			if (m_user_ui.key_event == SU_KEY_LONG_PRESS)	 /* Long press -> execute selected option from the menu */
			{
				switch(m_user_ui.menu_seek)
				{
				case SU_MENU_SCROLL_TOGGLE_LCD:
					m_user_ui.capacity_lcd_en = !m_user_ui.capacity_lcd_en;
					exit_scroll();
				break;

				case SU_MENU_SCROLL_STRIP_ANIMATION:
					m_user_ui.state = SU_STATE_STRIP_SELECTION;
					brightDOWN(AUVS_CONFIG_SLOW_ANIMATION_TIME_MS);
				break;
				}

				m_user_ui.menu_seek = 0;
				break;
			}

			else if (m_user_ui.key_event == SU_KEY_SHORT_PRESS)	/* Short press -> Move to the next element in the menu */
			{
				SU_MENU_SCROLL(m_user_ui.menu_seek, su_menu_scroll);	
				m_user_ui.state_exit_timer.reset();
			}
		break;

			/*****	END CASE *****/

		case SU_STATE_STRIP_SELECTION:

			if (m_user_ui.state_exit_timer.value() > SU_AUTO_EXIT_SCROLL_PERIOD)
			{
				exit_scroll();
			}

			showSEEK(su_menu_strip_animation[m_user_ui.menu_seek]);
			if (m_user_ui.key_event == SU_KEY_LONG_PRESS)
			{
				m_audio_system.set_strip_mode(m_user_ui.menu_seek);
				exit_scroll();
			}

			else if (m_user_ui.key_event == SU_KEY_SHORT_PRESS)	/* Short press -> Move to the next element in the menu */
			{
				SU_MENU_SCROLL(m_user_ui.menu_seek, su_menu_strip_animation);	
				m_user_ui.state_exit_timer.reset();
			}

		break;
			/*****	END CASE *****/

		default:
		break;
		}

		/**************************************************************************/
	}

	/* Speaker has been turned off while in state -> reset */
	else if (m_user_ui.state != SU_STATE_UNSET)
	{
		exit_scroll();
	}
}

void zaslon()
{
	
	if (m_Hardware.status_reg.charging_enabled)
	{
		if ( m_user_ui.LCD_timer.value() >= 1000)
		{
			toggleOUTPUT(BAT_LCD_pin, BAT_LCD_port);
			m_user_ui.LCD_timer.reset();
		}
	}

	else if (m_user_ui.capacity_lcd_en && m_Hardware.status_reg.powered_up)
	{

		if (m_user_ui.LCD_timer.value() < 5000)
		{
			writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
		}
		else if(m_user_ui.LCD_timer.value() <= 8000)
		{
			writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 1);
		}
		else
		{
			m_user_ui.LCD_timer.reset();
		}
	}

	else
	{
		writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
	}


}