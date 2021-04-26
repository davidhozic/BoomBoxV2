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
struct_AUDIO_SYS audio_system =
{
		/***  Strip parameters   ***/
		.strip_mode = NORMAL_FADE,		// Current strip mode
		.mic_mode = POTENCIOMETER,		// Current spike trigger detection mode
		
		/*** Strip current state ***/
		.current_color = {0, 0, 0},	// Current RGB color of the strip
		.current_brightness = 0,		// Current brightness level of the strip
		
		/***		Timers		 ***/
		.lucke_filter_timer = class_TIMER(),			// Timer that prevents strip from triggering too fast after last trigger (filter timer)
		.mic_ref_timer = class_TIMER(),				// Timer that delays reading in potentiometer spike trigger mode
		.average_v_timer= class_TIMER(),				// Timer that delays logging of max measured volume voltage
		
		/****   Strip lightup	***/
		.barva_selekt = 0,				// Index of color that strip will turn on
		.mikrofon_detect = 0,				// Is set to 1 if spike is detected and then strip is turned on
		.ref_glasnost = 2048,			// Variable that stores potentiometer setting of minimal spike trigger level
		.average_volume = 2048,			// Variable that stores the average volume
		/****   Task handles	***/
		.handle_average_volume = NULL,
		.handle_active_strip_mode = NULL,
		.handle_audio_system = NULL,
		/***	Strip mode functions ***/
		.array_strip_modes = {normal_fade_task, breathe_fade_task}
};


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
	volatile int i = 0;
void audio_visual(void *p) //Funkcija avdio-vizualnega sistema
{
	while (true)
	{
		
		if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_POWERED_UP))
		{
			switch (audio_system.mic_mode)				/* Microphone spike trigger level measurement */
			{
			case POTENCIOMETER:
				audio_system.mikrofon_detect = readANALOG(mic_pin) > audio_system.ref_glasnost; //Gleda ce je vrednost mikrofona nad referencno in se sprozi
				if (audio_system.mic_ref_timer.vrednost() > 1000) // Posodobi vsako sekundo
				{
					audio_system.mic_ref_timer.ponastavi();
					audio_system.ref_glasnost = readANALOG(mic_ref_pin) * (float) 180/1023 + 350; // Mic_ref = referencna adc vrednost za logicno enko mikrofon_detecta
				}
			break;
			
			case AVERAGE_VOLUME:
				audio_system.mikrofon_detect = readANALOG(mic_pin) >= (audio_system.average_volume + audio_system.average_volume * 0.20);
			break;
			}

 			if (audio_system.lucke_filter_timer.vrednost() >= 400 && audio_system.mikrofon_detect)  // STRIP task creation
			{
				audio_system.lucke_filter_timer.ponastavi();
				audio_system.barva_selekt = random() %  enum_BARVE::barve_end; // Color is randomly chosen
				deleteTASK(&audio_system.handle_active_strip_mode);
				xTaskCreate(audio_system.array_strip_modes[STRIP_MODE], "strip mode", 80, &audio_system, 4, &audio_system.handle_active_strip_mode);
			}
		}
		i = uxTaskGetStackHighWaterMark(audio_system.handle_audio_system);
		delayFREERTOS(4);
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
	STRIP_CURRENT_BRIGHT = 255;
	set_stripCOLOR(audio_system.barva_selekt);
	brightDOWN(10);
	audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}

void breathe_fade_task(void *input)
{
	set_stripCOLOR(audio_system.barva_selekt);
	brightUP(5);
	brightDOWN(5);
	audio_system.handle_active_strip_mode = NULL;
	vTaskDelete(NULL);
}


