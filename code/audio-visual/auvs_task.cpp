

#include "libs/outputs_inputs/outputs_inputs.hpp"
#include "common/inc/common.hpp"
#include "EEPROM.hpp"
#include "includes/audio.hpp"
#include "castimer/castimer.hpp"
#include <math.h>


AUVS m_audio_system;
/*******************************************************/
/*                                                     */
/*                                                     */
/*                AUDIO VISUAL TASK                    */
/*       Average volume meass & animation trigger      */
/*                                                     */
/*                                                     */
/*******************************************************/

void audio_visual_task(void* p)
{
	for (;;)
    {
        /* Read current value */
		m_audio_system.meass.current_value = readANALOG(GLOBAL_CFG_PIN_MICROPHONE);
		
		/*  Trigger LED LIGHT UP if strip is enabled, filtering has elapsed and we are above certain average %  */
		if (m_audio_system.strip.strip_mode != AUVS_AN_STRIP_OFF && 
            m_audio_system.meass.filter_timer.value() > AUVS_CFG_MEASS_FILTER_TIME_MS && 
            m_audio_system.meass.current_value > m_audio_system.meass.average_volume + m_audio_system.meass.average_volume * AUVS_CFG_MEASS_MIC_TRIGGER_PERCENT / 100)
        {
            m_audio_system.meass.filter_timer.reset();

            COLOR_NEXT(m_audio_system.strip.curr_color_index, AUVS::STRIP_t::strip_colors);
            m_audio_system.create_animation(m_audio_system.strip.strip_mode, m_audio_system.strip.curr_color_index);
        }

		/* Finds the signal amplitude */
		if (m_audio_system.meass.current_value > m_audio_system.meass.max_value)
			m_audio_system.meass.max_value = m_audio_system.meass.current_value;

		/* If period has elapsed, log the value*/
		if (m_audio_system.meass.val_log_timer.value() >= AUVS_CFG_MEASS_LOG_PERIOD_MS)
		{
			m_audio_system.meass.readings_sum += m_audio_system.meass.max_value;
			m_audio_system.meass.max_value = 0;
			m_audio_system.meass.readings_num++;
			m_audio_system.meass.val_log_timer.reset();
			
			/* Maximum number of readings has been reached, calculate average */
			if (m_audio_system.meass.readings_num >= AUVS_CFG_MEASS_MAX_READINGS)
			{
				m_audio_system.meass.average_volume  = m_audio_system.meass.readings_sum / m_audio_system.meass.readings_num;
				m_audio_system.meass.readings_sum  = 0;
				m_audio_system.meass.readings_num  = 0;
				m_audio_system.meass.max_value	 = 0;
			}
			
		}

		delay_FreeRTOS_ms(3);
		//END LOOP
	}
}


/*******************************************************/
/*                                                     */
/*              STRIP ANIMATION TASKS                  */
/*                                                     */
/*******************************************************/

void normal_fade_task(void *input) //Prizig na barbi in pocasen izklop
{
	m_audio_system.set_strip_brightness(255);
	brightDOWN(m_audio_system.strip.animation_time);
	m_audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}

void breathe_fade_task(void *input)
{
	brightUP(m_audio_system.strip.animation_time/2);
	brightDOWN(m_audio_system.strip.animation_time/2);
	m_audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}

void inverted_fade_task(void *input)
{
	m_audio_system.set_strip_brightness(0);
	brightUP(m_audio_system.strip.animation_time);
	m_audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}
