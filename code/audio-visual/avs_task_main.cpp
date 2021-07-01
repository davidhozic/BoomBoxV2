#include <stdlib.h>
#include "libs/EEPROM/EEPROM.hpp"
#include "includes/audio.hpp"
#include "castimer/castimer.hpp"
#include "input.hpp"
#include "libs/outputs_inputs/outputs_inputs.hpp"

AUVS m_audio_system;

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

void audio_visual_task(void *p) //Funkcija avdio-vizualnega sistema
{
	while (1)
	{	
		if (m_audio_system.strip_mode != AUVS_AN_STRIP_OFF)
		{	
			if (m_audio_system.lucke_filter_timer.value() >= 200 && m_audio_system.mikrofon_detect) /* mikrofon_detect gets triggered in the measurement task*/
			{
				m_audio_system.mikrofon_detect = 0;
				m_audio_system.lucke_filter_timer.reset();

				COLOR_NEXT(m_audio_system.curr_color_index, AUVS::strip_colors);
                
				m_audio_system.CREATE_ANIMATION(m_audio_system.strip_mode, m_audio_system.curr_color_index);
			}
		}
		delay_FreeRTOS_ms(100);
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
	m_audio_system.set_strip_brightness(255);
	brightDOWN(m_audio_system.animation_time);
	m_audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}

void breathe_fade_task(void *input)
{
	brightUP(m_audio_system.animation_time/2);
	brightDOWN(m_audio_system.animation_time/2);
	m_audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}

void inverted_fade_task(void *input)
{
	m_audio_system.set_strip_brightness(0);
	brightUP(m_audio_system.animation_time);
	m_audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}
