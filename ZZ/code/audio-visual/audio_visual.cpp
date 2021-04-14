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
TaskHandle_t active_strip_mode; //Holds address of the active strip mode task handle
	
// Microphone measuring tasks
TaskHandle_t handle_average_volume;
	
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
	uint8_t barva_selekt = 0;
	uint16_t ref_glasnost = 2048;
	class_TIMER lucke_filter_timer;
	class_TIMER mic_ref_timer;
	
	while (true)
	{
		
		if (audio_system.strip_mode == STRIP_OFF)		// Turn off task if strip isn't enabled
			holdTASK(&handle_audio_system);
		
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
			
		case AVERAGE_VOLUME:
			mikrofon_detect = readANALOG(mic_pin) >= (audio_system.average_volume + 60);
			break;
		}
		
		if (lucke_filter_timer.vrednost() > 100 && mikrofon_detect) // AUDIO_M machine
		{
			lucke_filter_timer.ponastavi();
			barva_selekt += 1 % barve_end;
			
			// STRIP task creation
			switch (audio_system.strip_mode)
			{
				
			case NORMAL_FADE: //Prizig in fade izklop
				create_strip_mode(normal_fade_task, "Normal fade", &barva_selekt, &active_strip_mode);
				break;
			
			case INVERSE_NORMAL_FADE:
				create_strip_mode(inverse_normal_fade_task, "Inverse fade", &barva_selekt, &active_strip_mode);
				break;
			
			case COLOR_FADE: //Prehod iz trenutne barve v zeljeno
				create_strip_mode(color_fade_task, "Color shift", &barva_selekt, &active_strip_mode);
				break;

			case BREATHE_FADE: //Dihalni nacin
				create_strip_mode(breathe_fade_task, "Breathing", &barva_selekt, &active_strip_mode);
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
	set_stripCOLOR(*((uint8_t*)BARVA));

	brightDOWN(5);
	vTaskDelete(NULL);
}

void inverse_normal_fade_task(void *BARVA){
	
	STRIP_CURRENT_BRIGHT = 0;
	set_stripCOLOR( *( (uint8_t*) BARVA ) );
	brightUP(12);	
}

void color_fade_task(void *BARVA) //Fade iz ene barve v drugo
{
	STRIP_CURRENT_BRIGHT = 255;
	colorSHIFT(*(uint8_t*)BARVA, 4); //prehod iz ene barve v drugo
	vTaskDelete(NULL);
}

void breathe_fade_task(void *BARVA)
{
	
	set_stripCOLOR(*((uint8_t*)BARVA));
	brightUP(3);
	brightDOWN(3);
	vTaskDelete(NULL);
}


