#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\EEPROM\src\EEPROM.h"
#include "C:\Program Files (x86)\Arduino\hardware\tools\avr\avr\include\avr\sleep.h"
#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "Vhod.h"
#include "castimer.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\global\stuff.h"

/*************************PROTOTIPI TASKOV************************/
void core(void *paramOdTaska);
void thermal(void *paramOdTaska);
void zaslon(void *paramOdTaska);
void audio_visual(void *paramOdTaska);
void polnjenje(void *paramOdTaska);
void readVoltage(void *paramOdTaska);
void events(void *paramOdTaska);
void mic_mode_change();
void audio_meritve(void *p);
/*************************KONEC PROTOTIPOV************************/

TaskHandle_t core_handle = NULL;
TaskHandle_t event_handle = NULL;
TaskHandle_t audio_system_control = NULL;

void setup()
{
  DDRD = 0b11101001;
  DDRB = 0b00101111;
  PORTD = 0b00000000;
  PORTB = 0b00010000;
  pinMode(13, OUTPUT);
  Hardware.POLKONC = EEPROM.read(5);
  xTaskCreate(events, "Events task", 64, NULL, 1, &event_handle);
  xTaskCreate(core, "_core", 140, NULL, 1, &core_handle);
  xTaskCreate(audio_visual, "AUVIS", 140, NULL, 1, &audio_system_control);
  xTaskCreate(zaslon, "LVCHRG", 64, NULL, 1, NULL);
  xTaskCreate(polnjenje, "CHRG", 55, NULL, 1, NULL);
  xTaskCreate(thermal, "therm", 80, NULL, 1, NULL);
  xTaskCreate(audio_meritve, "audio_meritve", 64, NULL, 1, NULL);
  xTaskCreate(readVoltage, "nap branje", 64, NULL, 1, NULL);
  delay(200);
  vTaskStartScheduler();
}

void loop(){};
