#include "castimer/castimer.h"
#include "VHOD/Vhod.h"
#include "includes/audio.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/global.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "util/delay.h"


/************************************************************************/
/*                            TASK HANDLES                              */
/************************************************************************/
TaskHandle_t normal_fade_handle;
TaskHandle_t color_fade_handle;
TaskHandle_t breathe_fade_handle;
TaskHandle_t average_volume_handle;
/************************************************************************/
/*							AUDIO VISUAL STRUCTS                        */
/************************************************************************/
struct_AUDIO_SYS audio_system;
struct_MOZNE_BARVE mozne_barve;


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
	uint16_t ref_glasnost = 2048;
	class_TIMER lucke_filter_timer;
	class_TIMER mic_ref_timer;
	xTaskCreate(avg_vol_task, "Average Volume Task", 128,NULL, 2, &average_volume_handle);
	while (true)
	{
		switch (audio_system.mic_mode)
		{
		case POTENCIOMETER:
		
			mikrofon_detect = readANALOG(mic_pin) > ref_glasnost; //Gleda ce je vrednost mikrofona nad referencno in se sprozi
			
			if (mic_ref_timer.vrednost() > 3000) // Posodobi vsako sekundo
			{
				mic_ref_timer.ponastavi();
				ref_glasnost = readANALOG(mic_ref_pin); // Mic_ref = referencna adc vrednost za logicno enko mikrofon_detecta
			}
			break;
		case AVG_VOL:
			mikrofon_detect = readANALOG(mic_pin) >= (audio_system.average_volume + 60);
			break;
		}
		
		if (lucke_filter_timer.vrednost() > 100 && mikrofon_detect) // AUDIO_M machine
		{
			lucke_filter_timer.ponastavi();
			static uint8_t barva_selekt = 0;
			barva_selekt += 1 % barve_end;
			switch (audio_system.strip_mode)
			{

			case NORMAL_FADE: //Prizig in fade izklop
				deleteTASK(normal_fade_handle);
				xTaskCreate(normal_fade_task, "NormalFade", 128, NULL, 4, &normal_fade_handle);
				break;

			case COLOR_FADE: //Prehod iz trenutne barve v zeljeno
				deleteTASK(color_fade_handle);
				xTaskCreate(color_fade_task, "ColorFade", 128, NULL, 4, &color_fade_handle);
				break;

			case BREATHE_FADE: //Dihalni nacin
				deleteTASK(normal_fade_handle);
				xTaskCreate(breathe_fade_task, "BreatheFade", 128, NULL, 4, &breathe_fade_handle);
				break;

			case OFF_A:
				holdTASK(audio_system_control); //Ne rabi hoditi v task ce je izkljucen
				break;
			}
		}

		delayFREERTOS(5);
		//End task loop
	}
}

/**************************************************************************************************************************
*                                                                                                                         *
*                                                         FADE TASKI                                                      *
*                                                                                                                         *
**************************************************************************************************************************/
void normal_fade_task(void *BARVA) //Prizig na barbi in pocasen izklop
{
	STRIP_CURRENT_BRIGHT = 255;
	set_strip_color(BARVA);

	brightDOWN(5);

	normal_fade_handle = NULL;
	vTaskDelete(NULL);
}



void color_fade_task(void *BARVA) //Fade iz ene barve v drugo
{

	if (breathe_fade_handle == NULL) // Ce diha on ne sme nastaviti svetlosti,
	{                            // saj jo nastavlja dihalni task
		STRIP_CURRENT_BRIGHT = 255;
	}

	colorSHIFT(BARVA); //prehod iz ene barve v drugo
	color_fade_handle = NULL;
	vTaskDelete(NULL);
}

void breathe_fade_task(void *BARVA)
{
	
	
	if (color_fade_handle == NULL)
	{

		set_strip_color(BARVA);
	}
	brightUP(3);
	brightDOWN(3);
	breathe_fade_handle = NULL;
	vTaskDelete(NULL);
}


