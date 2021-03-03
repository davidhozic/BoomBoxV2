#include <Arduino_FreeRTOS.h>
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\EEPROM\src\EEPROM.h"
#include "Vhod.h"
#include "castimer.h"
#include "src/includes/includes.h"

/*************************PROTOTIPI TASKOV************************/
void core(void *paramOdTaska);
void thermal(void *paramOdTaska);
void zaslon(void *paramOdTaska);
void audio_visual(void* p);
void polnjenje(void *paramOdTaska);
void events(void *paramOdTaska);
void mic_mode_change();
void audio_meritve(void *p);
/*************************KONEC PROTOTIPOV************************/

/**************************************************
/*                                               */
/*            FreeRTOS main task control         */
/*                                               */
/*************************************************/
/**/ TaskHandle_t core_control = NULL;         /**/
/**/ TaskHandle_t event_control = NULL;        /**/
/**/ TaskHandle_t audio_system_control = NULL; /**/
/**/ TaskHandle_t zaslon_control = NULL;       /**/
/**/ TaskHandle_t chrg_control = NULL;         /**/
/**/ TaskHandle_t thermal_control = NULL;      /**/
/**/ TaskHandle_t meas_control = NULL;         /**/
/*************************************************/

void setup()
{
  DDRD = 0b11101001;
  DDRB = 0b00001111;
  PORTD = 0b00000000;
  PORTB = 0b00010000;
#if SHRANI_BAT_STAT
  Hardware.POLKONC = EEPROM.read(battery_eeprom_addr);
#endif
  delay(200);
  xTaskCreate(core, "_core", 128, NULL, tskIDLE_PRIORITY, &core_control);
  xTaskCreate(events, "Events task", 64, NULL, tskIDLE_PRIORITY, &event_control);
  xTaskCreate(zaslon, "LVCHRG", 60, NULL, tskIDLE_PRIORITY, &zaslon_control);
  xTaskCreate(thermal, "therm", 60, NULL, tskIDLE_PRIORITY, &thermal_control);
  xTaskCreate(polnjenje, "CHRG", 60, NULL, tskIDLE_PRIORITY, &chrg_control);
  xTaskCreate(audio_visual, "auvs", 64, NULL, tskIDLE_PRIORITY, &audio_system_control);
  //xTaskCreate(audio_meritve, "audio_meritve", 64, NULL, 2, &meas_control);
}

void loop()
{
}
