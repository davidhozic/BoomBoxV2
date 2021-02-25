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
    COLOR_OFF_FADE,
    LENGTH_2,
    OFF
};

/*************************** KONSTANTE ************************/
extern const int mic_pin = A0;
int mic_mode = Average_volume;
int A_mode = OFF;
int barva_selekt = 0;
const int rdeca_LED_trak = 9;  //Rdeča lučka je na pinu 9
const int zelena_LED_trak = 3; //TR_BARVA[0] lučka je na pinu 3
const int modra_LED_trak = 11; //Modra lučka je na pinu 11
extern TaskHandle_t avg_VL;
extern TaskHandle_t frekVL;
extern TaskHandle_t audio_system;
TaskHandle_t fade_control;
TaskHandle_t color_fade_control;
TaskHandle_t color_fade_off_fade_control;
/*************************************************************/

/********** GLOBAL **********/
float TR_BARVA[3] = {0, 0, 0}; //Trenutna barva traku RGB
/****************************/

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
    int *barvni_ptr[8] = {WHITE, GREEN, RED, BLUE, YELLOW, AQUA, VIOLET, PINK};
} barva;

void turnOFFstrip()
{
    vTaskDelete(fade_control);
    vTaskDelete(color_fade_control);
    vTaskDelete(color_fade_off_fade_control);
    digitalWrite(rdeca_LED_trak, 0);
    digitalWrite(zelena_LED_trak, 0);
    digitalWrite(modra_LED_trak, 0);
    TR_BARVA[0] = 0;
    TR_BARVA[1] = 0;
    TR_BARVA[2] = 0;
}

void fade_task(void *taskPARAM) //Prizig na barbi in pocasen izklop
{
    int BARVA = *((int *)taskPARAM);
    int svetlost = 255;

    TR_BARVA[0] = barva.barvni_ptr[BARVA][0];
    TR_BARVA[1] = barva.barvni_ptr[BARVA][1];
    TR_BARVA[2] = barva.barvni_ptr[BARVA][2];

    //Prižig lučk na neki barvi
    while (true)
    {
        if (svetlost > 0)
        {
            analogWrite(rdeca_LED_trak, TR_BARVA[0] * svetlost / 255);
            analogWrite(zelena_LED_trak, TR_BARVA[1] * svetlost / 255);
            analogWrite(modra_LED_trak, TR_BARVA[2] * svetlost / 255);
            svetlost -= 5;
            vTaskDelay(20 / portTICK_PERIOD_MS);
        }
        else
        {
            vTaskDelete(fade_control);
        }
    }
}

void fade_create(int barva)
{
    vTaskDelete(fade_control);
    xTaskCreate(fade_task, "normalni fade_create", 64, &barva, 1, &fade_control);
}

void Color_Fade_task(void *b) //Fade iz ene barve v drugo
{                             //Fades from one color to another
    int8_t barva_param = *((int8_t *)b);
    float smer_r = 0;
    float smer_g = 0;
    float smer_b = 0;

    while (true)
    {
        smer_r = (barva.barvni_ptr[barva_param][0] - TR_BARVA[0]) / (abs(barva.barvni_ptr[barva_param][0] - TR_BARVA[0])); //barva.barvni_ptr ima shranjene naslove barvnih tabel
        smer_g = (barva.barvni_ptr[barva_param][1] - TR_BARVA[1]) / (abs(barva.barvni_ptr[barva_param][1] - TR_BARVA[1]));
        smer_b = (barva.barvni_ptr[barva_param][2] - TR_BARVA[2]) / (abs(barva.barvni_ptr[barva_param][2] - TR_BARVA[2]));

        smer_r = isnan(smer_r) ? 0 : smer_r;
        smer_g = isnan(smer_g) ? 0 : smer_g;
        smer_b = isnan(smer_b) ? 0 : smer_b;

        TR_BARVA[0] = TR_BARVA[0] + 7 * smer_r;
        TR_BARVA[1] = TR_BARVA[1] + 5 * smer_g;
        TR_BARVA[2] = TR_BARVA[2] + 7 * smer_b;
        analogWrite(rdeca_LED_trak, TR_BARVA[1]);
        analogWrite(zelena_LED_trak, TR_BARVA[0]);
        analogWrite(modra_LED_trak, TR_BARVA[2]);
        vTaskDelay(16 / portTICK_PERIOD_MS);
    }
}

void color_fade_create(int barva)
{
    vTaskDelete(color_fade_control);
    xTaskCreate(Color_Fade_task, "col_fade", 64, &barva, 1, &color_fade_control);
}

void Color_Fade_off_fade_task(void *b)
{
    int BARVA = *((int *)b);
    uint8_t svetlost = 255;
    float smer_r = 0;
    float smer_g = 0;
    float smer_b = 0;

    while (abs(barva.barvni_ptr[BARVA][0] - TR_BARVA[0]) > 20 || abs(barva.barvni_ptr[BARVA][1] - TR_BARVA[1]) > 20 || abs(barva.barvni_ptr[BARVA][2] - TR_BARVA[2]) > 20)
    {                                                                                                          // Fade Barve
        smer_r = (barva.barvni_ptr[BARVA][0] - TR_BARVA[0]) / (abs(barva.barvni_ptr[BARVA][0] - TR_BARVA[0])); //barva.barvni_ptr ima shranjene naslove barvnih tabel
        smer_g = (barva.barvni_ptr[BARVA][1] - TR_BARVA[1]) / (abs(barva.barvni_ptr[BARVA][1] - TR_BARVA[1]));
        smer_b = (barva.barvni_ptr[BARVA][2] - TR_BARVA[2]) / (abs(barva.barvni_ptr[BARVA][2] - TR_BARVA[2]));

        smer_r = isnan(smer_r) ? 0 : smer_r;
        smer_g = isnan(smer_g) ? 0 : smer_g;
        smer_b = isnan(smer_b) ? 0 : smer_b;

        TR_BARVA[0] = TR_BARVA[0] + 7 * smer_r;
        TR_BARVA[1] = TR_BARVA[1] + 5 * smer_g;
        TR_BARVA[2] = TR_BARVA[2] + 7 * smer_b;
        analogWrite(rdeca_LED_trak, TR_BARVA[1]);
        analogWrite(zelena_LED_trak, TR_BARVA[0]);
        analogWrite(modra_LED_trak, TR_BARVA[2]);
        vTaskDelay(16 / portTICK_PERIOD_MS);
    }

    while (svetlost > 0) // Zniza Svetlost
    {
        svetlost -= 5;
        analogWrite(rdeca_LED_trak, TR_BARVA[0] * svetlost / 255);
        analogWrite(zelena_LED_trak, TR_BARVA[1] * svetlost / 255);
        analogWrite(modra_LED_trak, TR_BARVA[2] * svetlost / 255);
        vTaskDelay(16 / portTICK_PERIOD_MS);
    }

    vTaskResume(audio_system);
    vTaskDelete(color_fade_off_fade_control);
}

void Color_Fade_off_fade_create(int b)
{
    xTaskCreate(Color_Fade_off_fade_task, "color fade with off fade", 64, &b, 1, &color_fade_off_fade_control);
    vTaskSuspend(audio_system);
}

void audio_visual(void *paramOdTaska) //Funkcija avdio-vizualnega sistema
{
    int mikrofon = 0;
    while (true)
    {
        uint8_t barva_selekt = random(0, barve::LENGHT);
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
        case NORMAL_FADE: //Prizig in fade izklop
            if (TIMERS_folder::lucke_filter_time.vrednost() > 100 && mikrofon == 1)
            { //Če se je vrednost spremenila
                TIMERS_folder::lucke_filter_time.ponastavi();

                fade_create(barva_selekt); //Ustvari RTOS task
            }
            break;

        case Direct_signal: //Vijolicna barva glede na direktn signal iz mikrofona
            mikrofon = 0;
            int16_t temp = analogRead(mic_pin) * 255 / 1023 - ceil(255 / 2);
            uint8_t Signal_level = temp >= 0 ? temp : 0;
            analogWrite(rdeca_LED_trak, Signal_level); // Direktna povezava mikrofona na izhod vijolicne barve
            analogWrite(modra_LED_trak, (Signal_level - 50) >= 0 ? Signal_level - 50 : 0);
            break;

        case COLOR_FADE:                     //Prehod iz trenutne barve v zeljeno
            color_fade_create(barva_selekt); //Ustvari RTOS task
            break;

        case COLOR_OFF_FADE:
            Color_Fade_off_fade_create(barva_selekt);
            break;

        case OFF:
            turnOFFstrip();
            A_mode = -1;
        }
    }
}

void mic_mode_change() // Switches audio mode ; 1s hold
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

void audio_mode_change(char *ch) // Double click
{
    vTaskSuspend(audio_system);
    turnOFFstrip();

    vTaskDelay(700 / portTICK_PERIOD_MS);

    if (ch == "off")
    {
        A_mode = OFF;
    }
    else
    {
        A_mode = (A_mode + 1) % audio_mode::LENGTH_2;
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
    vTaskResume(audio_system);
}