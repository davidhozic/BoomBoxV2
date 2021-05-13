#include "FreeRTOS.h"
#include "task.h"
#include "VHOD/Vhod.h"
#include "castimer/castimer.h"
#include <avr/io.h>
#include "common/inc/global.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "libs/outputs_inputs/outputs_inputs.h"





void zaslon(void *paramOdTaska)
{
	
	while(1)
	{
		if (Hardware.status_reg.external_power)
		{	
			if (Hardware.status_reg.charging_enabled)		/*	Charging is enabled -> flash lcd  */
			{
				toggleOUTPUT(BAT_LCD_pin, BAT_LCD_port);
				Hardware.status_reg.display_edge = true;
				delayFREERTOS(1000);												
			}
			else if (Hardware.status_reg.display_edge)
			{				
				Hardware.status_reg.display_edge = false;
				writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 1);
				delayFREERTOS(6000);
				writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
			}
		}
		
		else
		{
			if ( Hardware.status_reg.capacity_lcd_en )
			{
				writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
				delayFREERTOS(5000);
				writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 1);
				delayFREERTOS(3000);
			}
		
			else
			{
				writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
			}
		}
	
		/***********************************************************************************************************************/
		
		delayFREERTOS(100);
		// END WHILE	
	}
	// END TASK
}