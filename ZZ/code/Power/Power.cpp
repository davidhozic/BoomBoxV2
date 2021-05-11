

#include "common/inc/global.h"
#include "castimer/castimer.h"
#include "VHOD/Vhod.h"

#include "libs/outputs_inputs/outputs_inputs.h"
#include "FreeRTOS_def_decl.h"
#include "audio-visual/includes/audio.h"
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/interrupt.h>
/************************************************************************/
/*							PROTOTYPES                                  */
/************************************************************************/
void Shutdown();
void Power_UP();
void audio_visual();
void spanje();
void power_switch_ev(uint8_t mode);
/************************************************************************/

enum enum_power_switch_modes
{
	INTERNAL = 0,
	EXTERNAL
};
	
void power(void *paramOdTaska)
{
	
	 class_TIMER VOLT_timer(Hardware.timer_list);
	 class_TIMER stikaloOFFtime(Hardware.timer_list);
	 class_VHOD stikalo(main_power_switch_pin, main_power_switch_port, 0);
	 class_TIMER power_up_delay_timer(Hardware.timer_list);
	while (true)
	{ 
		/************************************************************************/
		/*							VOLTAGE READING                             */
		/************************************************************************/
		if (VOLT_timer.vrednost() > 500)
		{
			Hardware.battery_voltage = readANALOG(vDIV_pin) *	(double) adc_milivolt_ref/1023.00;
			VOLT_timer.ponastavi();
		}
		/************************************************************************/
		/*							POWER UP/SHUTDOWN					        */
		/************************************************************************/
		if (!Hardware.status_reg.powered_up && (Hardware.battery_voltage > sleep_voltage + 250 || Hardware.status_reg.external_power) && power_up_delay_timer.vrednost() >= 2000)
		{ // Elapsed 2000 ms, not overheated, enough power or (already switched to)external power and not already powered up
			Power_UP();
			power_up_delay_timer.ponastavi();
		}
		
		
		
		if (stikalo.vrednost() == 0 && stikaloOFFtime.vrednost() > 30)
		{	
			if (Hardware.status_reg.powered_up)
			{
				Shutdown();
			}
			power_up_delay_timer.ponastavi();		
		}
		
		else if (stikalo.vrednost() == 1)
			stikaloOFFtime.ponastavi();
			
		if (Hardware.battery_voltage < sleep_voltage && !napajalnik.vrednost() && Hardware.battery_voltage > 0) //Če je battery_voltage 0V, to pomeni da baterij še ni prebral ; V spanje gre pri 8% napolnjenosti
		{
			Shutdown();
			spanje();
		}
		
		
		/*************************************************************************************/
		/*									 POWER SWITCH						             */
		/*************************************************************************************/
		
		if (napajalnik.vrednost() && !Hardware.status_reg.external_power)
		{
			power_up_delay_timer.ponastavi();	// To make sure it waits 2 seconds before trying to turn on again
			power_switch_ev(EXTERNAL);
		}

		else if (napajalnik.vrednost() == 0 && Hardware.status_reg.external_power)
		{
			power_up_delay_timer.ponastavi();
			power_switch_ev(INTERNAL);
		}

		/*************************************************************************************/
		delayFREERTOS(200);
	}
}

void Shutdown()
{
	audio_system.stripOFF();
	writeOUTPUT(_12V_line_pin, _12V_line_port, 0);					 
	writeOUTPUT(main_mosfet_pin, main_mosfet_port , 0);
	Hardware.status_reg.powered_up = 0;
}

void Power_UP()
{
	writeOUTPUT(_12V_line_pin, _12V_line_port, 1);				
	writeOUTPUT(main_mosfet_pin, main_mosfet_port, 1);
	audio_system.stripON();
	Hardware.status_reg.powered_up = 1;
	delayFREERTOS(200);
}


void power_switch_ev(uint8_t mode)
{
	switch(mode)
	{
		case EXTERNAL:
			Shutdown();
			delayFREERTOS(20);
			writeOUTPUT(menjalnik_pin,menjalnik_port,1);
			Hardware.status_reg.external_power = 1;
			delayFREERTOS(500);
		break;
		
		case INTERNAL:
				Shutdown();
				delayFREERTOS(20);
				writeOUTPUT(menjalnik_pin,menjalnik_port, 0);
				Hardware.status_reg.external_power = 0;
				delayFREERTOS(500);
		break;
	}
}


void bujenje()
{ //Avtomaticno rising edge, saj se interrupt vklopi sele ob spanju
	sleep_disable();
	PCICR = 0;
	PCIFR = 0;
	PCMSK2 = 0;
	_delay_ms(200);
	wdt_enable(watchdog_time);
	WDTCSR |= (1 << WDIE);
	
}

void spanje()
{
	sei();
	wdt_disable();
	PCICR = (1 << PCIE2);	 // Vklopi PCINT interrupt
	PCIFR = (1 << PCIF2);	 // Vlopi ISR
	PCMSK2 = (1 << PCINT17); //Vklopi vector na 17
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sleep_cpu();
}