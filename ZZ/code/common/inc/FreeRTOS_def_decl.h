


#ifndef namespaces_H
#define namespaces_H
#include "semphr.h"

/************************************************************************/
/*					  Safe FreeRTOS functions/macros		            */
/************************************************************************/

#ifndef DEBUG
	#define delayFREERTOS(x)	vTaskDelay(x/portTICK_PERIOD_MS);
#else
	#include <math.h>
	#define delayFREERTOS(x)	vTaskDelay(		ceil (	x/(portTICK_PERIOD_MS*2000) 		)		)
#endif	

void holdTASK(TaskHandle_t* task);
void resumeTASK(TaskHandle_t* task);
void deleteTASK(TaskHandle_t* task);

	

#endif

