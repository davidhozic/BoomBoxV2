#include "FreeRTOS.h"
#include "includes/audio.h"
#include "libs/EEPROM/EEPROM.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include <stdlib.h>
#include <string.h>
/**************************************************************************************************************************
*                                                                                                                         *
*                                                           Pomozne funkcije                                              *
*                                                                                                                         *
**************************************************************************************************************************/


void update_strip()
{
	writePWM(strip_red_pin, strip_port, (float)STRIP_CURRENT_RED * STRIP_CURRENT_BRIGHT / 255.00f);
	writePWM(strip_green_pin, strip_port, (float)STRIP_CURRENT_GREEN * STRIP_CURRENT_BRIGHT / 255.00f);
	writePWM(strip_blue_pin, strip_port, (float)STRIP_CURRENT_BLUE * STRIP_CURRENT_BRIGHT / 255.00f);
}

void flash_strip() //Utripanje (Izhod iz scroll stata / menjava mikrofona)
{
	set_strip_color(BELA);						
	for (uint8_t i = 0; i < 5; i++)
	{
		writeOUTPUT(strip_red_pin, strip_port, 0);
		writeOUTPUT(strip_green_pin, strip_port, 0);
		writeOUTPUT(strip_blue_pin, strip_port, 0);
		delayFREERTOS(125);
		writeOUTPUT(strip_red_pin, strip_port, 1);
		writeOUTPUT(strip_green_pin, strip_port, 1);
		writeOUTPUT(strip_blue_pin, strip_port, 1);
		delayFREERTOS(125);
	}
}

void color_fade_funct(uint8_t *BARVA)
{
	while (STRIP_CURRENT_RED != mozne_barve.barvni_ptr[*BARVA][0] || STRIP_CURRENT_GREEN != mozne_barve.barvni_ptr[*BARVA][1] || STRIP_CURRENT_BLUE != mozne_barve.barvni_ptr[*BARVA][2]) //Trenutna razlicna od zeljene
	{
		char smer[3] = {0, 0, 0};
		mozne_barve.barvni_ptr[*BARVA][0] >= STRIP_CURRENT_RED ? smer[0] = 1 : smer[0] = -1;
		mozne_barve.barvni_ptr[*BARVA][1] >= STRIP_CURRENT_GREEN ? smer[1] = 1 : smer[1] = -1;
		mozne_barve.barvni_ptr[*BARVA][2] >= STRIP_CURRENT_BLUE ? smer[2] = 1 : smer[2] = -1;

		STRIP_CURRENT_RED += (10 * smer[0]);
		STRIP_CURRENT_GREEN += (10 * smer[1]);
		STRIP_CURRENT_BLUE += (10 * smer[2]);

		//Preveri prenihaj:

		STRIP_CURRENT_RED = (smer[0] == 1 && STRIP_CURRENT_RED > mozne_barve.barvni_ptr[*BARVA][0]) ||
		(smer[0] == -1 && STRIP_CURRENT_RED < mozne_barve.barvni_ptr[*BARVA][0]) ? mozne_barve.barvni_ptr[*BARVA][0]: STRIP_CURRENT_RED ;	//Ce je bila trenutna barva pod zeljeno ali na zeljeni in je zdaj trenudna nad zeljeno, se nastavi na zeljeno (prenihaj)
		
		STRIP_CURRENT_GREEN = (smer[1] == 1 && STRIP_CURRENT_GREEN > mozne_barve.barvni_ptr[*BARVA][1]) ||
		(smer[1] == -1 && STRIP_CURRENT_GREEN < mozne_barve.barvni_ptr[*BARVA][1]) ? mozne_barve.barvni_ptr[*BARVA][1]: STRIP_CURRENT_GREEN ;
		
		STRIP_CURRENT_BLUE = (smer[2] == 1 && STRIP_CURRENT_BLUE > mozne_barve.barvni_ptr[*BARVA][2]) ||
		(smer[2] == -1 && STRIP_CURRENT_BLUE < mozne_barve.barvni_ptr[*BARVA][2]) ? mozne_barve.barvni_ptr[*BARVA][2]: STRIP_CURRENT_BLUE ;

		update_strip();
		delayFREERTOS(5);
	}
}

void svetlost_mod_funct(char smer, uint8_t cas_krog)
{
	while (smer > 0 ? STRIP_CURRENT_BRIGHT < 255 : STRIP_CURRENT_BRIGHT > 0)
	{
		STRIP_CURRENT_BRIGHT += 8 * smer;
		STRIP_CURRENT_BRIGHT = STRIP_CURRENT_BRIGHT < 0 ? 0 : STRIP_CURRENT_BRIGHT;
		STRIP_CURRENT_BRIGHT = STRIP_CURRENT_BRIGHT > 255 ? 255 : STRIP_CURRENT_BRIGHT;
		update_strip();
		delayFREERTOS(cas_krog);
	}
}




void strip_mode_chg(const char* ch)
{
	if (strcmp(ch,"off"))
	STRIP_MODE = STRIP_OFF;

	else if (STRIP_MODE == STRIP_OFF)
	STRIP_MODE = NORMAL_FADE;

	else
	STRIP_MODE = (STRIP_MODE + 1) % end_strip_modes;

	EEPROM.pisi(audiomode_eeprom_addr, STRIP_MODE);
	deleteTASK_REC(strip_modes_ptr_arr);
}

