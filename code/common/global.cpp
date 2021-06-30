
#include "FreeRTOS.h"
#include "global.hpp"
#include "libs/input/input.hpp"


/************************************************************************/
/*								GLOBAL OBJECTS	                        */
/************************************************************************/

INPUT_t m_napajalnik(_21VPSU_pin, _21VPSU_port, 0);

/************************************************************************/
/*					          GLOBAL  STRUCTURES                        */
/************************************************************************/
HARDWARE m_Hardware;
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
		delay_FreeRTOS_ms(4); // Ensures task gets deleted
	}
}

