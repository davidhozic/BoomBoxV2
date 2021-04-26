
#ifndef AUDIO_H
#define AUDIO_H
#include "audio-visual/includes/barve.h"
#include "FreeRTOS.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "settings.h"
#include "EEPROM/EEPROM.h"
#include "libs/castimer/castimer.h"


/************************************************************************/
/*						AUDIO VISUAL SYSTEM MACROS	                    */
/************************************************************************/

	/********************* STRUCT ELEMENT MACROS **********************/

#define STRIP_CURRENT_RED								audio_system.current_color[0]  // Trenutna rdeca barva
#define STRIP_CURRENT_GREEN								audio_system.current_color[1]  // Trenutna zelena barva
#define STRIP_CURRENT_BLUE								audio_system.current_color[2]	 // Trenutna modra barva
#define STRIP_CURRENT_BRIGHT							audio_system.current_brightness
#define STRIP_CURRENT_COL								audio_system.current_color
#define STRIP_MODE										audio_system.strip_mode
#define MIC_MODE										audio_system.mic_mode

	/************************ FUNCTION MACROS *************************/

#define brightUP(cas_na_krog)				    			brightnessFADE(1, cas_na_krog);
#define brightDOWN(cas_na_krog)							brightnessFADE(-1, cas_na_krog);



/*********************************************/
/*			 ENUM,STRUCT DEFINICIJE          */
/*********************************************/

enum enum_STRIP_MODES
{
	NORMAL_FADE,
	BREATHE_FADE,
	end_strip_modes,
	STRIP_OFF
};

enum enum_MIC_MODES
{
	AVERAGE_VOLUME,
	POTENCIOMETER,
	end_mic_modes
};
	
/*********************************************/
/*         Prototipi pomoznih funkcij        */
/*********************************************/
void updateSTRIP();
void stripOFF();
void stripON();
void colorSHIFT(uint8_t BARVA, uint8_t cas_krog);
void brightnessFADE(char smer, uint8_t cas_krog);
void flashSTRIP();
void set_stripCOLOR(unsigned char barva_index);
/*********************************************/


/*********************************************/
/*             Prototipi taskov              */
/*********************************************/
void normal_fade_task(void *input);
void breathe_fade_task(void *input);
void avg_vol_task(void* BARVA);
/*********************************************/

	
 struct struct_AUDIO_SYS
{
	/***  Strip parameters   ***/
	uint16_t strip_mode;		// Current strip mode
	uint16_t mic_mode;		// Current spike trigger detection mode

	/*** Strip current state ***/
	int16_t current_color[3];	// Current RGB color of the strip
	int16_t current_brightness;			// Current brightness level of the strip
	
	/***		Timers		 ***/
	class_TIMER lucke_filter_timer;			// Timer that prevents strip from triggering too fast after last trigger (filter timer)
	class_TIMER mic_ref_timer;				// Timer that delays reading in potentiometer spike trigger mode
	class_TIMER average_v_timer;			// Timer that delays logging of max measured volume voltage 
	
	/****   Strip lightup	***/		
	uint8_t barva_selekt;				// Index of color that strip will turn on
	bool mikrofon_detect;				// Is set to 1 if spike is detected and then strip is turned on
	uint16_t ref_glasnost;			// Variable that stores potentiometer setting of minimal spike trigger level
	uint16_t average_volume;			// Variable that stores the average volume

	/****   Task handles	***/			
	TaskHandle_t handle_average_volume;	
	TaskHandle_t handle_active_strip_mode;
	TaskHandle_t handle_audio_system;
	
	/***	Strip mode functions ***/
	void (*array_strip_modes[2])(void*);	
};
extern struct_AUDIO_SYS audio_system;



#endif
