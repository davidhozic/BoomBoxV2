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
  Hardware.POLKONC = EEPROM.read(5);
  delay(200);
  xTaskCreate(core, "_core", 128, NULL, 1, &core_handle);
  xTaskCreate(events, "Events task", 58, NULL, 1, &event_handle);
  xTaskCreate(audio_visual, "AUVIS", 128, NULL, 1, &audio_system_control);
  xTaskCreate(zaslon, "LVCHRG", 58, NULL, 1, NULL);
  xTaskCreate(polnjenje, "CHRG", 55, NULL, 1, NULL);
  xTaskCreate(thermal, "therm", 70, NULL, 1, NULL);
  xTaskCreate(audio_meritve, "audio_meritve", 64, NULL, 1, NULL);
  vTaskStartScheduler();
}

void loop(){};
