

#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\Arduino.h"
#include "castimer.h"
#include "Vhod.h"
#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\header\stuff.h"

/* ************************** GLOBAL ************************************ */
VHOD napajalnik(2, 'D', 0);
VHOD stikalo(4, 'D', 0);
void spanje();
void Shutdown();
void Power_UP();
extern TaskHandle_t temp;
extern TaskHandle_t event_handle;
extern const short REF_napetost = 5000; //Referenčna napetost na ADC (4.999V - 5.002V)
extern int A_mode;
extern TaskHandle_t core_handle;
castimer nap_read;

void core(void *paramOdTaska)
{

    while (true)
    {
        if (stikalo.vrednost() == 0 && stikaloOFFtime.vrednost() > 30)
        {
            if (is_Powered_UP)
            {
                Shutdown();
            }
            stikaloCAS.ponastavi();
            if (stikalo.fallingEdge())
            {
                AMP_oheat = false;
            }
        }
        else if (stikalo.vrednost() == 1)
        {
            stikaloOFFtime.ponastavi();
        }

        if (nap_read.vrednost() > 500)
        {
            taskENTER_CRITICAL();
            napetost = (float)analogRead(A2) * (float)5.0 / 1023.00;
            nap_read.ponastavi();
            taskEXIT_CRITICAL();
        }
        //----------------------------------------------------------------------------------------------------------------------------------
        //                                               Power UP
        //----------------------------------------------------------------------------------------------------------------------------------
        if (stikaloCAS.vrednost() >= 2000 && !AMP_oheat && (napetost >= 3.10 || napajalnik.vrednost()) && is_Powered_UP == false && (!napajalnik.vrednost() || PSW))
        { /* Elapsed 2000 ms, not overheated, enough power or external power, not already powered up, if external power is set, wait till power is switched */
            vTaskSuspend(event_handle);
            Power_UP();
            vTaskResume(event_handle);
        }

        if (napetost <= 3050 && napajalnik.vrednost() == 0 && napetost != 0) //Če je napetost 0V, to pomeni da baterij še ni prebral ; V spanje gre pri 8% napolnjenosti
        {
            spanje();
        }
    }
}

void Shutdown()
{

    PORTB &= ~1;
    PORTD &= ~1;           //Izklop
    digitalWrite(9, LOW);  //R lucka
    digitalWrite(5, LOW);  //G lucka
    digitalWrite(11, LOW); //B lucka
    is_Powered_UP = false;
    A_mode = OFF;
}

void Power_UP()
{
    delay(20 / portTICK_PERIOD_MS);
    PORTB |= 1;
    is_Powered_UP = true;
    PORTD |= 1;
    delay(210 / portTICK_PERIOD_MS);
    A_mode = NORMAL_FADE;
}
