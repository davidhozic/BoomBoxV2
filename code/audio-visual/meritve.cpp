

#include "libs/outputs_inputs/outputs_inputs.hh"
#include "common/inc/global.hpp"
#include "EEPROM.hh"
#include "includes/audio.hh"
#include "castimer/castimer.hpp"
#include <math.h>

/************************************************************************/
	#define	MAX_READINGS	( 30 )
	#define LOG_PERIOD_MS	( 10 )
	#define	MIC_TRIGGER_PERCENT										((( 30.00/600 * m_microphone.average_volume + 6)/100.00))
/************************************************************************/



struct struct_microphone_t
{
	uint16_t  max_value = 0;
	uint32_t  readings_sum	 = 0;
	uint8_t	  readings_num : 7;
	bool	  value_logged : 1;
	uint16_t  current_value  = 0;
	uint16_t  previous_value = 0;
	
	uint32_t rise_time = 0;	

	uint16_t average_volume = 2048;
	uint16_t frequency = 2048;

	TIMER_t audio_meass_timer;		// Timer that delays logging of max measured volume voltage
}m_microphone;


/* Measures average volume */
void signal_measure(void* param)
{	
	while (1)
	{
		/************************************************************************/
		/*							 AVERAGE VOLUME		                        */
		/************************************************************************/
		m_microphone.current_value = readANALOG(mic_pin);
		
		/* Volume spike detected -> Trigger the led strip*/
		if (m_microphone.current_value > (double)m_microphone.average_volume + (double)m_microphone.average_volume * MIC_TRIGGER_PERCENT)
			m_audio_system.mikrofon_detect = 1;  // Gets cleared after strip has light up
		
		/* Finds the signal amplitude */
		if (m_microphone.current_value > m_microphone.max_value)
			m_microphone.max_value = m_microphone.current_value;

		/* If period has elapsed, log the value*/
		if (m_microphone.audio_meass_timer.value() >= LOG_PERIOD_MS)
		{
			m_microphone.readings_sum += m_microphone.max_value;
			m_microphone.max_value = 0;
			m_microphone.readings_num++;
			m_microphone.audio_meass_timer.reset();
			
			/* Maximum number of readings has been reached, calculate average */
			if (m_microphone.readings_num >= MAX_READINGS)
			{
				m_microphone.average_volume  = m_microphone.readings_sum / m_microphone.readings_num;
				m_microphone.readings_sum  = 0;
				m_microphone.readings_num  = 0;
				m_microphone.max_value	 = 0;
			}
			
		}
		delay_FreeRTOS_ms(2);
		//END LOOP
	}
}


