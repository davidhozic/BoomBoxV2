


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

#define deleteTASK_REC(handle_array, lengthh)				\
for (uint8_t index = 0; index < lengthh; index++)			\
{															\
	deleteTASK(handle_array[index]);						\
}

#define delayFREERTOS(x)					 vTaskDelay(x/portTICK_PERIOD_MS)	//pretvori ms v FreeRTOS ticke

/************************************************************************/
/*					      Global FreeRTOS structs	                    */
/************************************************************************/
extern TaskHandle_t audio_system_handle;
extern TaskHandle_t capacity_display_handle;
extern SemaphoreHandle_t voltage_semaphore;
/************************************************************************/
	

#endif

