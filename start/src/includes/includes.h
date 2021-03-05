#include "D:\Documents\Arduino\libraries\castimer\castimer.h"
#include "Arduino.h"
#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "settings.h"
#include <semphr.h>

#ifndef namespaces_H
#define namespaces_H

#define holdTASK(task)                                     \
    if (eTaskGetState(task) != eSuspended && task != NULL) \
    {                                                      \
        vTaskSuspend(task);                                \
    }

#define resumeTASK(task)                                   \
    if (eTaskGetState(task) == eSuspended && task != NULL) \
    {                                                      \
        vTaskResume(task);                                 \
    }

#define deleteTask(task)   \
    if (task != NULL)      \
    {                      \
        vTaskDelete(task); \
        task = NULL;       \
        vTaskDelay(3);     \
    }


#define delay_FRTOS(x) vTaskDelay(pdMS_TO_TICKS(x))

extern TaskHandle_t core_control;
extern TaskHandle_t event_control;
extern TaskHandle_t audio_system_control;
extern TaskHandle_t zaslon_control;
extern TaskHandle_t chrg_control;
extern TaskHandle_t thermal_control;
extern TaskHandle_t meas_control;
extern SemaphoreHandle_t Thermal_SEM;
extern SemaphoreHandle_t voltage_SEM;

struct hardware_struct
{
    const float REF_mVOLT = 5000.00; //Referenčna napetost na ADC (4.999V - 5.002V)
    const float REF_VOLT = 5.00;
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
    castimer SW2_off_timer;
    castimer average_v_timer;
    castimer frek_meassurement_t;
};
extern timer_struct Timers;

#endif
