
#ifndef AUDIO_H
#define AUDIO_H
#ifndef GLOBAL_H
#include "common/inc/global.hpp"
#endif
#include "audio-visual/includes/barve.hh"
#include "libs/castimer/castimer.hpp"
#ifndef EEPROM_H
	#error	"EEPROM must be included first"
#endif
#include "linked_list.hpp"

/************************************************************************/
/*						AUDIO VISUAL SYSTEM MACROS	                    */
/************************************************************************/

#define brightUP(animation_time)				    			m_audio_system.brightnessFADE(1,  animation_time)
#define brightDOWN(animation_time)								m_audio_system.brightnessFADE(-1, animation_time)


/*********************************************/
/*			 ENUM,STRUCT DEFINICIJE          */
/*********************************************/

enum enum_STRIP_MODES
{
	NORMAL_FADE = 0,
	INVERTED_FADE,
	BREATHE_FADE,
	STRIP_OFF,
};

enum enum_COLOR_SPACE_indexes
{
	RED = 0,
	GREEN,
	BLUE
};

/*********************************************/
/*             Prototipi taskov              */
/*********************************************/
void normal_fade_task(void *input);
void inverted_fade_task(void *input);
void breathe_fade_task(void *input);
void signal_measure(void* BARVA);
/*********************************************/

/************************************************************************/
/*							FUNCTION PROTOS                             */
/************************************************************************/

class class_AUDIO_SYS
{
public:
	void updateSTRIP();
	void stripOFF();
	void stripON();
	void colorSHIFT(uint8_t BARVA, unsigned short animation_time);
	void brightnessFADE(char smer, unsigned short animation_time);
	void flashSTRIP();
	
	
	inline void select_strip_color(unsigned char barva_index)
	{
		for(uint8_t i = 0; i < 3; i++)
		current_color[i] = m_mozne_barve.barvni_ptr[barva_index][i];
	}

	inline void set_strip_mode(uint8_t mode)
	{
		strip_mode = mode;
		EEPROM.pisi(strip_mode,EEPROM_ADDRESS_STRIP_MODE);
	}

	/****************************************************************************/
	/****  Strip parameters   ****/
	uint8_t strip_mode = NORMAL_FADE;						// Current strip mode
	uint16_t animation_time = NORMAL_ANIMATION_TIME_MS;
	/**** Strip current state ****/
	int16_t current_color[3] = {255, 255, 255};				// Current RGB color of the strip
	int16_t current_brightness = 0;							// Current brightness level of the strip
	uint8_t barva_selekt = 0;								// Index of color that strip will turn on
	bool mikrofon_detect = 0;								// Is set to 1 if spike is detected and then strip is turned on

	/****		Timers		 ****/
	TIMER_t  lucke_filter_timer;						// Timer that prevents strip from triggering too fast after last trigger (filter timer)
	
	/****	Measurements	****/
	uint16_t average_volume = 2048;			// Variable that stores the average volume
	
	/****   Task handles	****/
	TaskHandle_t handle_average_volume = NULL;
	TaskHandle_t handle_active_strip_mode = NULL;	
	/***	Strip mode functions ***/
	void (*list_strip_modes[3])(void*) =  {normal_fade_task, inverted_fade_task, breathe_fade_task};
};
extern class_AUDIO_SYS m_audio_system;



#endif
