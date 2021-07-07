#include "EEPROM.hpp"
#include "FreeRTOS.h"
#include "common/inc/common.hpp"
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
	SU_STATE_STRIP_SELECTION
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

static SETTINGS_UI_MENU_LIST su_menu_scroll[]  =
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

	unsigned short hold_time;
	uint8_t	menu_seek = 0;
	INPUT_t SW2 = INPUT_t(GLOBAL_CFG_PIN_SU_SWITCH, GLOBAL_CFG_PORT_SU_SWITCH, 0);
	TIMER_t state_exit_timer;
	TIMER_t hold_timer;
	
	/* LCD charge display module variables */
	TIMER_t lcd_call_timer;
	TIMER_t LCD_timer;
	uint8_t capacity_lcd_en;	/* This bit is set if it was set in settings ui */
	
	void init()
	{
		state = SU_STATE_UNSET;
		menu_seek =  SU_MENU_SCROLL_TOGGLE_LCD;
		hold_time = 0;
		hold_timer.reset();
		state_exit_timer.reset();
	}

	USER_UI()
	{
		init();
	}
};
static USER_UI m_user_ui;



/* user ui task */
void user_ui_task(void *p)
{
    for(;;)
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

                    if (m_user_ui.hold_time >= SU_CFG_LONG_PRESS_PERIOD)
                    {
                        m_user_ui.key_event = SU_KEY_LONG_PRESS;
                    }
                }
                else if (m_user_ui.hold_time > 0)
                {
                    if(m_user_ui.hold_time < SU_CFG_SHORT_PRESS_PERIOD)
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

        if (m_hw_status.powered_up)
        {
            switch (m_user_ui.state)
            {
            case SU_STATE_UNSET:
                if (m_user_ui.key_event == SU_KEY_LONG_PRESS)	/* Long press -> Enter into settings scroll menu */
                {
                    enter_scroll();
                }
            break;
                /*****	END CASE *****/

            case SU_STATE_SCROLL:

                if (m_user_ui.state_exit_timer.value() > SU_CFG_AUTO_EXIT_SCROLL_PERIOD)
                {
                    exit_scroll();
                    break;
                }

                
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
                        m_user_ui.menu_seek = 0;
                        brightDOWN(AUVS_CFG_SLOW_ANIMATION_TIME_MS);
                    break;
                    }

                    m_user_ui.menu_seek = 0;
                    break;
                }

                else if (m_user_ui.key_event == SU_KEY_SHORT_PRESS)	/* Short press -> Move to the next element in the menu */
                {
                    SU_NEXT(m_user_ui.menu_seek, su_menu_scroll);	
                    showSEEK(su_menu_scroll[m_user_ui.menu_seek]);
                    m_user_ui.state_exit_timer.reset();
                }
            break;

                /*****	END CASE *****/

            case SU_STATE_STRIP_SELECTION:

                if (m_user_ui.state_exit_timer.value() > SU_CFG_AUTO_EXIT_SCROLL_PERIOD)
                {
                    exit_scroll();
                    break;
                }       

                showSEEK(su_menu_strip_animation[m_user_ui.menu_seek]);
                if (m_user_ui.key_event == SU_KEY_LONG_PRESS)
                {
                    m_audio_system.set_strip_mode(su_menu_strip_animation[m_user_ui.menu_seek].index);
                    exit_scroll();
                }

                else if (m_user_ui.key_event == SU_KEY_SHORT_PRESS)	/* Short press -> Move to the next element in the menu */
                {
                    SU_NEXT(m_user_ui.menu_seek, su_menu_strip_animation);	
                    deleteTASK(&m_audio_system.handle_active_strip_mode);
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
            m_user_ui.init();
        }


        /******************************************/
        /*         Battery charge level           */
        /*         display                        */
        /******************************************/
        if (m_hw_status.charging_enabled)
        {
            if ( m_user_ui.LCD_timer.value() >= 1000)
            {
                toggleOUTPUT(GLOBAL_CFG_PIN_BATTERY_LCD, GLOBAL_CFG_PORT_BATTERY_LCD);
                m_user_ui.LCD_timer.reset();
            }
	    }

        else if (m_user_ui.capacity_lcd_en && m_hw_status.powered_up)
        {

            if (m_user_ui.LCD_timer.value() < 5000)
            {
                writeOUTPUT(GLOBAL_CFG_PIN_BATTERY_LCD, GLOBAL_CFG_PORT_BATTERY_LCD, 0);
            }
            else if(m_user_ui.LCD_timer.value() <= 8000)
            {
                writeOUTPUT(GLOBAL_CFG_PIN_BATTERY_LCD, GLOBAL_CFG_PORT_BATTERY_LCD, 1);
            }
            else
            {
                m_user_ui.LCD_timer.reset();
            }
        }

        else
        {
            m_user_ui.capacity_lcd_en = 0;
            writeOUTPUT(GLOBAL_CFG_PIN_BATTERY_LCD, GLOBAL_CFG_PORT_BATTERY_LCD, 0);
        }


        delay_FreeRTOS_ms(30);
        //End loop
    }
}





/*********************************************************
 *                                                       *
 *                      FUNCTIONS                        *          
 *                                                       *
 *********************************************************/


/**********************************************************************
 *  FUNCTION:    showSEEK
 *  PARAMETERS:  SETTINGS_UI_MENU_LIST menu_element
 *  DESCRIPTION: Displays the currently selected element in the
 *               settings ui.
 **********************************************************************/
inline void showSEEK(SETTINGS_UI_MENU_LIST element)  // Prikaze element v seeku ce je SU_STATE_SCROLL aktiven
{		
	switch(element.state)
	{
	case SU_STATE_SCROLL:
		m_audio_system.color_shift(element.index, AUVS_CFG_FAST_ANIMATION_TIME_MS);
        brightUP(AUVS_CFG_SLOW_ANIMATION_TIME_MS);
	break;

	case SU_STATE_STRIP_SELECTION:
		if (element.index == SU_MENU_STRIP_ANIMATION_STRIP_OFF)
		{
			deleteTASK(&m_audio_system.handle_active_strip_mode);
			m_audio_system.color_shift(COLOR_RED, AUVS_CFG_FAST_ANIMATION_TIME_MS);
            brightUP(AUVS_CFG_SLOW_ANIMATION_TIME_MS);
		}
		else if (m_audio_system.handle_active_strip_mode == NULL)
		{
            m_audio_system.create_animation(element.index, COLOR_WHITE);
		}
	break;

	default:
	break;
	}
}

/**********************************************************************
 *  FUNCTION:    exit_scroll
 *  PARAMETERS:  void
 *  DESCRIPTION: exits scroll menu to unset menu                         
 **********************************************************************/
void exit_scroll()
{
    deleteTASK(&m_audio_system.handle_active_strip_mode);
	brightDOWN(AUVS_CFG_SLOW_ANIMATION_TIME_MS);
	delay_FreeRTOS_ms(500);
	m_audio_system.strip_on();
	m_user_ui.init();
}

/**********************************************************************
 *  FUNCTION:    enter_scroll
 *  PARAMETERS:  void
 *  DESCRIPTION: Enters the scroll after animation                       
 **********************************************************************/
void enter_scroll()
{
    m_audio_system.strip_off();
    /* Set into scroll */
    m_user_ui.state = SU_STATE_SCROLL;
    m_user_ui.menu_seek = 0;
    showSEEK(su_menu_scroll[m_user_ui.menu_seek]);
}