

#include "castimer/castimer.h"
#include "VHOD/Vhod.h"
#include <FreeRTOS.h>
#include "audio/includes/audio.h"
#include "libs/EEPROM/EEPROM.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/global.h"
#include "FreeRTOS_def_decl.h"



/* ************************** Extenal ************************************ */
void Shutdown();
void Power_UP();
void audio_visual();
void spanje();
/* *********************************************************************** */


void power(void *paramOdTaska)
{
/************************************************************************/
/*                          LOCAL TASK VARIABLES                        */
/************************************************************************/
	castimer temperature_read_timer;	
	castimer VOLT_timer;
	castimer stikaloOFFtime;
	VHOD stikalo(1, 'K', 0);
	while (true)
	{ 
		
		/************************************************************************/
		/*							TEMPERATURE READING                         */
		/************************************************************************/		
		if (temperature_read_timer.vrednost() > 4000){
			taskENTER_CRITICAL();
			float TempReadV = (float)readANALOG(Temp_sensor_pin) * Hardware.REF_mVOLT / 1023.00f;
			Hardware.Hardware_temp = (float)(-0.073f) * (float)TempReadV + 192.754f;
			taskEXIT_CRITICAL();
			temperature_read_timer.ponastavi();
		}
		
		if (Hardware.Hardware_temp > 60)
		{
			Hardware.AMP_oheat = true;
			Shutdown();
		}
		
			
		/************************************************************************/
		/*							VOLTAGE READING                             */
		/************************************************************************/

		if (VOLT_timer.vrednost() > 500)
		{
			xSemaphoreTake(voltage_SEM, portMAX_DELAY);		// Prevent other task from reading while write operation is in effect
			Hardware.napetost = readANALOG(vDIV_pin) * 5000.00f/1023.00f;
			VOLT_timer.ponastavi();
		}
		
		xSemaphoreGive(voltage_SEM); // Da zeleno luc ostalim taskom
		

		/************************************************************************/
		/*								POWER UP/SHUTDOWN                       */
		/************************************************************************/
		if (stikaloCAS.vrednost() >= 2000 && !Hardware.AMP_oheat && (Hardware.napetost > sleep_voltage + 50 || Hardware.PSW) && !Hardware.is_Powered_UP)
		{ // Elapsed 2000 ms, not overheated, enough power or (already switched to)external power and not already powered up
			Power_UP();
		}
		
		if (stikalo.vrednost() == 0 && stikaloOFFtime.vrednost() > 30)
		{		
		
			if (Hardware.is_Powered_UP)
				Shutdown();
			stikaloCAS.ponastavi();
			Hardware.AMP_oheat = false;
		
		}
		else if (stikalo.vrednost() == 1)
			stikaloOFFtime.ponastavi();
			
		if (Hardware.napetost <= sleep_voltage && !napajalnik.vrednost() && Hardware.napetost > 0) //Če je napetost 0V, to pomeni da baterij še ni prebral ; V spanje gre pri 8% napolnjenosti
		{
			spanje();
		}
		delayFREERTOS(5);
	}
}

void Shutdown()
{
	writeOUTPUT(_12V_line_pin, _12V_line_port, 0); // izklopi izhod
	writeOUTPUT(main_mosfet_pin, main_mosfet_port , 0);
	Hardware.is_Powered_UP = false;
	trenutni_audio_mode = OFF_A;
}

void Power_UP()
{
	trenutni_audio_mode = EEPROM.beri(audiomode_eeprom_addr);
	writeOUTPUT(_12V_line_pin, _12V_line_port, 1); // izklopi izhod
	writeOUTPUT(main_mosfet_pin, main_mosfet_port, 1);
	Hardware.is_Powered_UP = true;
}
