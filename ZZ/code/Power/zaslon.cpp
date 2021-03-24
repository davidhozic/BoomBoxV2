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
	/************************************************************************/
	/*                          LOCAL TASK VARIABLES                        */
	/************************************************************************/
	castimer LCD_timer;	

    while (1)
    {
        if (Hardware.display_enabled)
        {
            if (napajalnik.vrednost() == 0)
            {
                if (LCD_timer.vrednost() >= 9000 || !Hardware.is_Powered_UP)
                {
                    LCD_timer.ponastavi();
					writeOUTPUT(BAT_LCD_pin,BAT_LCD_port, 0);
                }
                else if (LCD_timer.vrednost() > 6000)
                { //Prižig vsakih 6s za 3s, če zunanje napajanje ni priključeno
                    writeOUTPUT(BAT_LCD_pin, BAT_LCD_port, 1);
                }
            }

            else if (Hardware.polnjenjeON)
            {                     //Če je zunanje napajanje priključeno in baterije niso napolnjene, zaslon utripa
                delayFREERTOS(500); //1Hz utripanje
                toggleOUTPUT(BAT_LCD_pin, BAT_LCD_port);
                LCD_timer.ponastavi();
            }
            else // Ce je napajalnik izkljucen in se ne polni, potem 3s gori
            {
                writeOUTPUT(BAT_LCD_pin,BAT_LCD_port, 1);
                delayFREERTOS(3000);
                Hardware.display_enabled = false;
            }
        }
        else
        {
            PORTB &= ~(1 << BAT_LCD_pin);
            holdTASK(zaslon_control); //Resuma se v eventih
        }
        delayFREERTOS(200);
    }
}