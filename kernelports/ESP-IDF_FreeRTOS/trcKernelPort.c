/*
 * Trace Recorder for Tracealyzer v4.11.0
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The ESP-IDF FreeRTOS specific parts of the trace recorder
 */

#include <freertos/FreeRTOS.h>
#include <trcRecorder.h>

#if (!defined(TRC_USE_TRACEALYZER_RECORDER) && configUSE_TRACE_FACILITY == 1)

#error Trace Recorder: You need to include trcRecorder.h at the end of your FreeRTOSConfig.h!

#endif

#if (defined(TRC_USE_TRACEALYZER_RECORDER) && TRC_USE_TRACEALYZER_RECORDER == 1)

#if !CONFIG_FREERTOS_UNICORE
/**
 * @note    In multicore mode we require a portMUX to prevent threads on the separate cores from interfering
 *             with critical sections.
 */
portMUX_TYPE xTraceMutex = portMUX_INITIALIZER_UNLOCKED;
#endif

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
#endif /* (configUSE_TICKLESS_IDLE != 0 && (TRC_HWTC_TYPE == TRC_OS_TIMER_INCR || TRC_HWTC_TYPE == TRC_OS_TIMER_DECR)) */
	
#include <freertos/task.h>
#include <freertos/queue.h>

#if defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION == 1)

#if (TRC_CFG_ESP_IDF_VERSION >= TRC_ESP_IDF_VERSION_4_0_0)
static StackType_t stackTzCtrl[(TRC_CFG_CTRL_TASK_STACK_SIZE * sizeof(TraceUnsignedBaseType_t)) + (sizeof(TraceUnsignedBaseType_t) / sizeof(StackType_t))]; /* Add some extra space to avoid overflows */
static StaticTask_t tcbTzCtrl;
#else
#error "configSUPPORT_STATIC_ALLOCATION not supported before FreeRTOS v9"
#endif

#endif /* defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION == 1) */

/* The TzCtrl task - receives commands from Tracealyzer (start/stop) */
static portTASK_FUNCTION(TzCtrl, pvParameters);

#if (TRC_CFG_INCLUDE_TIMER_EVENTS == 1 && TRC_CFG_ESP_IDF_VERSION >= TRC_ESP_IDF_VERSION_4_3_0)

/* If the project does not include the FreeRTOS timers, TRC_CFG_INCLUDE_TIMER_EVENTS must be set to 0 */
#include <freertos/timers.h>

#endif /* (TRC_CFG_INCLUDE_TIMER_EVENTS == 1 && TRC_CFG_ESP_IDF_VERSION >= TRC_ESP_IDF_VERSION_4_3_0) */

#if (TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS == 1 && TRC_CFG_ESP_IDF_VERSION >= TRC_ESP_IDF_VERSION_4_3_0)
/* If the project does not include the FreeRTOS event groups, TRC_CFG_INCLUDE_TIMER_EVENTS must be set to 0 */
#include <freertos/event_groups.h>

#endif /* (TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS == 1 && TRC_CFG_ESP_IDF_VERSION >= TRC_ESP_IDF_VERSION_4_3_0) */

#if (TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS == 1 && TRC_CFG_ESP_IDF_VERSION >= TRC_ESP_IDF_VERSION_4_3_0)

/* If the project does not include the FreeRTOS stream buffers, TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS must be set to 0 */
#include <freertos/stream_buffer.h>

#endif /* (TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS == 1 && TRC_CFG_ESP_IDF_VERSION >= TRC_ESP_IDF_VERSION_4_3_0) */

//#if (TRC_CFG_ACKNOWLEDGE_QUEUE_SET_SEND != TRC_ACKNOWLEDGED) && (TRC_CFG_XTENSA_FREERTOS_VERSION == TRC_FREERTOS_VERSION_10_3_0 || TRC_CFG_XTENSA_FREERTOS_VERSION == TRC_FREERTOS_VERSION_10_3_1) && (configUSE_QUEUE_SETS == 1)
//#error "When using FreeRTOS v10.3.0 or v10.3.1, please make sure that the trace point in prvNotifyQueueSetContainer() in queue.c is renamed from traceQUEUE_SEND to traceQUEUE_SET_SEND in order to tell them apart from other traceQUEUE_SEND trace points. Then set TRC_CFG_ACKNOWLEDGE_QUEUE_SET_SEND in trcConfig.h to TRC_ACKNOWLEDGED to get rid of this error."
//#endif /* (TRC_CFG_ACKNOWLEDGE_QUEUE_SET_SEND != TRC_ACKNOWLEDGED) && (TRC_CFG_XTENSA_FREERTOS_VERSION == TRC_FREERTOS_VERSION_10_3_0 || TRC_CFG_XTENSA_FREERTOS_VERSION == TRC_FREERTOS_VERSION_10_3_1) && (configUSE_QUEUE_SETS == 1) */

#if CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_TCPIP || CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_UDP
/* NOTE: Make sure esp_netif and esp_event is added to the requirements in idf_component_register in kernelports/ESP-IDF_FreeRTOS/CMakeLists.txt:
 * REQUIRES esp_timer esp_netif esp_event
 */
#include "esp_netif.h"
#include "esp_event.h"

static int ip_event_received = 0;

void ip_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
	ip_event_received = 1;
}
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

#include <esp_timer.h>

uint32_t prvGetSMPTimestamp() {

	uint64_t ts = 0;
	ts = esp_timer_get_time();
	return (uint32_t)ts;
}

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
#endif

#ifdef configIDLE_TASK_NAME
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
		size_t current = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
		xTraceHeapCreate("System Heap", (TraceUnsignedBaseType_t)current, (TraceUnsignedBaseType_t)current, (TraceUnsignedBaseType_t)heap_caps_get_total_size(MALLOC_CAP_INTERNAL), &pxKernelPortData->xSystemHeapHandle);
	}
#endif

	if (pxKernelPortData->xTzCtrlHandle == 0)
	{
		/* Creates the TzCtrl task - receives trace commands (start, stop, ...) */
#if defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION == 1)
		pxKernelPortData->xTzCtrlHandle = xTaskCreateStatic(TzCtrl, STRING_CAST("TzCtrl"), TRC_CFG_CTRL_TASK_STACK_SIZE * sizeof(TraceUnsignedBaseType_t), 0, TRC_CFG_CTRL_TASK_PRIORITY, stackTzCtrl, &tcbTzCtrl);
	
#else
		xTaskCreate(TzCtrl, STRING_CAST("TzCtrl"), TRC_CFG_CTRL_TASK_STACK_SIZE * sizeof(TraceUnsignedBaseType_t), 0, TRC_CFG_CTRL_TASK_PRIORITY, &pxKernelPortData->xTzCtrlHandle);
#endif

		if (pxKernelPortData->xTzCtrlHandle == 0)
		{
			xTraceError(TRC_ERROR_TZCTRLTASK_NOT_CREATED);

			return TRC_FAIL;
		}
	}
	
	return TRC_SUCCESS;
}

#if ((TRC_CFG_CTRL_TASK_DELAY * configTICK_RATE_HZ) / 1000) == 0
#error "TRC_CFG_CTRL_TASK_DELAY is too low and results in no delay - increase PERCEPIO_TRC_CTRL_TASK_DELAY in menuconfig"
#endif

static portTASK_FUNCTION(TzCtrl, pvParameters)
{
	(void)pvParameters;

#if CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_TCPIP || CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_UDP
	esp_event_handler_instance_t ip_event_handler_instance;
	/* Register to ip events so we know when it is safe to use sockets. IP_EVENT_STA_START and IP_EVENT_AP_START should be enough. */
	while (esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, ip_event_handler, NULL, &ip_event_handler_instance) != ESP_OK)
	{
		/* Wait a bit for event loop to be initialized */
		vTaskDelay(pdMS_TO_TICKS(TRC_CFG_CTRL_TASK_DELAY));
	}
	while (!ip_event_received)
	{
		/* Wait a bit for IP event */
		vTaskDelay(pdMS_TO_TICKS(TRC_CFG_CTRL_TASK_DELAY));
	}
	/* Unregister event handler instance */
	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, ESP_EVENT_ANY_ID, ip_event_handler_instance));
#endif

	while (1)
	{
		xTraceTzCtrl();

		vTaskDelay(pdMS_TO_TICKS(TRC_CFG_CTRL_TASK_DELAY));
	}
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

#if (TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS == 1 && TRC_CFG_ESP_IDF_VERSION >= TRC_ESP_IDF_VERSION_4_0_0)

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

/**
 * If the esp version being used is version 5.0.0 and above, use the ESP_SYSTEM_INIT_FN hook system instead to
 * register to the initialization hooks for the Tracerecorder. By doing this, less modifications to the startup.c file
 * are required.
 */
#if TRC_CFG_ESP_IDF_VERSION >= TRC_ESP_IDF_VERSION_5_0_0
esp_err_t xTraceEspKernelPortInitialize()
{
	xTraceInitialize();
	return ESP_OK;
}

esp_err_t xTraceEspKernelPortEnable()
{
#if CONFIG_PERCEPIO_TRC_START_MODE_START == 1
	xTraceEnable(TRC_START);
#elif CONFIG_PERCEPIO_TRC_START_MODE_START_AWAIT_HOST == 1
	xTraceEnable(TRC_START_AWAIT_HOST);
#elif CONFIG_PERCEPIO_TRC_START_MODE_START_FROM_HOST == 1
	xTraceEnable(TRC_START_FROM_HOST);
#endif
	return ESP_OK;
}

#include <esp_private/startup_internal.h>

#if TRC_CFG_ESP_IDF_VERSION > TRC_ESP_IDF_VERSION_5_2_0
ESP_SYSTEM_INIT_FN(xTraceEspKernelPortInitialize, CORE, BIT(0), 102)
{
	return xTraceEspKernelPortInitialize();
}

ESP_SYSTEM_INIT_FN(xTraceEspKernelPortEnable, CORE, BIT(0), 999)
{
	return xTraceEspKernelPortEnable();
}
#else
ESP_SYSTEM_INIT_FN(xTraceEspKernelPortInitialize, BIT(0), 102)
{
	return xTraceEspKernelPortInitialize();
}

ESP_SYSTEM_INIT_FN(xTraceEspKernelPortEnable, BIT(0), 240)
{
	return xTraceEspKernelPortEnable();
}
#endif /* TRC_CFG_ESP_IDF_VERSION > TRC_ESP_IDF_VERSION_5_4_0 */

#endif

#endif
