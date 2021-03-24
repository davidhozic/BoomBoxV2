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

void holdALL_tasks() // Zbrise obstojece taske ce obstajajo
{
	holdTASK(fade_control);
	holdTASK(color_fade_control);
	holdTASK(Breathe_control);
}

void deleteALL_subAUDIO_tasks()
{
	deleteTask(fade_control);
	deleteTask(color_fade_control);
	deleteTask(Breathe_control);
	delayFREERTOS(15);
}

void writeTRAK()
{
	writePWM(r_trak, 'B', (float)tr_r * (float)tr_bright / 255.00);
	writePWM(z_trak, 'B', (float)tr_z * (float)tr_bright / 255.00);
	writePWM(m_trak, 'B', (float)tr_m * (float)tr_bright / 255.00);
}

void flash_strip() //Utripanje (Izhod iz scroll stata / menjava mikrofona)
{
	for (uint8_t i = 0; i < 5; i++)
	{
		writeOUTPUT(r_trak, strip_port, 0);
		writeOUTPUT(z_trak, strip_port, 0);
		writeOUTPUT(m_trak, strip_port, 0);
		delayFREERTOS(125);
		writeOUTPUT(r_trak, strip_port, 1);
		writeOUTPUT(z_trak, strip_port, 1);
		writeOUTPUT(m_trak, strip_port, 1);
		delayFREERTOS(125);
	}
}

void color_fade_funct(uint8_t *BARVA)
{
	while (tr_r != mozne_barve.barvni_ptr[*BARVA][0] || tr_z != mozne_barve.barvni_ptr[*BARVA][1] || tr_m != mozne_barve.barvni_ptr[*BARVA][2]) //Trenutna razlicna od zeljene
	{
		char smer[3] = {0, 0, 0};
		mozne_barve.barvni_ptr[*BARVA][0] >= tr_r ? smer[0] = 1 : smer[0] = -1;
		mozne_barve.barvni_ptr[*BARVA][1] >= tr_z ? smer[1] = 1 : smer[1] = -1;
		mozne_barve.barvni_ptr[*BARVA][2] >= tr_m ? smer[2] = 1 : smer[2] = -1;

		tr_r = tr_r + (10 * smer[0]);
		tr_z = tr_z + (10 * smer[1]);
		tr_m = tr_m + (10 * smer[2]);

		//Preveri prenihaj:

		tr_r = (smer[0] == 1 && tr_r > mozne_barve.barvni_ptr[*BARVA][0]) || 
		(smer[0] == -1 && tr_r < mozne_barve.barvni_ptr[*BARVA][0]) ? mozne_barve.barvni_ptr[*BARVA][0]: tr_r ; //Ce je bila trenutna barva pod zeljeno ali na zeljeni in je zdaj trenudna nad zeljeno, se nastavi na zeljeno (prenihaj)
		
		tr_z = (smer[1] == 1 && tr_z > mozne_barve.barvni_ptr[*BARVA][1]) ||
		(smer[1] == -1 && tr_z < mozne_barve.barvni_ptr[*BARVA][1]) ? mozne_barve.barvni_ptr[*BARVA][1]: tr_z ;
		
		tr_m = (smer[2] == 1 && tr_m > mozne_barve.barvni_ptr[*BARVA][2]) || 
		(smer[2] == -1 && tr_m < mozne_barve.barvni_ptr[*BARVA][2]) ? mozne_barve.barvni_ptr[*BARVA][2]: tr_m ;


		writeTRAK();
		delayFREERTOS(5);
	}
}

void svetlost_mod_funct(char smer, uint8_t cas_krog)
{
	while (smer > 0 ? tr_bright < 255 : tr_bright > 0)
	{
		tr_bright += 8 * smer;
		tr_bright = tr_bright < 0 ? 0 : tr_bright;
		tr_bright = tr_bright > 255 ? 255 : tr_bright;
		writeTRAK();
		delayFREERTOS(cas_krog);
	}
}




void strip_mode_chg(char* ch)
{
	if (strcmp(ch,"off"))
		trenutni_audio_mode = OFF_A;

	else if (trenutni_audio_mode == OFF_A)
		trenutni_audio_mode = NORMAL_FADE;

	else
		trenutni_audio_mode = (trenutni_audio_mode + 1) % LENGTH_2;

	EEPROM.pisi(audiomode_eeprom_addr, trenutni_audio_mode);
	deleteALL_subAUDIO_tasks();
}
