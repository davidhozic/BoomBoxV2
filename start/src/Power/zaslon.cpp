
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\Arduino.h"
#include <Arduino_FreeRTOS.h>
#include "Vhod.h"
#include "castimer.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\PolnenjeZvoc\code\start\src\header\namespaces.h"

VHOD BAT_PRIKAZ_SW(4, 'B', 1);
extern VHOD napajalnik;
void zaslon(void *paramOdTaska)
{
    vTaskDelay(200);
    while (1)
    {
        if (Hardware::display_enabled)
        {
            if (napajalnik.vrednost() == 0)
            {
                if (Hardware::is_Powered_UP && TIMERS_folder::BATCHARGE_T.vrednost() > 6000)
                { //Prižig vsakih 6s za 3s, če zunanje napajanje ni priključeno
                    PORTB |= (1 << 2);
                }

                if (TIMERS_folder::BATCHARGE_T.vrednost() > 9000)
                {
                    TIMERS_folder::BATCHARGE_T.ponastavi();
                    PORTB &= ~(1 << 2);
                }
            }

            else if (Hardware::polnjenjeON)
            {                                         //Če je zunanje napajanje priključeno in baterije niso napolnjene, zaslon utripa
                vTaskDelay(500 / portTICK_PERIOD_MS); //1Hz utripanje
                PORTB = PORTB ^ 0b00000100;
            }
            else // Ce je napajalnik izkljucen in se ne polni, potem 3s gori
            {    /* Rising edge resets only after the risingEdge function is called which means it will still be on for 3s after charging finishes */
                PORTB |= (1 << 2);
                vTaskDelay(3000 / portTICK_PERIOD_MS);
                PORTB &= ~(1 << 2);
                Hardware::display_enabled = false;
            }
        }
        else
        {
            TIMERS_folder::BATCHARGE_T.ponastavi();
            PORTB &= ~(1 << 2);
        }
        /************************************************************************************************************/
    }
}