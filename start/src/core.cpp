

#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\Arduino.h"
#include "castimer.h"
#include "Vhod.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\header\Errors.h"
#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\header\namespaces.h"
#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"

/* ************************** GLOBAL ************************************ */
VHOD napajalnik(2, 'D', 0);
VHOD stikalo(4, 'D', 0);
void spanje();
void Shutdown();
void Power_UP();
extern TaskHandle_t event_handle;
extern const int REF_napetost = 5000; //Referenčna Hardware::napetost na ADC (4.999V - 5.002V)
extern int A_mode;

enum audio_mode
{
    Direct_signal, //Signal iz mikrofona -> lucke
    NORMAL_FADE,
    color_fade_create,
    LENGTH_2,
    OFF,
};

void core(void *paramOdTaska)
{
    while (true)
    {
        Hardware::napetost = 4000;
        if (stikalo.vrednost() == 0)
        {
            if (Hardware::is_Powered_UP)
            {
                Shutdown();
            }
            TIMERS_folder::stikaloCAS.ponastavi();
            if (stikalo.fallingEdge())
            {
                Hardware::Napaka.AMP_oheat = false;
            }
        }

        //----------------------------------------------------------------------------------------------------------------------------------
        //                                               Power UP
        //----------------------------------------------------------------------------------------------------------------------------------

        if (TIMERS_folder::stikaloCAS.vrednost() >= 2000 && Hardware::Napaka.AMP_oheat == false && (Hardware::napetost >= 3100 || napajalnik.vrednost()) && Hardware::is_Powered_UP == false && (!napajalnik.vrednost() || Hardware::PSW))
        { /* Elapsed 2000 ms, not overheated, enough power or external power, not already powered up, if external power is set, wait till power is switched */
            Serial.println("ON");
            vTaskSuspend(event_handle);
            Power_UP();
            vTaskResume(event_handle);
        }

        if (Hardware::napetost <= 3050 && napajalnik.vrednost() == 0 && Hardware::napetost != 0) //Če je Hardware::napetost 0V, to pomeni da baterij še ni prebral ; V spanje gre pri 8% napolnjenosti
        {
            spanje();
        }
    }
}
void readVoltage(void *paramOdTaska)
{
    int vFirstRead = 0;
    while (true)
    {
        if (Hardware::napetost != 0) /*Delay only after voltage has been read for the first time*/
        {
            vTaskDelay(3000 / portTICK_PERIOD_MS);
        }

        vFirstRead = analogRead(A2) * REF_napetost / 1023.00;
        vTaskDelay(300 / portTICK_PERIOD_MS);
        int vSecRead = analogRead(A2) * REF_napetost / 1023.00f;
        if (abs(vSecRead - vFirstRead) <= 50)
        {
            Hardware::napetost = vSecRead;
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
    Hardware::is_Powered_UP = false;
    A_mode = OFF;
}

void Power_UP()
{
    vTaskDelay(20 / portTICK_PERIOD_MS);
    PORTB |= 1;
    Hardware::is_Powered_UP = true;
    PORTD |= 1;
    vTaskDelay(210 / portTICK_PERIOD_MS);
    A_mode = NORMAL_FADE;
}
