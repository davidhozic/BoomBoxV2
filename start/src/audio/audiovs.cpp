#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "castimer.h"
#include "Vhod.h"
#include "Arduino.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\header\namespaces.h"

enum mic_detection_mode
{
    Average_volume, // Meri povprecno glasnost
    Frequency_mode, // frekvenca
    LENGHT_1
};

enum audio_mode
{
    Direct_signal, //Signal iz mikrofona -> lucke
    NORMAL_FADE,
    COLOR_FADE,
    LENGTH_2,
    OFF,
};

/*************************** KONSTANTE ************************/
extern const int mic_pin = A0;
int mic_mode = Average_volume;
int A_mode = OFF;
int barva_selekt = 0;
const int rdeca_LED_trak = 9;  //Rdeča lučka je na pinu 9
const int zelena_LED_trak = 3; //Zelena lučka je na pinu 3
const int modra_LED_trak = 11; //Modra lučka je na pinu 11
extern TaskHandle_t avg_VL;
extern TaskHandle_t frekVL;
extern TaskHandle_t audio_system;
TaskHandle_t fade_control;
TaskHandle_t color_fade_control;
float MODRA = 0;
float RDECA = 0;
float ZELENA = 0;
/*************************************************************/

enum barve
{
    WHITE,
    GREEN,
    RED,
    BLUE,
    YELLOW,
    AQUA,
    VIOLET,
    PINK,
    LENGHT
};

struct c
{
    int WHITE[3] = {255, 255, 255};
    int GREEN[3] = {0, 255, 0};
    int RED[3] = {255, 0, 0};
    int BLUE[3] = {0, 0, 255};
    int YELLOW[3] = {255, 255, 0};
    int AQUA[3] = {0, 255, 255};
    int VIOLET[3] = {255, 0, 255};
    int PINK[3] = {255, 20, 147};
    int *barvni_ptr[8] = {&WHITE[3], &GREEN[3], &RED[3], &BLUE[3], &YELLOW[3], &AQUA[3], &VIOLET[3], &PINK[3]};
} barva;

void turnOFFstrip()
{
    vTaskDelete(fade_control);
    digitalWrite(rdeca_LED_trak, 0);
    digitalWrite(zelena_LED_trak, 0);
    digitalWrite(modra_LED_trak, 0);
    RDECA = 0;
    ZELENA = 0;
    MODRA = 0;
}

void fade_task(void *taskPARAM)
{
    int BARVA = *((int *)taskPARAM);
    int svetlost = 255;
    switch (BARVA)
    {
    case WHITE:
        RDECA = barva.WHITE[0];
        ZELENA = barva.WHITE[1];
        MODRA = barva.WHITE[2];
        break;
    case RED:
        RDECA = barva.RED[0];
        ZELENA = barva.RED[1];
        MODRA = barva.RED[2];
        break;
    case GREEN:
        RDECA = barva.GREEN[0];
        ZELENA = barva.GREEN[1];
        MODRA = barva.GREEN[2];
        break;
    case BLUE:
        RDECA = barva.BLUE[0];
        ZELENA = barva.BLUE[1];
        MODRA = barva.BLUE[2];
        break;
    case YELLOW:
        RDECA = barva.YELLOW[0];
        ZELENA = barva.YELLOW[1];
        MODRA = barva.YELLOW[2];
        break;
    case AQUA:
        RDECA = barva.AQUA[0];
        ZELENA = barva.AQUA[1];
        MODRA = barva.AQUA[2];
        break;
    case VIOLET:
        RDECA = barva.VIOLET[0];
        ZELENA = barva.VIOLET[1];
        MODRA = barva.VIOLET[2];
        break;
    case PINK:
        RDECA = barva.PINK[0];
        ZELENA = barva.PINK[1];
        MODRA = barva.PINK[2];
        break;
    }
    //Prižig lučk na neki barvi
    while (true)
    {
        if (svetlost > 0)
        {
            analogWrite(rdeca_LED_trak, RDECA * svetlost / 255);
            analogWrite(zelena_LED_trak, ZELENA * svetlost / 255);
            analogWrite(modra_LED_trak, MODRA * svetlost / 255);
            svetlost -= 5;
            vTaskDelay(20 / portTICK_PERIOD_MS);
        }
        else
        {
            vTaskDelete(fade_control);
        }
    }
}

void fade(int barva)
{
    vTaskDelete(fade_control);
    xTaskCreate(fade_task, "normalni fade", 64, &barva, 1, &fade_control);
}

void Color_Fade_task(void *b)
{ //Fades from one color to another
    int8_t barva_param = *((int8_t *)b);
    int8_t smer_r = 0;
    int8_t smer_g = 0;
    int8_t smer_b = 0;

    while (true)
    {
        smer_r = (barva.barvni_ptr[barva_param][0] - RDECA) / (abs(barva.barvni_ptr[barva_param][0] - RDECA)); //barva.barvni_ptr ima shranjene naslove barvnih tabel
        smer_g = (barva.barvni_ptr[barva_param][1] - ZELENA) / (abs(barva.barvni_ptr[barva_param][1] - ZELENA));
        smer_b = (barva.barvni_ptr[barva_param][2] - MODRA) / (abs(barva.barvni_ptr[barva_param][2] - MODRA));
        RDECA = RDECA + 5.5 * smer_r;
        ZELENA = ZELENA + 5.5 * smer_g;
        MODRA = MODRA + 5.5 * smer_b;
        analogWrite(rdeca_LED_trak, RDECA);
        analogWrite(zelena_LED_trak, ZELENA);
        analogWrite(modra_LED_trak, MODRA);
        vTaskDelay(16 / portTICK_PERIOD_MS);
    }
}

void Color_Fade(int barva)
{
    vTaskDelete(color_fade_control);
    xTaskCreate(Color_Fade_task, "col_fade", 64, &barva, 1, &color_fade_control);
}

void audio_visual(void *paramOdTaska) //Funkcija avdio-vizualnega sistema
{
    int mikrofon = 0;
    while (true)
    {
        switch (mic_mode)
        {
        case Average_volume:
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

        case Frequency_mode:
            if (Hardware::frekvenca > 0 && Hardware ::frekvenca <= 60)
            {
                mikrofon = 1;
            }
            else
            {
                mikrofon = 0;
            }
            break;

        default:
            mikrofon = 0;
            break;
        }

        switch (A_mode)
        {
        case NORMAL_FADE:
            if (TIMERS_folder::lucke_filter_time.vrednost() > 100 && mikrofon == 1)
            { //Če se je vrednost spremenila
                TIMERS_folder::lucke_filter_time.ponastavi();
                uint8_t barva_selekt = random(0, barve::LENGHT);
                fade(barva_selekt);
            }
            break;

        case Direct_signal:
            mikrofon = 0;
            int16_t temp = analogRead(mic_pin) * 255 / 1023 - ceil(255 / 2);
            uint8_t Signal_level = temp >= 0 ? temp : 0;
            analogWrite(rdeca_LED_trak, Signal_level); // Direktna povezava mikrofona na izhod vijolicne barve
            analogWrite(modra_LED_trak, (Signal_level - 50) >= 0 ? Signal_level - 50 : 0);
            break;

        case COLOR_FADE:
            uint8_t barva_selekt = random(0, barve::LENGHT-1);
            Color_Fade(barva_selekt);
            break;

        case OFF:
            turnOFFstrip();
        }
    }
}

void mic_mode_change() // Switches audio mode
{
    vTaskSuspend(avg_VL);
    vTaskSuspend(frekVL);
    vTaskSuspend(audio_system);
    int ct = 0, delay_switch = 300;

    mic_mode = (mic_mode + 1) % mic_detection_mode::LENGHT_1;

    switch (mic_mode) // Resumes meassure tasks
    {
    case Frequency_mode:
        vTaskResume(frekVL);
        break;
    case Average_volume:
        vTaskResume(avg_VL);
        break;
    }

    digitalWrite(rdeca_LED_trak, 0);
    digitalWrite(zelena_LED_trak, 0);
    digitalWrite(modra_LED_trak, 0);
    vTaskDelay(700 / portTICK_PERIOD_MS);

    while (ct < (mic_mode + 1) * 2 && mic_mode != OFF) // n+1 blink = n+1 audio_mode
    {
        PORTB ^= (1 << 1);                  //R
        PORTD ^= (1 << 3);                  //G
        PORTB ^= (1 << 3);                  //B
        delay_switch = 1000 - delay_switch; // Switches between 300ms and 700ms
        vTaskDelay(delay_switch / portTICK_PERIOD_MS);
        ct++;
    }

    PORTB &= ~(1 << 1); //R
    PORTD &= ~(1 << 3); //G
    PORTB &= ~(1 << 3); //B
    vTaskResume(audio_system);
}

void audio_mode_change(char *ch)
{
    turnOFFstrip();
    vTaskSuspend(audio_system);
    vTaskDelay(700);

    if (ch == "off")
    {
        A_mode = OFF;
    }
    else
    {
        A_mode = (A_mode + 1) % audio_mode::LENGTH_2;
        fade(WHITE);
    }
    fade(RED);
    vTaskDelay(500);
    vTaskResume(audio_system);
}