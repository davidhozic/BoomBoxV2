#include "EEPROM.hh"
#include "FreeRTOS.h"
#include "common/inc/global.hh"
#include "../audio-visual/includes/audio.hh"
#include "libs/outputs_inputs/outputs_inputs.hh"
#include "input/input.hpp"
#include "castimer/castimer.hpp"
#include "user_ui.hh"
#include "events.hh"

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
	MENU_TOGGLE_LCD = 0,
	MENU_STRIP_ANIMATION,
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


SETTINGS_UI_MENU_LIST su_menu_scroll[] =
{
		{ MENU_TOGGLE_LCD, 			SU_STATE_SCROLL },
		{ MENU_STRIP_ANIMATION, 	SU_STATE_SCROLL }
};

SETTINGS_UI_MENU_LIST su_menu_strip_animation[] =
{
		{ MENU_NORMAL_FADE, 		SU_STATE_STRIP_SELECTION },
		{ MENU_INVERTED_FADE,		SU_STATE_STRIP_SELECTION },
		{ MENU_BREATHE_FADE,		SU_STATE_STRIP_SELECTION },
		{ MENU_STRIP_OFF,			SU_STATE_STRIP_SELECTION }
};

/******************************************************************************************/
/*                                 RAZLICNI MENIJI oz. STANJA                             */
/******************************************************************************************/


struct SETTINGS_UI
{
	SETTINGS_UI_STATES state = SU_STATE_UNSET;

	SETTINGS_UI_KEY_EVENT key_event = SU_KEY_CLEAR;

	unsigned short hold_time;

	uint8_t	menu_seek = 0;

	INPUT_t SW2 = INPUT_t(red_button_pin, red_button_port, 0);

	TIMER_t SW2_off_timer;
	TIMER_t state_exit_timer;
	TIMER_t hold_timer;



	void init()
	{
		state = SU_STATE_UNSET;
		menu_seek =  MENU_TOGGLE_LCD;
		SW2 = INPUT_t(red_button_pin, red_button_port, 0);
		hold_time = 0;
		hold_timer.reset();
		state_exit_timer.reset();
	}

	SETTINGS_UI()
	{
		init();
	}
}m_su;


/******************************************************************************************/
/*                                 FUNKCIJE | MAKRI EVENTOV                               */
/******************************************************************************************/


void showSEEK(SETTINGS_UI *control_block)  // Prikaze element v seeku ce je SU_STATE_SCROLL aktiven
{		
	switch(control_block->state)
	{
	case SU_STATE_SCROLL:
		m_audio_system.current_brightness = 255;
		m_audio_system.colorSHIFT(control_block->menu_seek, SLOW_ANIMATION_TIME_MS);
		break;

	case SU_STATE_STRIP_SELECTION:
		if (control_block->menu_seek == MENU_STRIP_OFF)
		{
			m_audio_system.colorSHIFT(RED, SLOW_ANIMATION_TIME_MS);
		}
		else if (m_audio_system.handle_active_strip_mode == NULL)
		{
			xTaskCreate(m_audio_system.list_strip_modes[control_block->menu_seek], "seek", 256, NULL, 4, &m_audio_system.handle_active_strip_mode);
		}
		break;

	default:
		break;
	}
}

void exit_scroll()
{
	m_su.init();
	m_audio_system.current_brightness = 255;
	brightDOWN(SLOW_ANIMATION_TIME_MS);
	delayFREERTOS(500);
	m_audio_system.stripON();
}
/*******************************************************************************************/


/* Main user ui task */
void user_ui_task(void *p)
{
	for(;;)
	{
		/********************************/
		/*		USER UI SUB MODULES     */
		/********************************/

		/* Settigns (scroll) menu */
		settings_UI();

		/* Battery charge level display */
		zaslon();

		delayFREERTOS(50);
	}

}




/*******************************************************************************************/

void settings_UI()
{

	/**** 	KEY EVENTS	****/

	/* Ignore all next presses until falling edge is reached */
	if (m_su.key_event != SU_KEY_CLEAR)
		m_su.key_event = SU_KEY_IGNORE_KEY;


	if (m_su.key_event != SU_KEY_IGNORE_KEY)
	{

		if (m_su.SW2.value())
		{
			m_su.hold_time = m_su.hold_timer.value();

			if (m_su.hold_time >= SU_LONG_PRESS_PERIOD)
			{
				m_su.key_event = SU_KEY_LONG_PRESS;
			}
		}

		else if (m_su.hold_time > 0 && m_su.hold_time < SU_SHORT_PRESS_PERIOD)
		{
			m_su.key_event = SU_KEY_SHORT_PRESS;
		}

	}
	else if (!m_su.SW2.value() == 0)
	{
		m_su.key_event = SU_KEY_CLEAR;
		m_su.hold_timer.reset();
		m_su.hold_time = 0;
	}

	/**************************************************************************/

	/*		State machine	    */

	if (m_Hardware.status_reg.powered_up)
	{
		switch (m_su.state)
		{
		case SU_STATE_UNSET:
			/* Waits until long press event is triggered before entering into scroll mode */
			if (m_su.key_event == SU_KEY_LONG_PRESS)
			{
				m_audio_system.stripOFF();
				m_su.state = SU_STATE_SCROLL;
				m_su.menu_seek = MENU_TOGGLE_LCD;
				m_audio_system.flashSTRIP();
			}
		break;
			/*****	END CASE *****/

		case SU_STATE_SCROLL:

			showSEEK(&m_su);
			if (m_su.key_event == SU_KEY_LONG_PRESS)
			{

				switch(m_su.menu_seek)
				{
				case MENU_TOGGLE_LCD:
					m_Hardware.status_reg.capacity_lcd_en = !m_Hardware.status_reg.capacity_lcd_en;
					exit_scroll();
				break;

				case MENU_STRIP_ANIMATION:
					m_su.state = SU_STATE_STRIP_SELECTION;
				break;
				}

				m_su.menu_seek = 0;
				break;
			}

			else if (m_su.key_event == SU_KEY_SHORT_PRESS)
				MENU_NEXT(m_su.menu_seek, su_menu_scroll);

		break;

			/*****	END CASE *****/

		case SU_STATE_STRIP_SELECTION:

			showSEEK(&m_su);
			if (m_su.key_event == SU_KEY_LONG_PRESS)
			{
				m_audio_system.set_strip_mode(m_su.menu_seek);
				exit_scroll();
			}

			else if (m_su.key_event == SU_KEY_SHORT_PRESS)
			{
				MENU_NEXT(m_su.menu_seek, su_menu_strip_animation);
			}

		break;
			/*****	END CASE *****/

		default:
		break;
		}

		/**************************************************************************/
	}

	else if (m_su.state != SU_STATE_UNSET)
	{
		m_su.init();
	}
}




TIMER_t LCD_timer;

void zaslon()
{
	
	if (m_Hardware.status_reg.charging_enabled)
	{
		if ( LCD_timer.value() >= 1000)
		{
			toggleOUTPUT(BAT_LCD_pin, BAT_LCD_port);
			LCD_timer.reset();
		}
	}

	else if (m_Hardware.status_reg.capacity_lcd_en && m_Hardware.status_reg.powered_up)
	{

		if (LCD_timer.value() < 5000)
		{
			writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
		}
		else if(LCD_timer.value() <= 8000)
		{
			writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 1);
		}
		else
		{
			LCD_timer.reset();
		}
	}

	else
	{
		writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
	}


}
