#include <Arduino_FreeRTOS.h>
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\EEPROM\src\EEPROM.h"
#include "Vhod.h"
#include "castimer.h"
#include "src/includes/includes.h"

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
  xTaskCreate(core, "_core", 80, NULL, 1, &core_control);
  xTaskCreate(events, "Events task", 80, NULL,1, &event_control);
 // xTaskCreate(audio_visual, "AUVIS", 80, NULL, 1, &audio_system_control);
  //xTaskCreate(zaslon, "LVCHRG", 55, NULL, 1, &zaslon_control);
  //xTaskCreate(polnjenje, "CHRG", 55, NULL, 1, &chrg_control);
  xTaskCreate(thermal, "therm", 55, NULL, 1, &thermal_control);
  //xTaskCreate(audio_meritve, "audio_meritve", 55, NULL, 1, &meas_control);
}

void loop()
{
}
