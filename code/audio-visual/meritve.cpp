

#include "libs/outputs_inputs/outputs_inputs.hpp"
#include "common/inc/common.hpp"
#include "EEPROM.hpp"
#include "includes/audio.hpp"
#include "castimer/castimer.hpp"
#include <math.h>



struct struct_microphone_t
{
	uint16_t  max_value = 0;
	uint32_t  readings_sum	 = 0;
	uint8_t	  readings_num : 7;
	bool	  value_logged : 1;
	uint16_t  current_value  = 0;
	uint16_t  previous_value = 0;

	uint16_t average_volume = 2048;
    
	TIMER_t val_log_timer;		// Timer that delays logging of max measured volume voltage
    TIMER_t filter_timer;
}m_microphone;



/* Measures average volume */
void signal_measure(void* p)
{	
	while (1)
	{
		/************************************************************************/
		/*							 AVERAGE VOLUME		                        */
		/************************************************************************/
		m_microphone.current_value = readANALOG(GLOBAL_CFG_PIN_MICROPHONE);
		
		/* Volume spike detected -> Trigger the led strip*/
		if (m_microphone.filter_timer.value() > AUVS_CFG_MEASS_FILTER_TIME_MS && m_microphone.current_value > m_microphone.average_volume + m_microphone.average_volume * AUVS_CFG_MEASS_MIC_TRIGGER_PERCENT)
        {
            m_audio_system.mikrofon_detect = 1;
            m_microphone.filter_timer.reset();
        }

		/* Finds the signal amplitude */
		if (m_microphone.current_value > m_microphone.max_value)
			m_microphone.max_value = m_microphone.current_value;

		/* If period has elapsed, log the value*/
		if (m_microphone.val_log_timer.value() >= AUVS_CFG_MEASS_LOG_PERIOD_MS)
		{
			m_microphone.readings_sum += m_microphone.max_value;
			m_microphone.max_value = 0;
			m_microphone.readings_num++;
			m_microphone.val_log_timer.reset();
			
			/* Maximum number of readings has been reached, calculate average */
			if (m_microphone.readings_num >= AUVS_CFG_MEASS_MAX_READINGS)
			{
				m_microphone.average_volume  = m_microphone.readings_sum / m_microphone.readings_num;
				m_microphone.readings_sum  = 0;
				m_microphone.readings_num  = 0;
				m_microphone.max_value	 = 0;
			}
			
		}

		delay_FreeRTOS_ms(3);
		//END LOOP
	}
}


