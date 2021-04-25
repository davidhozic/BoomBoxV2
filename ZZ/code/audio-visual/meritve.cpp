
#include "castimer/castimer.h"
#include "includes/audio.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/global.h"

void avg_vol_task(void* param)
{
	unsigned long vsota_branj = 0;
	unsigned short st_branj = 0;
	unsigned short max_izmerjeno = 0;
	unsigned short tr_vrednost = 0;
	unsigned char over_average_ct = 0;
	
	while (1){

		tr_vrednost = readANALOG(mic_pin);
		if (tr_vrednost > max_izmerjeno)
		max_izmerjeno = tr_vrednost;

		if (audio_system.average_v_timer.vrednost() >= 12 && (max_izmerjeno < audio_system.average_volume + 65 || over_average_ct >= 6))
		{
			vsota_branj += max_izmerjeno;
			st_branj++;
			max_izmerjeno = 0;
			audio_system.average_v_timer.ponastavi();
			over_average_ct = 0;
		}
		else if (audio_system.average_v_timer.vrednost() >= 12)
		{
			over_average_ct++;
			max_izmerjeno = 0;
			audio_system.average_v_timer.ponastavi();
		}

		if (st_branj >= 50)
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
