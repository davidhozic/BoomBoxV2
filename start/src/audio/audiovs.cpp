#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "D:\Documents\Arduino\libraries\castimer\castimer.h"
#include "D:\Documents\Arduino\libraries\VHOD\Vhod.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\global\stuff.h"
#define barv_razlika_cond_true abs(mozne_barve.barvni_ptr[BARVA][0] - tr_r) > 5 || abs(mozne_barve.barvni_ptr[BARVA][1] - tr_z) > 5 || abs(mozne_barve.barvni_ptr[BARVA][2] - tr_m) > 5
#define BARVA *((byte *)B)
/*************************** task nadzor  **************************/
extern TaskHandle_t audio_system_control;
TaskHandle_t fade_control;
TaskHandle_t color_fade_control;
TaskHandle_t Mixed_fade_control;
TaskHandle_t Breathe_control;
/*******************************************************************/

/**************************************************************************************************************************
*                                                                                                                         *
*                                                                                                                         *
*                                                       AUDIO                                                             *
*                                                       VIZUALNI                                                          *
*                                                       SISTEM                                                            *
*                                                                                                                         *
*                                                                                                                         *
**************************************************************************************************************************/

/**************************************************************************************************************************
*                                                                                                                         *
*                                                    Definicije barv                                                      *
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
    digitalWrite(r_trak, 0);
    digitalWrite(z_trak, 0);
    digitalWrite(m_trak, 0);
    tr_r = 0;
    tr_z = 0;
    tr_m = 0;
    Hardware.AUSYS_vars.A_mode = -1;
    taskEXIT_CRITICAL();
}

void writeTRAK()
{
    taskENTER_CRITICAL();
    analogWrite(r_trak, tr_r * tr_bright / 255);
    analogWrite(z_trak, tr_z * tr_bright / 255);
    analogWrite(m_trak, tr_m * tr_bright / 255);
    taskEXIT_CRITICAL();
}

void color_fade_funct(byte *B)
{

    float smer_r = (mozne_barve.barvni_ptr[BARVA][0] - tr_r) / (abs(mozne_barve.barvni_ptr[BARVA][0] - tr_r)); //barva.barvni_ptr ima shranjene naslove barvnih tabel
    float smer_g = (mozne_barve.barvni_ptr[BARVA][1] - tr_z) / (abs(mozne_barve.barvni_ptr[BARVA][1] - tr_z));
    float smer_b = (mozne_barve.barvni_ptr[BARVA][2] - tr_m) / (abs(mozne_barve.barvni_ptr[BARVA][2] - tr_m));

    smer_r = isnan(smer_r) ? 0 : smer_r;
    smer_g = isnan(smer_g) ? 0 : smer_g;
    smer_b = isnan(smer_b) ? 0 : smer_b;

    tr_r += 5 * smer_r;
    tr_z += 5 * smer_g;
    tr_m += 5 * smer_b;
    writeTRAK();
}

void svetlost_mod_funct(byte *svetlost, byte smer)
{
    *svetlost += 5 * smer;
    writeTRAK();
}

void mic_mode_change() // Switches audio mode ; 1s hold
{
    vTaskSuspend(audio_system_control);

    int ct = 0, delay_switch = 300;

    Hardware.AUSYS_vars.mic_mode = (Hardware.AUSYS_vars.mic_mode + 1) % mic_detection_mode::LENGHT_1;

    digitalWrite(r_trak, 0);
    digitalWrite(z_trak, 0);
    digitalWrite(m_trak, 0);
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
void fade_task(void *B) //Prizig na barbi in pocasen izklop
{
    tr_bright = 255;
    tr_r = mozne_barve.barvni_ptr[BARVA][0];
    tr_z = mozne_barve.barvni_ptr[BARVA][1];
    tr_m = mozne_barve.barvni_ptr[BARVA][2];

    while (tr_bright > 0)
    {
        if (Timers.brightness_timer.vrednost() > 15)
        {
            svetlost_mod_funct(&tr_bright, -1);
            Timers.brightness_timer.ponastavi();
        }
    }

    TaskHandle_t temp = fade_control;
    fade_control = NULL;
    vTaskDelete(temp);
}

void Color_Fade_task(void *B) //Fade iz ene barve v drugo
{
    bool is_breathing = eTaskGetState(Breathe_control) != NULL;
    while (barv_razlika_cond_true)
    {
        if (!is_breathing) // Dihalni nacin svetlosti se ne izvaja
            tr_bright = 255;

        if (Timers.color_timer.vrednost() > 15)
        {
            color_fade_funct(&BARVA);
            Timers.color_timer.ponastavi();
        }
    }
    TaskHandle_t temp = color_fade_control;
    color_fade_control = NULL;
    vTaskDelete(temp);
}

void Fade_Breathe_Task(void *B)
{
    vTaskSuspend(audio_system_control);

    tr_r = mozne_barve.barvni_ptr[BARVA][0];
    tr_z = mozne_barve.barvni_ptr[BARVA][1];
    tr_m = mozne_barve.barvni_ptr[BARVA][2];
    tr_bright = 0;

    while (tr_bright < 255)
    {
        svetlost_mod_funct(&tr_bright, 1);
    }

    while (tr_bright > 0)
    {
        svetlost_mod_funct(&tr_bright, -1);
    }
    vTaskResume(audio_system_control);
    TaskHandle_t temp = Breathe_control;
    Breathe_control = NULL;
    vTaskDelete(temp);
}

void Mesan_fade_task(void *b)
{
    vTaskDelete(Breathe_control);
    xTaskCreate(Fade_Breathe_Task, "breathe fade", 45, b, 1, &Breathe_control);
    vTaskDelete(color_fade_control);
    xTaskCreate(Color_Fade_task, "col_fade", 45, b, 1, &color_fade_control);

    while (eTaskGetState(Breathe_control) != NULL || eTaskGetState(color_fade_control) != NULL)
    {
    };

    TaskHandle_t temp = Mixed_fade_control;
    Mixed_fade_control = NULL;
    vTaskDelete(temp);
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
                xTaskCreate(fade_task, "normalni fade_create", 45, (void *)&barva_selekt, 1, &fade_control);
                break;

            case COLOR_FADE: //Prehod iz trenutne barve v zeljeno
                vTaskDelete(color_fade_control);
                xTaskCreate(Color_Fade_task, "col_fade", 45, (void *)&barva_selekt, 1, &color_fade_control);
                break;

            case MIXED_FADE:
                vTaskDelete(Mixed_fade_control);
                xTaskCreate(Mesan_fade_task, "color fade with off fade", 20, (void *)&barva_selekt, 1, &Mixed_fade_control);
                break;

            case Fade_Breathe:
                vTaskDelete(Breathe_control);
                xTaskCreate(Fade_Breathe_Task, "breathe fade", 45, (void *)&barva_selekt, 1, &Breathe_control);
                break;

            case Direct_signal: //Vijolicna barva glede na direktn signal iz mikrofon_detect = 1a
                mikrofon_detect = 0;
                unsigned short Signal_level = analogRead(Hardware.AUSYS_vars.mic_pin) * 255 / 1023;
                analogWrite(r_trak, Signal_level); // Direktna povezava mikrofon_detect = 1a na izhod vijolicne barve
                analogWrite(m_trak, (Signal_level - 50) >= 0 ? Signal_level - 50 : 0);
                break;

            case OFF:
                turnOFFstrip();
            }
            taskEXIT_CRITICAL();
        }
    }
}
