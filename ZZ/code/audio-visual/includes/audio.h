
#ifndef AUDIO_H
#define AUDIO_H
#ifndef GLOBAL_H
#include "common/inc/global.h"
#endif
#include "audio-visual/includes/barve.h"
#include "settings.h"
#include "libs/castimer/castimer.h"
#ifndef EEPROM_H
	#error	"EEPROM must be included first"
#endif


/************************************************************************/
/*						AUDIO VISUAL SYSTEM MACROS	                    */
/************************************************************************/

#define brightUP(animation_time)				    			m_audio_system.brightnessFADE(1,  animation_time)
#define brightDOWN(animation_time)								m_audio_system.brightnessFADE(-1, animation_time)

#define  mic_trigger_level_percent							( (((double) 30/600 * m_audio_system.average_volume + 6)/100.00)    )

/*********************************************/
/*			 ENUM,STRUCT DEFINICIJE          */
/*********************************************/

enum enum_STRIP_MODES
{
	NORMAL_FADE = 0,
	INVERTED_FADE,
	BREATHE_FADE,
	STRIP_OFF,
	end_strip_modes,
};

enum enum_COLOR_SPACE_indexes
{
	RED = 0,
	GREEN,
	BLUE
};


enum enum_STRIP_SPEED
{
	NORMAL,
	SLOW,
	FAST
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

	inline void save_strip_mode()
	{
		EEPROM.pisi(strip_mode,EEPROM_ADDRESS_STRIP_MODE);
	}

	/****************************************************************************/
	/****  Strip parameters   ****/
	uint8_t	 strip_mode = NORMAL_FADE;			// Current strip mode
	uint16_t animation_time = NORMAL_ANIMATION_TIME_MS;
	/**** Strip current state ****/
	int16_t current_color[3] = {0, 0, 0};	// Current RGB color of the strip
	int16_t current_brightness = 0;	// Current brightness level of the strip
	uint8_t barva_selekt = 0;				// Index of color that strip will turn on
	bool mikrofon_detect = 0;				// Is set to 1 if spike is detected and then strip is turned on

	/****		Timers		 ****/
	class_TIMER lucke_filter_timer;				// Timer that prevents strip from triggering too fast after last trigger (filter timer)
	
	/****	Measurements	****/
	uint16_t average_volume = 2048;			// Variable that stores the average volume
	
	/****   Task handles	****/
	TaskHandle_t handle_average_volume = NULL;
	TaskHandle_t handle_active_strip_mode = NULL;	
	/***	Strip mode functions ***/
	void (*array_strip_modes[enum_STRIP_MODES::end_strip_modes-1])(void*) = {normal_fade_task, inverted_fade_task, breathe_fade_task};
};
extern class_AUDIO_SYS m_audio_system;



#endif