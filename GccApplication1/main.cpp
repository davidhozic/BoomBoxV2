#define  F_CPU 16000000UL
#include "VHOD/Vhod.h"
#include "castimer/castimer.h"
#include "src/includes/includes.h"
#include "src/audio/includes/audio.h"
#include <avr/io.h>

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
/**/ TaskHandle_t core_control = NULL;          /**/
/**/ TaskHandle_t event_control = NULL;         /**/
/**/ TaskHandle_t audio_system_control = NULL;  /**/
/**/ TaskHandle_t zaslon_control = NULL;        /**/
/**/ TaskHandle_t chrg_control = NULL;          /**/
/**/ TaskHandle_t thermal_control = NULL;       /**/
/**/ SemaphoreHandle_t voltage_SEM = NULL; /**/ // Preprecuje da bi dva taska dostopala do napetosti naenkrat
/*************************************************/
void init()
{

DDRE |= 1;
DDRH |= 1 << 3 | 1 << 4 | 1 << 5;
DDRB |= 0b11110000;

/************************/
/* Nastavitev Timerja */

TCCR5A = 0b00000010;
TCCR5B = (1 << CS50);
OCR5A = 16000;
TIMSK5 = (1 << OCIE5A);

/************************/
ADMUX = (1 << REFS0) | (1 << 0b00000011) | (1 << ADLAR); //Nastavi referenco za napetost
ADCSRA |= (1 << ADEN);                                   //Vklop adc in zacetek konverzije
ADCSRA |= (1 << ADSC);
DIDR0 = 0b11111111; // Izklop digitalnih registrov od 0 to 7
DIDR1 = 0b11111100; //Isto samo za ostale
}

int main()
{

init();

writeOUTPUT(4, 'B', 1); // PULL up
//Hardware.POLKONC = EEPROM.read(battery_eeprom_addr);
voltage_SEM = xSemaphoreCreateMutex();
xSemaphoreGive(voltage_SEM); /*   (GIVE = ostali lahko vzamejo dostop, TAKE = task ostalim taskom vzame dostop do semaforja)  */

trenutni_audio_mode = OFF_A;
xTaskCreate(core, "_core", 256, NULL, tskIDLE_PRIORITY, &core_control);
xTaskCreate(events, "Events task", 256, NULL, 3, &event_control);
xTaskCreate(zaslon, "LVCHRG", 256, NULL, tskIDLE_PRIORITY, &zaslon_control);
xTaskCreate(thermal, "therm", 256, NULL, 1, &thermal_control);
xTaskCreate(polnjenje, "CHRG", 256, NULL, tskIDLE_PRIORITY, &chrg_control);
xTaskCreate(audio_visual, "auvs", 256, NULL, 2, &audio_system_control);

vTaskStartScheduler();
return 0;
}
