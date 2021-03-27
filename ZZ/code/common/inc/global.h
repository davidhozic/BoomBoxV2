
#ifndef GLOBAL_STRUCTS_HEADER
#define GLOBAL_STRUCTS_HEADER
#include "settings.h"
#include "castimer/castimer.h"
#include "VHOD/Vhod.h"

/****************************************************************************************************************************/
/*														 GLOBAL MACROS										                */
/****************************************************************************************************************************/
enum enum_STATUS_REGISTERS
{
	/************************************************************************/
	/*				  HARDWARE STRUCT STATUS REGISTER BITS                  */
	/************************************************************************/
	
		HARDWARE_STATUS_REG_POWERED_UP = 0,
		HARDWARE_STATUS_REG_EXTERNAL_POWER,
		HARDWARE_STATUS_REG_CHARGING_FINISHED,
		HARDWARE_STATUS_REG_CHARGING_EN,
		HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
};								


#define readBIT(reg, bit)				( ( reg  &  (1 << bit) ) >  0 )
#define writeBIT(reg, bit, val)			( val ? reg |=(1 << bit) : reg &= ~(1 << bit) )	

/****************************************************************************************************************************/
/*													GLOBAL OBJECTS													        */
/****************************************************************************************************************************/
extern class_VHOD napajalnik;
extern class_TIMER stikaloCAS;

/****************************************************************************************************************************/
/*													GLOBAL STRUCTS															*/
/****************************************************************************************************************************/
struct struct_HARDWARE
{		
	const short adc_milivolt_ref = 5000;	 //Referencna battery_voltage na ADC (4.999V - 5.002V)
	const float adc_volt_ref = 5.00;
	unsigned char status_reg = 0;
	float battery_voltage = 0;
};

extern struct_HARDWARE Hardware;


#endif