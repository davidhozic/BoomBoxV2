#include "D:\Documents\Arduino\libraries\castimer\castimer.h"
#include "Arduino.h"
#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "settings.h"

#ifndef namespaces_H
#define namespaces_H

extern TaskHandle_t core_control;
extern TaskHandle_t event_control;
extern TaskHandle_t audio_system_control;
extern TaskHandle_t zaslon_control;
extern TaskHandle_t chrg_control;
extern TaskHandle_t thermal_control;
extern TaskHandle_t meas_control;



struct hardware_struct
{
    const short REF_VOLT = 5000; //Referenčna napetost na ADC (4.999V - 5.002V)
    bool is_Powered_UP = 0;
    bool PSW = 0;
    bool POLKONC;
    bool polnjenjeON;
    bool AMP_oheat = 0;
    bool display_enabled = 1;
    float Amplifier_temp = 0;
    unsigned int napetost = 0;
};
extern hardware_struct Hardware;

struct timer_struct
{
    castimer stikaloCAS;
    castimer lucke_filter_time;
    castimer audio_timer;
    castimer LCD_timer;
    castimer stikaloOFFtime;
    castimer VOLT_timer;
    castimer brightness_timer;
    castimer color_timer;
};
extern timer_struct Timers;

#endif
