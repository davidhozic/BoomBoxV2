

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
	class_TIMER VOLT_timer;
	class_TIMER stikaloOFFtime;
	class_VHOD stikalo(1, 'K', 0);
	
	while (true)
	{ 
	
		/************************************************************************/
		/*							VOLTAGE READING                             */
		/************************************************************************/

		if (VOLT_timer.vrednost() > 500)
		{
			xSemaphoreTake(voltage_SEM, portMAX_DELAY);		// Prevent other task from reading while write operation is in effect
			Hardware.battery_voltage = readANALOG(vDIV_pin) * 5000.00/1023.00;
			VOLT_timer.ponastavi();
		}
		
		xSemaphoreGive(voltage_SEM); // Da zeleno luc ostalim taskom
		

		/************************************************************************/
		/*								POWER UP/SHUTDOWN                       */
		/************************************************************************/
		if (stikaloCAS.vrednost() >= 2000 && (Hardware.battery_voltage > sleep_voltage + 100 || readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_EXTERNAL_POWER)) && !readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_POWERED_UP))
		{ // Elapsed 2000 ms, not overheated, enough power or (already switched to)external power and not already powered up
			Power_UP();
		}
		
		if (stikalo.vrednost() == 0 && stikaloOFFtime.vrednost() > 30)
		{	
			if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_POWERED_UP))
				Shutdown();
			stikaloCAS.ponastavi();		
		}
		
		else if (stikalo.vrednost() == 1)
			stikaloOFFtime.ponastavi();
			
		if (Hardware.battery_voltage <= sleep_voltage && !napajalnik.vrednost() && Hardware.battery_voltage > 0) //Če je battery_voltage 0V, to pomeni da baterij še ni prebral ; V spanje gre pri 8% napolnjenosti
		{
			spanje();
		}
		delayFREERTOS(10);
	}
}

void Shutdown()
{
	writeOUTPUT(_12V_line_pin, _12V_line_port, 0); // izklopi izhod
	writeOUTPUT(main_mosfet_pin, main_mosfet_port , 0);
	writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_POWERED_UP, 0);
	STRIP_MODE = OFF_A;
}

void Power_UP()
{
	STRIP_MODE = EEPROM.beri(audiomode_eeprom_addr);
	writeOUTPUT(_12V_line_pin, _12V_line_port, 1); // izklopi izhod
	writeOUTPUT(main_mosfet_pin, main_mosfet_port, 1);
	writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_POWERED_UP, 1);
}
