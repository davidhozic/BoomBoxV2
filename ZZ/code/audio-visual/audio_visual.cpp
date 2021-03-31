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

// Strip mode handles
TaskHandle_t handle_normal_fade;
TaskHandle_t handle_inverse_normal_fade;
TaskHandle_t handle_color_fade;
TaskHandle_t handle_breathe_fade;

// Microphone measuring tasks
TaskHandle_t handle_average_volume;

TaskHandle_t strip_mode_handle_arr [] = {handle_normal_fade, handle_inverse_normal_fade, handle_color_fade, handle_breathe_fade, NULL}; // TaskHandle_t is a TaskControlBlock pointer, no need to make TaskHandle pointer
TaskHandle_t mic_mode_handle_arr[] = {handle_average_volume, NULL};	
	
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
				deleteTASK(handle_normal_fade);
				xTaskCreate(normal_fade_task, "NormalFade", 128, &barva_selekt, 4, &handle_normal_fade);
				break;
			
			case INVERSE_NORMAL_FADE:
				deleteTASK(handle_inverse_normal_fade);
				xTaskCreate(inverse_normal_fade_task, "InverseNF", 128, &barva_selekt, 4, &handle_inverse_normal_fade);
				break;
			
			case COLOR_FADE: //Prehod iz trenutne barve v zeljeno
				deleteTASK(handle_color_fade);
				xTaskCreate(color_fade_task, "ColorFade", 128, &barva_selekt, 4, &handle_color_fade);
				break;

			case BREATHE_FADE: //Dihalni nacin
				deleteTASK(handle_normal_fade);
				xTaskCreate(breathe_fade_task, "BreatheFade", 128, &barva_selekt, 4, &handle_breathe_fade);
				break;

			case STRIP_OFF:
				holdTASK(handle_audio_system); //Ne rabi hoditi v task ce je izkljucen
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
	set_strip_color(*((uint8_t*)BARVA));

	brightDOWN(5);

	handle_normal_fade = NULL;
	vTaskDelete(NULL);
}

void inverse_normal_fade_task(void *BARVA){
	
	STRIP_CURRENT_BRIGHT = 0;
	set_strip_color( *( (uint8_t*) BARVA ) );
	brightUP(12);	
}

void color_fade_task(void *BARVA) //Fade iz ene barve v drugo
{
	STRIP_CURRENT_BRIGHT = 255;
	colorSHIFT(BARVA, 4); //prehod iz ene barve v drugo
	handle_color_fade = NULL;
	vTaskDelete(NULL);
}

void breathe_fade_task(void *BARVA)
{
	
	set_strip_color(*((uint8_t*)BARVA));
	brightUP(3);
	brightDOWN(3);
	handle_breathe_fade = NULL;
	vTaskDelete(NULL);
}


