
#include "FreeRTOS.h"
#include "common.hpp"
#include "libs/input/input.hpp"
#include "avr/interrupt.h"
/************************************************************************/
/*						    COMMON STRUCTS                              */
/************************************************************************/

HW_STATUS_t m_hw_status = {0};


/************************************************************************/
/*						SAFE FREERTOS FUNCTIONS                         */
/************************************************************************/

void holdTASK(TaskHandle_t* task)
{
	if (*task != NULL && eTaskGetState(*task) != eSuspended)
	vTaskSuspend(*task);
}
void resumeTASK(TaskHandle_t* task)
{
	if (*task != NULL && eTaskGetState(*task) != eRunning)
	vTaskResume(*task);
}

void deleteTASK(TaskHandle_t* task)
{
	if (*task != NULL)
	{
		vTaskDelete(*task);
		*task = NULL;
        delay_FreeRTOS_ms(3);
	}
}
