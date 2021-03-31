


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

#define deleteTASK(task)									\
if (task != NULL)											\
{															\
	vTaskDelete(task);										\
	task = NULL;											\
	delayFREERTOS(2);										\
}

#define deleteTASK_REC(handle_array)						\
for (uint8_t i = 0; handle_array[i] != NULL; i++)			\
{															\
	deleteTASK(handle_array[i]);							\
}

#define delayFREERTOS(x)					 vTaskDelay(x/portTICK_PERIOD_MS)	//pretvori ms v FreeRTOS ticke

/************************************************************************/
/*					      Global FreeRTOS structs	                    */
/************************************************************************/
extern TaskHandle_t handle_audio_system;
extern TaskHandle_t handle_capacity_display;
extern SemaphoreHandle_t voltage_semaphore;
/************************************************************************/
	

#endif

