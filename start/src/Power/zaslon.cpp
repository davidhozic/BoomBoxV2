
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\Arduino.h"
#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "Vhod.h"
#include "castimer.h"
#include "../includes/includes.h"

const int lcd_pb_pin = 2;
extern VHOD napajalnik;

void zaslon(void *paramOdTaska)
{

    while (1)
    {
        if (Hardware.display_enabled)
        {
            if (napajalnik.vrednost() == 0)
            {

                if (Timers.LCD_timer.vrednost() >= 9000)
                {
                    Timers.LCD_timer.ponastavi();
                    PORTB &= ~(1 << lcd_pb_pin);
                }
                else if (Hardware.is_Powered_UP && Timers.LCD_timer.vrednost() > 6000)
                { //Prižig vsakih 6s za 3s, če zunanje napajanje ni priključeno
                    PORTB |= (1 << lcd_pb_pin);
                }
            }

            else if (Hardware.polnjenjeON)
            {                                         //Če je zunanje napajanje priključeno in baterije niso napolnjene, zaslon utripa
                vTaskDelay(500 / portTICK_PERIOD_MS); //1Hz utripanje
                PORTB = PORTB ^ 0b00000100;
                Timers.LCD_timer.ponastavi();
            }
            else // Ce je napajalnik izkljucen in se ne polni, potem 3s gori
            {
                PORTB |= (1 << lcd_pb_pin);
                vTaskDelay(3000 / portTICK_PERIOD_MS);
                Hardware.display_enabled = false;
            }
        }
        else
        {
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
}