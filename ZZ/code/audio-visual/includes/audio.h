
#ifndef AUDIO_H
#define AUDIO_H
#include "audio-visual/includes/barve.h"
#include "FreeRTOS.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "settings.h"
#include "EEPROM/EEPROM.h"

/*********************************************/
/*         Prototipi pomoznih funkcij        */
/*********************************************/
void update_strip();
void color_fade_funct(uint8_t *BARVA);
void svetlost_mod_funct(char smer, uint8_t cas_krog);
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
/*	           Task related	                 */
/*********************************************/
extern TaskHandle_t normal_fade_handle;
extern TaskHandle_t color_fade_handle;
extern TaskHandle_t breathe_fade_handle;
extern TaskHandle_t average_volume_handle;
extern TaskHandle_t strip_mode_handle_arr [];
extern TaskHandle_t mic_mode_handle_arr[];

#define strip_mode_handle_arr_length	3
#define mic_mode_handle_arr_length 2

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

#define brightUP(cas_na_krog)				    		svetlost_mod_funct(1, cas_na_krog);
#define brightDOWN(cas_na_krog)							svetlost_mod_funct(-1, cas_na_krog);
#define colorSHIFT(index_barve)							color_fade_funct((uint8_t *)index_barve);

#define stripOFF()															\
    holdTASK(audio_system_handle);											\
	deleteTASK_REC(strip_mode_handle_arr, strip_mode_handle_arr_length);									\
    brightDOWN(15);
	
#define set_strip_color(x)													\
    STRIP_CURRENT_RED = mozne_barve.barvni_ptr[x][0];			\
    STRIP_CURRENT_GREEN = mozne_barve.barvni_ptr[x][1];		\
    STRIP_CURRENT_BLUE = mozne_barve.barvni_ptr[x][2];
		
#define strip_mode_change(ch, strip_list)									\
	if (strcmp(ch,"off"))													\
		STRIP_MODE = STRIP_OFF;												\
	else if (STRIP_MODE == STRIP_OFF)										\
		STRIP_MODE = NORMAL_FADE;											\
	else																	\
		STRIP_MODE = (STRIP_MODE + 1) % end_strip_modes;					\
	EEPROM.pisi(audiomode_eeprom_addr, STRIP_MODE);							\
	deleteTASK_REC(strip_list, strip_mode_handle_arr_length);
	
#define mic_mode_change(mic_mode_list)										\
	MIC_MODE = (MIC_MODE + 1) %	end_mic_modes;								\
	deleteTASK_REC(mic_mode_list, mic_mode_handle_arr_length);					\
	if (MIC_MODE == AVERAGE_VOLUME)											\
		xTaskCreate(avg_vol_task, "Average Volume Task", 128,NULL, 2, &average_volume_handle);		
				
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
	FREQUENCY,
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



#endif
