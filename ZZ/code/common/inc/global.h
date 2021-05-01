
#ifndef GLOBAL_H
#define GLOBAL_H
#include "settings.h"
#include "FreeRTOSConfig.h" 
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOS_def_decl.h"
#include "castimer/castimer.h"
#include "VHOD/Vhod.h"

enum enum_HARDWARE_STATUS_REGISTER
{

	HARDWARE_STATUS_REG_POWERED_UP = 0,
	HARDWARE_STATUS_REG_EXTERNAL_POWER,
	HARDWARE_STATUS_REG_CHARGING_FINISHED,
	HARDWARE_STATUS_REG_CHARGING_EN,
	HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN
};

enum enum_HARDWARE_ERROR_REGISTER
{
	HARDWARE_ERROR_REG_WATCHDOG_FAIL = 0,
	HARDWARE_ERROR_REG_SWITCH_FAIL
};								


// BIT MANIPULATION
#define readBIT(reg, bit)				(		((reg >> bit) & 0x1) 													)
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
	unsigned char  status_reg	   = 0;
	unsigned char  error_reg	   = 0;
	unsigned short battery_voltage = 0;
	Vozlisce <class_TIMER*> timer_list;
};

extern struct_HARDWARE Hardware;



#endif