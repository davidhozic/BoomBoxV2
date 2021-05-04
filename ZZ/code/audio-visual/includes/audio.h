
#ifndef AUDIO_H
#define AUDIO_H
#ifndef GLOBAL_H
#include "common/inc/global.h"
#endif
#include "audio-visual/includes/barve.h"
#include "settings.h"

#include "libs/castimer/castimer.h"



/************************************************************************/
/*						AUDIO VISUAL SYSTEM MACROS	                    */
/************************************************************************/

#define brightUP(cas_na_krog)				    			audio_system.brightnessFADE(1, cas_na_krog)
#define brightDOWN(cas_na_krog)								audio_system.brightnessFADE(-1, cas_na_krog)



/*********************************************/
/*			 ENUM,STRUCT DEFINICIJE          */
/*********************************************/

enum enum_STRIP_MODES
{
	STRIP_OFF = 0,
	NORMAL_FADE = 1,
	BREATHE_FADE,
	end_strip_modes,
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
void breathe_fade_task(void *input);
void avg_vol_task(void* BARVA);
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
	void colorSHIFT(uint8_t BARVA, uint8_t cas_krog);
	void brightnessFADE(char smer, uint8_t cas_krog);
	void flashSTRIP();
	
	
	inline void select_strip_color(unsigned char barva_index)
	{
		for(uint8_t i = 0; i < 3; i++)
		current_color[i] = mozne_barve.barvni_ptr[barva_index][i];
	}

	/****************************************************************************/
	/***  Strip parameters   ***/
	int8_t strip_mode = STRIP_OFF;			// Current strip mode
	
	/*** Strip current state ***/
	int16_t current_color[3] = {0, 0, 0};	// Current RGB color of the strip
	int16_t current_brightness = 0;	// Current brightness level of the strip
	
	/***		Timers		 ***/
	class_TIMER lucke_filter_timer	= class_TIMER(Hardware.timer_list);				// Timer that prevents strip from triggering too fast after last trigger (filter timer)
	
	/****   Strip lightup	***/
	uint8_t barva_selekt = 0;				// Index of color that strip will turn on
	bool mikrofon_detect = 0;				// Is set to 1 if spike is detected and then strip is turned on
	float trigger_level_percent = 100;	// Variable that stores potentiometer setting of minimal spike trigger level
	uint16_t average_volume = 2048;			// Variable that stores the average volume
	/****   Task handles	***/
	TaskHandle_t handle_average_volume = NULL;
	TaskHandle_t handle_active_strip_mode = NULL;	
	/***	Strip mode functions ***/
	void (*array_strip_modes[enum_STRIP_MODES::end_strip_modes])(void*) = {normal_fade_task, breathe_fade_task};
};
extern class_AUDIO_SYS audio_system;



#endif