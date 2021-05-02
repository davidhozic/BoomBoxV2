#include "global.h"
#include "includes/audio.h"
#include "libs/EEPROM/EEPROM.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include <string.h>
/**************************************************************************************************************************
*                                                                                                                         *
*                                                           Pomozne funkcije                                              *
*                                                                                                                         *
**************************************************************************************************************************/



void class_AUDIO_SYS::updateSTRIP()
{
	writePWM(strip_red_pin, strip_port,		audio_system.current_color[RED] * audio_system.current_brightness/255.00);
	writePWM(strip_green_pin, strip_port,	audio_system.current_color[GREEN] * audio_system.current_brightness/255.00);
	writePWM(strip_blue_pin, strip_port,	audio_system.current_color[BLUE] * audio_system.current_brightness/255.00);
}

void class_AUDIO_SYS::flashSTRIP() //Utripanje (Izhod iz STATE_SCROLL stata / menjava mikrofona)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		audio_system.current_brightness = 0;
		updateSTRIP();
		delayFREERTOS(125);
		audio_system.current_brightness = 255;
		updateSTRIP();
		delayFREERTOS(125);
	}
}

void class_AUDIO_SYS::colorSHIFT(uint8_t BARVA, uint8_t cas_krog)
{
	char smer[3];
	while (audio_system.current_color[RED]	 != mozne_barve.barvni_ptr[BARVA][0] ||
	audio_system.current_color[GREEN] != mozne_barve.barvni_ptr[BARVA][1] ||
	audio_system.current_color[BLUE]	 != mozne_barve.barvni_ptr[BARVA][2])	// While current colors different from wanted
	{
		mozne_barve.barvni_ptr[BARVA][0] >= audio_system.current_color[RED]	? smer[0] = 1 : smer[0] = -1;
		mozne_barve.barvni_ptr[BARVA][1] >= audio_system.current_color[GREEN]? smer[1] = 1 : smer[1] = -1;
		mozne_barve.barvni_ptr[BARVA][2] >= audio_system.current_color[BLUE]	? smer[2] = 1 : smer[2] = -1;

		audio_system.current_color[RED]		+=	(6 * smer[0]);
		audio_system.current_color[GREEN]	+=	(6 * smer[1]);
		audio_system.current_color[BLUE]	+=	(6 * smer[2]);

		//Preveri prenihaj:

		audio_system.current_color[RED] = (smer[0] == 1 && audio_system.current_color[RED] > mozne_barve.barvni_ptr[BARVA][0]) ||
		(smer[0] == -1 && audio_system.current_color[RED] < mozne_barve.barvni_ptr[BARVA][0]) ? mozne_barve.barvni_ptr[BARVA][0]: audio_system.current_color[RED] ;	//Ce je bila trenutna barva pod zeljeno ali na zeljeni in je zdaj trenudna nad zeljeno, se nastavi na zeljeno (prenihaj)
		
		audio_system.current_color[GREEN] = (smer[1] == 1 && audio_system.current_color[GREEN] > mozne_barve.barvni_ptr[BARVA][1]) ||
		(smer[1] == -1 && audio_system.current_color[GREEN] < mozne_barve.barvni_ptr[BARVA][1]) ? mozne_barve.barvni_ptr[BARVA][1]: audio_system.current_color[GREEN] ;
		
		audio_system.current_color[BLUE] = (smer[2] == 1 && audio_system.current_color[BLUE] > mozne_barve.barvni_ptr[BARVA][2]) ||
		(smer[2] == -1 && audio_system.current_color[BLUE] < mozne_barve.barvni_ptr[BARVA][2]) ? mozne_barve.barvni_ptr[BARVA][2]: audio_system.current_color[BLUE] ;

		updateSTRIP();
		delayFREERTOS(cas_krog);
	}
}

void class_AUDIO_SYS::brightnessFADE(char smer, uint8_t cas_krog)
{
	while (smer > 0 ? audio_system.current_brightness < 255 : audio_system.current_brightness > 0)
	{
		audio_system.current_brightness += 6 * smer;
		audio_system.current_brightness = audio_system.current_brightness < 0 ? 0 : audio_system.current_brightness;
		audio_system.current_brightness = audio_system.current_brightness > 255 ? 255 : audio_system.current_brightness;
		updateSTRIP();
		delayFREERTOS(cas_krog);
	}
}


void class_AUDIO_SYS::stripOFF()
{
	strip_mode = STRIP_OFF;
	deleteTASK(&handle_average_volume);
	deleteTASK(&handle_active_strip_mode);
	delayFREERTOS(10);
	brightDOWN(15);
}

void class_AUDIO_SYS::stripON()
{
	if (mic_mode == AVERAGE_VOLUME)
	{
		xTaskCreate(avg_vol_task, "avg_vol", 80, NULL, 3, &handle_average_volume);
	}
	volatile uint8_t temp = EEPROM.beri(strip_mode_address);
	strip_mode = temp >= STRIP_OFF && temp <  end_strip_modes ? temp : NORMAL_FADE;	// Checks if reading returned value in range
	delayFREERTOS(10);
}