
#include "FreeRTOS.h"
#include "global.h"
#include "common/inc/FreeRTOS_def_decl.h"


/************************************************************************/
/*								GLOBAL OBJECTS	                        */
/************************************************************************/
class_VHOD napajalnik(0, 'K', 0);
class_TIMER stikaloCAS;

/************************************************************************/
/*					          GLOBAL  STRUCTURES                        */
/************************************************************************/
struct_HARDWARE Hardware;

/************************************************************************/
/*						SAFE FREERTOS FUNCTIONS                         */
/************************************************************************/

void holdTASK(TaskHandle_t* task)
{
	if (eTaskGetState(*task) != eSuspended && *task != NULL)
	vTaskSuspend(*task);
}
void resumeTASK(TaskHandle_t* task)
{
	if (eTaskGetState(*task) == eSuspended && *task != NULL)
	vTaskResume(*task);
}

void deleteTASK(TaskHandle_t* task)
{
	if (*task != NULL)
	{
		vTaskDelete(*task);
		*task = NULL;
		delayFREERTOS(2);
	}
}


