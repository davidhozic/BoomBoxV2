
#ifndef GLOBAL_H
#define GLOBAL_H
#include "settings.h"
#include "FreeRTOSConfig.h" 
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_def_decl.h"
#include "castimer/castimer.h"
#include "VHOD/Vhod.h"


// BIT MANIPULATION
#define readBIT(reg, bit)				(		(	(reg >> bit) & 0x1	)												)
#define writeBIT(reg, bit, val)			(		(reg = val ? ( reg | (0x1 << bit) ) : ( reg  &  ~(0x1 << bit)))			)


/****************************************************************************************************************************/
/*													GLOBAL OBJECTS													        */
/****************************************************************************************************************************/

extern class_VHOD napajalnik;

/****************************************************************************************************************************/
/*													GLOBAL STRUCTS															*/
/****************************************************************************************************************************/

struct struct_HARDWARE
{		
	struct bitfield_status_reg
	{
		uint8_t powered_up			: 1;
		uint8_t external_power		: 1;
		uint8_t charging_finished	: 1;
		uint8_t charging_enabled	: 1;
		uint8_t capacity_lcd_en		: 1;
	}status_reg = {0};

	unsigned short battery_voltage = 0;
	Vozlisce <class_TIMER*> timer_list;
};

extern struct_HARDWARE Hardware;



#endif