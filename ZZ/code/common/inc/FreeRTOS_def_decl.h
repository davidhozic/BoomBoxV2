


#ifndef namespaces_H
#define namespaces_H
#include "semphr.h"

/************************************************************************/
/*							Safe FreeRTOS macros                        */
/************************************************************************/
#define holdTASK(task)										\
if (eTaskGetState(task) != eSuspended && task != NULL)		\
{															\
	vTaskSuspend(task);										\
}

#define resumeTASK(task)									\
if (eTaskGetState(task) == eSuspended && task != NULL)	    \
{															\
	vTaskResume(task);										\
}

#define deleteTask(task)   \
if (task != NULL)      \
{                      \
	vTaskDelete(task); \
	task = NULL;       \
	delayFREERTOS(1);     \
}

#define delayFREERTOS(x)	vTaskDelay(x/portTICK_PERIOD_MS)	//pretvori ms v FreeRTOS ticke

/************************************************************************/
/*					      Global FreeRTOS variables                     */
/************************************************************************/

extern TaskHandle_t core_control;
extern TaskHandle_t event_control;
extern TaskHandle_t audio_system_control;
extern TaskHandle_t zaslon_control;
extern TaskHandle_t chrg_control;
extern TaskHandle_t meas_control;
extern SemaphoreHandle_t voltage_SEM;
/************************************************************************/
	




#endif

