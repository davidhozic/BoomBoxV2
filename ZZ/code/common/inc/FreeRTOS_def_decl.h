


#ifndef namespaces_H
#define namespaces_H
#include "semphr.h"

/************************************************************************/
/*					  Safe FreeRTOS functions/macros		            */
/************************************************************************/

#ifndef DEBUG
	#define delayFREERTOS(x)	vTaskDelay((double)x/portTICK_PERIOD_MS);
#else
	#include <math.h>
	#define delayFREERTOS(x)	vTaskDelay(			2		)
#endif	

void holdTASK(TaskHandle_t* task);
void resumeTASK(TaskHandle_t* task);
void deleteTASK(TaskHandle_t* task);

	

#endif

