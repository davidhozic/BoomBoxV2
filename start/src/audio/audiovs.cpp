#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "castimer.h"
#include "Vhod.h"
#include "Arduino.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\global\stuff.h"
#define barv_razlika_cond_true abs(mozne_barve.barvni_ptr[BARVA][0] - Hardware.AUSYS_vars.TR_BARVA[0]) > 7 || abs(mozne_barve.barvni_ptr[BARVA][1] - Hardware.AUSYS_vars.TR_BARVA[1]) > 7 || abs(mozne_barve.barvni_ptr[BARVA][2] - Hardware.AUSYS_vars.TR_BARVA[2]) > 7
#define povp_razlik_barv (abs(mozne_barve.barvni_ptr[*BARVA][0] - Hardware.AUSYS_vars.TR_BARVA[0]) + abs(mozne_barve.barvni_ptr[*BARVA][1] - Hardware.AUSYS_vars.TR_BARVA[1]) + abs(mozne_barve.barvni_ptr[*BARVA][2] - Hardware.AUSYS_vars.TR_BARVA[2])) / 3
#define t_en_krog *CAS * 5 / (povp_razlik_barv) //Cas za eno znizanje

/*************************** CONTROL BLOCKS ************************/
extern TaskHandle_t audio_system_control;
TaskHandle_t fade_control;
TaskHandle_t color_fade_control;
TaskHandle_t Mixed_fade_control;
TaskHandle_t Breathe_control;
/*******************************************************************/

/**************************************************************************************************************************
*                                                                                                                         *
*                                                                                                                         *                                                                                                                         
*                                                                                                                         * 
*                                                  AUDIO VIZUALNI SISTEM                                                  *
*                                                                                                                         *
*                                                                                                                         * 
*                                                                                                                         *  
**************************************************************************************************************************/

/*************************************************************************************************************************
*                                                                                                                         *
*                                                           Definicije                                                    *                                                                                                                         
*                                                             barv                                                        * 
*                                                                                                                         *
**************************************************************************************************************************/
enum barve
{
    BELA,
    ZELENA,
    RDECA,
    MODRA,
    RUMENA,
    MORSKO_MODRA,
    VIJOLICNA,
    ROZA,
    LENGHT
};

struct c
{
    int BELA[3] = {255, 255, 255};
    int ZELENA[3] = {0, 255, 0};
    int RDECA[3] = {255, 0, 0};
    int MODRA[3] = {0, 0, 255};
    int RUMENA[3] = {255, 255, 0};
    int MORSKO_MODRA[3] = {0, 255, 255};
    int VIJOLICNA[3] = {255, 0, 255};
    int ROZA[3] = {255, 20, 147};
    int *barvni_ptr[8] = {BELA, ZELENA, RDECA, MODRA, RUMENA, MORSKO_MODRA, VIJOLICNA, ROZA};
} mozne_barve;
/**************************************************************************************************************************/

/**************************************************************************************************************************
*                                                                                                                         *
*                                                           Pomozne funkcije                                              *                                                                                                                         
*                                                                                                                         *
**************************************************************************************************************************/
void turnOFFstrip()
{

    taskENTER_CRITICAL();
    vTaskDelete(fade_control);
    vTaskDelete(color_fade_control);
    vTaskDelete(Mixed_fade_control);
    vTaskDelete(Breathe_control);

    digitalWrite(Hardware.AUSYS_vars.rdeca_LED_trak, 0);
    digitalWrite(Hardware.AUSYS_vars.zelena_LED_trak, 0);
    digitalWrite(Hardware.AUSYS_vars.modra_LED_trak, 0);
    Hardware.AUSYS_vars.TR_BARVA[0] = 0;
    Hardware.AUSYS_vars.TR_BARVA[1] = 0;
    Hardware.AUSYS_vars.TR_BARVA[2] = 0;
    taskEXIT_CRITICAL();
}

void color_fade_funct(byte *BARVA, int *CAS)
{

    float smer_r = (mozne_barve.barvni_ptr[*BARVA][0] - Hardware.AUSYS_vars.TR_BARVA[0]) / (abs(mozne_barve.barvni_ptr[*BARVA][0] - Hardware.AUSYS_vars.TR_BARVA[0])); //barva.barvni_ptr ima shranjene naslove barvnih tabel
    float smer_g = (mozne_barve.barvni_ptr[*BARVA][1] - Hardware.AUSYS_vars.TR_BARVA[1]) / (abs(mozne_barve.barvni_ptr[*BARVA][1] - Hardware.AUSYS_vars.TR_BARVA[1]));
    float smer_b = (mozne_barve.barvni_ptr[*BARVA][2] - Hardware.AUSYS_vars.TR_BARVA[2]) / (abs(mozne_barve.barvni_ptr[*BARVA][2] - Hardware.AUSYS_vars.TR_BARVA[2]));

    smer_r = isnan(smer_r) ? 0 : smer_r;
    smer_g = isnan(smer_g) ? 0 : smer_g;
    smer_b = isnan(smer_b) ? 0 : smer_b;

    Hardware.AUSYS_vars.TR_BARVA[0] = Hardware.AUSYS_vars.TR_BARVA[0] + 5 * smer_r;
    Hardware.AUSYS_vars.TR_BARVA[1] = Hardware.AUSYS_vars.TR_BARVA[1] + 5 * smer_g;
    Hardware.AUSYS_vars.TR_BARVA[2] = Hardware.AUSYS_vars.TR_BARVA[2] + 5 * smer_b;
    analogWrite(Hardware.AUSYS_vars.rdeca_LED_trak, Hardware.AUSYS_vars.TR_BARVA[0]);
    analogWrite(Hardware.AUSYS_vars.zelena_LED_trak, Hardware.AUSYS_vars.TR_BARVA[1]);
    analogWrite(Hardware.AUSYS_vars.modra_LED_trak, Hardware.AUSYS_vars.TR_BARVA[2]);
    delay(t_en_krog);
}

void svetlost_mod_funct(int *svetlost, int smer)
{

    *svetlost += 5 * smer;
    analogWrite(Hardware.AUSYS_vars.rdeca_LED_trak, Hardware.AUSYS_vars.TR_BARVA[0] * *svetlost / 255);
    analogWrite(Hardware.AUSYS_vars.zelena_LED_trak, Hardware.AUSYS_vars.TR_BARVA[1] * *svetlost / 255);
    analogWrite(Hardware.AUSYS_vars.modra_LED_trak, Hardware.AUSYS_vars.TR_BARVA[2] * *svetlost / 255);
    delay(15);
}

void mic_mode_change() // Switches audio mode ; 1s hold
{
    vTaskSuspend(audio_system_control);

    int ct = 0, delay_switch = 300;

    Hardware.AUSYS_vars.mic_mode = (Hardware.AUSYS_vars.mic_mode + 1) % mic_detection_mode::LENGHT_1;

    digitalWrite(Hardware.AUSYS_vars.rdeca_LED_trak, 0);
    digitalWrite(Hardware.AUSYS_vars.zelena_LED_trak, 0);
    digitalWrite(Hardware.AUSYS_vars.modra_LED_trak, 0);
    delay(700);

    while (ct < (Hardware.AUSYS_vars.mic_mode + 1) * 2) // n+1 blink = n+1 audio_mode
    {
        PORTB ^= (1 << 1);                  //R
        PORTD ^= (1 << 3);                  //G
        PORTB ^= (1 << 3);                  //B
        delay_switch = 1000 - delay_switch; // Switches between 300ms and 700ms
        delay(delay_switch);
        ct++;
    }

    PORTB &= ~(1 << 1); //R
    PORTD &= ~(1 << 3); //G
    PORTB &= ~(1 << 3); //B
    vTaskResume(audio_system_control);
}

void audio_mode_change(char *ch) // Double click
{
    vTaskSuspend(audio_system_control);
    turnOFFstrip();

    vTaskDelay(500 / portTICK_PERIOD_MS);

    if (ch == "off")
    {
        Hardware.AUSYS_vars.A_mode = OFF;
    }
    else
    {
        Hardware.AUSYS_vars.A_mode = (Hardware.AUSYS_vars.A_mode + 1) % audio_mode::LENGTH_2;
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
    vTaskResume(audio_system_control);
}
/*************************************************************************************************************************/

/**************************************************************************************************************************
*                                                                                                                         *
*                                                  GLAVNI NACINI TASKI                                                    *                                                                                                                         
*                                                                                                                         *
**************************************************************************************************************************/
void fade_task(void *taskPARAM) //Prizig na barbi in pocasen izklop
{
    byte BARVA = *((int *)taskPARAM);
    int svetlost = 255;
    Hardware.AUSYS_vars.TR_BARVA[0] = mozne_barve.barvni_ptr[BARVA][0];
    Hardware.AUSYS_vars.TR_BARVA[1] = mozne_barve.barvni_ptr[BARVA][1];
    Hardware.AUSYS_vars.TR_BARVA[2] = mozne_barve.barvni_ptr[BARVA][2];

    while (svetlost > 0)
        svetlost_mod_funct(&svetlost, -1);

    vTaskDelete(fade_control);
}

void Color_Fade_task(void *b)  //Fade iz ene barve v drugo
{                              //Fades from one color to another
    int t = random(500, 1001); //CAS
    byte BARVA = *((int *)b);
    while (barv_razlika_cond_true)
        color_fade_funct(&BARVA, &t);

    vTaskDelete(color_fade_control);
}

void Mesan_fade_task(void *b)
{
    int t = random(500, 1001);
    byte BARVA = *((int *)b);
    int svetlost = random(230, 256);
    while (barv_razlika_cond_true || svetlost > 0)
    {
        if (barv_razlika_cond_true)
            color_fade_funct(&BARVA, &t);

        if (svetlost > 0)
            svetlost_mod_funct(&svetlost, -1);
    }

    vTaskDelete(Mixed_fade_control);
}

void Fade_Breathe_Task(void *p)
{
    vTaskSuspend(audio_system_control);
    byte BARVA = *((int *)p);
    int svetlost = 0;

    Hardware.AUSYS_vars.TR_BARVA[0] = mozne_barve.barvni_ptr[BARVA][0];
    Hardware.AUSYS_vars.TR_BARVA[1] = mozne_barve.barvni_ptr[BARVA][1];
    Hardware.AUSYS_vars.TR_BARVA[2] = mozne_barve.barvni_ptr[BARVA][2];

    while (svetlost < 255)
        svetlost_mod_funct(&svetlost, 1);

    while (svetlost > 0)
        svetlost_mod_funct(&svetlost, -1);
    vTaskDelete(Breathe_control);
    vTaskResume(audio_system_control);
}

/**************************************************************************************************************************/

/**************************************************************************************************************************
*                                                                                                                         *
*                                            GLAVNI UPRAVLJALNI SISTEM (TASK)                                             *                                                                                                                         
*                                                                                                                         *
**************************************************************************************************************************/

void audio_visual(void *paramOdTaska) //Funkcija avdio-vizualnega sistema
{
    bool mikrofon_detect = 0;
    while (true)
    {
        switch (Hardware.AUSYS_vars.mic_mode) //MIC machine
        {
        case Average_volume:
            int trenutna_vrednost = analogRead(Hardware.AUSYS_vars.mic_pin);
            if ((trenutna_vrednost - Hardware.AUSYS_vars.povprecna_glasnost) > 150 && Hardware.AUSYS_vars.povprecna_glasnost != 0)
            {
                mikrofon_detect = 1;
            }
            else
            {
                mikrofon_detect = 0;
            }
            break;

        case Frequency_mode:
            if (Hardware.AUSYS_vars.frekvenca > 0 && Hardware.AUSYS_vars.frekvenca <= 60)
            {
                mikrofon_detect = 1;
            }
            else
            {
                mikrofon_detect = 0;
            }
            break;

        case OFF:
            mikrofon_detect = 0;
            Hardware.AUSYS_vars.mic_mode = -1;
            break;
        }

        if (Timers.lucke_filter_time.vrednost() > 100 && mikrofon_detect == 1) // AUDIO_M machine
        {                                                                      //Če se je vrednost spremenila

            Timers.lucke_filter_time.ponastavi();
            int barva_selekt = random(0, barve::LENGHT);
            taskENTER_CRITICAL();
            switch (Hardware.AUSYS_vars.A_mode)
            {
            case NORMAL_FADE: //Prizig in fade izklop

                vTaskDelete(fade_control);
                xTaskCreate(fade_task, "normalni fade_create", 60, (void *)&barva_selekt, 1, &fade_control);
                break;

            case COLOR_FADE: //Prehod iz trenutne barve v zeljeno
                vTaskDelete(color_fade_control);
                xTaskCreate(Color_Fade_task, "col_fade", 60, (void *)&barva_selekt, 1, &color_fade_control);
                break;

            case MIXED_FADE:
                vTaskDelete(Mixed_fade_control);
                xTaskCreate(Mesan_fade_task, "color fade with off fade", 60, (void *)&barva_selekt, 1, &Mixed_fade_control);
                break;

            case Fade_Breathe:
                vTaskDelete(Breathe_control);
                xTaskCreate(Fade_Breathe_Task, "breathe fade", 60, (void *)&barva_selekt, 1, &Breathe_control);
                break;

            case Direct_signal: //Vijolicna barva glede na direktn signal iz mikrofon_detect = 1a
                mikrofon_detect = 0;
                unsigned short Signal_level = analogRead(Hardware.AUSYS_vars.mic_pin) * 255 / 1023;
                analogWrite(Hardware.AUSYS_vars.rdeca_LED_trak, Signal_level); // Direktna povezava mikrofon_detect = 1a na izhod vijolicne barve
                analogWrite(Hardware.AUSYS_vars.modra_LED_trak, (Signal_level - 50) >= 0 ? Signal_level - 50 : 0);
                break;

            case OFF:
                turnOFFstrip();
                Hardware.AUSYS_vars.A_mode = -1;
            }
            taskEXIT_CRITICAL();
        }
    }
}
