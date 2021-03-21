
#include "settings.h"
#include "FreeRTOS.h"
#include "castimer/castimer.h"
#include "semphr.h"
#include "util/delay.h"

#ifndef namespaces_H
#define namespaces_H

#define holdTASK(task)                                     \
if (eTaskGetState(task) != eSuspended && task != NULL) \
{                                                      \
	vTaskSuspend(task);                                \
}

#define resumeTASK(task)                                   \
if (eTaskGetState(task) == eSuspended && task != NULL) \
{                                                      \
	vTaskResume(task);                                 \
}

#define deleteTask(task)   \
if (task != NULL)      \
{                      \
	vTaskDelete(task); \
	task = NULL;       \
	delayFREERTOS(1);     \
}

#define delayFREERTOS(x)	vTaskDelay(x/portTICK_PERIOD_MS)	//pretvori ms v FreeRTOS ticke


extern TaskHandle_t core_control;
extern TaskHandle_t event_control;
extern TaskHandle_t audio_system_control;
extern TaskHandle_t zaslon_control;
extern TaskHandle_t chrg_control;
extern TaskHandle_t thermal_control;
extern TaskHandle_t meas_control;
extern SemaphoreHandle_t voltage_SEM;

struct Hardware_t
{
	float REF_mVOLT; //Referenčna napetost na ADC (4.999V - 5.002V)
	float REF_VOLT;
	bool is_Powered_UP;
	bool PSW;
	bool POLKONC;
	bool polnjenjeON;
	bool AMP_oheat;
	bool display_enabled;
	float Amplifier_temp;
	float napetost;
	uint32_t timeFROMboot;
	
	Hardware_t(){
		REF_mVOLT = 5000.00; //Referenčna napetost na ADC (4.999V - 5.002V)
		REF_VOLT = 5.00;
		is_Powered_UP = 0;
		PSW = 0;
		POLKONC = false;
		polnjenjeON = false;
		AMP_oheat = 0;
		display_enabled = 1;
		Amplifier_temp = 0;
		napetost = 0;
		timeFROMboot = 0;
	}
};
extern Hardware_t Hardware;

struct Timers_t
{
	castimer stikaloCAS;
	castimer lucke_filter_time;
	castimer LCD_timer;
	castimer stikaloOFFtime;
	castimer VOLT_timer;
	castimer SW2_off_timer;
	castimer average_v_timer;
	castimer mic_ref_timer;
};
extern Timers_t Timers;




/*  Prototipi  */

void writeOUTPUT(unsigned char pin, char port, bool value); // writeOUTPUT(uint8_t pin, char port, bool value)
void writePWM(uint8_t pin, char port, uint8_t vrednost);
void pwmOFF(uint8_t pin, char port);
unsigned short readANALOG(uint8_t pin);


#endif

