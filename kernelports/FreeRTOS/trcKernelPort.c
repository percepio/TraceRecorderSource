/*
 * Trace Recorder for Tracealyzer v4.11.0
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The FreeRTOS specific parts of the trace recorder
 */

#include <FreeRTOS.h>
#include <trcRecorder.h>

#if (!defined(TRC_USE_TRACEALYZER_RECORDER) && configUSE_TRACE_FACILITY == 1)

#error Trace Recorder: You need to include trcRecorder.h at the end of your FreeRTOSConfig.h!

#endif

#if (defined(TRC_USE_TRACEALYZER_RECORDER) && TRC_USE_TRACEALYZER_RECORDER == 1)

#ifndef TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS

/* TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS is missing in trcConfig.h. */
#error "TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS must be defined in trcConfig.h."

#endif

#ifndef TRC_CFG_INCLUDE_TIMER_EVENTS

/* TRC_CFG_INCLUDE_TIMER_EVENTS is missing in trcConfig.h. */
#error "TRC_CFG_INCLUDE_TIMER_EVENTS must be defined in trcConfig.h."

#endif

#ifndef TRC_CFG_INCLUDE_PEND_FUNC_CALL_EVENTS

/* TRC_CFG_INCLUDE_PEND_FUNC_CALL_EVENTS is missing in trcConfig.h. */
#error "TRC_CFG_INCLUDE_PEND_FUNC_CALL_EVENTS must be defined in trcConfig.h."

#endif

#ifndef TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS

/* TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS is missing in trcConfig.h. Define this as 1 if using FreeRTOS v10 or later and like to trace stream buffer or message buffer events, otherwise 0. */
#error "TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS must be defined in trcConfig.h."

#endif

#if (configUSE_TICKLESS_IDLE != 0 && (TRC_HWTC_TYPE == TRC_OS_TIMER_INCR || TRC_HWTC_TYPE == TRC_OS_TIMER_DECR))
/* 	
	The below error message is to alert you on the following issue:
	
	The hardware port selected in trcConfig.h uses the operating system timer for the 
	timestamping, i.e., the periodic interrupt timer that drives the OS tick interrupt.
			
	When using "tickless idle" mode, the recorder needs an independent time source in
	order to correctly record the durations of the idle times. Otherwise, the trace may appear
	to have a different length than in reality, and the reported CPU load is also affected.
	
	You may override this warning by defining the TRC_CFG_ACKNOWLEDGE_TICKLESS_IDLE_WARNING
	macro in your trcConfig.h file. But then the time scale may be incorrect during
	tickless idle periods.
	
	To get this correct, override the default timestamping by setting TRC_CFG_HARDWARE_PORT
	in trcConfig.h to TRC_HARDWARE_PORT_APPLICATION_DEFINED and define the HWTC macros
	accordingly, using a free running counter or an independent periodic interrupt timer.
	See trcHardwarePort.h for details.
			
	For ARM Cortex-M3, M4 and M7 MCUs this is not an issue, since the recorder uses the 
	DWT cycle counter for timestamping in these cases.		
*/

#ifndef TRC_CFG_ACKNOWLEDGE_TICKLESS_IDLE_WARNING
#error Trace Recorder: This timestamping mode is not recommended with Tickless Idle.
#endif

#endif
	
#include <task.h>
#include <queue.h>

#if defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION == 1)

#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_9_0_0)

static StackType_t stackTzCtrl[TRC_CFG_CTRL_TASK_STACK_SIZE];
static StaticTask_t tcbTzCtrl;

#else

#error "configSUPPORT_STATIC_ALLOCATION not supported before FreeRTOS v9"

#endif

#endif

/* The TzCtrl task - receives commands from Tracealyzer (start/stop) */
static portTASK_FUNCTION(TzCtrl, pvParameters);

#if (TRC_CFG_INCLUDE_TIMER_EVENTS == 1 && TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_8_X_X)

/* If the project does not include the FreeRTOS timers, TRC_CFG_INCLUDE_TIMER_EVENTS must be set to 0 */
#include <timers.h>

#endif

#if (TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS == 1 && TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_8_X_X)

/* If the project does not include the FreeRTOS event groups, TRC_CFG_INCLUDE_TIMER_EVENTS must be set to 0 */
#include <event_groups.h>

#endif

#if (TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS == 1 && TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_10_0_0)

/* If the project does not include the FreeRTOS stream buffers, TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS must be set to 0 */
#include <stream_buffer.h>

#endif

#if (TRC_CFG_ACKNOWLEDGE_QUEUE_SET_SEND != TRC_ACKNOWLEDGED) && (TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_10_3_0 || TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_10_3_1) && (configUSE_QUEUE_SETS == 1)

#error "When using FreeRTOS v10.3.0 or v10.3.1, please make sure that the trace point in prvNotifyQueueSetContainer() in queue.c is renamed from traceQUEUE_SEND to traceQUEUE_SET_SEND in order to tell them apart from other traceQUEUE_SEND trace points. Then set TRC_CFG_ACKNOWLEDGE_QUEUE_SET_SEND in trcConfig.h to TRC_ACKNOWLEDGED to get rid of this error."

#endif

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)

traceResult xTraceKernelPortGetUnusedStack(void* pvTask, TraceUnsignedBaseType_t* puxUnusedStack)
{
	*puxUnusedStack = uxTaskGetStackHighWaterMark(pvTask);

	return TRC_SUCCESS;
}

#endif

traceResult xTraceKernelPortDelay(uint32_t uiTicks)
{
	vTaskDelay(uiTicks);

	return TRC_SUCCESS;
}

unsigned char xTraceKernelPortIsSchedulerSuspended(void)
{
	/* Assumed to be available in FreeRTOS. According to the FreeRTOS docs,
	INCLUDE_xTaskGetSchedulerState or configUSE_TIMERS must be set to 1 in
	FreeRTOSConfig.h for this function to be available. */

	return xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED;
}

#if (TRC_CFG_ENABLE_TASK_MONITOR == 1) && (!defined(configNUM_THREAD_LOCAL_STORAGE_POINTERS) || (configNUM_THREAD_LOCAL_STORAGE_POINTERS == 0))
#error configNUM_THREAD_LOCAL_STORAGE_POINTERS cannot be 0 if TRC_CFG_ENABLE_TASK_MONITOR is enabled
#endif

typedef struct TraceKernelPortData
{
	TraceHeapHandle_t xSystemHeapHandle;
	TraceKernelPortTaskHandle_t xTzCtrlHandle;
} TraceKernelPortData_t;

static TraceKernelPortData_t* pxKernelPortData TRC_CFG_RECORDER_DATA_ATTRIBUTE;

#define TRC_PORT_MALLOC(size) pvPortMalloc(size)

traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* pxBuffer)
{
	TRC_ASSERT_EQUAL_SIZE(TraceKernelPortDataBuffer_t, TraceKernelPortData_t);
	
	if (pxBuffer == 0)
	{
		return TRC_FAIL;
	}
	
	pxKernelPortData = (TraceKernelPortData_t*)pxBuffer;

	pxKernelPortData->xSystemHeapHandle = 0;
	pxKernelPortData->xTzCtrlHandle = 0;
	
	return TRC_SUCCESS;
}

traceResult xTraceKernelPortEnable(void)
{
#ifdef configIDLE_TASK_NAME
	TraceEntryHandle_t xIdleHandle;
	uint32_t i;

	for (i = 0; ((configIDLE_TASK_NAME)[i] != 0) && (i < 128); i++) {}

	if (xTraceEntryCreate(&xIdleHandle) == TRC_SUCCESS)
	{
		xTraceEntrySetSymbol(xIdleHandle, configIDLE_TASK_NAME, i);
		xTraceEntrySetOptions(xIdleHandle, TRC_ENTRY_OPTION_IDLE_NAME);
	}
#endif
	
#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
	if (pxKernelPortData->xSystemHeapHandle == 0)
	{
#if defined(configTOTAL_HEAP_SIZE)
		xTraceHeapCreate("System Heap", 0, 0, configTOTAL_HEAP_SIZE, &pxKernelPortData->xSystemHeapHandle);
#else
		/* A heap type is used that doesn't define configTOTAL_HEAP_SIZE so heap size needs to be configured manually. Define TRC_CFG_TOTAL_HEAP_SIZE in trcConfig.h. */
		xTraceHeapCreate("System Heap", 0, 0, (TRC_CFG_TOTAL_HEAP_SIZE), &pxKernelPortData->xSystemHeapHandle);
#endif
	}
#endif
	
	if (pxKernelPortData->xTzCtrlHandle == 0)
	{
		/* Creates the TzCtrl task - receives trace commands (start, stop, ...) */
#if defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION == 1)
		pxKernelPortData->xTzCtrlHandle = xTaskCreateStatic(TzCtrl, STRING_CAST("TzCtrl"), TRC_CFG_CTRL_TASK_STACK_SIZE, 0, TRC_CFG_CTRL_TASK_PRIORITY, stackTzCtrl, &tcbTzCtrl);
#else
		xTaskCreate(TzCtrl, STRING_CAST("TzCtrl"), TRC_CFG_CTRL_TASK_STACK_SIZE, 0, TRC_CFG_CTRL_TASK_PRIORITY, &pxKernelPortData->xTzCtrlHandle);
#endif

		if (pxKernelPortData->xTzCtrlHandle == 0)
		{
			xTraceError(TRC_ERROR_TZCTRLTASK_NOT_CREATED);

			return TRC_FAIL;
		}
	}
	
	return TRC_SUCCESS;
}

static portTASK_FUNCTION(TzCtrl, pvParameters)
{
	(void)pvParameters;

	while (1)
	{
		xTraceTzCtrl();

		vTaskDelay(TRC_CFG_CTRL_TASK_DELAY);
	}
}

traceResult xTraceKernelPortSetTaskMonitorData(void* pvTask, void* pvData)
{
#if (TRC_CFG_ENABLE_TASK_MONITOR == 1)
	if (pvTask == (void*)0)
	{
		return TRC_FAIL;
	}
	
	if (pvData == (void*)0)
	{
		return TRC_FAIL;
	}
	
	vTaskSetThreadLocalStoragePointer(pvTask, TRC_CFG_KERNEL_PORT_TASK_MONITOR_TLS_INDEX, pvData);
	
	return TRC_SUCCESS;
#else
	(void)pvTask;
	(void)pvData;
	
	return TRC_FAIL;
#endif
}

traceResult xTraceKernelPortGetTaskMonitorData(void* pvTask, void** ppvData)
{
#if (TRC_CFG_ENABLE_TASK_MONITOR == 1)
	if (pvTask == (void*)0)
	{
		return TRC_FAIL;
	}
	
	if (ppvData == (void**)0)
	{
		return TRC_FAIL;
	}
	
	*ppvData = (void*)0;
	
	*ppvData = pvTaskGetThreadLocalStoragePointer(pvTask, TRC_CFG_KERNEL_PORT_TASK_MONITOR_TLS_INDEX);
	
	if (*ppvData == (void*)0)
	{
		return TRC_FAIL;
	}
	
	return TRC_SUCCESS;
#else
	(void)pvTask;
	(void)ppvData;
	
	return TRC_FAIL;
#endif
}

#if (TRC_CFG_SCHEDULING_ONLY == 0)

void vTraceSetQueueName(void* pvQueue, const char* szName)
{
	xTraceObjectSetNameWithoutHandle(pvQueue, szName);
}

void vTraceSetSemaphoreName(void* pvSemaphore, const char* szName)
{
	xTraceObjectSetNameWithoutHandle(pvSemaphore, szName);
}

void vTraceSetMutexName(void* pvMutex, const char* szName)
{
	xTraceObjectSetNameWithoutHandle(pvMutex, szName);
}

#if (TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS == 1 && TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_8_X_X)

void vTraceSetEventGroupName(void* pvEventGroup, const char* szName)
{
	xTraceObjectSetNameWithoutHandle(pvEventGroup, szName);
}

#endif

#if (TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS == 1 && TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_10_0_0)

void vTraceSetStreamBufferName(void* pvStreamBuffer, const char* szName)
{
	xTraceObjectSetNameWithoutHandle(pvStreamBuffer, szName);
}

void vTraceSetMessageBufferName(void* pvMessageBuffer, const char* szName)
{
	xTraceObjectSetNameWithoutHandle(pvMessageBuffer, szName);
}

#endif

#endif

TraceHeapHandle_t xTraceKernelPortGetSystemHeapHandle(void)
{
	return pxKernelPortData->xSystemHeapHandle;
}

#endif
