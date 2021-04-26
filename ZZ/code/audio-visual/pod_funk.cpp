#include "FreeRTOS.h"
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


void updateSTRIP()
{
	writePWM(strip_red_pin, strip_port,		(float)STRIP_CURRENT_RED	*  STRIP_CURRENT_BRIGHT / 255.00f);
	writePWM(strip_green_pin, strip_port,	(float)STRIP_CURRENT_GREEN	*  STRIP_CURRENT_BRIGHT / 255.00f);
	writePWM(strip_blue_pin, strip_port,	(float)STRIP_CURRENT_BLUE	*  STRIP_CURRENT_BRIGHT / 255.00f);
}

void flashSTRIP() //Utripanje (Izhod iz STATE_SCROLL stata / menjava mikrofona)
{					
	for (uint8_t i = 0; i < 5; i++)
	{
		STRIP_CURRENT_BRIGHT = 0;
		updateSTRIP();
		delayFREERTOS(125);
		STRIP_CURRENT_BRIGHT = 255;
		updateSTRIP();
		delayFREERTOS(125);
	}
}

void colorSHIFT(uint8_t BARVA, uint8_t cas_krog)
{
	char smer[3];
	while (STRIP_CURRENT_RED != mozne_barve.barvni_ptr[BARVA][0] || STRIP_CURRENT_GREEN != mozne_barve.barvni_ptr[BARVA][1] || STRIP_CURRENT_BLUE != mozne_barve.barvni_ptr[BARVA][2]) //Trenutna razlicna od zeljene
	{
		mozne_barve.barvni_ptr[BARVA][0] >= STRIP_CURRENT_RED	? smer[0] = 1 : smer[0] = -1;
		mozne_barve.barvni_ptr[BARVA][1] >= STRIP_CURRENT_GREEN? smer[1] = 1 : smer[1] = -1;
		mozne_barve.barvni_ptr[BARVA][2] >= STRIP_CURRENT_BLUE	? smer[2] = 1 : smer[2] = -1;

		STRIP_CURRENT_RED	+=	(6 * smer[0]);
		STRIP_CURRENT_GREEN	+=	(6 * smer[1]);
		STRIP_CURRENT_BLUE	+=	(6 * smer[2]);

		//Preveri prenihaj:

		STRIP_CURRENT_RED = (smer[0] == 1 && STRIP_CURRENT_RED > mozne_barve.barvni_ptr[BARVA][0]) ||
		(smer[0] == -1 && STRIP_CURRENT_RED < mozne_barve.barvni_ptr[BARVA][0]) ? mozne_barve.barvni_ptr[BARVA][0]: STRIP_CURRENT_RED ;	//Ce je bila trenutna barva pod zeljeno ali na zeljeni in je zdaj trenudna nad zeljeno, se nastavi na zeljeno (prenihaj)
		
		STRIP_CURRENT_GREEN = (smer[1] == 1 && STRIP_CURRENT_GREEN > mozne_barve.barvni_ptr[BARVA][1]) ||
		(smer[1] == -1 && STRIP_CURRENT_GREEN < mozne_barve.barvni_ptr[BARVA][1]) ? mozne_barve.barvni_ptr[BARVA][1]: STRIP_CURRENT_GREEN ;
		
		STRIP_CURRENT_BLUE = (smer[2] == 1 && STRIP_CURRENT_BLUE > mozne_barve.barvni_ptr[BARVA][2]) ||
		(smer[2] == -1 && STRIP_CURRENT_BLUE < mozne_barve.barvni_ptr[BARVA][2]) ? mozne_barve.barvni_ptr[BARVA][2]: STRIP_CURRENT_BLUE ;

		updateSTRIP();
		delayFREERTOS(cas_krog);
	}
}

void brightnessFADE(char smer, uint8_t cas_krog)
{
	while (smer > 0 ? STRIP_CURRENT_BRIGHT < 255 : STRIP_CURRENT_BRIGHT > 0)
	{
		STRIP_CURRENT_BRIGHT += 6 * smer;
		STRIP_CURRENT_BRIGHT = STRIP_CURRENT_BRIGHT < 0 ? 0 : STRIP_CURRENT_BRIGHT;
		STRIP_CURRENT_BRIGHT = STRIP_CURRENT_BRIGHT > 255 ? 255 : STRIP_CURRENT_BRIGHT;
		updateSTRIP();
		delayFREERTOS(cas_krog);
	}
}

void set_stripCOLOR(unsigned char barva_index)
{
	STRIP_CURRENT_RED	= mozne_barve.barvni_ptr[barva_index][0];
	STRIP_CURRENT_GREEN	= mozne_barve.barvni_ptr[barva_index][1];
	STRIP_CURRENT_BLUE	= mozne_barve.barvni_ptr[barva_index][2];
}


void stripOFF()
{
	deleteTASK(&audio_system.handle_average_volume);
	holdTASK(&audio_system.handle_audio_system);
	deleteTASK(&audio_system.handle_active_strip_mode);	
	delayFREERTOS(1);
	brightDOWN(15);
}

void stripON()
{
	resumeTASK(&audio_system.handle_audio_system);
	if (audio_system.mic_mode == AVERAGE_VOLUME)
	{	xTaskCreate(avg_vol_task, "avg_vol", 80, NULL, 2, &audio_system.handle_average_volume);}
	audio_system.strip_mode = EEPROM.beri(strip_mode_address);
	delayFREERTOS(1);
}