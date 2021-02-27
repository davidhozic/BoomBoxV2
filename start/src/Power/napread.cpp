
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\global\stuff.h"
#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#define read_volt analogRead(Hardware.vdiv_pin) * 5000 / 1023;

void readVoltage(void *paramOdTaska)
{
    while (true)
    {
        int vFREAD = read_volt;

        if (Hardware.napetost != 0.0)
            vTaskDelay(3000 / portTICK_PERIOD_MS);
        else
            vTaskDelay(200 / portTICK_PERIOD_MS);

        int vSREAD = read_volt;
        taskENTER_CRITICAL();
        if (abs(vFREAD - vSREAD) <= 50)
            Hardware.napetost = vSREAD / 1000;
        taskEXIT_CRITICAL();
    }
}