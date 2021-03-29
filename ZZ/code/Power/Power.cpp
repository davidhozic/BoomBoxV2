

#include "castimer/castimer.h"
#include "VHOD/Vhod.h"
#include <FreeRTOS.h>
#include "audio-visual/includes/audio.h"
#include "libs/EEPROM/EEPROM.h"
#include "libs/outputs_inputs/outputs_inputs.h"
#include "common/inc/global.h"
#include "FreeRTOS_def_decl.h"
#include <avr/sleep.h>
#include <util/delay.h>


/************************************************************************/
/*							PROTOTYPES                                  */
/************************************************************************/
void Shutdown();
void Power_UP();
void audio_visual();
void spanje();
void external_power_switch_ev();
void internal_power_switch_ev();
/************************************************************************/


	
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
			Hardware.battery_voltage = readANALOG(vDIV_pin) * adc_milivolt_ref/1023.00f;
			VOLT_timer.ponastavi();
		}
			

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
			Shutdown();
			spanje();
		}
		
		/************************************************************************/
		/*							 POWER SWITCH	                            */
		/************************************************************************/
		
		if (napajalnik.vrednost() && readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_EXTERNAL_POWER) == false)
		{
			external_power_switch_ev();
		}

		else if (napajalnik.vrednost() == 0 && readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_EXTERNAL_POWER))
		{
			internal_power_switch_ev();
		}

		/*************************************************************************************/
		
		
		delayFREERTOS(15);
	}
}

void Shutdown()
{
	writeOUTPUT(_12V_line_pin, _12V_line_port, 0); // izklopi izhod
	writeOUTPUT(main_mosfet_pin, main_mosfet_port , 0);
	writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_POWERED_UP, 0);
	STRIP_MODE = enum_STRIP_MODES::STRIP_OFF;
}

void Power_UP()
{
	STRIP_MODE = EEPROM.beri(audiomode_eeprom_addr);
	writeOUTPUT(_12V_line_pin, _12V_line_port, 1); // izklopi izhod
	writeOUTPUT(main_mosfet_pin, main_mosfet_port, 1);
	writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_POWERED_UP, 1);
}



void external_power_switch_ev()
{
	Shutdown();
	delayFREERTOS(20);
	writeOUTPUT(menjalnik_pin,menjalnik_port,1);
	stikaloCAS.ponastavi();
	writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_EXTERNAL_POWER, 1);
}

void internal_power_switch_ev()
{

	Shutdown();
	delayFREERTOS(20);
	writeOUTPUT(menjalnik_pin,menjalnik_port, 0);
	stikaloCAS.ponastavi();
	delayFREERTOS(20);
	writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_EXTERNAL_POWER, 0);
}

void bujenje()
{ //Avtomaticno rising edge, saj se interrupt vklopi sele ob spanju
	sleep_disable();
	PCICR = 0;
	PCIFR = 0;
	PCMSK2 = 0;
	_delay_ms(200);
}

void spanje()
{
	asm("sei");					 //vklop zunanjih interruptov
	PCICR = (1 << PCIE2);	 // Vklopi PCINT interrupt
	PCIFR = (1 << PCIF2);	 // Vlopi ISR
	PCMSK2 = (1 << PCINT17); //Vklopi vector na 17
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_cpu();
}