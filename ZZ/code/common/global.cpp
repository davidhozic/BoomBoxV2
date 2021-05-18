
#include "FreeRTOS.h"
#include "global.h"
#include "common/inc/FreeRTOS_def_decl.h"
#include "libs/VHOD/Vhod.h"

/************************************************************************/
/*								GLOBAL OBJECTS	                        */
/************************************************************************/

class_VHOD m_napajalnik(_21VPSU_pin, _21VPSU_port, 0);



/************************************************************************/
/*					          GLOBAL  STRUCTURES                        */
/************************************************************************/
struct_m_Hardware m_Hardware;
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
		delayFREERTOS(2); // Ensures task gets deleted
	}
}


