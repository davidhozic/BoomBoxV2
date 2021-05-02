#include <stdlib.h>
#include "includes/audio.h"
#include "castimer/castimer.h"
#include "VHOD/Vhod.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/FreeRTOS_def_decl.h"


/************************************************************************/
/*                            TASK HANDLES                              */
/************************************************************************/

// Microphone measuring tasks

/************************************************************************/
/*							AUDIO VISUAL STRUCTS                        */
/************************************************************************/
class_AUDIO_SYS audio_system;
struct_MOZNE_BARVE mozne_barve =
{
	.BELA	 =		{255, 255, 255},
	.ZELENA	 =		{0, 255, 0},
	.RDECA	 =		{255, 0, 0},
	.MODRA   =		{0, 0, 255},
	.RUMENA  =		{255, 255, 0},
	.SVETLO_MODRA =	{0, 255, 255},
	.VIJOLICNA =	{255, 0, 255},
	.ROZA =			{255, 20, 147},
	.barvni_ptr  = {mozne_barve.BELA, mozne_barve.ZELENA, mozne_barve.RDECA, mozne_barve.MODRA, mozne_barve.RUMENA, mozne_barve.SVETLO_MODRA, mozne_barve.VIJOLICNA, mozne_barve.ROZA}
};
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
		
		if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_POWERED_UP) && audio_system.strip_mode != STRIP_OFF)
		{
			switch (audio_system.mic_mode)															/* Microphone spike trigger level measurement */
			{
				case POTENTIOMETER:
					audio_system.mikrofon_detect = readANALOG(mic_pin) >= audio_system.ref_glasnost;		//Gleda ce je vrednost mikrofona nad referencno in se sprozi
					if (audio_system.mic_ref_timer.vrednost() > 1000)									// Posodobi vsako sekundo
					{
						audio_system.mic_ref_timer.ponastavi();
						audio_system.ref_glasnost = readANALOG(mic_ref_pin) * (float) 693/1023 + 330;	// Mic_ref = referencna adc vrednost za logicno enko mikrofon_detecta
					}
				break;
				
				case AVERAGE_VOLUME:
					audio_system.mikrofon_detect = readANALOG(mic_pin) >= audio_system.average_volume + audio_system.average_volume * 0.18;
				break;
			}

			if (audio_system.lucke_filter_timer.vrednost() >= 400 && audio_system.mikrofon_detect)	 // STRIP task creation
			{
				audio_system.lucke_filter_timer.ponastavi();
				audio_system.barva_selekt = random() %  enum_BARVE::barve_end;	// Color is randomly chosen
				deleteTASK(&audio_system.handle_active_strip_mode);
				xTaskCreate(audio_system.array_strip_modes[audio_system.strip_mode], "strip mode", 128, &audio_system.barva_selekt, 4, &audio_system.handle_active_strip_mode);
			}
		}
		
		delayFREERTOS(2);
		//End task loop
	}
}

/**************************************************************************************************************************
*                                                                                                                         *
*                                                         FADE TASKI                                                      *
*                                                                                                                         *
**************************************************************************************************************************/
void normal_fade_task(void *input) //Prizig na barbi in pocasen izklop
{
	audio_system.current_brightness = 255;
	audio_system.select_strip_color(*(uint8_t*)input);
	brightDOWN(10);
	audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}

void breathe_fade_task(void *input)
{
	audio_system.select_strip_color(*(uint8_t*)input);
	brightUP(5);
	brightDOWN(5);
	audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}

