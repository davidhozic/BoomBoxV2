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
    while (1)
    {
        if (readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN))
        {
            if (napajalnik.vrednost() == 0)
            {
                if (LCD_timer.vrednost() >= 9000 || !readBIT(Hardware.status_reg, HARDWARE_STATUS_REG_POWERED_UP))
                {
                    LCD_timer.ponastavi();
					writeOUTPUT(BAT_LCD_pin,BAT_LCD_port, 0);
                }
                else if (LCD_timer.vrednost() > 6000)
                { //Prižig vsakih 6s za 3s, če zunanje napajanje ni priključeno
                    writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 1);
                }
            }

            else if (readBIT(Hardware.status_reg,HARDWARE_STATUS_REG_CHARGING_EN))
            {                     //Če je zunanje napajanje priključeno in baterije niso napolnjene, zaslon utripa
                delayFREERTOS(500); //1Hz utripanje
                toggleOUTPUT(BAT_LCD_pin, BAT_LCD_port);
                LCD_timer.ponastavi();
            }
            else // Ce je napajalnik vkljucen in se ne polni, potem 3s gori
            {
                writeOUTPUT(BAT_LCD_pin,BAT_LCD_port, 1);
                delayFREERTOS(3000);
                writeBIT(Hardware.status_reg, HARDWARE_STATUS_REG_CAPACITY_DISPLAY_EN, 0);
            }
        }
        else
        {
            writeOUTPUT(BAT_LCD_pin,BAT_LCD_port, 0);
            holdTASK(&handle_capacity_display); //Resuma se v eventih
        }
        delayFREERTOS(15);
		//END WHILE
    }
}