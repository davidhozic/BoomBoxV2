#include "castimer/castimer.h"
#include "VHOD/Vhod.h"
#include "includes/audio.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/global.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "util/delay.h"
/*************************** task nadzor  **************************/
TaskHandle_t fade_control;
TaskHandle_t color_fade_control;
TaskHandle_t Breathe_control;
/*******************************************************************/

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
	bool mikrofon_detect = false;
	uint16_t povprecna_glasnost = 0;
	uint16_t ref_glasnost = 2048;
	castimer lucke_filter_time;
	castimer mic_ref_timer;

	while (true)
	{
		switch (Audio_vars.MIC_MODE)
		{

		case POTENCIOMETER:
			mikrofon_detect = readANALOG(mic_pin) > ref_glasnost; //Gleda ce je vrednost mikrofona nad referencno in se sprozi

			if (mic_ref_timer.vrednost() > 1000) // Posodobi vsako sekundo
			{
				mic_ref_timer.ponastavi();
				ref_glasnost = readANALOG(mic_ref_pin); // Mic_ref = referencna adc vrednost za logicno enko mikrofon_detecta
			}
			break;
		case AVG_VOL:
			mikrofon_detect = readANALOG(mic_pin) >= (povprecna_glasnost + 80) && povprecna_glasnost != 0 ? true : false;
			break;
		}
		
		if (lucke_filter_time.vrednost() > 100 && mikrofon_detect) // AUDIO_M machine
		{
			lucke_filter_time.ponastavi();
			static uint8_t barva_selekt = 0;
			barva_selekt += 1 % barve_end;
			switch (Audio_vars.STRIP_MODE)
			{

			case NORMAL_FADE: //Prizig in fade izklop
				cr_fade_tsk(fade_task, "Normal Fade", barva_selekt, fade_control);
				break;

			case COLOR_FADE: //Prehod iz trenutne barve v zeljeno
				cr_fade_tsk(Color_Fade_task, "Color shift", barva_selekt, color_fade_control);
				break;

			case Fade_Breathe: //Dihalni nacin
				cr_fade_tsk(Fade_Breathe_task, "Breathe Fade", barva_selekt, Breathe_control);
				break;

			case OFF_A:
				holdTASK(audio_system_control); //Ne rabi hoditi v task ce je izkljucen
				break;
			}
		}

		delayFREERTOS(10);
		//End task loop
	}
}
