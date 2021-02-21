#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "castimer.h"
#include "Vhod.h"
#include "Arduino.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\PolnenjeZvoc\code\start\src\header\namespaces.h"


/*************************** KONSTANTE ************************/
extern const int mic_pin = A0;
int mic_mode = 0;
const int lucke1 = 9;  //Rdeča lučka je na pinu 9
const int lucke2 = 3;  //Zelena lučka je na pinu 3
const int lucke3 = 11; //Modra lučka je na pinu 11
extern TaskHandle_t avg_VL;
extern TaskHandle_t frekVL;
/*************************************************************/

enum audio_mode
{
    AVG_VOLUME = 0,
    FREK_VOLUME = 1,
    OFF = 2
};


void audio_visual(void *paramOdTaska) //Funkcija avdio-vizualnega sistema
{

    int mikrofon = 0;
    int MODRA = 0; // spremenljivka, ki hrani vrednost svetlosti modre lučke
    int RDECA = 0;
    int ZELENA = 0;

    while (true)
    {
        switch (mic_mode)
        {

        case OFF:

            mikrofon = 0;
            break;

        case AVG_VOLUME:
            int trenutna_vrednost = analogRead(mic_pin);
            if ((trenutna_vrednost - Hardware::povprecna_glasnost) > 150 && Hardware::povprecna_glasnost != 0)
            {
                mikrofon = 1;
            }
            else
            {
                mikrofon = 0;
            }
            break;

        case FREK_VOLUME:
            if (Hardware::frekvenca > 0 && Hardware ::frekvenca <= 60)
            {
                mikrofon = 1;
            }
            else
            {
                mikrofon = 0;
            }
            break;
        }

        if (RDECA > 0)
        {
            if (TIMERS_folder::timerL1.vrednost() > 10)
            {
                RDECA = RDECA - 5;
                analogWrite(lucke1, RDECA);
                TIMERS_folder::timerL1.ponastavi();
            }
        }

        if (ZELENA > 0)
        {
            if (TIMERS_folder::timerL2.vrednost() > 10)
            {
                ZELENA = ZELENA - 5;
                analogWrite(lucke2, ZELENA);
                TIMERS_folder::timerL2.ponastavi();
            }
        }

        if (MODRA > 0)
        {
            if (TIMERS_folder::timerL3.vrednost() > 10)
            {
                MODRA = MODRA - 5;
                analogWrite(lucke3, MODRA);
                TIMERS_folder::timerL3.ponastavi();
            }
        }

        if (TIMERS_folder::lucke_filter_time.vrednost() > 100 && mikrofon == 1)
        { //Če se je vrednost spremenila
            TIMERS_folder::lucke_filter_time.ponastavi();
            int barva_selekt = random(0, 7);
            //Barve:
            switch (barva_selekt)
            {
            case 0:
                RDECA = 255;
                MODRA = 0;
                ZELENA = 0;
                break;
            case 1:
                RDECA = 0;
                MODRA = 255;
                ZELENA = 0;
                break;
            case 2:
                RDECA = 0;
                MODRA = 0;
                ZELENA = 255;
                break;
            case 3:
                RDECA = 51;
                MODRA = 255;
                ZELENA = 255;
                break;
            case 4:
                RDECA = 255;
                MODRA = 51;
                ZELENA = 255;
                break;
            case 5:
                RDECA = 255;
                MODRA = 255;
                ZELENA = 255;
                break;
            case 6:
                RDECA = 0;
                MODRA = 204;
                ZELENA = 204;
                break;
            case 7:
                RDECA = 255;
                MODRA = 0;
                ZELENA = 128;
            }
            //Prižig lučk na neki barvi
            analogWrite(lucke1, RDECA);
            analogWrite(lucke2, MODRA);
            analogWrite(lucke2, ZELENA);
        }
    }
}

void audio_mode_change(char ch[]) // Switches audio mode
{
    vTaskSuspend(avg_VL);
    vTaskSuspend(frekVL);

    int ct = 0, delay_switch = 200;
    if (ch == "off")
    {
        mic_mode = OFF;
    }
    else
    {
        mic_mode = (mic_mode + 1) % OFF;
    }

    switch (mic_mode) // Resumes meassure tasks
    {
    case FREK_VOLUME:
        vTaskResume(frekVL);
        break;
    case AVG_VOLUME:
        vTaskResume(avg_VL);
        break;
    }

    digitalWrite(lucke1, 0);
    digitalWrite(lucke2, 0);
    digitalWrite(lucke3, 0);
    
    while (ct < (mic_mode)*2) // n blink = n audio_mode
    {
        PORTB ^= (1 << 1);                 //R
        PORTD ^= (1 << 3);                 //G
        PORTB ^= (1 << 3);                 //B
        delay_switch = 900 - delay_switch; // Switches between 200ms and 700ms
        vTaskDelay(delay_switch / portTICK_PERIOD_MS);
        ct++;
    }

    PORTB &= ~(1 << 1); //R
    PORTD &= ~(1 << 3); //G
    PORTB &= ~(1 << 3); //B
    vTaskDelay(500 / portTICK_PERIOD_MS);
}
