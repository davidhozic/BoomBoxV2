/*
 * FreeRTOS V202012.00
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#define configUSE_PREEMPTION 0		

/*		FreeRTOS tick generation	*/
#define configCPU_CLOCK_HZ (F_CPU)
#define configTICK_RATE_HZ	(2000)
#define configTICK_PRESCALER (64)   // 1, 8, 64, 256 or 1024 (or 32 for timer2)

#define portUSE_TIMER0				1	// Select which timer you want to use for RTOS tick generation (portUSE_TIMER0:5)
/************************************************/

	
#define configMAX_PRIORITIES 5				// Maximum priority each task can have 
#define configMINIMAL_STACK_SIZE 256		// Stack size for the idle task
#define configMAX_TASK_NAME_LEN 8			// Max length of the task name
#define configUSE_16_BIT_TICKS 1			// Size of the Tick_t type, if 0, will be 32 bit, if 1 then the performance will be improved on 16 bit/8 bit CPUs (but timers will bi limited to max 2^16 -1 ticks)
#define configIDLE_SHOULD_YIELD 0			// If this option is 1, the idle task and next task in line will share one time slice, if 0, all tasks will have same time slices
#define configUSE_TASK_NOTIFICATIONS 1		
#define configUSE_MUTEXES 0					// Mutual exclusion for a semaphore
#define configUSE_RECURSIVE_MUTEXES 0		// Tasks can take more than one "key" from the semaphore in the same line of your code
#define configUSE_COUNTING_SEMAPHORES 0		// Semaphores won't have just one "key" but multiple
#define configQUEUE_REGISTRY_SIZE 2			
#define configUSE_QUEUE_SETS 0			
#define configUSE_TIME_SLICING 0			// Tasks with same prioriy will time slice (each gets to run for a small ammount of time)
/* Memory allocation related definitions. */
#define configSUPPORT_STATIC_ALLOCATION 0
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configAPPLICATION_ALLOCATED_HEAP 0 //	Reserve heap inside program
#define configTOTAL_HEAP_SIZE		  (4096)

/* Hook function related definitions. */
#define configUSE_IDLE_HOOK 0
#define configUSE_TICK_HOOK 0
#define configCHECK_FOR_STACK_OVERFLOW 0
#define configUSE_MALLOC_FAILED_HOOK 0
#define configUSE_DAEMON_TASK_STARTUP_HOOK 0

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS 0
#define configUSE_TRACE_FACILITY 0
#define configUSE_STATS_FORMATTING_FUNCTIONS 0

/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES 0
#define configMAX_CO_ROUTINE_PRIORITIES 2

/* Software timer related definitions. */
#define configUSE_TIMERS 0
#define configTIMER_TASK_PRIORITY ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH 5
#define configTIMER_TASK_STACK_DEPTH ( 128 * 2 )

/* Define to trap errors during development. */
//#define configASSERT(  x  ) if( ( x ) == 0 ) { asm volatile ("cli"); while(1){ asm volatile ("BREAK"); } }

/* Optional functions - most linkers will remove unused functions anyway. */
#define INCLUDE_vTaskPrioritySet 0
#define INCLUDE_uxTaskPriorityGet 0
#define INCLUDE_vTaskDelete 1
#define INCLUDE_vTaskSuspend 1
#define INCLUDE_xResumeFromISR 0
#define INCLUDE_vTaskDelayUntil 0
#define INCLUDE_vTaskDelay 1
#define INCLUDE_xTaskGetSchedulerState 0
#define INCLUDE_xTaskGetCurrentTaskHandle 1
#define INCLUDE_uxTaskGetStackHighWaterMark 0
#define INCLUDE_xTaskGetIdleTaskHandle 0
#define INCLUDE_eTaskGetState 1
#define INCLUDE_xEventGroupSetBitFromISR 0
#define INCLUDE_xTimerPendFunctionCall 0
#define INCLUDE_xTaskAbortDelay 0
#define INCLUDE_xTaskGetHandle 0
#define INCLUDE_xTaskResumeFromISR 0


#define pdMS_TO_TICKS(xTimeInMs) ((TickType_t)(((uint32_t)(xTimeInMs) * (uint32_t)configTICK_RATE_HZ) / (uint32_t)1000))

#define recmuRECURSIVE_MUTEX_TEST_TASK_STACK_SIZE ( configMINIMAL_STACK_SIZE )

#endif /* FREERTOS_CONFIG_H */
