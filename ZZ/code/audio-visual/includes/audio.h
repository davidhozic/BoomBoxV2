
#ifndef AUDIO_H
#define AUDIO_H
#include "audio-visual/includes/barve.h"
#include "FreeRTOS.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "settings.h"
#include "EEPROM/EEPROM.h"
#include <string>
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

#define brightUP(cas_na_krog)				    		brightnessFADE(1, cas_na_krog);
#define brightDOWN(cas_na_krog)							brightnessFADE(-1, cas_na_krog);



/*********************************************/
/*			 ENUM,STRUCT DEFINICIJE          */
/*********************************************/

enum enum_STRIP_MODES
{
	NORMAL_FADE,
	INVERSE_NORMAL_FADE,
	COLOR_FADE,
	BREATHE_FADE,
	end_strip_modes,
	STRIP_OFF
};

enum enum_MIC_MODES
{
	AVERAGE_VOLUME,
	POTENCIOMETER,
	FREQUENCY,
	end_mic_modes
};
	
	
 struct struct_AUDIO_SYS
{
	unsigned short strip_mode = NORMAL_FADE;
	short current_color[3] = {0, 0, 0};
	short current_brightness = 0;
	unsigned short mic_mode = POTENCIOMETER;
	unsigned short average_volume = 2048;
	
	class_TIMER lucke_filter_timer;
	class_TIMER mic_ref_timer;
	class_TIMER average_v_timer;
	
	uint8_t barva_selekt = 0;
	bool mikrofon_detect = 0;
	uint16_t ref_glasnost = 2048;
	
	TaskHandle_t handle_average_volume = NULL;
	TaskHandle_t handle_active_strip_mode = NULL;
	TaskHandle_t handle_audio_system;
};

extern struct_AUDIO_SYS audio_system;


/*********************************************/
/*         Prototipi pomoznih funkcij        */
/*********************************************/
void updateSTRIP();
void stripOFF();
void colorSHIFT(uint8_t BARVA, uint8_t cas_krog);
void brightnessFADE(char smer, uint8_t cas_krog);
void flashSTRIP();
void set_stripCOLOR(unsigned char barva_index);
void mic_mode_CHANGE();
void strip_mode_CHANGE(std::string ukaz);
/*********************************************/

/*********************************************/
/*             Prototipi taskov              */
/*********************************************/
void normal_fade_task(void* BARVA);
void inverse_normal_fade_task(void* BARVA);
void color_fade_task(void* BARVA);
void breathe_fade_task(void* BARVA);
void avg_vol_task(void* BARVA);
/*********************************************/


#endif
