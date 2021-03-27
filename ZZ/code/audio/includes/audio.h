
#ifndef AUDIO_H
#define AUDIO_H
#include "audio/includes/barve.h"
#include "FreeRTOS.h"
#include "task.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "settings.h"
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

	/************************ FUNCTION MACROS *************************/

#define brightUP(cas_na_krog)				    		svetlost_mod_funct(1, cas_na_krog);
#define brightDOWN(cas_na_krog)							svetlost_mod_funct(-1, cas_na_krog);
#define colorSHIFT(index_barve)							color_fade_funct((uint8_t *)index_barve);

#define stripOFF()															\
    holdTASK(audio_system_handle);											\
	deleteTASK_REC(strip_modes_ptr_arr);									\
    brightDOWN(15);
	
#define set_strip_color(x)													\
    STRIP_CURRENT_RED = mozne_barve.barvni_ptr[*((uint8_t *)x)][0];			\
    STRIP_CURRENT_GREEN = mozne_barve.barvni_ptr[*((uint8_t *)x)][1];		\
    STRIP_CURRENT_BLUE = mozne_barve.barvni_ptr[*((uint8_t *)x)][2];
	
#define deleteTASK_REC(handle_array)										\
	for (uint8_t index = 0; handle_array[index] != NULL; index++){			\
		deleteTASK(handle_array[index]);									\
	}		
				
/********************************************************************/


/*********************************************/
/*			 ENUM,STRUCT DEFINICIJE          */
/*********************************************/

enum enum_STRIP_MODES
{
	NORMAL_FADE,
	COLOR_FADE,
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
	
 struct struct_AUDIO_SYS
{
	unsigned short strip_mode = STRIP_OFF;
	short current_color[3] = {0, 0, 0};
	short current_brightness = 0;
	unsigned short mic_mode = POTENCIOMETER;
	unsigned short average_volume = 2048;
};


extern struct_AUDIO_SYS audio_system;


/*********************************************/
/*         Prototipi pomoznih funkcij        */
/*********************************************/
void update_strip();
void color_fade_funct(uint8_t *BARVA);
void svetlost_mod_funct(char smer, uint8_t cas_krog);
void strip_mode_chg(char *ch);
void flash_strip();
/*********************************************/

/*********************************************/
/*             Prototipi taskov              */
/*********************************************/
void normal_fade_task(void *B);
void color_fade_task(void *B);
void breathe_fade_task(void *B);
void avg_vol_task(void* param);
/*********************************************/

/*********************************************/
/*	           Task Handles	                 */
/*********************************************/
extern TaskHandle_t normal_fade_handle;
extern TaskHandle_t color_fade_handle;
extern TaskHandle_t breathe_fade_handle;
extern TaskHandle_t strip_modes_ptr_arr [];

#endif
