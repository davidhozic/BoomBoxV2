
#ifndef GLOBAL_STRUCTS_HEADER
#define GLOBAL_STRUCTS_HEADER
#include "settings.h"
#include "castimer/castimer.h"
#include "VHOD/Vhod.h"

/****************************************************************************************************************************/
/*														 GLOBAL MACROS										                */
/****************************************************************************************************************************/
enum enum_HARDWARE_STATUS_REGISTER
{
	/************************************************************************/
	/*				  HARDWARE STRUCT STATUS REGISTER BITS                  */
	/************************************************************************/
	
		HARDWARE_STATUS_REG_POWERED_UP = 0,
		HARDWARE_STATUS_REG_EXTERNAL_POWER,
		HARDWARE_STATUS_REG_CHARGING_FINISHED,
		HARDWARE_STATUS_REG_CHARGING_EN,
		HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN
};								

/************************************************************************/
/*							 MACRO FUCTIONS                             */
/************************************************************************/

// BIT MANIPULATION
#define readBIT(reg, bit)				(		((reg >> bit) & 1) 												) 
#define writeBIT(reg, bit, val)			(		(reg = val ? ( reg | (1 << bit) ) : ( reg  &  ~(1 << bit)))		)	


/****************************************************************************************************************************/
/*													GLOBAL OBJECTS													        */
/****************************************************************************************************************************/

extern class_VHOD napajalnik;

/****************************************************************************************************************************/
/*													GLOBAL STRUCTS															*/
/****************************************************************************************************************************/
struct struct_HARDWARE
{		
	unsigned char status_reg = 0;
	unsigned short battery_voltage = 0;
};

extern struct_HARDWARE Hardware;


#endif