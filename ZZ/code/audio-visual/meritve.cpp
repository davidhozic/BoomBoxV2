

#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/global.h"
#include "includes/audio.h"
#include "castimer/castimer.h"

/************************************************************************/
	#define volume_spike		( (audio_system.average_volume + audio_system.average_volume * 0.15) )
	#define	max_readings_num	( 25 )
	#define reading_period_ms	( 15 )
/************************************************************************/



struct struct_average_volume
{
	uint8_t   spike_counter = 0;
	uint16_t  max_value = 0;
	uint16_t  readings_sum	 = 0;
	uint8_t	  readings_num : 7;
	bool	  value_logged : 1;
	uint16_t  current_value  = 0;
	uint16_t  previous_value = 0;
	class_TIMER average_v_timer		= class_TIMER(Hardware.timer_list);				// Timer that delays logging of max measured volume voltage
	
	struct_average_volume()
	{
		readings_num = 0;
		value_logged = false;	
	}
}average_volume;


#if USE_TIMED_AVERAGE == 1

void avg_vol_task(void* param)
{	
	while (1)
	{
		average_volume.current_value = readANALOG(mic_pin);
		if (average_volume.current_value > average_volume.max_value)
			average_volume.max_value = average_volume.current_value;

		if (average_volume.average_v_timer.vrednost() >= reading_period_ms && (average_volume.max_value < volume_spike || average_volume.spike_counter > max_readings_num) )
		{
			average_volume.readings_sum += average_volume.max_value;
			average_volume.max_value = 0;
			average_volume.readings_num++;
			average_volume.average_v_timer.ponastavi();
		}
		
		else if(average_volume.max_value >= volume_spike  && average_volume.average_v_timer.vrednost() > reading_period_ms)
		{
			average_volume.spike_counter++;	
			average_volume.max_value = 0;
			average_volume.average_v_timer.ponastavi();
		}
		
		
		if (average_volume.readings_num >= max_readings_num)
		{
			audio_system.average_volume  = average_volume.readings_sum / average_volume.readings_num;
			average_volume.readings_sum  = 0;
			average_volume.readings_num  = 0;
			average_volume.max_value	 = 0;
			average_volume.spike_counter = 0;
		}
		
		delayFREERTOS(1);
		
		//END LOOP
	}
}
#else

void avg_vol_task(void* param)
{
	while(1)
	{
		average_volume.current_value = readANALOG(mic_pin);
	
		if (average_volume.current_value >= average_volume.previous_value) /* Keep track of the rising value*/
		{
			 average_volume.previous_value =  average_volume.current_value;		 
			 average_volume.value_logged = false;		/* Reset logged status if value has started to rise */
		}	
		
		else if (average_volume.current_value <  average_volume.previous_value)	/* Value has dropped -> maximum volume was clearly reached */
		{
			if (!average_volume.value_logged)		/* Log value only once  & don't log spikes*/
			{
				average_volume.readings_sum += average_volume.current_value;
				average_volume.readings_num++;
				average_volume.value_logged = true;
				average_volume.spike_counter = 0;
			}
			average_volume.previous_value = average_volume.current_value;
		}
		
		if (average_volume.readings_num >= max_readings_num)
		{
			audio_system.average_volume = average_volume.readings_sum/average_volume.readings_num;
			average_volume.readings_sum = 0;
			average_volume.readings_num = 0;
		}

	}
	delayFREERTOS(1);
}

#endif
