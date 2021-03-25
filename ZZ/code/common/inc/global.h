
#ifndef GLOBAL_STRUCTS_HEADER
#define GLOBAL_STRUCTS_HEADER
#include "settings.h"
#include "castimer/castimer.h"
#include "VHOD/Vhod.h"


/************************************************************************************************/
/*                          GLOBAL MACROS										                */
/************************************************************************************************/
enum Status_reg_hardware_enum
{
	STATUS_REG_POWERED_UP,	
	STATUS_REG_EXTERNAL_POWER,
	STATUS_REG_CHARGING_FINISHED,
	STATUS_REG_CHARGING_EN,
	STATUS_REG_CAPACITY_DISPLAY_EN
};								



#define readBIT(reg, bit)				( ( reg  &  (1 << bit) ) >  0 )
#define writeBIT(reg, bit, val)			( val ? reg |=(1 << bit) : reg &= ~(1 << bit) )	

/************************************************************************************************/
/*							GLOBAL OBJECTS												        */
/************************************************************************************************/
extern VHOD napajalnik;
extern castimer stikaloCAS;

/************************************************************************************************/
/*							GLOBAL STRUCTS														*/
/************************************************************************************************/
struct Hardware_t
{		
	const short adc_milivolt_ref = 5000;	 //Referencna battery_voltage na ADC (4.999V - 5.002V)
	const float adc_volt_ref = 5.00;
	unsigned char status_reg = 0;
	float battery_voltage = 0;
	unsigned long sys_time = 0;
};

extern Hardware_t Hardware;


#endif