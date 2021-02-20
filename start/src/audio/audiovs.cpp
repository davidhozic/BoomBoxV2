#include <Arduino_FreeRTOS.h>
#include "castimer.h"
#include "Vhod.h"
#include "Arduino.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\PolnenjeZvoc\code\start\src\header\namespaces.h"

/*************************** KONSTANTE ************************/
const int mic_pin = A0;
int mic_mode = 0;     
const int lucke1 = 9;  //Rdeča lučka je na pinu 9
const int lucke2 = 3;  //Zelena lučka je na pinu 3
const int lucke3 = 11; //Modra lučka je na pinu 1
/*************************************************************/

enum audio_mode
{
    FIX_VOLUME = 0,
    AVG_VOLUME = 1,
    FREK_VOLUME = 2,
    OFF = 3
};

int merjenje_frekvence(int mic_pin);
int povprecna_glasnost(int mic_pin);

void audio_visual(void *paramOdTaska) //Funkcija avdio-vizualnega sistema
{

    int mikrofon = 0;
    int povprecna_gl = 0;
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
        case FIX_VOLUME:

            if (analogRead(mic_pin) > 600) //Najbolj preprost nacin za detekcijo mikrofona
            {
                mikrofon = 1;
            }
            else
            {
                mikrofon = 0;
            }
            break;

        case AVG_VOLUME:
            int tmp = povprecna_glasnost(mic_pin);
            if (tmp != -1)
            {
                povprecna_gl = tmp;
            }

            int trenutna_vrednost = analogRead(mic_pin);
            if ((trenutna_vrednost - povprecna_gl) > 300 && povprecna_gl != 0)
            {
                mikrofon = 1;
            }
            else
            {
                mikrofon = 0;
            }
            break;

        case FREK_VOLUME:
            int frek = merjenje_frekvence(mic_pin);
            static int frekvenca = 0;
            if (frek != -1)
                frekvenca = frek;

            if (frekvenca <= 60 && frekvenca > 0)
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
            static int barva_selekt = random(0,7);
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

void audio_mode_change(char *ch) // Switches audio mode
{
    int ct = 0, delay_switch = 200;
    if (ch == "off")
    {
        mic_mode = OFF;
    }
    else
    {
        mic_mode = (mic_mode + 1) % OFF;
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
