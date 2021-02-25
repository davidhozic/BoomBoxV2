#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\EEPROM\src\EEPROM.h"
#include "C:\Program Files (x86)\Arduino\hardware\tools\avr\avr\include\avr\sleep.h"
#include "D:\Documents\Arduino\libraries\FreeRTOS\src\Arduino_FreeRTOS.h"
#include "Vhod.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\header\Errors.h"
#include "castimer.h"
#include "C:\Users\McHea\Google Drive\Projekti\Zvocnik (zakljucna naloga)\BoomBoxV2\start\src\header\namespaces.h"

/*************************PROTOTIPI TASKOV************************/
void core(void *paramOdTaska);
void thermal(void *paramOdTaska);
void zaslon(void *paramOdTaska);
void audio_visual(void *paramOdTaska);
void polnjenje(void *paramOdTaska);
void readVoltage(void *paramOdTaska);
void events(void *paramOdTaska);
void povprecna_glasnost(void *input);
void merjenje_frekvence(void *input);
/*************************KONEC PROTOTIPOV************************/
extern const int mic_pin;
TaskHandle_t core_handle;
TaskHandle_t event_handle;
TaskHandle_t avg_VL;
TaskHandle_t frekVL;
TaskHandle_t audio_system;
extern VHOD stikalo;


void setup()
{
  DDRD = 0b11101001;
  DDRB = 0b00101111;
  PORTD = 0b00000000;
  PORTB = 0b00010000;
  Hardware::POLKONC = EEPROM.read(5);
  Serial.begin(9600);
  xTaskCreate(events, "Events task", 64, NULL, 1, &event_handle);
  xTaskCreate(readVoltage, "VOLT_BRANJE", 64, NULL, 1, NULL);
  xTaskCreate(core, "_core", 64, NULL, 1, &core_handle);
  xTaskCreate(audio_visual, "AUVIS", 64, NULL, 1, &audio_system);
  xTaskCreate(zaslon, "LVCHRG", 64, NULL, 1, NULL);
  xTaskCreate(polnjenje, "CHRG", 64, NULL, 1, NULL);
  xTaskCreate(thermal, "therm", 64, NULL, 1, NULL);
  xTaskCreate(povprecna_glasnost, "avg_vol", 64, (void *)&mic_pin, 1, &avg_VL);
  xTaskCreate(merjenje_frekvence, "frek meri", 64, (void *)&mic_pin, 1, &frekVL);
  vTaskSuspend(frekVL);
}

void loop(){};
