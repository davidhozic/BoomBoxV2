

#include "castimer/castimer.h"
#include "VHOD/Vhod.h"
#include <FreeRTOS.h>
#include "includes/includes.h"
#include "audio/includes/audio.h"
VHOD napajalnik(2, 'D', 0);
VHOD stikalo(4, 'D', 0);
/* ************************** Extenal ************************************ */
void Shutdown();
void Power_UP();
void events(void *paramOdTaska);
void audio_visual();
/* *********************************************************************** */

void core(void *paramOdTaska)
{
	while (true)
	{
		if (stikalo.vrednost() == 0 && Timers.stikaloOFFtime.vrednost() > 30)	{
			if (Hardware.is_Powered_UP)
			{
				Shutdown();
			}
			Timers.stikaloCAS.ponastavi();
			if (stikalo.fallingEdge())
			{
				Hardware.AMP_oheat = false;
			}
		}
		else if (stikalo.vrednost() == 1)
		{
			Timers.stikaloOFFtime.ponastavi();
		}
		/***********************************************************************************************
		*                                                                                             *
		*                                                                                             *
		*                                      BRANJE VOLT                                            *
		*                                                                                             *
		*                                                                                             *
		***********************************************************************************************/

		if (xSemaphoreTake(voltage_SEM, portMAX_DELAY) == pdTRUE) // Vzame dostop do napetostnega semaforja -> ostali taski ne morajo brati napetosti
		{

			if (Timers.VOLT_timer.vrednost() > 500)
			{
				Timers.VOLT_timer.ponastavi();
				Hardware.napetost = readANALOG(vDIV_pin) * (float)Hardware.REF_mVOLT / 1023.00f;
			}

			xSemaphoreGive(voltage_SEM); // Da zeleno luc ostalim taskom
		}

		//----------------------------------------------------------------------------------------------------------------------------------
		//                                               Power UP
		//----------------------------------------------------------------------------------------------------------------------------------
		if (Timers.stikaloCAS.vrednost() >= 2000 && !Hardware.AMP_oheat && (Hardware.napetost > sleep_voltage + 50 || Hardware.PSW) && !Hardware.is_Powered_UP)
		{ // Elapsed 2000 ms, not overheated, enough power or (already switched to)external power and not already powered up
			Power_UP();
		}
		if (Hardware.napetost <= sleep_voltage && napajalnik.vrednost() == 0 && Hardware.napetost != 0 && Hardware.is_Powered_UP) //Če je napetost 0V, to pomeni da baterij še ni prebral ; V spanje gre pri 8% napolnjenosti
		{
			Shutdown();
		}
		delay_FRTOS(200);
	}
}

void Shutdown()
{
	writeOUTPUT(_12V_line, 'B', 0); // izklopi izhod
	writeOUTPUT(main_mosfet_pin, 'H', 0);
	Hardware.is_Powered_UP = false;
	trenutni_audio_mode = OFF_A;
}

void Power_UP()
{
	// trenutni_audio_mode = EEPROM.read(audiomode_eeprom_addr);
	writeOUTPUT(_12V_line, 'B', 1); // izklopi izhod
	writeOUTPUT(main_mosfet_pin, 'H', 1);
	Hardware.is_Powered_UP = true;
}
