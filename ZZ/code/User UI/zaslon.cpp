#include "FreeRTOS.h"
#include "task.h"
#include "VHOD/Vhod.h"
#include "castimer/castimer.h"
#include <avr/io.h>
#include "common/inc/global.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "libs/outputs_inputs/outputs_inputs.h"



class_TIMER LCD_timer;	

void zaslon(void *paramOdTaska)
{
	delayFREERTOS(1000);
	while(1)
	{
		switch(napajalnik.vrednost())
		{
			case 1:
				if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN))		/*	Charging is enabled -> flash lcd  */
				{
					toggleOUTPUT(BAT_LCD_pin, BAT_LCD_port);
					delayFREERTOS(500);												
				}
				else if (napajalnik.risingEdge())		/*	Rising edge resets only after the function call or input going to 0  */
				{				
					writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 1);		
					delayFREERTOS(4000);
					writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
				}
			break;
			
			case 0:
				if ( (Hardware.status_reg >> HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN) & 0x1)
				{
					writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
					delayFREERTOS(3000);
					writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 1);
					delayFREERTOS(1000);
				}
				
				else
				{
					writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
				}
			break;	
		}
				
		delayFREERTOS(100);
		// END WHILE	
	}
	// END TASK
}