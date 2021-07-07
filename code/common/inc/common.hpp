
#ifndef GLOBAL_H
#define GLOBAL_H
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "input.hpp"

struct HW_STATUS_t
{
    uint8_t charging_enabled   : 1;
    uint8_t charging_finished  : 1;
    uint8_t external_power     : 1;
    uint8_t powered_up         : 1;
}extern m_hw_status;





/************************************************************************/
/*					  Safe FreeRTOS functions/macros		            */
/************************************************************************/
#ifndef DEBUG
	#define delay_FreeRTOS_ms(x)	vTaskDelay(x/portTICK_PERIOD_MS)
#else
	#define delay_FreeRTOS_ms(x)	vTaskDelay(1)
#endif

void holdTASK	(TaskHandle_t* task);
void resumeTASK	(TaskHandle_t* task);
void deleteTASK	(TaskHandle_t* task);





#endif