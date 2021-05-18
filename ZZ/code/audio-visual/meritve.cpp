

#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/global.h"
#include "EEPROM.h"
#include "includes/audio.h"
#include "castimer/castimer.h"
#include <math.h>
/************************************************************************/
	#define	max_readings_num	( 30 )
	#define reading_period_ms	( 10 )
/************************************************************************/



struct struct_average_volume
{
	uint16_t  spike_counter = 0;
	uint16_t  max_value = 0;
	uint16_t  readings_sum	 = 0;
	uint8_t	  readings_num : 7;
	bool	  value_logged : 1;
	uint16_t  current_value  = 0;
	uint16_t  previous_value = 0;
	
	class_TIMER average_v_timer;		// Timer that delays logging of max measured volume voltage

	struct_average_volume()
	{
		readings_num = 0;
		value_logged = false;	
	}
}microphone;


/* Measures average volume and frequency*/
void signal_measure(void* param)
{	
	while (1)
	{
		/************************************************************************/
		/*							 AVERAGE VOLUME		                        */
		/************************************************************************/
		microphone.current_value = readANALOG(mic_pin);
		
		m_audio_system.mikrofon_detect = microphone.current_value > (double)m_audio_system.average_volume + (double)m_audio_system.average_volume * mic_trigger_level_percent; 
		if (microphone.current_value > microphone.max_value)
			microphone.max_value = microphone.current_value;

		if (microphone.average_v_timer.vrednost() >= reading_period_ms)
		{
			microphone.readings_sum += microphone.max_value;
			microphone.max_value = 0;
			microphone.readings_num++;
			microphone.average_v_timer.ponastavi();
		}
		
		if (microphone.readings_num >= max_readings_num)
		{
			m_audio_system.average_volume  = microphone.readings_sum / microphone.readings_num;
			microphone.readings_sum  = 0;
			microphone.readings_num  = 0;
			microphone.max_value	 = 0;
			microphone.spike_counter = 0;
		}	
		
		
		delayFREERTOS(1);
		
		//END LOOP
	}
}


