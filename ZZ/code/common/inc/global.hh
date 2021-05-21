
#ifndef GLOBAL_H
#define GLOBAL_H
#include "settings.hh"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Vhod.hh"





/****************************************************************************************************************************/
/*													GLOBAL STRUCTS															*/
/****************************************************************************************************************************/

struct struct_m_Hardware
{		
	struct bitfield_status_reg
	{
		uint8_t powered_up			: 1;	/* Is 1 when speaker is turned on */
		uint8_t external_power		: 1;	/* This bit is set when power switcher switched to PSU instead of battery */
		uint8_t charging_finished	: 1;	/* This bit is set when charging reaches maximum voltage level set in settings.hh */
		uint8_t charging_enabled	: 1;	/* This bit is set when speaker is charging */
		uint8_t capacity_lcd_en		: 1;	/* This bit is set if it was set in settings ui */
		uint8_t sleeping			: 1;	/* This bit is set when the speaker is sleeping */
	}status_reg = {0};

	unsigned short battery_voltage = 0;
};

/************************************************************************/
/*							COMMIN STRUCTS/CLASS	                    */
/************************************************************************/
extern struct_m_Hardware m_Hardware;
extern class_VHOD m_napajalnik;

/************************************************************************/
/*							GLOBAL DEFINITIONS                          */
/************************************************************************/
#define BATTERY_VOLTAGE_PERCENT		(100 * ((double)m_Hardware.battery_voltage-min_battery_voltage)/(max_battery_voltage-min_battery_voltage) )




/************************************************************************/
/*					  Safe FreeRTOS functions/macros		            */
/************************************************************************/

#ifndef DEBUG
	#define delayFREERTOS(x)	vTaskDelay((double)x/portTICK_PERIOD_MS);
#else
	#include <math.h>
	#define delayFREERTOS(x)	vTaskDelay(			2		)
#endif	

void holdTASK(TaskHandle_t* task);
void resumeTASK(TaskHandle_t* task);
void deleteTASK(TaskHandle_t* task);





#endif