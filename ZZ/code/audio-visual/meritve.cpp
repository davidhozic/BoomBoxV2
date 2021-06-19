

#include "libs/outputs_inputs/outputs_inputs.hh"
#include "common/inc/global.hh"
#include "EEPROM.hh"
#include "includes/audio.hh"
#include "castimer/castimer.hh"
#include <math.h>

/************************************************************************/
	#define	MAX_READINGS	( 30 )
	#define LOG_PERIOD_MS	( 10 )
	#define	MIC_TRIGGER_PERCENT										((( 30.00/600 * m_audio_system.average_volume + 6)/100.00))
/************************************************************************/



struct struct_microphone_t
{
	uint16_t  max_value = 0;
	uint16_t  readings_sum	 = 0;
	uint8_t	  readings_num : 7;
	bool	  value_logged : 1;
	uint16_t  current_value  = 0;
	uint16_t  previous_value = 0;
	
	TIMER_t average_v_timer;		// Timer that delays logging of max measured volume voltage
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
		
		/* Volume spike detected -> Trigger the led strip*/
		if (microphone.current_value > (double)m_audio_system.average_volume + (double)m_audio_system.average_volume * MIC_TRIGGER_PERCENT)
			m_audio_system.mikrofon_detect = 1;  // Gets cleared after strip has light up
		
		/* Finds the signal amplitude */
		if (microphone.current_value > microphone.max_value)
			microphone.max_value = microphone.current_value;

		/* If period has elapsed, log the value*/
		if (microphone.average_v_timer.value() >= LOG_PERIOD_MS)
		{
			microphone.readings_sum += microphone.max_value;
			microphone.max_value = 0;
			microphone.readings_num++;
			microphone.average_v_timer.reset();
			
			/* Maximum number of readings has been reached, calculate average */
			if (microphone.readings_num >= MAX_READINGS)
			{
				m_audio_system.average_volume  = microphone.readings_sum / microphone.readings_num;
				microphone.readings_sum  = 0;
				microphone.readings_num  = 0;
				microphone.max_value	 = 0;
			}
			
		}
		delayFREERTOS(2);
		//END LOOP
	}
}


