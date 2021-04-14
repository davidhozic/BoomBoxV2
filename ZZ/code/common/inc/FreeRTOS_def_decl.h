


#ifndef namespaces_H
#define namespaces_H
#include "semphr.h"
#include <vector>

/************************************************************************/
/*					  Safe FreeRTOS functions/macros		            */
/************************************************************************/
#define delayFREERTOS(x)					 vTaskDelay(x/portTICK_PERIOD_MS)	//pretvori ms v FreeRTOS ticke

void holdTASK(TaskHandle_t* task);
void resumeTASK(TaskHandle_t* task);
void deleteTASK(TaskHandle_t* task);

/************************************************************************/
/*					      Global FreeRTOS structs	                    */
/************************************************************************/
extern TaskHandle_t handle_audio_system;
extern TaskHandle_t handle_capacity_display;
extern SemaphoreHandle_t voltage_semaphore;
/************************************************************************/
	

#endif

