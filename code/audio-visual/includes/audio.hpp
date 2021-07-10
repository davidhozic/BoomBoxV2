
#ifndef AUDIO_H
#define AUDIO_H


#include "common/inc/common.hpp"
#include "libs/castimer/castimer.hpp"
#include "linked_list.hpp"


/************************************************************************/
/*						AUDIO VISUAL SYSTEM MACROS	                    */
/************************************************************************/

#define brightUP(animation_time)				    			m_audio_system.brightness_fade(1,  animation_time)
#define brightDOWN(animation_time)								m_audio_system.brightness_fade(-1, animation_time)

#define COLOR_NEXT( ind, color_arr )							ind = color_arr[ind + 1].color_index >= COLOR_TERMINATOR ? 0 : ind + 1


/*********************************************/
/*             Prototipi taskov              */
/*********************************************/
void normal_fade_task(void *);
void inverted_fade_task(void *);
void breathe_fade_task(void *);
void audio_visual_task (void *);
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
    COLOR_BLUE,
	COLOR_ZELENA,
	COLOR_YELLOW,
	COLOR_LBLUE,
	COLOR_VIOLET,
	COLOR_PINK,
    COLOR_RED,
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
    /***   Function Prototypes   ***/
    void update_strip();
	void strip_off();
	void strip_on();
    void color_shift(uint8_t BARVA, unsigned short animation_time);
	void brightness_fade(char smer, unsigned short animation_time);
	void set_strip_color(unsigned char barva_index);
	void set_strip_mode(uint8_t mode);
	void set_strip_brightness(uint8_t value);
    void create_animation(uint8_t task_index, uint8_t color);
    /****************************************************************************/

    struct STRIP_t
    {
        /****  Strip parameters   ****/
        uint8_t  strip_mode = AUVS_AN_STRIP_OFF;							// Current strip mode
        uint16_t animation_time = AUVS_CFG_NORMAL_ANIMATION_TIME_MS;
        /**** Strip current state ****/
        int16_t current_color[3] = {0, 0, 0};					// Current RGB color of the strip
        int16_t current_brightness = 0;								// Current brightness level of the strip
        uint8_t curr_color_index = 0;									// Index of color that strip will turn on
        /***	Strip mode functions ***/
        static STRIP_ANIMATION_t strip_animations[];
        /***	Strip colors	***/
        static STRIP_COLOR_t strip_colors[];
    }strip;

    /***    Meassurement    ***/
    struct MEASSUREMENT_t
    {
        uint16_t  max_value = 0;
        uint32_t  readings_sum	 = 0;
        uint8_t	  readings_num : 7;
        bool	  value_logged : 1;
        uint16_t  current_value  = 0;
        uint16_t  previous_value = 0;

        uint16_t average_volume = 2048;
        
        TIMER_t val_log_timer;		// Timer that delays logging of max measured volume voltage
        TIMER_t filter_timer;
    }meass;

    /****   Task handles	****/
	TaskHandle_t handle_audio_system = NULL;
	TaskHandle_t handle_active_strip_mode = NULL;	
};


extern AUVS m_audio_system;


#endif
