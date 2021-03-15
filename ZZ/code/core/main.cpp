
#include "VHOD/Vhod.h"
#include "castimer/castimer.h"
#include "includes/includes.h"
#include "audio/includes/audio.h"
#include "Hardware Functions/EEPROM/EEPROM.h"
#include <avr/io.h>
#include "avr/interrupt.h"
/*************************PROTOTIPI TASKOV************************/
void core(void *paramOdTaska);
void thermal(void *paramOdTaska);
void zaslon(void *paramOdTaska);
void audio_visual(void *p);
void polnjenje(void *paramOdTaska);
void events(void *paramOdTaska);
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

TCCR0A = 0;
TCCR0B = 0;
TCNT0 = 0;
TCCR0A = (1 << WGM01);
OCR0A = 250;
TIMSK0 = (1 << OCIE0A);
TCCR0B = (1 << CS01) || (1 << CS00);



/************************/
ADMUX = (1 << REFS0) | (1 << 0b00000011) | (1 << ADLAR); //Nastavi referenco za napetost
ADCSRA |= (1 << ADEN);                                   //Vklop adc in zacetek konverzije
ADCSRA |= (1 << ADSC);
DIDR0 = 0b11111111; // Izklop digitalnih registrov od 0 to 7
DIDR1 = 0b11111100; //Isto samo za ostale
}


ISR (TIMER0_COMPA_vect){
Hardware.timeFROMboot +=1; //Belezi cas od zagona krmilnika (Timer 0)
TCNT0 = 0;				   //Reset timerja, drugace bodo zamiki enaki overflowu casu
}


int main()
{

init();
writeOUTPUT(4, 'B', 1); // PULL up
EEPROM.pisi(0,battery_eeprom_addr);
Hardware.POLKONC = EEPROM.beri(battery_eeprom_addr);
voltage_SEM = xSemaphoreCreateMutex();
xSemaphoreGive(voltage_SEM); /*   (GIVE = ostali lahko vzamejo dostop, TAKE = task ostalim taskom vzame dostop do semaforja)  */

xTaskCreate(core, "_core", 256, NULL, tskIDLE_PRIORITY, &core_control);
xTaskCreate(events, "Events task", 256, NULL, 3, &event_control);
xTaskCreate(zaslon, "LVCHRG", 256, NULL, tskIDLE_PRIORITY, &zaslon_control);
xTaskCreate(thermal, "therm", 256, NULL, 1, &thermal_control);
xTaskCreate(polnjenje, "CHRG", 256, NULL, tskIDLE_PRIORITY, &chrg_control);
xTaskCreate(audio_visual, "AUSYS", 256, NULL, 2, &audio_system_control);

vTaskStartScheduler();
return 0;
}
