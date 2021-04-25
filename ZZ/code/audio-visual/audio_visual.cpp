#include "castimer/castimer.h"
#include "VHOD/Vhod.h"
#include "includes/audio.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/global.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "util/delay.h"
#include <stdlib.h>
/************************************************************************/
/*                            TASK HANDLES                              */
/************************************************************************/
	
// Microphone measuring tasks

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
	while (true)
	{
		
		switch (audio_system.mic_mode)
		{
		case POTENCIOMETER:
			audio_system.mikrofon_detect = readANALOG(mic_pin) > audio_system.ref_glasnost; //Gleda ce je vrednost mikrofona nad referencno in se sprozi
			if (audio_system.mic_ref_timer.vrednost() > 1000) // Posodobi vsako sekundo
			{
				audio_system.mic_ref_timer.ponastavi();
				audio_system.ref_glasnost = readANALOG(mic_ref_pin) * (float) 190/1023 + 300; // Mic_ref = referencna adc vrednost za logicno enko mikrofon_detecta
			}
			break;
			
		case AVERAGE_VOLUME:
			audio_system.mikrofon_detect = readANALOG(mic_pin) >= (audio_system.average_volume + 65);
			break;
		}
		
		
 		if (audio_system.lucke_filter_timer.vrednost() >= 300 && audio_system.mikrofon_detect) // AUDIO_M machine
		{
			audio_system.lucke_filter_timer.ponastavi();
			audio_system.barva_selekt = random() %  enum_BARVE::barve_end;
			// STRIP task creation
			switch (audio_system.strip_mode)
			{
				
			case NORMAL_FADE: //Prizig in fade izklop
				deleteTASK(&audio_system.handle_active_strip_mode);
				xTaskCreate(normal_fade_task,"normFade", 128, &audio_system.barva_selekt, 4, &audio_system.handle_active_strip_mode);
				break;
			
			case COLOR_FADE: //Prehod iz trenutne barve v zeljeno
				deleteTASK(&audio_system.handle_active_strip_mode);
				xTaskCreate(color_fade_task,"colorFade", 128, &audio_system.barva_selekt, 4, &audio_system.handle_active_strip_mode);
				break;

			case BREATHE_FADE: //Dihalni nacin
				deleteTASK(&audio_system.handle_active_strip_mode);
				xTaskCreate(breathe_fade_task,"breatheFade", 128, &audio_system.barva_selekt, 4, &audio_system.handle_active_strip_mode);
				break;
			}
		}

		delayFREERTOS(4);
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
	brightDOWN(10);
	audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}

void color_fade_task(void *BARVA) //Fade iz ene barve v drugo
{
	STRIP_CURRENT_BRIGHT = 255;
	colorSHIFT(*(uint8_t*)BARVA, 5); //prehod iz ene barve v drugo
	audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}

void breathe_fade_task(void *BARVA)
{
	set_stripCOLOR(*((uint8_t*)BARVA));
	brightUP(5);
	brightDOWN(5);
	audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}


