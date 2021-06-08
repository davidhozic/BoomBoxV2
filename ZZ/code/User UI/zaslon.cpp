#include "FreeRTOS.h"
#include "task.h"
#include "input/input.hh"
#include "castimer/castimer.hh"
#include <avr/io.h>
#include "common/inc/global.hh"

#include "libs/outputs_inputs/outputs_inputs.hh"


void zaslon(void *paramOdTaska)
{
	/*  Timers  */
	class_TIMER LCD_timer;
	
	while(1)
	{
		if (m_Hardware.status_reg.charging_enabled)
		{
			if ( LCD_timer.value() >= 1000)
			{
				toggleOUTPUT(BAT_LCD_pin, BAT_LCD_port);
				LCD_timer.reset();
			}
		}

		else if (m_Hardware.status_reg.capacity_lcd_en && m_Hardware.status_reg.powered_up)
		{

			if (LCD_timer.value() < 5000)
			{
				writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
			}
			else if(LCD_timer.value() <= 8000)
			{
				writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 1);
			}
			else
			{
				LCD_timer.reset();
			}
		}
		
		else
		{
			writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 0);
		}

		delayFREERTOS(100);
		// END WHILE	
	}
	// END TASK
}