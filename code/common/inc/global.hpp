
#ifndef GLOBAL_H
#define GLOBAL_H
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "input.hpp"

/****************************************************************************************************************************/
/*													GLOBAL STRUCTS															*/
/****************************************************************************************************************************/

struct HARDWARE
{		
	struct bitfield_status_reg
	{
		uint8_t powered_up			: 1;	/* Is 1 when speaker is turned on */
		uint8_t external_power		: 1;	/* This bit is set when power switcher switched to PSU instead of battery */
		uint8_t charging_finished	: 1;	/* This bit is set when charging reaches maximum voltage level set in settings.hpp */
		uint8_t charging_enabled	: 1;	/* This bit is set when speaker is charging */
	}status_reg = {0};
		
	
	unsigned short battery_voltage = 0;
};

/************************************************************************/
/*							COMMON STRUCTS/CLASS	                    */
/************************************************************************/
extern HARDWARE m_Hardware;
extern INPUT_t m_napajalnik;

/************************************************************************/
/*							GLOBAL DEFINITIONS                          */
/************************************************************************/
#define BATTERY_VOLTAGE_PERCENT		(100.00 * ((double)m_Hardware.battery_voltage-min_battery_voltage)/(max_battery_voltage-min_battery_voltage) )



/************************************************************************/
/*					  Safe FreeRTOS functions/macros		            */
/************************************************************************/
#ifndef DEBUG
	#define delay_FreeRTOS_ms(x)	vTaskDelay(x/portTICK_PERIOD_MS)
#else
	#define delay_FreeRTOS_ms(x)	vTaskDelay(1)
#endif

void holdTASK	(TaskHandle_t* task);
void resumeTASK	(TaskHandle_t* task);
void deleteTASK	(TaskHandle_t* task);





#endif