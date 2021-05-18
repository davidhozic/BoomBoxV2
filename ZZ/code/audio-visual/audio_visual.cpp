#include <stdlib.h>
#include "libs/EEPROM/EEPROM.h"
#include "includes/audio.h"
#include "castimer/castimer.h"
#include "VHOD/Vhod.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/FreeRTOS_def_decl.h"


/************************************************************************/
/*                            TASK HANDLES                              */
/************************************************************************/

// Microphone measuring tasks

/************************************************************************/
/*							AUDIO VISUAL STRUCTS                        */
/************************************************************************/
class_AUDIO_SYS m_audio_system;
struct_MOZNE_BARVE m_mozne_barve =
{
	.BELA	 =		{255, 255, 255},
	.ZELENA	 =		{0, 255, 0},
	.RDECA	 =		{255, 0, 0},
	.MODRA   =		{0, 0, 255},
	.RUMENA  =		{255, 255, 0},
	.SVETLO_MODRA =	{0, 255, 255},
	.VIJOLICNA =	{255, 0, 255},
	.ROZA =			{255, 20, 100},
	.barvni_ptr  = {m_mozne_barve.BELA, m_mozne_barve.ZELENA, m_mozne_barve.RDECA, m_mozne_barve.MODRA, m_mozne_barve.RUMENA, m_mozne_barve.SVETLO_MODRA, m_mozne_barve.VIJOLICNA, m_mozne_barve.ROZA}
};
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

void audio_visual(void *p) //Funkcija avdio-vizualnega sistema
{
	while (true)
	{
		
		if (m_Hardware.status_reg.powered_up && m_audio_system.strip_mode != STRIP_OFF)
		{	
			if (m_audio_system.lucke_filter_timer.vrednost() >= 200 && m_audio_system.mikrofon_detect)	 // STRIP task creation
			{
				m_audio_system.lucke_filter_timer.ponastavi();
				m_audio_system.barva_selekt = (m_audio_system.barva_selekt + 1) %  enum_BARVE::barve_end;	
				deleteTASK(&m_audio_system.handle_active_strip_mode);
				xTaskCreate(m_audio_system.array_strip_modes[m_audio_system.strip_mode], "strip mode", 128, &m_audio_system.barva_selekt, 4, &m_audio_system.handle_active_strip_mode);
			}
		}


		delayFREERTOS(1);
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
	m_audio_system.current_brightness = 255;
	m_audio_system.select_strip_color(*(uint8_t*)input);
	

	brightDOWN(m_audio_system.animation_time);

	m_audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}

void breathe_fade_task(void *input)
{
	m_audio_system.select_strip_color(*(uint8_t*)input);
	
	brightUP(m_audio_system.animation_time);
	brightDOWN(m_audio_system.animation_time);
	
	m_audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}

void inverted_fade_task(void *input)
{
	
	m_audio_system.current_brightness = 0;
	m_audio_system.select_strip_color(*(uint8_t*)input);
	
	brightUP(m_audio_system.animation_time);
	
	m_audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
	
	
}