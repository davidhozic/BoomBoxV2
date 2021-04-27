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
	while(1)
	{
		
		if (Hardware.error_reg)
		{
			delayFREERTOS(500);
			
		}
		else
		{		
			if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_EXTERNAL_POWER))
			{
				if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CHARGING_EN))		/*	Charging is enabled -> flash lcd  */
				{
					toggleOUTPUT(BAT_LCD_pin, BAT_LCD_port);
					delayFREERTOS(2000);												
				}
				else if (napajalnik.risingEdge())		/*	Rising edge resets only after the function call or input going to 0  */
				{				
					writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 1);		
					delayFREERTOS(2000);
					writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
					delayFREERTOS(6000);
				}
			}
			else
			{
				if ( readBIT(Hardware.status_reg,HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN) )
				{
					writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
					delayFREERTOS(5000);
					writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 1);
					delayFREERTOS(1000);
				}
			
				else
				{
					writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
				}
			}
		}
		/***********************************************************************************************************************/
		
		delayFREERTOS(100);
		// END WHILE	
	}
	// END TASK
}