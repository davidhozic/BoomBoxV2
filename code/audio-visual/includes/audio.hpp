
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
