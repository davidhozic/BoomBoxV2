



============================================================================================
    ! ! ! AUTOMATICNO GENERIRANA DATOTEKA PREK SKRIPTE ! ! !    
    --------------------------------------------------------------
    -> STEVILO SKUPNIH VRSTIC (s presledki): 2215
    -> STEVILO DATOTEK VKLJUCENIH V SKUPNO DATOTEKO: 29
============================================================================================




/*###############################################################################################################################################
-> IME DATOTEKE: ./code/audio-visual/includes/audio.hpp 
-> JEZIK: C++
#################################################################################################################################################*/

#ifndef AUDIO_H
#define AUDIO_H


#include "common/inc/common.hpp"
#include "libs/castimer/castimer.hpp"
#include "linked_list.hpp"


/************************************************************************/
/*								CONFIG				                    */
/************************************************************************/
#define  AUVS_CONFIG_NORMAL_ANIMATION_TIME_MS			500		// Animation time for mic detection light up
#define  AUVS_CONFIG_FAST_ANIMATION_TIME_MS				250		// Animation time used in flash strip
#define	 AUVS_CFG_SLOW_ANIMATION_TIME_MS				1000	// Animation time for exit animations and settings ui showcase

#define  AUVS_CONFIG_BRIGHTNESS_CHANGE					5



/************************************************************************/
/*						AUDIO VISUAL SYSTEM MACROS	                    */
/************************************************************************/

#define brightUP(animation_time)				    			m_audio_system.brightness_fade(1,  animation_time)
#define brightDOWN(animation_time)								m_audio_system.brightness_fade(-1, animation_time)

#define COLOR_NEXT( ind, color_arr )							ind = color_arr[ind + 1].color_index >= COLOR_TERMINATOR ? 0 : ind + 1


/*********************************************/
/*             Prototipi taskov              */
/*********************************************/
void normal_fade_task(void *input);
void inverted_fade_task(void *input);
void breathe_fade_task(void *input);
void signal_measure(void* p);
/*********************************************/

/****************************************************/
/*			 ENUM,STRUCT, CLASS DEFINICIJE          */
/****************************************************/

enum enum_STRIP_MODES
{
	AUVS_AN_NORMAL_FADE = 0,
	AUVS_AN_INVERTED_FADE,
	AUVS_AN_BREATHE_FADE,
	AUVS_AN_STRIP_OFF,
};


enum enum_COLOR_SPACE_indexes
{
	STRIP_RED = 0,
	STRIP_GREEN,
	STRIP_BLUE
};


enum enum_BARVE
{
	COLOR_WHITE = 0,
	COLOR_ZELENA,
	COLOR_RED,
	COLOR_BLUE,
	COLOR_YELLOW,
	COLOR_LBLUE,
	COLOR_VIOLET,
	COLOR_PINK,
	COLOR_TERMINATOR
};


struct STRIP_COLOR_t
{
	uint8_t color_index;
	uint8_t color_data[3];
};



struct STRIP_ANIMATION_t
{
	uint8_t animation_index;
	void (*f_ptr)(void *);
};



class AUVS
{
public:
	
    /*   Function Prototypes   */
    void update_strip();
	void strip_off();
	void strip_on();
	void color_shift(uint8_t BARVA, unsigned short animation_time);
	void brightness_fade(char smer, unsigned short animation_time);
	void flash_strip();
	void set_strip_color(unsigned char barva_index);
	void set_strip_mode(uint8_t mode);
	void set_strip_brightness(uint8_t value);
    void CREATE_ANIMATION(uint8_t task_index, uint8_t color);
	
    
    /****************************************************************************/
	/****  Strip parameters   ****/
	uint8_t strip_mode = AUVS_AN_STRIP_OFF;							// Current strip mode
	uint16_t animation_time = AUVS_CONFIG_NORMAL_ANIMATION_TIME_MS;
	/**** Strip current state ****/
	int16_t current_color[3] = {255, 255, 255};					// Current RGB color of the strip
	int16_t current_brightness = 0;								// Current brightness level of the strip
	uint8_t curr_color_index = 0;									// Index of color that strip will turn on
	bool mikrofon_detect = 0;									// Is set to 1 if spike is detected and then strip is turned on

	/****		Timers		 ****/
	TIMER_t  lucke_filter_timer;								// Timer that prevents strip from triggering too fast after last trigger (filter timer)
	
	/****   Task handles	****/
	TaskHandle_t handle_audio_meass = NULL;
	TaskHandle_t handle_active_strip_mode = NULL;	
	/***	Strip mode functions ***/
	static STRIP_ANIMATION_t strip_animations[];
	/***	Strip colors	***/
	static STRIP_COLOR_t strip_colors[];
};



extern AUVS m_audio_system;


#endif

/*###############################################################################################################################################
-> IME DATOTEKE: ./code/audio-visual/audio_system_functions.cpp 
-> JEZIK: C++
#################################################################################################################################################*/
#include "common.hpp"
#include "libs/EEPROM/EEPROM.hpp"
#include "includes/audio.hpp"
#include "libs/outputs_inputs/outputs_inputs.hpp"

/**************************************************************************************************************************
*                                                                                                                         *
*                                                           Pomozne funkcije                                              *
*                                                                                                                         *
**************************************************************************************************************************/




/**********************************************************************
 *  FUNCTION:    CREATE_ANIMATION
 *  PARAMETERS:  uint8_t task_index, uint8_t color
 *  DESCRIPTION: Deletes previously active task, sets strip color and 
 *               creates a strip task                            
 **********************************************************************/
void AUVS::CREATE_ANIMATION(uint8_t task_index, uint8_t color)                
{
    set_strip_color(color);
    deleteTASK(&handle_active_strip_mode);
    xTaskCreate(AUVS::strip_animations[task_index].f_ptr, "str", 64, NULL, 4, &handle_active_strip_mode);
}


/**********************************************************************
 *  FUNCTION:    update_strip
 *  PARAMETERS:  void
 *  DESCRIPTION: updates strip with current color and brightness                         
 **********************************************************************/
void AUVS::update_strip()
{
	writePWM(GLOBAL_CFG_PIN_LED_STRIP_R, GLOBAL_CFG_PORT_LED_STRIP,		current_color[STRIP_RED]	*	 current_brightness / 255.00);
	writePWM(GLOBAL_CFG_PIN_LED_STRIP_G, GLOBAL_CFG_PORT_LED_STRIP, 	current_color[STRIP_GREEN]  *	 current_brightness / 255.00);
	writePWM(GLOBAL_CFG_PIN_LED_STRIP_G, GLOBAL_CFG_PORT_LED_STRIP,	    current_color[STRIP_BLUE]	*    current_brightness / 255.00);
}


/**********************************************************************
 *  FUNCTION:    flash_strip
 *  PARAMETERS:  void
 *  DESCRIPTION: flashes the strip (used in settings_ui in state
 *               transition)                         
 **********************************************************************/
void AUVS::flash_strip() //Utripanje (Izhod iz STATE_SCROLL stata / menjava mikrofona)
{
	set_strip_color(COLOR_WHITE);
	for (uint8_t i = 0; i < 5; i++)
	{
		set_strip_brightness(0);
        update_strip();
		delay_FreeRTOS_ms(125);
		set_strip_brightness(255);
        update_strip();
		delay_FreeRTOS_ms(125);
	}
}


/**********************************************************************
 *  FUNCTION:    color_shift
 *  PARAMETERS:  uint8_t color_index, uint16_t animation_time
 *  DESCRIPTION: fades strip from the current color to the wanted color                         
 **********************************************************************/
void AUVS::color_shift(uint8_t BARVA, unsigned short animation_time)
{
	char smer[3];
    do
	{
		strip_colors[BARVA].color_data[STRIP_RED] >= current_color[STRIP_RED]	  ? smer[STRIP_RED]	= 1 : smer[STRIP_RED]	  = -1;
		strip_colors[BARVA].color_data[STRIP_GREEN] >= current_color[STRIP_GREEN] ? smer[STRIP_GREEN] = 1 : smer[STRIP_GREEN] = -1;
		strip_colors[BARVA].color_data[STRIP_BLUE] >= current_color[STRIP_BLUE]  ? smer[STRIP_BLUE]  = 1 : smer[STRIP_BLUE]  = -1;

		current_color[STRIP_RED]	+=	(AUVS_CONFIG_BRIGHTNESS_CHANGE * smer[STRIP_RED]);
		current_color[STRIP_GREEN]	+=	(AUVS_CONFIG_BRIGHTNESS_CHANGE * smer[STRIP_GREEN]);
		current_color[STRIP_BLUE]	+=	(AUVS_CONFIG_BRIGHTNESS_CHANGE * smer[STRIP_BLUE]);

		/* Check over color for seperate color indexes */
		if (smer[STRIP_RED] == 1 && current_color[STRIP_RED] > strip_colors[BARVA].color_data[STRIP_RED] || smer[STRIP_RED] == -1 && current_color[STRIP_RED] < strip_colors[BARVA].color_data[STRIP_RED] )
		{
			current_color[STRIP_RED] = strip_colors[BARVA].color_data[STRIP_RED];
		}
	
		if (smer[STRIP_GREEN] == 1 && current_color[STRIP_GREEN] > strip_colors[BARVA].color_data[STRIP_GREEN] 	|| smer[STRIP_GREEN] == -1 && current_color[STRIP_GREEN] < strip_colors[BARVA].color_data[STRIP_GREEN] )
		{
			current_color[STRIP_GREEN] = strip_colors[BARVA].color_data[STRIP_GREEN];
		}

		if (smer[STRIP_BLUE] == 1 && current_color[STRIP_BLUE] > strip_colors[BARVA].color_data[STRIP_BLUE] || smer[STRIP_BLUE] == -1 && current_color[STRIP_BLUE] < strip_colors[BARVA].color_data[STRIP_BLUE] )
		{
			current_color[STRIP_BLUE] = strip_colors[BARVA].color_data[STRIP_BLUE];
		}


		update_strip();
		
		delay_FreeRTOS_ms(  (animation_time*AUVS_CONFIG_BRIGHTNESS_CHANGE) / 255  );

	}while ( current_color[STRIP_RED]	  != strip_colors[BARVA].color_data[STRIP_RED]   ||
			 current_color[STRIP_GREEN]   != strip_colors[BARVA].color_data[STRIP_GREEN] ||
		 	 current_color[STRIP_BLUE]	  != strip_colors[BARVA].color_data[STRIP_BLUE]	);	// While current colors different from wanted
}

/**********************************************************************
 *  FUNCTION:    brightness_fade
 *  PARAMETERS:  int8_t direction, uint16_t animation time
 *  DESCRIPTION: Fades strip brightness into direction to max/min value
 **********************************************************************/
void AUVS::brightness_fade(char smer, unsigned short animation_time)
{
	do
	{
		current_brightness += AUVS_CONFIG_BRIGHTNESS_CHANGE * smer;
		current_brightness = current_brightness < 0 ? 0 :    current_brightness;
		current_brightness = current_brightness > 255 ? 255 : current_brightness;
		update_strip();
		delay_FreeRTOS_ms(  (animation_time*AUVS_CONFIG_BRIGHTNESS_CHANGE)/255  );
	}while (smer > 0 ? current_brightness < 255 : current_brightness > 0);
}

/**********************************************************************
 *  FUNCTION:    strip_off
 *  PARAMETERS:  void
 *  DESCRIPTION: Sets strip mode to OFF, deletes auvs task and changes
 *               animation time to slow for use in settings ui                         
 **********************************************************************/
void AUVS::strip_off()
{
	strip_mode = AUVS_AN_STRIP_OFF;
	deleteTASK(&handle_audio_meass);
	deleteTASK(&handle_active_strip_mode);
	delay_FreeRTOS_ms(10);
	animation_time = AUVS_CFG_SLOW_ANIMATION_TIME_MS;
	brightDOWN(AUVS_CFG_SLOW_ANIMATION_TIME_MS);
}

/**********************************************************************
 *  FUNCTION:    strip_on
 *  PARAMETERS:  void
 *  DESCRIPTION: Creates meassuring tasks, reads strip mode from eeprom,
 *               changes the animation time back to normal                         
 **********************************************************************/
void AUVS::strip_on()
{
	xTaskCreate(signal_measure, "avg_vol", 64, NULL, 1, &handle_audio_meass);
	strip_mode = EEPROM.beri(GLOBAL_CFG_EEPROM_ADDR_STRIP_MODE);
	
	/* EEPROM address is empty */
	if (strip_mode == 0xFF)
	{
		strip_mode = 0;
	}   

	animation_time = AUVS_CONFIG_NORMAL_ANIMATION_TIME_MS;
	delay_FreeRTOS_ms(10);
}

/**********************************************************************
 *  FUNCTION:    set_strip_mode
 *  PARAMETERS:  uint8_t mode
 *  DESCRIPTION: Sets strip mode index to wanted mode and updates
 *               EEPROM                         
 **********************************************************************/
void AUVS::set_strip_mode(uint8_t mode)
{
	strip_mode = mode;
	EEPROM.pisi(strip_mode,GLOBAL_CFG_EEPROM_ADDR_STRIP_MODE);
}

/**********************************************************************
 *  FUNCTION:    set_strip_color
 *  PARAMETERS:  uint8_t color_index
 *  DESCRIPTION: Sets the color to wanted index      
 **********************************************************************/
void AUVS::set_strip_color(unsigned char barva_index)
{
		current_color[STRIP_RED]   = strip_colors[barva_index].color_data[STRIP_RED];
		current_color[STRIP_GREEN] = strip_colors[barva_index].color_data[STRIP_GREEN];
		current_color[STRIP_BLUE]  = strip_colors[barva_index].color_data[STRIP_BLUE];
        update_strip();
}



/**********************************************************************
 *  FUNCTION:    set_strip_brightness 
 *  PARAMETERS:  uint8_t brightness
 *  DESCRIPTION: Sets strip brightness to wanted brightness and updates
 *               strip       
 **********************************************************************/
void AUVS::set_strip_brightness(uint8_t value)
{
	current_brightness = value;
    update_strip();
}
/*###############################################################################################################################################
-> IME DATOTEKE: ./code/audio-visual/avs_colors_animations.cpp 
-> JEZIK: C++
#################################################################################################################################################*/
#include "audio.hpp"

STRIP_COLOR_t AUVS::strip_colors[] =
{
	{ COLOR_WHITE, 	    {255, 255, 255}  },
	{ COLOR_ZELENA,     {0, 255, 0}	     },
	{ COLOR_RED, 	    {255, 0, 0} 	 },
	{ COLOR_BLUE,	    {0,0, 255} 	     },
	{ COLOR_YELLOW ,    {255, 255, 0}    },
	{ COLOR_LBLUE,      {0, 255, 255}	 },
	{ COLOR_VIOLET,     {255, 0, 255} 	 },
	{ COLOR_PINK, 	    {255, 20, 100}	 },
    { COLOR_TERMINATOR, {0, 0, 0}        }
};


/* Needs to be sorted by enum */
STRIP_ANIMATION_t AUVS::strip_animations[] = 
{ 
	{ AUVS_AN_NORMAL_FADE,    normal_fade_task   },
 	{ AUVS_AN_INVERTED_FADE,  inverted_fade_task },
 	{ AUVS_AN_BREATHE_FADE,   breathe_fade_task  },
};
/*###############################################################################################################################################
-> IME DATOTEKE: ./code/audio-visual/avs_task_main.cpp 
-> JEZIK: C++
#################################################################################################################################################*/
#include <stdlib.h>
#include "libs/EEPROM/EEPROM.hpp"
#include "includes/audio.hpp"
#include "castimer/castimer.hpp"
#include "input.hpp"
#include "libs/outputs_inputs/outputs_inputs.hpp"

AUVS m_audio_system;

/**************************************************************************************************************************
*                                                                                                                         *
*                                                                                                                         *
*                                                       AUDIO                                                             *
*                                                       VIZUALNI                                                          *
*                                                       SISTEM                                                            *
*                                                                                                                         *
*                                                                                                                         *
**************************************************************************************************************************/
/**************************************************************************************************************************
*                                                                                                                         *
*                                            GLAVNI UPRAVLJALNI SISTEM (TASK)                                             *
*                                                                                                                         *
**************************************************************************************************************************/

void audio_visual_task(void *p) //Funkcija avdio-vizualnega sistema
{
	while (1)
	{	
		if (m_audio_system.strip_mode != AUVS_AN_STRIP_OFF)
		{	
			if (m_audio_system.lucke_filter_timer.value() >= 200 && m_audio_system.mikrofon_detect) /* mikrofon_detect gets triggered in the measurement task*/
			{
				m_audio_system.mikrofon_detect = 0;
				m_audio_system.lucke_filter_timer.reset();

				COLOR_NEXT(m_audio_system.curr_color_index, AUVS::strip_colors);
                
				m_audio_system.CREATE_ANIMATION(m_audio_system.strip_mode, m_audio_system.curr_color_index);
			}
		}
		delay_FreeRTOS_ms(100);
		//End task loop
	}
}



/**************************************************************************************************************************
*                                                                                                                         *
*                                                         FADE TASKI                                                      *
*                                                                                                                         *
**************************************************************************************************************************/
void normal_fade_task(void *input) //Prizig na barbi in pocasen izklop
{
	m_audio_system.set_strip_brightness(255);
	brightDOWN(m_audio_system.animation_time);
	m_audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}

void breathe_fade_task(void *input)
{
	brightUP(m_audio_system.animation_time/2);
	brightDOWN(m_audio_system.animation_time/2);
	m_audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}

void inverted_fade_task(void *input)
{
	m_audio_system.set_strip_brightness(0);
	brightUP(m_audio_system.animation_time);
	m_audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}

/*###############################################################################################################################################
-> IME DATOTEKE: ./code/audio-visual/meritve.cpp 
-> JEZIK: C++
#################################################################################################################################################*/


#include "libs/outputs_inputs/outputs_inputs.hpp"
#include "common/inc/common.hpp"
#include "EEPROM.hpp"
#include "includes/audio.hpp"
#include "castimer/castimer.hpp"
#include <math.h>

/************************************************************************/
	#define	MAX_READINGS	( 30 )
	#define LOG_PERIOD_MS	( 10 )
	#define	MIC_TRIGGER_PERCENT										((( 30.00/600 * m_microphone.average_volume + 6)/100.00))
/************************************************************************/

struct struct_microphone_t
{
	uint16_t  max_value = 0;
	uint32_t  readings_sum	 = 0;
	uint8_t	  readings_num : 7;
	bool	  value_logged : 1;
	uint16_t  current_value  = 0;
	uint16_t  previous_value = 0;
	
	uint32_t rise_time = 0;	

	uint16_t average_volume = 2048;
	uint16_t frequency = 2048;

	TIMER_t audio_meass_timer;		// Timer that delays logging of max measured volume voltage
}m_microphone;





/* Measures average volume */
void signal_measure(void* p)
{	
	while (1)
	{
		/************************************************************************/
		/*							 AVERAGE VOLUME		                        */
		/************************************************************************/
		m_microphone.current_value = readANALOG(GLOBAL_CFG_PIN_MICROPHONE);
		
		/* Volume spike detected -> Trigger the led strip*/
		if (m_microphone.current_value > (double)m_microphone.average_volume + (double)m_microphone.average_volume * MIC_TRIGGER_PERCENT)
			m_audio_system.mikrofon_detect = 1;  // Gets cleared after strip has light up
		
		/* Finds the signal amplitude */
		if (m_microphone.current_value > m_microphone.max_value)
			m_microphone.max_value = m_microphone.current_value;

		/* If period has elapsed, log the value*/
		if (m_microphone.audio_meass_timer.value() >= LOG_PERIOD_MS)
		{
			m_microphone.readings_sum += m_microphone.max_value;
			m_microphone.max_value = 0;
			m_microphone.readings_num++;
			m_microphone.audio_meass_timer.reset();
			
			/* Maximum number of readings has been reached, calculate average */
			if (m_microphone.readings_num >= MAX_READINGS)
			{
				m_microphone.average_volume  = m_microphone.readings_sum / m_microphone.readings_num;
				m_microphone.readings_sum  = 0;
				m_microphone.readings_num  = 0;
				m_microphone.max_value	 = 0;
			}
			
		}

		delay_FreeRTOS_ms(3);
		//END LOOP
	}
}



/*###############################################################################################################################################
-> IME DATOTEKE: ./code/common/inc/comment_templates.h 
-> JEZIK: C
#################################################################################################################################################*/

/*  Function description template  */ 

/**********************************************************************
 *  FUNCTION:    
 *  PARAMETERS: 
 *  DESCRIPTION:                          
 **********************************************************************/
/*###############################################################################################################################################
-> IME DATOTEKE: ./code/common/inc/common.hpp 
-> JEZIK: C++
#################################################################################################################################################*/

#ifndef GLOBAL_H
#define GLOBAL_H
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "input.hpp"

struct HW_STATUS_t
{
    bool charging_enabled   : 1;
    bool charging_finished  : 1;
    bool external_power     : 1;
    bool powered_up         : 1;
}extern m_hw_status;


/************************************************************************/
/*					  Safe FreeRTOS functions/macros		            */
/************************************************************************/
#ifndef DEBUG
	#define delay_FreeRTOS_ms(x)	vTaskDelay(x/portTICK_PERIOD_MS)
#else
	#define delay_FreeRTOS_ms(x)	vTaskDelay(1)
#endif

void holdTASK	(TaskHandle_t* task);
void resumeTASK	(TaskHandle_t* task);
void deleteTASK	(TaskHandle_t* task);





#endif
/*###############################################################################################################################################
-> IME DATOTEKE: ./code/common/common.cpp 
-> JEZIK: C++
#################################################################################################################################################*/

#include "FreeRTOS.h"
#include "common.hpp"
#include "libs/input/input.hpp"

/************************************************************************/
/*						    COMMON STRUCTS                              */
/************************************************************************/

HW_STATUS_t m_hw_status = {0};


/************************************************************************/
/*						SAFE FREERTOS FUNCTIONS                         */
/************************************************************************/

void holdTASK(TaskHandle_t* task)
{
	if (eTaskGetState(*task) != eSuspended && *task != NULL)
	vTaskSuspend(*task);
}
void resumeTASK(TaskHandle_t* task)
{
	if (eTaskGetState(*task) == eSuspended && *task != NULL)
	vTaskResume(*task);
}

void deleteTASK(TaskHandle_t* task)
{
	if (*task != NULL)
	{
		vTaskDelete(*task);
		*task = NULL;
		delay_FreeRTOS_ms(4); // Ensures task gets deleted
	}
}


/*###############################################################################################################################################
-> IME DATOTEKE: ./code/Events/inc/events.hpp 
-> JEZIK: C++
#################################################################################################################################################*/



enum enum_system_event
{
	EV_POWER_UP,	
	EV_SHUTDOWN,
	EV_POWER_SWITCH_EXTERNAL,
	EV_POWER_SWITCH_INTERNAL,
	EV_INITIALIZATION
};

void system_event(enum_system_event event);

/*###############################################################################################################################################
-> IME DATOTEKE: ./code/Events/events.cpp 
-> JEZIK: C++
#################################################################################################################################################*/
#include "events.hpp"
#include "common.hpp"
#include "outputs_inputs.hpp"
#include "EEPROM.hpp"
#include "audio.hpp"

/************************************************************************/
/*								 PROTOS						            */
/************************************************************************/
void power_task(void *);
void zaslon_task(void *);
void audio_visual_task(void *);
void charging_task(void *);
void user_ui_task(void *);
void power_switch_ev(uint8_t mode);


void system_event(enum_system_event eventt){
	switch(eventt)
	{
		case EV_POWER_SWITCH_EXTERNAL:
		case EV_POWER_SWITCH_INTERNAL:
			power_switch_ev(eventt);
		break;
		
		case EV_POWER_UP:
			writeOUTPUT(GLOBAL_CFG_PIN_12V_LINE, GLOBAL_CFG_PORT_12V_LINE, 1);
			writeOUTPUT(GLOBAL_CFG_PIN_OUTPUT_MOSFET, GLOBAL_CFG_PORT_OUTPUT_MOSFET, 1);
			m_audio_system.strip_on();
			m_hw_status.powered_up = 1;
			delay_FreeRTOS_ms(10);
		break;
		
		case EV_SHUTDOWN:
			m_hw_status.powered_up = 0;
			m_audio_system.strip_off();
			writeOUTPUT(GLOBAL_CFG_PIN_12V_LINE, GLOBAL_CFG_PORT_12V_LINE, 0);
			writeOUTPUT(GLOBAL_CFG_PIN_OUTPUT_MOSFET, GLOBAL_CFG_PORT_OUTPUT_MOSFET , 0);

		break;
		
		case EV_INITIALIZATION:
			/************************************************************************/
			/*						SET DATA DIRECTION REGISTERS			        */
			/************************************************************************/
			DDRE = 1;
			DDRH = 1 << PH3 | 1 << PH4 | 1 << PH5;
			DDRB = 1 << PB4 | 1 << PB5 | 1 << PB6 | 1 << PB7;
			/************************************************************************/
			/*						SETUP TIMER3 FOR TIMER_t LIBRARY                  */
			/************************************************************************/
			TCCR3A  = 0;
			OCR3A = 249;												// Output compare match na 249 tickov (1ms)
			TIMSK3	= 1 << OCIE3A;										// Vklopi output compare match ISR
			TCCR3B	= 1 << WGM32 | 1 << CS31 | 1 << CS30;				// Clear timer on compare match	, /64 prescaler
			/************************************************************************/
			/*								SETUP ADC                               */
			/************************************************************************/
			ADMUX   =	(1 << REFS0);
			ADCSRA  =	(1 << ADPS0) | (1 << ADPS2);
			ADCSRB  =	 0;
			ADCSRA |=	(1 << ADEN);
			DIDR0   =	0xFF;
			ADMUX = 1;
			ADCSRA |=	(1 << ADSC);

			
			/************************************************************************/
			/*							   SETUP TASKS                              */
			/************************************************************************/
			xTaskCreate(power_task, "Power", 64, NULL, 1, NULL);
			xTaskCreate(user_ui_task, "User UI", 64, NULL, 3, NULL);
			xTaskCreate(audio_visual_task, "m_audio_system", 64, NULL, 2, NULL);
			/************************************************************************/
			/*								OTHER                                   */
			/************************************************************************/
			m_hw_status.charging_finished = EEPROM.beri(GLOBAL_CFG_EEPROM_ADDR_BATTERY_CHARGING_STATUS);
			/************************************************************************/
			/*								START TASKS                             */
			/************************************************************************/
			vTaskStartScheduler();
		break;
	}
}

void power_switch_ev(uint8_t mode)
{
	switch(mode)
	{
		case EV_POWER_SWITCH_EXTERNAL:
			system_event(EV_SHUTDOWN);
			delay_FreeRTOS_ms(3);
			writeOUTPUT(GLOBAL_CFG_PIN_MENJALNIK,GLOBAL_CFG_PORT_MENJALNIK,1);
			m_hw_status.external_power = 1;
		break;
		
		case EV_POWER_SWITCH_INTERNAL:
			system_event(EV_SHUTDOWN);
			delay_FreeRTOS_ms(3);
			writeOUTPUT(GLOBAL_CFG_PIN_MENJALNIK,GLOBAL_CFG_PORT_MENJALNIK, 0);
			m_hw_status.external_power = 0;
		break;
	}
}




/*###############################################################################################################################################
-> IME DATOTEKE: ./code/libs/castimer/castimer.cpp 
-> JEZIK: C++
#################################################################################################################################################*/



#include "castimer.hpp"
#include <math.h>
#if (SOURCE_INTERUPT == 1)
	#include "util/atomic.h"
#endif

/************************************************************************/
/*							ERRORS AND WARNINGS                         */
/************************************************************************/
#if (	SOURCE_INTERUPT == 1	)
#pragma message "Timer library will use one of the hardware timers' interrupt as a source. Hardware timer must be configured manually to trigger interrupt every TIMER_INCREMENT_VALUE_MS"
#ifndef TIMER_ISR_VECTOR
#error TIMER_ISR_VECTOR "is not defined!"
#endif

#elif ( SOURCE_SYSTEM_TIME == 1 )
#ifndef SYSTEM_TIME_FUNCTION
#error "SYSTEM_TIME_FUNCTION is not defined!"
#endif
#pragma message "Timer library will use system time as a source."
#endif
/************************************************************************/

/* Initialization of timer list */
#if (SOURCE_INTERUPT == 1)
	LIST_t <TIMER_t *> TIMER_t::timer_list;
#endif

uint32_t TIMER_t::value()
{
#if (SOURCE_INTERUPT == 1)

	if (!initialized)	/* Must initialize post FreeRTOS start due to memory issues */
	{
		init();
	}

	timer_enabled = true;				
	uint32_t temp_timer_value;
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		temp_timer_value = timer_value;		
	}
		
	return temp_timer_value; 

#elif (SOURCE_SYSTEM_TIME == 1)					// Use System time as source for the timer

	if (timer_enabled == false)
	{
		timer_enabled = true;
		timer_value = SYSTEM_TIME_FUNCTION;	 // Start value
		return 0;
	}

	return	SYSTEM_TIME_FUNCTION - timer_value;		
#endif
}

void TIMER_t::reset()
{
#if (SOURCE_INTERUPT == 1)
	this->timer_value = 0;
#endif
	this->timer_enabled = false;
}

#if (SOURCE_INTERUPT == 1)

void TIMER_t::init()
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		TIMER_t::timer_list += this;
		initialized = true;
	}
}

#endif


#if (SOURCE_INTERUPT == 1)

	void TIMER_t::increment()
	{
		if (timer_enabled)
		{
			timer_value += TIMER_INCREMENT_VALUE_MS;
			if (function_ptr != NULL && timer_value >= function_call_period)
			{
				timer_value = 0;
				function_ptr(function_parameter);
			}
		}
	}

	ISR(TIMER_ISR_VECTOR)
	{
		for (uint16_t ind = 0, len = TIMER_t::timer_list.length() ; ind < len ; ind++)
		{
			TIMER_t::timer_list[ind]->increment();
		}
	}
	
	void TIMER_t::set_hook(void (*function_ptr)(void*), uint32_t call_period ,void* function_param_ptr)
	{
		this->function_ptr = function_ptr;
		this->function_call_period = call_period;
		this->function_parameter = function_param_ptr;
		this->timer_enabled = 1;
	}
	
#endif
/*###############################################################################################################################################
-> IME DATOTEKE: ./code/libs/castimer/castimer.hpp 
-> JEZIK: C++
#################################################################################################################################################*/

#ifndef CASTIMER_H
#define CASTIMER_H
#include "timer_settings.hpp"

#if (	SOURCE_INTERUPT == 1	)	
	#ifndef SEZNAM_INC
	#include "linked_list.hpp"
	#endif	
	#include <avr/interrupt.h>
#endif

class TIMER_t
{
private:
	bool timer_enabled = false;
	uint32_t timer_value = 0;

	/* Hook Function */
	void (*function_ptr)(void*) = NULL;
	uint32_t function_call_period = 0;
	void *function_parameter = NULL;

	#if (SOURCE_INTERUPT)
		bool initialized = false;
	#endif

public:
    uint32_t value();
    void reset();
	void increment();
	#if (SOURCE_INTERUPT == 1)
		void set_hook(void (*function_ptr)(void*), uint32_t call_period, void* function_param_ptr);
		void init();
		static LIST_t <TIMER_t *> timer_list;
	#endif
};



#endif







/*###############################################################################################################################################
-> IME DATOTEKE: ./code/libs/castimer/timer_settings.hpp 
-> JEZIK: C++
#################################################################################################################################################*/
/************************************************************************************************************/

/**											USAGE INSTRUCTIONS 											   **/
/************************************************************************************************************/
/* NOTE! The function hook functionality is only available in interrupt mode */
/*

CHANGING THE TIMER MODE:
    To change the timer to INTERRUPT MULTIPLEXING mode (calls functions every period), simply call the
    - timer_name.set_hook(function name, calling period, parameter pointer);
    and change the parameters. 
    *NOTE*! This mode is only available if the a hardware timer interrupt is selected as a source (SOURCE_INTERRUPT) to guarantee
    the correct period.

    To change the timer back to NORMAL mode, simply call the function
    - timer_name.set_hook(function name, calling period, parameter pointer);
    where first and last parameter are NULL, and the second 0.

STARTING THE TIMER:
    - To start the timer, call timer.value() function.
GETTING THE TIMER VALUE:
    - To get the timer value, call the timer.value() function
RESETTING THE ELAPSED TIME:
    - To reset the elapsed time call the timer.reset() function */																		
/************************************************************************************************************/

/*********************************************	SETTINGS	*************************************************/

#define SOURCE_INTERUPT								( 1 )			// Use ISR as timing source
	#define TIMER_ISR_VECTOR						( TIMER3_COMPA_vect )
    #ifndef DEBUG	
	    #define TIMER_INCREMENT_VALUE_MS				( 1 )
    #else
        #define TIMER_INCREMENT_VALUE_MS            ( 500 )
    #endif
#define SOURCE_SYSTEM_TIME							( 0 )			// Use System time as source
#if (SOURCE_SYSTEM_TIME == 1)
	#include "path_to_system_time_function_prototype"	
	#define SYSTEM_TIME_FUNCTION				   	(  some_function_call()	 )
#endif

/* 
 If SOURCE_INTERRUPT is enabled, the timers will be added to a linked list and get incremented thru an
 Interrupt Service Rouitine (this is just the source for the ticks, not connected with the timer mode).
 To use this settings you need to manually start a hardware timer and write it's vector
 in the TIMER_ISR_VECTOR macro, the timer period does not have to be 1ms, it can be anything, but make sure you update the TIMER_INCREMENT_VALUE_MS
 
 If SOURCE_SYSTEM_TIME is enabled and SOURCE_INTERRUPT disabled, then the timer will use the difference between the system time and the timestamp from last reset */
/*###############################################################################################################################################
-> IME DATOTEKE: ./code/libs/EEPROM/eeprom.cpp 
-> JEZIK: C++
#################################################################################################################################################*/

#include "avr/io.h"
#include "FreeRTOS.h"
#include "avr/interrupt.h"
#include "libs/EEPROM/EEPROM.hpp"
#include "common.hpp"

void EEPROM_t::pisi(uint8_t podatek, uint16_t naslov){
	if (EEPROM_t::beri(naslov) == podatek)
		return;
	while(EECR & (1<<EEPE));			// Cakaj da se prejsnje branje/pisanje zakljuci
	EEAR = naslov;					//Izberi index bajta na eepromu
	EEDR = podatek;					//Podatek na zacasen bajt
	EECR |= (1<<EEMPE);				//Vklopi interrupt
	EECR |= (1<<EEPE);				//Pisi
}

uint8_t EEPROM_t::beri (uint16_t naslov){
	while(EECR & (1<<EEPE));							// Cakaj da se prejsnje branje/pisanje zakljuci
	EEAR = naslov;										//Izberi index bajta na eepromu
	EECR |= (1<<EERE);									//Beri
	while(EECR & 1<<EERE);		
	return EEDR;										//Vrni vrednost
}
EEPROM_t EEPROM;




/*###############################################################################################################################################
-> IME DATOTEKE: ./code/libs/EEPROM/EEPROM.hpp 
-> JEZIK: C++
#################################################################################################################################################*/

#ifndef EEPROM_H

#define EEPROM_H

class EEPROM_t
{
public:
	void pisi(uint8_t podatek, uint16_t naslov);
	uint8_t beri (uint16_t naslov);
};

extern EEPROM_t EEPROM;

#endif
/*###############################################################################################################################################
-> IME DATOTEKE: ./code/libs/input/input.cpp 
-> JEZIK: C++
#################################################################################################################################################*/
#include "input.hpp"


bool INPUT_t::value()
{
	/* Switch is used for compatibility with ALL boards */
	switch (port)
	{
	#ifdef PINA
		case 'A':
			register_value = readBIT(PINA, pin);
		break;
	#endif

	#ifdef PINB
		case 'B':
			register_value = readBIT(PINB, pin);
		break;
	#endif

	#ifdef PINC
		case 'C':
			register_value = readBIT(PINC, pin);
		break;
	#endif

	#ifdef PIND
		case 'D':
			register_value = readBIT(PIND, pin);
		break;
	#endif

	#ifdef PINE
		case 'E':
			register_value = readBIT(PINE, pin);
		break;
	#endif

	#ifdef PINF
		case 'F':
			register_value = readBIT(PINF, pin);
		break;
	#endif

	#ifdef PING
		case 'G':
			register_value = readBIT(PING, pin);
		break;
	#endif

	#ifdef PINH
		case 'H':
			register_value = readBIT(PINH, pin);
		break;
	#endif

	#ifdef PINI
		case 'I':
			register_value = readBIT(PINI, pin);
		break;
	#endif

	#ifdef PINJ
		case 'J':
			register_value = readBIT(PINJ, pin);
		break;
	#endif


	#ifdef PINK
		case 'K':
			register_value = readBIT(PINK, pin);
		break;
	#endif

	#ifdef PINL
		case 'L':
			register_value = readBIT(PINL, pin);
		break;
	#endif

	#ifdef PINM
		case 'M':
			register_value = readBIT(PINM, pin);
		break;
	#endif

		default:
		break;
	}

	// END OF REGISTER READING
	/************************/
	
	/*		Invert output if unpressed input state is 1		*/
	if (default_state)
	{
		register_value = !register_value;
	}	
	
#if (USE_FILTERING == 1)
	/* Filter input with a timer */
	if (register_value != filtered_curr_state && filter_timer.value() >= FILTER_TIME_MS)
	{
		filtered_curr_state = register_value;
	}
	else if(register_value == filtered_curr_state)
	{
		filter_timer.reset();
	}
#else
	filtered_curr_state = register_value;
#endif
	/* END OF FILTERING*/

	
	/* Edge detection */
	if (prev_state != filtered_curr_state)
	{
		/* If state has changed and input is high -> rising edge*/
		if (filtered_curr_state)
		{
			rising_edge_var = 1;
		}
		else
		{
			falling_edge_var = 1;
		}	
		prev_state = filtered_curr_state;
	}
	
	/* Clear edges if input state doesn't match */
	if (!filtered_curr_state)
	{
		rising_edge_var = 0;
	}
	else if(filtered_curr_state){
		falling_edge_var = 0;
	}
	

	return filtered_curr_state;
}

#if (INCLUDE_risen_edge == 1)
bool INPUT_t::risen_edge()
{
	value();
	if (rising_edge_var)
	{
		rising_edge_var = 0;			
		return true;
	}
	return false;
}
#endif

#if (INCLUDE_fallen_edge == 1)
bool INPUT_t::fallen_edge()
{
	value();
	if (falling_edge_var)
	{
		falling_edge_var = 0;
		return true;
	}
	return false;
}
#endif

INPUT_t::INPUT_t(unsigned char pin, char port, char default_state)
{
	this->port = port;
	this->pin = pin;
	this->default_state	  = default_state;
	this->filtered_curr_state   = 0;
	this->register_value = 0;
	this->rising_edge_var  = 0;
	this->falling_edge_var = 0;
}




/************************************************************************/
/*							WARNINGS/ERRORS                             */
/************************************************************************/
#if (USE_FILTERING == 1)
	#warning "LITL: USE_FILTERING is enabled, LITL's timer will be used"
#endif
/*###############################################################################################################################################
-> IME DATOTEKE: ./code/libs/input/input.hpp 
-> JEZIK: C++
#################################################################################################################################################*/

#ifndef VHOD_H
#define	VHOD_H
#include "input_settings.hpp"
#include <stdint.h>
#include "avr/io.h"

#if (USE_FILTERING)
	#include "castimer.hpp"
#endif

class INPUT_t // pin, port, stanje ko ni pritisnjen
{
private:
/* Bitfield variables */
	uint8_t filtered_curr_state		:	1;
	uint8_t register_value			:	1;	
	uint8_t prev_state				:	1;
	uint8_t rising_edge_var			:	1;
	uint8_t falling_edge_var		:	1;
	uint8_t default_state			:	1;
/* Setting variables */
	uint8_t pin;
    uint8_t port;

#if (USE_FILTERING == 1)
	TIMER_t filter_timer;
#endif

public:
    bool value();
    bool risen_edge();
    bool fallen_edge();
	INPUT_t(uint8_t pin, char port, char default_state);
};

/* Easier readings in the value function */
#define readBIT(reg, bit)				(						(	(reg >> bit) & 0x1	)								)
#define writeBIT(reg, bit, val)			(		(reg = val ? ( reg | (0x1 << bit) ) : ( reg  &  ~(0x1 << bit)))			)


#endif



/*###############################################################################################################################################
-> IME DATOTEKE: ./code/libs/input/input_settings.hpp 
-> JEZIK: C++
#################################################################################################################################################*/
/************************************************************/
					/*		 SETTINGS		*/

/* Timer will wait for FILTER_TIME_MS befor updating the input state after the state has changed (if the state goes back to unchanged, the timer auto resets) */
#define USE_FILTERING									1    
#define FILTER_TIME_MS									1
#define USE_FREERTOS									1

/* Included object methods */
#define INCLUDE_risen_edge                              1
#define INCLUDE_fallen_edge                             1
/************************************************************/
/*###############################################################################################################################################
-> IME DATOTEKE: ./code/libs/linked_list/linked_list.hpp 
-> JEZIK: C++
#################################################################################################################################################*/
#ifndef SEZNAM_INC
#define SEZNAM_INC
#include <stdlib.h>
#include <stdint.h>
#include "list_settings.hpp"

#if (USE_FREERTOS == 1)
	#include "FreeRTOS.h"
#endif

#if (INCLUDE_IOSTREAM == 1)
    #include <iostream>
#endif




template <typename tip>
class LIST_t
{
private:

    struct vpdt
    {
        friend class LIST_t<tip>;
    private:
        vpdt *naslednji;
        vpdt *prejsnji;
        tip podatek;
        
    };

    vpdt *glava = NULL;
    unsigned long count = 0;
    unsigned long glava_index = 0;

    void pojdi_zacetek();
    void pojdi_konec();
    void head_to_index(uint32_t index);
    
public:

    unsigned short length();
	void add_front(tip vrednost);
    void add_end(tip vrednost);
    tip pop_end();

#if (INCLUDE_SORT == 1)
    void sort(tip(*comparator_fnct)(tip , tip));
#endif


#if (INCLUDE_IOSTREAM == 1)
    void print_console();
#endif

    void remove_by_index(uint32_t index);
    void splice(uint32_t index, uint32_t num_to_remove);


    /*************************************************/
#if (USE_OPERATORS == 1)

    tip &operator[](unsigned long index);
   
    LIST_t<tip> operator+(tip pod);
    
    void operator+=(tip pod);

#endif

};
    
#if (USE_OPERATORS == 1)
template <typename tip, class cl>
LIST_t<tip> operator+(tip pod, cl obj);
#endif




#include "llist_funct.hpp"

#endif


/*###############################################################################################################################################
-> IME DATOTEKE: ./code/libs/linked_list/list_settings.hpp 
-> JEZIK: C++
#################################################################################################################################################*/
/************************************************************************/
/*							  SETTINGS                                  */
/************************************************************************/
#define  USE_FREERTOS           1	        /* If you're using FreeRTOS */
#define  INCLUDE_IOSTREAM       0
#define  INCLUDE_SORT           0
#define  USE_OPERATORS          1
/************************************************************************/

/*###############################################################################################################################################
-> IME DATOTEKE: ./code/libs/linked_list/llist_funct.hpp 
-> JEZIK: C++
#################################################################################################################################################*/
#include "task.h"


template <typename tip>
void LIST_t<tip>::pojdi_zacetek()
{
    while (glava != NULL && glava->prejsnji != NULL)
    {
        glava = glava->prejsnji;
    }
    glava_index = 0;
}
template <typename tip>
void LIST_t<tip>::pojdi_konec()
{
    while (glava != NULL && glava->naslednji != NULL)
    {
        glava = glava->naslednji;
    }
    if (count > 0)
        glava_index = count - 1;
}
template <typename tip>
void LIST_t<tip>::head_to_index(uint32_t index)
{
    while (glava_index > index)
    {
        glava = glava->prejsnji;
        glava_index--;
    }

    while (glava_index < index)
    {
        glava = glava->naslednji;
        glava_index++;
    }
}
template <typename tip>
unsigned short LIST_t<tip>::length()
{
    return count;
}
template <typename tip>
void LIST_t<tip>::add_front(tip vrednost)
{
	vpdt *nov;

	#if USE_FREERTOS == 1
	    nov = (vpdt *)pvPortMalloc(sizeof(vpdt));
	#else
	    nov = (vpdt *)malloc(sizeof(vpdt));
	#endif
	
    pojdi_zacetek();

    if (glava != NULL)
    {
        glava->prejsnji = nov;
    }
    nov->prejsnji = NULL;
    nov->naslednji = glava;
    nov->podatek = vrednost;
    glava = nov;
    count++;
    glava_index = 0;
}
template <typename tip>
void LIST_t<tip>::add_end(tip vrednost)
{
	
	vpdt *nov;

	#if USE_FREERTOS == 1
	    nov = (vpdt *)pvPortMalloc(sizeof(vpdt));
	#else
	    nov = (vpdt *)malloc(sizeof(vpdt));
	#endif
		
	
    pojdi_konec();
    if (glava != NULL)
    {
        glava->naslednji = nov;
    }
    nov->prejsnji = glava;
    nov->naslednji = NULL;
    nov->podatek = vrednost;
    glava = nov;
    count++;
    glava_index = count - 1;

}
template <typename tip>
tip LIST_t<tip>::pop_end()
{
    tip return_data;
    pojdi_konec();
    return_data = glava->podatek;
    vpdt *prev = glava->prejsnji;
    if (prev != NULL)
        prev->naslednji = NULL;

	#if USE_FREERTOS == 1
		pvPortFree(glava);
	#else
	    free(glava);
	#endif

    glava = prev;
    count--;
    glava_index--;
    
    return return_data;
}

#if (INCLUDE_SORT == 1)
template <typename tip>
void LIST_t<tip>::sort(tip (*comparator_fnct)(tip, tip))
{
    tip temp;
    for (uint32_t i = 0; i < count - 1;)
    {
        if (comparator_fnct((*this)[i], (*this)[i + 1]) > 0)
        {
            temp = (*this)[i];
            (*this)[i] = (*this)[i + 1];
            (*this)[i + 1] = temp;
            if (i > 0)
                i--;
        }
        else
            i++;
    }
}
#endif

#if (INCLUDE_IOSTREAM == 1)
template <typename tip>
void LIST_t<tip>::print_console()
{
    pojdi_zacetek();
    for (uint32_t i = 0; i < count; i++)
    {
        using namespace std;
        cout << (*this)[i] << endl;
    }
}
#endif
template <typename tip>
void LIST_t<tip>::remove_by_index(uint32_t index)
{
    head_to_index(index);
    vpdt *new_head;

    if (glava->naslednji != NULL)
        glava->naslednji->prejsnji = glava->prejsnji;

    if (glava->prejsnji != NULL)
        glava->prejsnji->naslednji = glava->naslednji;

    if (glava->naslednji != NULL)
    {
        new_head = glava->naslednji;
    }
    else
    {
        new_head = glava->prejsnji;
        glava_index--;
    }


    free(glava);


    glava = new_head;
    count--;
}
template <typename tip>
void LIST_t<tip>::splice(uint32_t index, uint32_t num_to_remove)
{
    while (num_to_remove > 0)
    {
        remove_by_index(index);
        num_to_remove--;
    }
}




/*************************************************/
#if (USE_OPERATORS == 1)
template <typename tip>
tip &LIST_t<tip>::operator[](unsigned long index)
{
    
    head_to_index(index);
    
    return (glava->podatek);
}

template <typename tip>
LIST_t<tip> LIST_t<tip>::operator+(tip pod)
{
    this->add_end(pod);
    return (*this);
}

template <typename tip>
void LIST_t<tip>::operator+=(tip pod)
{
    this->add_end(pod);
}





#endif

/*###############################################################################################################################################
-> IME DATOTEKE: ./code/libs/outputs_inputs/outputs_inputs.cpp 
-> JEZIK: C++
#################################################################################################################################################*/

#include <avr/io.h>
#include "outputs_inputs.hpp"
#include "util/atomic.h"
#include "common.hpp"

void writeOUTPUT(unsigned char pin, char port, bool vrednost)
{
	pwmOFF(pin, port); // izklopi pwm
	if (port <= 'G')
		writeBIT( *((unsigned char*)&PORTA + 3*(port - 'A')), pin, vrednost);	
	else if (port == 'H')
		writeBIT(PORTH, pin, vrednost);
	else
		writeBIT( *((unsigned char*)&PORTJ + 3*(port - 'J')), pin, vrednost);	
}

void toggleOUTPUT(unsigned char pin, char port)
{
	pwmOFF(pin, port); // izklopi pwm
	if (port <= 'G')
		*(	(unsigned char*)&PORTA + 3* (port - 'A')  )	^= (1 << pin);	// Move forwared by 3 * (length from A to reg) and XOR 
	else if (port == 'H')
		PORTH ^= (1 << pin);
	else
		*(	(unsigned char*)&PORTJ + 3* (port - 'J')  )	^= (1 << pin);
}

void pwmOFF(uint8_t pin, char port)
{
	switch (port)
	{
		case 'B':
			switch (pin)
			{	
				/*************** PIN SWITCH  ******************/
				case 4:
					TCCR2A &= ~(1 << COM2A1); //izklopi komparator
					if ( (TCCR2A & (1 << COM2B1)) == 0  ){ //Ce je tudi primerjalnik B izklopljen se izklopi
						TCCR2B = 0;
						TCNT2 = 0;
					}
					break;
			
				case 5:
					TCCR1A &= ~(1 << COM1A1); //izklopi komparator
					if ( (TCCR1A & (1 << COM1B1)) == 0  ){ //Ce je tudi primerjalnik B izklopljen se izklopi
						TCCR1B = 0;
						TCNT1 = 0;
					}
					break;
				case 6:
					TCCR1A &= ~(1 << COM1B1); //izklopi komparator
					if ( (TCCR1A & (1 << COM1A1)) == 0  ){ //Ce je tudi primerjalnik B izklopljen se izklopi
						TCCR1B = 0;
						TCNT1 = 0;
					}
					break;
				/**********************************************/
			}
		break;

		default:
		break;
	}
}




unsigned short readANALOG(uint8_t channel)
{
	while (ADCSRA & (1 << ADSC)); //Dokler se bit ne resetira
	ADMUX = (1 << REFS0);
						
	if (channel < 8)	//	Stevilka ADC pina ustreza binarni obliki ki jo je treba pisati za ADC0:7. Pri ADC8:15 je potreba binarna
	{					//	oblika enaka kot da bi sli steti od 0 naprej z izjemo da je bit 5 vedno postavljen (po dokumentaciji)
		ADCSRB &= ~(1 << MUX5);
		ADMUX |= channel;
	}
	else
	{
		ADCSRB |= (1 << MUX5);
		ADMUX  |=  channel%8;
	}
	
	ADCSRA |= (1 << ADSC); //Start konverzija			
	while (ADCSRA & (1 << ADSC)); //Dokler se bit ne resetira
	ADMUX &= ~(channel);
	return ADC;
}

void writePWM(uint8_t pin, char port, uint8_t vrednost)
{

	switch (port)
	{
		case 'B':
			DDRB = DDRB | (1 << pin); //Nastavi na output
			switch (pin)
			{
				case 4:
					TCCR2A |= (1 << WGM21) | (1 << WGM20) | (1 << COM2A1);  //fast pwm, clear on compare match
					TCCR2B = (1 << CS20); //brez prescalrja
					OCR2A = vrednost;		//vrednost kjer se izhod ugasne
				break;
			
				case 5:
					TCCR1A |= (1 << COM1A1) | (1 << WGM10);
					TCCR1B = (1 << CS10) | (1 << WGM12);
					OCR1A = vrednost;
				break;
			
				case 6:
					TCCR1A |= (1 << COM1B1) | (1 << WGM10);
					TCCR1B = (1 << CS10) | (1 << WGM12);
					OCR1B = vrednost;
				break;
			}
		break;
	}
}

/*###############################################################################################################################################
-> IME DATOTEKE: ./code/libs/outputs_inputs/outputs_inputs.hpp 
-> JEZIK: C++
#################################################################################################################################################*/


void writeOUTPUT(unsigned char pin, char port, bool value); // writeOUTPUT(uint8_t pin, char port, bool value)
void toggleOUTPUT(unsigned char pin, char port); // toggleOUTPUT(uint8_t pin, char port)
void writePWM(unsigned char pin, char port, unsigned char vrednost);
void pwmOFF(unsigned char pin, char port);
unsigned short readANALOG(unsigned char channel);
/*###############################################################################################################################################
-> IME DATOTEKE: ./code/Power/inc/power.hpp 
-> JEZIK: C++
#################################################################################################################################################*/



#define BATTERY_VOLTAGE_PERCENT(voltage)		(100.00 * ((double)voltage-GLOBAL_CFG_MIN_BATTERY_VOLTAGE)/(GLOBAL_CFG_MAX_BATTERY_VOLTAGE-GLOBAL_CFG_MIN_BATTERY_VOLTAGE) )

/*###############################################################################################################################################
-> IME DATOTEKE: ./code/Power/Power.cpp 
-> JEZIK: C++
#################################################################################################################################################*/

#include "common.hpp"
#include "events.hpp"
#include "input.hpp"
#include "castimer.hpp"
#include "outputs_inputs.hpp"
#include "EEPROM.hpp"
#include "power.hpp"


/**    Structs    **/
struct POWER_t
{  
	/*	   	INPUT_t objects		*/
	INPUT_t switch_pwr = INPUT_t(GLOBAL_CFG_PIN_MAIN_POWER_SWITCH, GLOBAL_CFG_PORT_MAIN_POWER_SWITCH, 0);
    INPUT_t napajalnik = INPUT_t(GLOBAL_CFG_PIN_21V_PSU, GLOBAL_CFG_PORT_21V_PSU, 0);

	/*		TIMER_t objects		*/
	TIMER_t power_up_delay_timer;  // Turns on the speaker if all conditions met for at least 2 seconds
    
    /*      Other        */
    uint16_t battery_voltage;   // Battery voltage in milivolts
}m_power;



void power_task(void *p)
{
    for (;;)
    {
        m_power.battery_voltage = readANALOG(GLOBAL_CFG_PIN_VOLTAGE_DIV);

        /* No need to worry about the timer running because the first condition will always be false while speaker is enabled, meaning other elements won't execute in the if statement */
        if ( !m_hw_status.powered_up && (BATTERY_VOLTAGE_PERCENT(m_power.battery_voltage) > 8 || m_hw_status.external_power) && m_power.power_up_delay_timer.value() >= 2000 )
        { // Elapsed 2000 ms, not overheated, enough power or (already switched to)external power and not already powered up
            system_event(EV_POWER_UP);
            m_power.power_up_delay_timer.reset();
        }			
        
        
        if (!m_power.switch_pwr.value() || (BATTERY_VOLTAGE_PERCENT(m_power.battery_voltage) > 0 && BATTERY_VOLTAGE_PERCENT(m_power.battery_voltage) < 5 && !m_hw_status.external_power) )
        {
            m_power.power_up_delay_timer.reset();
            if (m_hw_status.powered_up)
            {
                system_event(EV_SHUTDOWN);
            }
        }
        

        if (m_power.napajalnik.value() && !m_hw_status.external_power)
        {
            system_event(EV_POWER_SWITCH_EXTERNAL);
        }

        else if (m_power.napajalnik.value() == 0 && m_hw_status.external_power)
        {
            system_event(EV_POWER_SWITCH_INTERNAL);
        }


        
        if (m_hw_status.charging_finished == 0 && BATTERY_VOLTAGE_PERCENT(m_power.battery_voltage) >= 100)
        {
            m_hw_status.charging_finished = 1;
            EEPROM.pisi(m_hw_status.charging_finished, GLOBAL_CFG_EEPROM_ADDR_BATTERY_CHARGING_STATUS); //Posodobitev EEPROM-a na bajtu 1 z spremenljivko readBIT(m_hw_status, m_Hardware_STATUS_REG_CHARGING_EN); Na vsake 5000 pisanj zamenja bajt na katerega piše
        }

        else if (m_hw_status.charging_finished && BATTERY_VOLTAGE_PERCENT(m_power.battery_voltage) <= 100 - GLOBAL_CFG_CHARGE_HYSTERESIS)
        {	/* Histereza */
            m_hw_status.charging_finished = 0;
            EEPROM.pisi(m_hw_status.charging_finished, GLOBAL_CFG_EEPROM_ADDR_BATTERY_CHARGING_STATUS);
        }


        if (m_hw_status.charging_enabled && (m_hw_status.charging_finished || m_power.napajalnik.value() == 0))
        {
            writeOUTPUT(GLOBAL_CFG_PIN_CHARGE, GLOBAL_CFG_PORT_CHARGE,0);
            m_hw_status.charging_enabled = 0;
        }

        else if (m_hw_status.charging_enabled == 0 && m_power.napajalnik.value() && !m_hw_status.charging_finished)
        {
            writeOUTPUT(GLOBAL_CFG_PIN_CHARGE, GLOBAL_CFG_PORT_CHARGE,1);
            m_hw_status.charging_enabled = 1;
        }
    } 
}
/*###############################################################################################################################################
-> IME DATOTEKE: ./code/User_UI/inc/user_ui.hpp 
-> JEZIK: C++
#################################################################################################################################################*/




/*******************************************************************************************/
/*									    SETTING 			                               */
/*******************************************************************************************/
#define SU_AUTO_EXIT_SCROLL_PERIOD			(15000)
#define SU_LONG_PRESS_PERIOD				(1000)
#define SU_SHORT_PRESS_PERIOD				(500)
/********************************************************************************************/
/*                                      STRUCTURES                                          */
/********************************************************************************************/
struct SETTINGS_UI_MENU_LIST
{
    uint8_t index;
    uint8_t state;
};

/********************************************************************************************/
/*                                         ENUMS                                            */
/********************************************************************************************/

enum SETTINGS_UI_KEY_EVENT
{
	SU_KEY_CLEAR = 0,
	SU_KEY_SHORT_PRESS,
	SU_KEY_LONG_PRESS,
	SU_KEY_IGNORE_KEY
};

/*********************************************************************************************/
/*											PROTOS											 */
/*********************************************************************************************/
void zaslon();
void settings_UI();
void exit_scroll();
void showSEEK(SETTINGS_UI_MENU_LIST element) ;
/************************/
/*         MACROS       */
/************************/

#define SU_MENU_SCROLL(index, menu_table)  index + 1 >= sizeof(menu_table)/sizeof(menu_table[0])  ?  index = 0 :  index++




/*###############################################################################################################################################
-> IME DATOTEKE: ./code/User_UI/user_ui.cpp 
-> JEZIK: C++
#################################################################################################################################################*/
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

/* Does not need to be sorted, but is more clean if it is */
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
		SW2 = INPUT_t(GLOBAL_CFG_PIN_SU_SWITCH, GLOBAL_CFG_PORT_SU_SWITCH, 0);
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
		/* Settigns (scroll) menu */
		settings_UI();
		zaslon();
		delay_FreeRTOS_ms(30);
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
			else if (m_user_ui.hold_time > 0)
			{
				if(m_user_ui.hold_time < SU_SHORT_PRESS_PERIOD)
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
					brightDOWN(AUVS_CFG_SLOW_ANIMATION_TIME_MS);
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
				m_audio_system.set_strip_mode(su_menu_strip_animation[m_user_ui.menu_seek].index);
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
		writeOUTPUT(GLOBAL_CFG_PIN_BATTERY_LCD, GLOBAL_CFG_PORT_BATTERY_LCD, 0);
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
            m_audio_system.CREATE_ANIMATION(element.index, COLOR_WHITE);
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
	m_audio_system.flash_strip();
	brightDOWN(AUVS_CFG_SLOW_ANIMATION_TIME_MS);
	delay_FreeRTOS_ms(1000);
	m_audio_system.strip_on();
	m_user_ui.init();
}
/*###############################################################################################################################################
-> IME DATOTEKE: ./code/bootloader.cpp 
-> JEZIK: C++
#################################################################################################################################################*/
#include "events.hpp"
#include "outputs_inputs.hpp"
#include "util/delay.h"
int main()
{

	system_event(EV_INITIALIZATION);

	return 0;
}

/*###############################################################################################################################################
-> IME DATOTEKE: ./code/settings.hpp 
-> JEZIK: C++
#################################################################################################################################################*/


#ifndef SETTINGS_H
#define SETTINGS_H



/************************************************************************/
/*								EEPROM IDs                              */
/************************************************************************/
    #define GLOBAL_CFG_EEPROM_ADDR_BATTERY_CHARGING_STATUS		0x0		  
    #define GLOBAL_CFG_EEPROM_ADDR_STRIP_MODE					0x1		  
/************************************************************************/


/************************************************************************/
/*					        VOLTAGE SETTINGS					        */
/************************************************************************/
	#define GLOBAL_CFG_ADC_REF_mV							        5000	  
	#define GLOBAL_CFG_MIN_BATTERY_VOLTAGE							3000	  
	#define GLOBAL_CFG_MAX_BATTERY_VOLTAGE							4150	  
	#define GLOBAL_CFG_CHARGE_HYSTERESIS 							5		// When charging finishes, the speaker needs to discharge for atleast this percent
/************************************************************************/

/************************************************************************/                             
/*							  ADC PIN MACROS               		        */
/************************************************************************/    
	#define GLOBAL_CFG_PIN_MICROPHONE				0	//ADC				  
    #define GLOBAL_CFG_PIN_VOLTAGE_DIV				2	//ADC				  
/************************************************************************/


/************************************************************************/
/*						  DIGITAL PINS & PORTS                          */
/************************************************************************/
	#define GLOBAL_CFG_PIN_BATTERY_LCD						0
	#define GLOBAL_CFG_PORT_BATTERY_LCD				       'E'

	#define GLOBAL_CFG_PIN_21V_PSU						    0
	#define GLOBAL_CFG_PORT_21V_PSU				           'K'
	
	#define	GLOBAL_CFG_PIN_MAIN_POWER_SWITCH			    1
	#define GLOBAL_CFG_PORT_MAIN_POWER_SWITCH		       'K'
		
	#define GLOBAL_CFG_PIN_OUTPUT_MOSFET					3			
	#define GLOBAL_CFG_PORT_OUTPUT_MOSFET			       'H'	

	#define GLOBAL_CFG_PIN_MENJALNIK				       	4	
	#define GLOBAL_CFG_PORT_MENJALNIK			           'H'	
	
	#define GLOBAL_CFG_PIN_CHARGE					        5			
	#define GLOBAL_CFG_PORT_CHARGE			        	   'H'	
	
	#define GLOBAL_CFG_PIN_SU_SWITCH				        5
	#define GLOBAL_CFG_PORT_SU_SWITCH				       'G'

	#define  GLOBAL_CFG_PIN_12V_LINE				        7			
	#define  GLOBAL_CFG_PORT_12V_LINE				       'B'
	
	#define GLOBAL_CFG_PIN_LED_STRIP_R				    	4 
	#define GLOBAL_CFG_PIN_LED_STRIP_G				    	5
	#define GLOBAL_CFG_PIN_LED_STRIP_B					    6
	#define GLOBAL_CFG_PORT_LED_STRIP					   'B'
/************************************************************************/
/*								OTHER                                   */
/************************************************************************/
	
#endif
