#include <Arduino_FreeRTOS.h>
#include "C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\EEPROM\src\EEPROM.h"
#include "Vhod.h"
#include "castimer.h"
#include "src/includes/includes.h"
#include "src/audio/includes/audio.h"

/*************************PROTOTIPI TASKOV************************/
void core(void *paramOdTaska);
void thermal(void *paramOdTaska);
void zaslon(void *paramOdTaska);
void audio_visual(void *p);
void polnjenje(void *paramOdTaska);
void events(void *paramOdTaska);
void mic_mode_change();
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
/**/ SemaphoreHandle_t Thermal_SEM = NULL;
/**/ SemaphoreHandle_t voltage_SEM = NULL;
/*************************************************/

void setup()
{
  DDRD = 0b11101001;
  DDRB = 0b00001111;

  writeOUTPUT(4, 'B', 1); // PULL up
  Hardware.POLKONC = EEPROM.read(battery_eeprom_addr);
  Thermal_SEM = xSemaphoreCreateMutex();
  voltage_SEM = xSemaphoreCreateMutex();
  xSemaphoreGive(Thermal_SEM); /* Semafor za prepecitev dostopa do spremenljivke iz vec taskov naenkrat  */
  xSemaphoreGive(voltage_SEM); /*   (GIVE = ostali lahko vzamejo dostop, TAKE = task ostalim taskom vzame dostop do semaforja)  */

  trenutni_audio_mode = OFF_A;
  xTaskCreate(core, "_core", 64, NULL, tskIDLE_PRIORITY, &core_control);
  xTaskCreate(events, "Events task", 64, NULL, 3, &event_control);
  xTaskCreate(zaslon, "LVCHRG", 64, NULL, tskIDLE_PRIORITY, &zaslon_control);
  xTaskCreate(thermal, "therm", 64, NULL, 1, &thermal_control);
  xTaskCreate(polnjenje, "CHRG", 64, NULL, tskIDLE_PRIORITY, &chrg_control);
  xTaskCreate(audio_visual, "auvs", 64, NULL, 2, &audio_system_control);
}
