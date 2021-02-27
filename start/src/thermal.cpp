#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\Arduino.h"
#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\global\stuff.h"



extern const unsigned short error_temp = 60;


void Shutdown();
extern TaskHandle_t core_handle;

void thermal(void *paramOdTaska)
{
  while (true)
  {
    delay(1500);
    taskENTER_CRITICAL();
    float AMP_Temp_S_Voltage = (float)analogRead(A1) * (float)Hardware.REF_napetost / 1023.00;
    Hardware.Amplifier_temp = (float)(-0.073) * (float)AMP_Temp_S_Voltage + 192.754;
    if (Hardware.Amplifier_temp >= error_temp)
    {
      Hardware.AMP_oheat = true;
      if (Hardware.is_Powered_UP)
      {
        delay(200);
        Shutdown();
      }
    }
    taskEXIT_CRITICAL();
    /*
      if (hlajenjeCas.vrednost() < 300000 && Hardware.Amplifier_temp < 42.00 || napajalnik.vrednost() == 0)
      {
        digitalWrite(ventilator, 0);
      }

      if (napajalnik.vrednost() == 1)
      {
        if (hlajenjeCas.vrednost() >= 300000)
        { //Ventilator se prižge pet minut po začetku hlajenjeCas, izklopi se, ko je hlajenjeCas večji od spodnje spremenljivke "thermal" (odvisna je od temperature napajalnika) in je temperatura manjša ali enaka od 43s
          //(Potem se hlajenjeCas v istem koraku poenostavi in cikel se ponovi).
          if (Hardware.Amplifier_temp < 42.00)
          {
            analogWrite(ventilator, 51); //20%
          }
        }
        float hladilni_K = 12.50;

        if (Hardware.Amplifier_temp >= 42.00 && Hardware.Amplifier_temp < error_temp)
        {
          analogWrite(ventilator, pow((float)Hardware.Amplifier_temp / (float)hladilni_K, 4));
        }
      }

      if (hlajenjeCas.vrednost() >= 420000)
      {
        hlajenjeCas.ponastavi();
      }

      if (Hardware.Amplifier_temp >= error_temp)
      {
        digitalWrite(ventilator, HIGH);
      }
      }
    */
  }
}