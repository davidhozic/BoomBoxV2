

#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/global.h"
#include "includes/audio.h"
#include "castimer/castimer.h"

/************************************************************************/
	#define volume_spike		( (audio_system.average_volume + audio_system.average_volume * 0.15) )
	#define max_spikes			(  6 )
	#define	max_readings_num	( 25 )
	#define reading_period_ms	( 12 )
/************************************************************************/


void avg_vol_task(void* param)
{
	unsigned long vsota_branj = 0;
	uint8_t st_branj = 0;
	unsigned short max_izmerjeno = 0;
	unsigned short tr_vrednost = 0;
	uint8_t spike_counter = 0;
	
	while (1)
	{
		tr_vrednost = readANALOG(mic_pin);
		if (tr_vrednost > max_izmerjeno)
			max_izmerjeno = tr_vrednost;

		if (audio_system.average_v_timer.vrednost() >= reading_period_ms && max_izmerjeno < volume_spike)
		{
			vsota_branj += max_izmerjeno;
			max_izmerjeno = 0;
			st_branj++;
			audio_system.average_v_timer.ponastavi();
			spike_counter = 0;					/* Prevent the filtering of volume spikes from blocking the readings if volume is increased permanently via potentiometer	*/
		}
		
		else if(max_izmerjeno >= volume_spike && spike_counter < max_spikes)
		{
			spike_counter++;	
			max_izmerjeno = 0;
			audio_system.average_v_timer.ponastavi();
		}
		
		else if (spike_counter >= max_spikes)	/* If volume is permanently increased, start measuring from beginning */
		{
			spike_counter = 0;
			audio_system.average_volume = 2048;
			vsota_branj = 0;
			st_branj = 0;
			max_izmerjeno = 0;
		}
		
		if (st_branj >= max_readings_num)
		{
			audio_system.average_volume = vsota_branj / st_branj;
			vsota_branj = 0;
			st_branj = 0;
			max_izmerjeno = 0;
		}
		
		delayFREERTOS(2);
		
		//END LOOP
	}
}
