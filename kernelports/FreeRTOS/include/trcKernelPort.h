/*
 * Trace Recorder for Tracealyzer v4.11.0
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * FreeRTOS specific definitions needed by the trace recorder
 */

#ifndef TRC_KERNEL_PORT_H
#define TRC_KERNEL_PORT_H

#include <trcDefines.h>
#include <FreeRTOS.h>	/* Defines configUSE_TRACE_FACILITY */

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_USE_TRACEALYZER_RECORDER configUSE_TRACE_FACILITY

/* FreeRTOS version codes */
#define FREERTOS_VERSION_NOT_SET				0
#define TRC_FREERTOS_VERSION_7_3_X				1 /* v7.3 is earliest supported.*/
#define TRC_FREERTOS_VERSION_7_4_X				2
#define TRC_FREERTOS_VERSION_7_5_X				3
#define TRC_FREERTOS_VERSION_7_6_X				TRC_FREERTOS_VERSION_7_5_X
#define TRC_FREERTOS_VERSION_8_X_X				4
#define TRC_FREERTOS_VERSION_9_0_0				5
#define TRC_FREERTOS_VERSION_9_0_1				6
#define TRC_FREERTOS_VERSION_9_0_2				7
#define TRC_FREERTOS_VERSION_10_0_0				8
#define TRC_FREERTOS_VERSION_10_0_1				TRC_FREERTOS_VERSION_10_0_0
#define TRC_FREERTOS_VERSION_10_1_0				9
#define TRC_FREERTOS_VERSION_10_1_1				TRC_FREERTOS_VERSION_10_1_0
#define TRC_FREERTOS_VERSION_10_2_0				TRC_FREERTOS_VERSION_10_1_0
#define TRC_FREERTOS_VERSION_10_2_1				TRC_FREERTOS_VERSION_10_1_0
#define TRC_FREERTOS_VERSION_10_3_0				10
#define TRC_FREERTOS_VERSION_10_3_1				TRC_FREERTOS_VERSION_10_3_0
#define TRC_FREERTOS_VERSION_10_4_0				11
#define TRC_FREERTOS_VERSION_10_4_1				TRC_FREERTOS_VERSION_10_4_0
#define TRC_FREERTOS_VERSION_10_4_2				TRC_FREERTOS_VERSION_10_4_0
#define TRC_FREERTOS_VERSION_10_4_3				TRC_FREERTOS_VERSION_10_4_0
#define TRC_FREERTOS_VERSION_10_5_0				TRC_FREERTOS_VERSION_10_4_0
#define TRC_FREERTOS_VERSION_10_5_1				TRC_FREERTOS_VERSION_10_4_0
#define TRC_FREERTOS_VERSION_10_6_0				TRC_FREERTOS_VERSION_10_4_0
#define TRC_FREERTOS_VERSION_10_6_1				TRC_FREERTOS_VERSION_10_4_0
#define TRC_FREERTOS_VERSION_10_6_2				TRC_FREERTOS_VERSION_10_4_0
#define TRC_FREERTOS_VERSION_11_0_0				TRC_FREERTOS_VERSION_10_4_0
#define TRC_FREERTOS_VERSION_11_0_1				TRC_FREERTOS_VERSION_10_4_0
#define TRC_FREERTOS_VERSION_11_1_0				TRC_FREERTOS_VERSION_10_4_0

/* Legacy FreeRTOS version codes for backwards compatibility with old trace configurations */
#define TRC_FREERTOS_VERSION_7_3				TRC_FREERTOS_VERSION_7_3_X
#define TRC_FREERTOS_VERSION_7_4				TRC_FREERTOS_VERSION_7_4_X
#define TRC_FREERTOS_VERSION_7_5_OR_7_6			TRC_FREERTOS_VERSION_7_5_X
#define TRC_FREERTOS_VERSION_8_X				TRC_FREERTOS_VERSION_8_X_X

#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_10_0_0)
#define prvGetStreamBufferType(x) ((( StreamBuffer_t * )(x) )->ucFlags & sbFLAGS_IS_MESSAGE_BUFFER)
#else
#define prvGetStreamBufferType(x) 0
#endif

/* Added mainly for our internal testing. This makes it easier to create test applications that 
   runs on multiple FreeRTOS versions. */
#if (TRC_CFG_FREERTOS_VERSION < TRC_FREERTOS_VERSION_8_X_X)
	/* FreeRTOS v7.x */	
	#define STRING_CAST(x) ( (signed char*) x )
	#define TraceKernelPortTickType_t portTickType
	#define TraceKernelPortTaskHandle_t xTaskHandle
	#define TraceKernelPortQueueHandle_t xQueueHandle
#else
	/* FreeRTOS v8.0 and later */
	#define STRING_CAST(x) x
	#define TraceKernelPortTickType_t TickType_t
	#define TraceKernelPortTaskHandle_t TaskHandle_t
	#define TraceKernelPortQueueHandle_t QueueHandle_t
#endif

#if (defined(TRC_USE_TRACEALYZER_RECORDER)) && (TRC_USE_TRACEALYZER_RECORDER == 1)

/**
 * @def TRC_PLATFORM_CFG
 * @brief DO NOT CHANGE THIS
 * This defines the basis for version specific lookup of
 * platform configuration files.
 */
#define TRC_PLATFORM_CFG "FreeRTOS"

/**
 * @def TRC_PLATFORM_CFG_MAJOR
 * @brief DO NOT CHANGE THIS
 * Major release version for platform definition file.
 * Does NOT need to match the RTOS version.
 */
#define TRC_PLATFORM_CFG_MAJOR 1

/**
 * @def TRC_PLATFORM_CFG_MINOR
 * @brief DO NOT CHANGE THIS
 * Minor release version for platform definition file.
 * Does NOT need to match the RTOS version.
 */
#define TRC_PLATFORM_CFG_MINOR 2

/**
 * @def TRC_PLATFORM_CFG_PATCH
 * @brief DO NOT CHANGE THIS
 * Patchlevel release version for platform definition file.
 * Does NOT need to match the RTOS version.
 */
#define TRC_PLATFORM_CFG_PATCH 0

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)

/**
 * @brief Specifies if the kernel allow tasks to be deleted. If the recorder knows that
 * tasks/threads cannot be deleted it can perform certain optimizations regarding stack
 * monitoring.
 */
#define TRC_CFG_ALLOW_TASK_DELETE INCLUDE_vTaskDelete

/* Required for stack monitoring */
#undef INCLUDE_uxTaskGetStackHighWaterMark
#define INCLUDE_uxTaskGetStackHighWaterMark 1

#endif

/* INCLUDE_xTaskGetCurrentTaskHandle must be set to 1 for tracing to work properly */
#undef INCLUDE_xTaskGetCurrentTaskHandle
#define INCLUDE_xTaskGetCurrentTaskHandle 1

#define TRC_KERNEL_PORT_KERNEL_CAN_SWITCH_TO_SAME_TASK 0

#include <trcHeap.h>

#define TRC_KERNEL_PORT_BUFFER_SIZE (sizeof(TraceHeapHandle_t) + sizeof(void*))

#if (TRC_CFG_FREERTOS_VERSION == FREERTOS_VERSION_NOT_SET)
#error "Please set TRC_CFG_FREERTOS_VERSION in trcKernelPortConfig.h to the FreeRTOS version used."
#endif

#if (defined(configNUM_THREAD_LOCAL_STORAGE_POINTERS) && (configNUM_THREAD_LOCAL_STORAGE_POINTERS > 0))
#define TRC_KERNEL_PORT_SUPPORTS_TLS 1
#endif

/**
 * @internal The kernel port data buffer
 */
typedef struct TraceKernelPortDataBuffer	/* Aligned */
{
	uint8_t buffer[TRC_KERNEL_PORT_BUFFER_SIZE];
} TraceKernelPortDataBuffer_t;

/**
 * @internal Initializes the kernel port
 * 
 * @param[in] pxBuffer Kernel port data buffer
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* pxBuffer);

/**
 * @internal Enables the kernel port
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortEnable(void);

/**
 * @internal Calls on FreeRTOS vTaskDelay(...)
 *
 * @param[in] uiTicks Tick count to delay
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortDelay(uint32_t uiTicks);

/**
 * @internal Query if FreeRTOS scheduler is suspended
 *
 * @retval 1 Scheduler suspended
 * @retval 0 Scheduler not suspended
 */
unsigned char xTraceKernelPortIsSchedulerSuspended(void);

/**
 * @internal Set data in TLS.
 
 * @param[in] pvTask Task address
 * @param[in] pvData Pointer to the data
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortSetTaskMonitorData(void* pvTask, void* pvData);

/**
 * @internal Get data from TLS.
 
 * @param[in] pvTask Task address
 * @param[out] ppvData Returns pointer to the TLS data
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortGetTaskMonitorData(void* pvTask, void** ppvData);

/**
 * @brief Kernel specific way to properly allocate critical sections
 */
#define TRC_KERNEL_PORT_ALLOC_CRITICAL_SECTION() TraceUnsignedBaseType_t TRACE_ALLOC_CRITICAL_SECTION_NAME;

/**
 * @brief Kernel specific way to properly allocate critical sections
 */
#define TRC_KERNEL_PORT_ENTER_CRITICAL_SECTION() TRACE_ALLOC_CRITICAL_SECTION_NAME = 0; portENTER_CRITICAL()

/**
 * @brief Kernel specific way to properly allocate critical sections
 */
#define TRC_KERNEL_PORT_EXIT_CRITICAL_SECTION() (void)TRACE_ALLOC_CRITICAL_SECTION_NAME; portEXIT_CRITICAL()

/**
* @brief Kernel specific way to set interrupt mask
*/
#define TRC_KERNEL_PORT_SET_INTERRUPT_MASK() (TraceUnsignedBaseType_t)portSET_INTERRUPT_MASK_FROM_ISR()

#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_8_X_X)

/**
 * @brief Kernel specific way to clear interrupt mask
 */
#define TRC_KERNEL_PORT_CLEAR_INTERRUPT_MASK(xMask) portCLEAR_INTERRUPT_MASK_FROM_ISR((TraceUnsignedBaseType_t)(xMask))

#else

/**
 * @brief Kernel specific way to clear interrupt mask
 */
#define TRC_KERNEL_PORT_CLEAR_INTERRUPT_MASK(xMask) portCLEAR_INTERRUPT_MASK_FROM_ISR((TraceUnsignedBaseType_t)xMask)
#endif

#if (TRC_CFG_SCHEDULING_ONLY == 0)

/**
 * @brief Set the queue name
 *
 * @param[in] pvQueue Queue pointer
 * @param[in] szName Queue name
 */
void vTraceSetQueueName(void* pvQueue, const char* szName);

/**
 * @brief Set the semaphore name
 *
 * @param[in] pvSemaphore Semaphore pointer
 * @param[in] szName Semaphore name
 */
void vTraceSetSemaphoreName(void* pvSemaphore, const char* szName);

/**
 * @brief Set the mutex name
 *
 * @param[in] pvMutex Mutex pointer
 * @param[in] szName Mutex name
 */
void vTraceSetMutexName(void* pvMutex, const char* szName);

#if (TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS == 1)

/**
 * @brief Set the event group name
 *
 * @param[in] pvEventGroup Event group pointer
 * @param[in] szName Event group name
 */
void vTraceSetEventGroupName(void* pvEventGroup, const char* szName);

#else

/**
 * @brief Disabled by TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS
 */
#define vTraceSetEventGroupName(__pvEventGroup, __szName) ((void)(__pvEventGroup), (void)(__szName))

#endif

#if (TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS == 1)

/**
 * @brief Set the stream buffer name
 *
 * @param[in] pvStreamBuffer Stream buffer pointer
 * @param[in] szName Stream buffer name
 */
void vTraceSetStreamBufferName(void* pvStreamBuffer, const char* szName);

/**
 * @brief Set the message buffer name
 *
 * @param[in] pvMessageBuffer Message buffer pointer
 * @param[in] szName Message buffer name
 */
void vTraceSetMessageBufferName(void* pvMessageBuffer, const char* szName);

#else

/**
 * @brief Disabled by TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS
 */
#define vTraceSetStreamBufferName(__pvStreamBuffer, __szName) ((void)(__pvStreamBuffer), (void)(__szName))

/**
 * @brief Disabled by TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS
 */
#define vTraceSetMessageBufferName(__pvMessageBuffer, __szName) ((void)(__pvMessageBuffer), (void)(__szName))

#endif

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1)

 /**
  * @internal Retrieves the unused stack for a task
  *
  * @param[in] pvTask Task pointer
  * @param[out] puxUnusedStack The unused stack
  *
  * @retval TRC_FAIL Failure
  * @retval TRC_SUCCESS Success
  */
traceResult xTraceKernelPortGetUnusedStack(void* pvTask, TraceUnsignedBaseType_t *puxUnusedStack);

#endif

#else

/**
 * @brief Disabled by TRC_CFG_SCHEDULING_ONLY
 */
#define vTraceSetQueueName(__pvQueue, __szName) ((void)(__pvQueue), (void)(__szName))

/**
 * @brief Disabled by TRC_CFG_SCHEDULING_ONLY
 */
#define vTraceSetSemaphoreName(__pvSemaphore, __szName) ((void)(__pvSemaphore), (void)(__szName))

/**
 * @brief Disabled by TRC_CFG_SCHEDULING_ONLY
 */
#define vTraceSetMutexName(__pvMutex, __szName) ((void)(__pvMutex), (void)(__szName))

/**
 * @brief Disabled by TRC_CFG_SCHEDULING_ONLY
 */
#define vTraceSetEventGroupName(__pvEventGroup, __szName) ((void)(__pvEventGroup), (void)(__szName))

/**
 * @brief Disabled by TRC_CFG_SCHEDULING_ONLY
 */
#define vTraceSetStreamBufferName(__pvStreamBuffer, __szName) ((void)(__pvStreamBuffer), (void)(__szName))

/**
 * @brief Disabled by TRC_CFG_SCHEDULING_ONLY
 */
#define vTraceSetMessageBufferName(__pvMessageBuffer, __szName) ((void)(__pvMessageBuffer), (void)(__szName))

/**
 * @brief Disabled by TRC_CFG_SCHEDULING_ONLY
 */
#define xTraceKernelPortGetUnusedStack(pvTask, puxUnusedStack) ((void)(pvTask), (void)(puxUnusedStack))

#endif

/* Required for ISR tracing */
#undef INCLUDE_xTaskGetSchedulerState
#define INCLUDE_xTaskGetSchedulerState 1

/**
 * @internal Legacy ID used by Tracealyzer to identify FreeRTOS traces
 */
#define TRACE_KERNEL_VERSION 0x1AA1

/**
 * @internal Kernel specific tick rate frequency definition
 */
#define TRC_TICK_RATE_HZ configTICK_RATE_HZ /* Defined in "FreeRTOS.h" */

/**
 * @internal Kernel specific CPU clock frequency definition
 */
#define TRACE_CPU_CLOCK_HZ configCPU_CLOCK_HZ /* Defined in "FreeRTOSConfig.h" */

#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC)
/**
 * @internal Kernel port specific heap initialization
 */
#define TRC_KERNEL_PORT_HEAP_INIT(size)

/**
 * @internal Kernel port specific heap malloc definition
 */
#define TRC_KERNEL_PORT_HEAP_MALLOC(size) pvPortMalloc(size)
#endif /* (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC) */

#if (defined(configUSE_TIMERS) && (configUSE_TIMERS == 1))

#undef INCLUDE_xTimerGetTimerDaemonTaskHandle
#define INCLUDE_xTimerGetTimerDaemonTaskHandle 1

#endif

#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_XMOS_XCOREAI)

#undef TRC_CFG_CORE_COUNT
#ifdef configNUMBER_OF_CORES
#define TRC_CFG_CORE_COUNT configNUMBER_OF_CORES
#else
#define TRC_CFG_CORE_COUNT configNUM_CORES
#endif

#undef TRC_CFG_GET_CURRENT_CORE
#define TRC_CFG_GET_CURRENT_CORE() rtos_core_id_get()

#endif

#ifdef _CMSIS_RP2040_H_
#undef TRC_CFG_CORE_COUNT
#ifdef configNUMBER_OF_CORES
#define TRC_CFG_CORE_COUNT configNUMBER_OF_CORES
#else
#define TRC_CFG_CORE_COUNT configNUM_CORES
#endif

#undef TRC_CFG_GET_CURRENT_CORE
#define TRC_CFG_GET_CURRENT_CORE() get_core_num()
#endif 

#if (TRC_CFG_FREERTOS_VERSION == TRC_FREERTOS_VERSION_9_0_1)

/**
 * @brief Fix for FreeRTOS v9.0.1 to correctly identify xQueuePeek events.
 *
 * In FreeRTOS v9.0.1, the below trace hooks are incorrectly used from three
 * different functions. This as the earlier function xQueueGenericReceive
 * has been replaced by xQueuePeek, xQueueSemaphoreTake and xQueueReceive.
 *
 * xQueueGenericReceive had a parameter "xJustPeeking", used by the trace hooks
 * to tell between xQueuePeek events and others. This is no longer present, so
 * we need another way to correctly identify peek events. Since all three
 * functions call the same trace macros, the context of these macro is unknown.
 *
 * We therefore check the __LINE__ macro inside of the trace macros. This gives
 * the line number of queue.c, where the macros are used. This can be used to
 * tell if the context is xQueuePeek or another function.
 * __LINE__ is a standard compiler feature since ancient times, so it should
 * work on all common compilers.
 *
 * This might seem as a quite brittle and unusual solution, but works in this
 * particular case and is only for FreeRTOS v9.0.1.
 * Future versions of FreeRTOS should not need this fix, as we have submitted
 * a correction of queue.c with individual trace macros for each function.
 */
#define isQueueReceiveHookActuallyPeek (__LINE__ > 1674) /* Half way between the closes trace points */

#elif (TRC_CFG_FREERTOS_VERSION <= TRC_FREERTOS_VERSION_9_0_0)

/**
 * @brief Is receive actually a peek
 */
#define isQueueReceiveHookActuallyPeek xJustPeeking

#elif (TRC_CFG_FREERTOS_VERSION > TRC_FREERTOS_VERSION_9_0_1)

/**
 * @brief Is never a peek for this FreeRTOS version
 */
#define isQueueReceiveHookActuallyPeek (__LINE__ < 0) /* instead of pdFALSE to fix a warning of "constant condition" */

#endif

#if (TRC_CFG_SCHEDULING_ONLY == 0)
/* We only do this if we're using it */

/* Helpers needed to correctly expand names */
#define TZ__CAT2(a,b) a ## b
#define TZ__CAT(a,b) TZ__CAT2(a, b)

/*
 * The following xQueueGiveFromISR macro hacks make sure xQueueGiveFromISR also has a xCopyPosition parameter
 */

/* Expands name if this header is included... uxQueueType must be a macro that only exists in queue.c or whatever, and it must expand to nothing or to something that's valid in identifiers */
#define xQueueGiveFromISR(a,b) TZ__CAT(xQueueGiveFromISR__, uxQueueType) (a,b)

/* If in queue.c, the "uxQueueType" macro expands to "pcHead". queueSEND_TO_BACK is the value we need to send in */
#define xQueueGiveFromISR__pcHead(__a, __b) MyWrapper_xQueueGiveFromISR(__a, __b, const BaseType_t xCopyPosition); \
BaseType_t xQueueGiveFromISR(__a, __b) { return MyWrapper_xQueueGiveFromISR(xQueue, pxHigherPriorityTaskWoken, queueSEND_TO_BACK); } \
BaseType_t MyWrapper_xQueueGiveFromISR(__a, __b, const BaseType_t xCopyPosition)

/* If not in queue.c, "uxQueueType" isn't expanded */
#define xQueueGiveFromISR__uxQueueType(__a, __b) xQueueGiveFromISR(__a,__b)

#endif

TraceHeapHandle_t xTraceKernelPortGetSystemHeapHandle(void);

/*************************************************************************/
/* KERNEL SPECIFIC OBJECT CONFIGURATION									 */
/*************************************************************************/

/*******************************************************************************
 * The event codes - should match the offline config file.
 ******************************************************************************/

/*** Event codes for streaming - should match the Tracealyzer config file *****/
#define PSF_EVENT_NULL_EVENT								0x00

#define PSF_EVENT_TRACE_START								0x01
#define PSF_EVENT_TS_CONFIG									0x02
#define PSF_EVENT_OBJ_NAME									0x03
#define PSF_EVENT_TASK_PRIORITY								0x04
#define PSF_EVENT_TASK_PRIO_INHERIT							0x05
#define PSF_EVENT_TASK_PRIO_DISINHERIT						0x06
#define PSF_EVENT_DEFINE_ISR								0x07

#define PSF_EVENT_TASK_CREATE								0x10
#define PSF_EVENT_QUEUE_CREATE								0x11
#define PSF_EVENT_SEMAPHORE_BINARY_CREATE					0x12
#define PSF_EVENT_MUTEX_CREATE								0x13
#define PSF_EVENT_TIMER_CREATE								0x14
#define PSF_EVENT_EVENTGROUP_CREATE							0x15
#define PSF_EVENT_SEMAPHORE_COUNTING_CREATE					0x16
#define PSF_EVENT_MUTEX_RECURSIVE_CREATE					0x17
#define PSF_EVENT_STREAMBUFFER_CREATE						0x18
#define PSF_EVENT_MESSAGEBUFFER_CREATE						0x19

#define PSF_EVENT_TASK_DELETE								0x20
#define PSF_EVENT_QUEUE_DELETE								0x21
#define PSF_EVENT_SEMAPHORE_DELETE							0x22
#define PSF_EVENT_MUTEX_DELETE								0x23
#define PSF_EVENT_TIMER_DELETE								0x24
#define PSF_EVENT_EVENTGROUP_DELETE							0x25
#define PSF_EVENT_STREAMBUFFER_DELETE						0x28
#define PSF_EVENT_MESSAGEBUFFER_DELETE						0x29

#define PSF_EVENT_TASK_READY								0x30
#define PSF_EVENT_NEW_TIME									0x31
#define PSF_EVENT_NEW_TIME_SCHEDULER_SUSPENDED				0x32
#define PSF_EVENT_ISR_BEGIN									0x33
#define PSF_EVENT_ISR_RESUME								0x34
#define PSF_EVENT_TS_BEGIN									0x35
#define PSF_EVENT_TS_RESUME									0x36
#define PSF_EVENT_TASK_ACTIVATE								0x37

#define PSF_EVENT_MALLOC									0x38
#define PSF_EVENT_FREE										0x39

#define PSF_EVENT_LOWPOWER_BEGIN							0x3A
#define PSF_EVENT_LOWPOWER_END								0x3B

#define PSF_EVENT_IFE_NEXT									0x3C
#define PSF_EVENT_IFE_DIRECT								0x3D

#define PSF_EVENT_TASK_CREATE_FAILED						0x40
#define PSF_EVENT_QUEUE_CREATE_FAILED						0x41
#define PSF_EVENT_SEMAPHORE_BINARY_CREATE_FAILED			0x42
#define PSF_EVENT_MUTEX_CREATE_FAILED						0x43
#define PSF_EVENT_TIMER_CREATE_FAILED						0x44
#define PSF_EVENT_EVENTGROUP_CREATE_FAILED					0x45
#define PSF_EVENT_SEMAPHORE_COUNTING_CREATE_FAILED			0x46
#define PSF_EVENT_MUTEX_RECURSIVE_CREATE_FAILED				0x47
#define PSF_EVENT_STREAMBUFFER_CREATE_FAILED				0x49
#define PSF_EVENT_MESSAGEBUFFER_CREATE_FAILED				0x4A

#define PSF_EVENT_TIMER_DELETE_FAILED						0x48

#define PSF_EVENT_QUEUE_SEND								0x50
#define PSF_EVENT_SEMAPHORE_GIVE							0x51
#define PSF_EVENT_MUTEX_GIVE								0x52

#define PSF_EVENT_QUEUE_SEND_FAILED							0x53
#define PSF_EVENT_SEMAPHORE_GIVE_FAILED						0x54
#define PSF_EVENT_MUTEX_GIVE_FAILED							0x55

#define PSF_EVENT_QUEUE_SEND_BLOCK							0x56
#define PSF_EVENT_SEMAPHORE_GIVE_BLOCK						0x57
#define PSF_EVENT_MUTEX_GIVE_BLOCK							0x58

#define PSF_EVENT_QUEUE_SEND_FROMISR						0x59
#define PSF_EVENT_SEMAPHORE_GIVE_FROMISR					0x5A

#define PSF_EVENT_QUEUE_SEND_FROMISR_FAILED					0x5C
#define PSF_EVENT_SEMAPHORE_GIVE_FROMISR_FAILED				0x5D

#define PSF_EVENT_QUEUE_RECEIVE								0x60
#define PSF_EVENT_SEMAPHORE_TAKE							0x61
#define PSF_EVENT_MUTEX_TAKE								0x62

#define PSF_EVENT_QUEUE_RECEIVE_FAILED						0x63
#define PSF_EVENT_SEMAPHORE_TAKE_FAILED						0x64
#define PSF_EVENT_MUTEX_TAKE_FAILED							0x65

#define PSF_EVENT_QUEUE_RECEIVE_BLOCK						0x66
#define PSF_EVENT_SEMAPHORE_TAKE_BLOCK						0x67
#define PSF_EVENT_MUTEX_TAKE_BLOCK							0x68

#define PSF_EVENT_QUEUE_RECEIVE_FROMISR						0x69
#define PSF_EVENT_SEMAPHORE_TAKE_FROMISR					0x6A

#define PSF_EVENT_QUEUE_RECEIVE_FROMISR_FAILED				0x6C
#define PSF_EVENT_SEMAPHORE_TAKE_FROMISR_FAILED				0x6D

#define PSF_EVENT_QUEUE_PEEK								0x70
#define PSF_EVENT_SEMAPHORE_PEEK							0x71
#define PSF_EVENT_MUTEX_PEEK								0x72

#define PSF_EVENT_QUEUE_PEEK_FAILED							0x73
#define PSF_EVENT_SEMAPHORE_PEEK_FAILED						0x74	
#define PSF_EVENT_MUTEX_PEEK_FAILED							0x75

#define PSF_EVENT_QUEUE_PEEK_BLOCK							0x76
#define PSF_EVENT_SEMAPHORE_PEEK_BLOCK						0x77
#define PSF_EVENT_MUTEX_PEEK_BLOCK							0x78

#define PSF_EVENT_TASK_DELAY_UNTIL							0x79
#define PSF_EVENT_TASK_DELAY								0x7A
#define PSF_EVENT_TASK_SUSPEND								0x7B
#define PSF_EVENT_TASK_RESUME								0x7C
#define PSF_EVENT_TASK_RESUME_FROMISR						0x7D

#define PSF_EVENT_TIMER_PENDFUNCCALL						0x80
#define PSF_EVENT_TIMER_PENDFUNCCALL_FROMISR				0x81
#define PSF_EVENT_TIMER_PENDFUNCCALL_FAILED					0x82
#define PSF_EVENT_TIMER_PENDFUNCCALL_FROMISR_FAILED			0x83

/* We reserve 0x08 slots for this */
#define PSF_EVENT_USER_EVENT								0x90

/* We reserve 0x08 slots for this */
#define PSF_EVENT_USER_EVENT_FIXED							0x98

#define PSF_EVENT_TIMER_START								0xA0
#define PSF_EVENT_TIMER_RESET								0xA1
#define PSF_EVENT_TIMER_STOP								0xA2
#define PSF_EVENT_TIMER_CHANGEPERIOD						0xA3
#define PSF_EVENT_TIMER_START_FROMISR						0xA4
#define PSF_EVENT_TIMER_RESET_FROMISR						0xA5
#define PSF_EVENT_TIMER_STOP_FROMISR						0xA6
#define PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR				0xA7
#define PSF_EVENT_TIMER_START_FAILED						0xA8
#define PSF_EVENT_TIMER_RESET_FAILED						0xA9
#define PSF_EVENT_TIMER_STOP_FAILED							0xAA
#define PSF_EVENT_TIMER_CHANGEPERIOD_FAILED					0xAB
#define PSF_EVENT_TIMER_START_FROMISR_FAILED				0xAC
#define PSF_EVENT_TIMER_RESET_FROMISR_FAILED				0xAD
#define PSF_EVENT_TIMER_STOP_FROMISR_FAILED					0xAE
#define PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR_FAILED			0xAF

#define PSF_EVENT_EVENTGROUP_SYNC							0xB0
#define PSF_EVENT_EVENTGROUP_WAITBITS						0xB1
#define PSF_EVENT_EVENTGROUP_CLEARBITS						0xB2
#define PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR				0xB3
#define PSF_EVENT_EVENTGROUP_SETBITS						0xB4
#define PSF_EVENT_EVENTGROUP_SETBITS_FROMISR				0xB5
#define PSF_EVENT_EVENTGROUP_SYNC_BLOCK						0xB6
#define PSF_EVENT_EVENTGROUP_WAITBITS_BLOCK					0xB7
#define PSF_EVENT_EVENTGROUP_SYNC_FAILED					0xB8
#define PSF_EVENT_EVENTGROUP_WAITBITS_FAILED				0xB9

#define PSF_EVENT_QUEUE_SEND_FRONT							0xC0
#define PSF_EVENT_QUEUE_SEND_FRONT_FAILED					0xC1
#define PSF_EVENT_QUEUE_SEND_FRONT_BLOCK					0xC2
#define PSF_EVENT_QUEUE_SEND_FRONT_FROMISR					0xC3
#define PSF_EVENT_QUEUE_SEND_FRONT_FROMISR_FAILED			0xC4
#define PSF_EVENT_MUTEX_GIVE_RECURSIVE						0xC5
#define PSF_EVENT_MUTEX_GIVE_RECURSIVE_FAILED				0xC6
#define PSF_EVENT_MUTEX_TAKE_RECURSIVE						0xC7
#define PSF_EVENT_MUTEX_TAKE_RECURSIVE_FAILED				0xC8

#define PSF_EVENT_TASK_NOTIFY								0xC9
#define PSF_EVENT_TASK_NOTIFY_WAIT							0xCA
#define PSF_EVENT_TASK_NOTIFY_WAIT_BLOCK					0xCB
#define PSF_EVENT_TASK_NOTIFY_WAIT_FAILED					0xCC
#define PSF_EVENT_TASK_NOTIFY_FROM_ISR						0xCD

#define PSF_EVENT_TIMER_EXPIRED								0xD2

#define PSF_EVENT_STREAMBUFFER_SEND							0xD3
#define PSF_EVENT_STREAMBUFFER_SEND_BLOCK					0xD4
#define PSF_EVENT_STREAMBUFFER_SEND_FAILED					0xD5
#define PSF_EVENT_STREAMBUFFER_RECEIVE						0xD6
#define PSF_EVENT_STREAMBUFFER_RECEIVE_BLOCK				0xD7
#define PSF_EVENT_STREAMBUFFER_RECEIVE_FAILED				0xD8
#define PSF_EVENT_STREAMBUFFER_SEND_FROM_ISR				0xD9
#define PSF_EVENT_STREAMBUFFER_SEND_FROM_ISR_FAILED			0xDA
#define PSF_EVENT_STREAMBUFFER_RECEIVE_FROM_ISR				0xDB
#define PSF_EVENT_STREAMBUFFER_RECEIVE_FROM_ISR_FAILED		0xDC
#define PSF_EVENT_STREAMBUFFER_RESET						0xDD

#define PSF_EVENT_MESSAGEBUFFER_SEND						0xDE
#define PSF_EVENT_MESSAGEBUFFER_SEND_BLOCK					0xDF
#define PSF_EVENT_MESSAGEBUFFER_SEND_FAILED					0xE0
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE						0xE1
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE_BLOCK				0xE2
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE_FAILED				0xE3
#define PSF_EVENT_MESSAGEBUFFER_SEND_FROM_ISR				0xE4
#define PSF_EVENT_MESSAGEBUFFER_SEND_FROM_ISR_FAILED		0xE5
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE_FROM_ISR			0xE6
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE_FROM_ISR_FAILED		0xE7
#define PSF_EVENT_MESSAGEBUFFER_RESET						0xE8

#define PSF_EVENT_MALLOC_FAILED								0xE9
#define PSF_EVENT_FREE_FAILED								0xEA

#define PSF_EVENT_UNUSED_STACK								0xEB

#define PSF_EVENT_STATEMACHINE_STATE_CREATE					0xEC
#define PSF_EVENT_STATEMACHINE_CREATE						0xED
#define PSF_EVENT_STATEMACHINE_STATECHANGE					0xEE

#define PSF_EVENT_INTERVAL_CHANNEL_CREATE					0xEF
#define PSF_EVENT_INTERVAL_START							0xF0

#define PSF_EVENT_EXTENSION_CREATE							0xF1

#define PSF_EVENT_HEAP_CREATE								0xF2

#define PSF_EVENT_COUNTER_CREATE							0xF3
#define PSF_EVENT_COUNTER_CHANGE							0xF4
#define PSF_EVENT_COUNTER_LIMIT_EXCEEDED					0xF5

#define PSF_EVENT_MUTEX_TAKE_RECURSIVE_BLOCK				0xF6

#define PSF_EVENT_INTERVAL_STOP								0xF7
#define PSF_EVENT_INTERVAL_CHANNEL_SET_CREATE				0xF8

#define PSF_EVENT_RUNNABLE_REGISTER							0xF9
#define PSF_EVENT_RUNNABLE_START							0xFA
#define PSF_EVENT_RUNNABLE_STOP								0xFB

#define PSF_EVENT_DEPENDENCY_REGISTER						0xFC

#define TRC_EVENT_LAST_ID									(PSF_EVENT_DEPENDENCY_REGISTER)

/*** The trace macros for streaming ******************************************/

/* A macro that will update the tick count when returning from tickless idle */
#undef traceINCREASE_TICK_COUNT
/* Note: This can handle time adjustments of max 2^32 ticks, i.e., 35 seconds at 120 MHz. Thus, tick-less idle periods longer than 2^32 ticks will appear "compressed" on the time line.*/
#define traceINCREASE_TICK_COUNT( xCount ) { uint32_t uiTraceTickCount; (void)xTraceTimestampGetOsTickCount(&uiTraceTickCount); (void)xTraceTimestampSetOsTickCount(uiTraceTickCount + (xCount)); }

#if (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1)

#define OS_TICK_EVENT(uxSchedulerSuspended, xTickCount) if ((uxSchedulerSuspended) == (TraceUnsignedBaseType_t) pdFALSE) { prvTraceEventCreate1(PSF_EVENT_NEW_TIME, xTickCount); }

#else

#define OS_TICK_EVENT(uxSchedulerSuspended, xTickCount)

#endif

/* Called on each OS tick. Will call uiPortGetTimestamp to make sure it is called at least once every OS tick. */
#undef traceTASK_INCREMENT_TICK
#if TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_10_3_0

#define traceTASK_INCREMENT_TICK( xTickCount ) \
	if (uxSchedulerSuspended == ( TraceUnsignedBaseType_t ) pdTRUE || xPendedTicks == 0) { (void)xTraceTimestampSetOsTickCount((xTickCount) + 1); } \
	OS_TICK_EVENT(uxSchedulerSuspended, (xTickCount) + 1)

#elif TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_7_5_X

#define traceTASK_INCREMENT_TICK( xTickCount ) \
	if (uxSchedulerSuspended == ( TraceUnsignedBaseType_t ) pdTRUE || uxPendedTicks == 0) { (void)xTraceTimestampSetOsTickCount((xTickCount) + 1); } \
	OS_TICK_EVENT(uxSchedulerSuspended, (xTickCount) + 1)

#else

#define traceTASK_INCREMENT_TICK( xTickCount ) \
	if (uxSchedulerSuspended == ( TraceUnsignedBaseType_t ) pdTRUE || uxMissedTicks == 0) { (void)xTraceTimestampSetOsTickCount((xTickCount) + 1); } \
	OS_TICK_EVENT(uxSchedulerSuspended, (xTickCount) + 1)

#endif

/* Called before each task-switch */
#undef traceTASK_SWITCHED_OUT
#define traceTASK_SWITCHED_OUT() \
	(void)xTraceTaskMonitorSwitchOut(pxCurrentTCB)

/* Called on each task-switch */
#undef traceTASK_SWITCHED_IN
#define traceTASK_SWITCHED_IN() \
	(void)xTraceTaskSwitch(pxCurrentTCB, pxCurrentTCB->uxPriority)

/* Called for each task that becomes ready */
#undef traceMOVED_TASK_TO_READY_STATE
#define traceMOVED_TASK_TO_READY_STATE( pxTCB ) \
	(void)xTraceTaskReady(pxTCB);

#undef traceTASK_CREATE
#if TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_9_0_0

#define traceTASK_CREATE(pxNewTCB) \
	if ((pxNewTCB) != 0) \
	{ \
		(void)xTraceTaskRegisterWithoutHandle((void*)(pxNewTCB), (pxNewTCB)->pcTaskName, (pxNewTCB)->uxPriority); \
	}

#else

#define traceTASK_CREATE(pxNewTCB) \
	if (pxNewTCB != 0) \
	{ \
		(void)xTraceTaskRegisterWithoutHandle((void*)pxNewTCB, (const char*)pcName, (uint32_t)uxPriority); \
	}

#endif

/* Called in vTaskCreate, if it fails (typically if the stack can not be allocated) */
#undef traceTASK_CREATE_FAILED
#define traceTASK_CREATE_FAILED() \
	prvTraceEventCreate0(PSF_EVENT_TASK_CREATE_FAILED)

/* Called on vTaskDelete */
#undef traceTASK_DELETE				// We don't allow for filtering out "delete" events. They are important and not very frequent. Moreover, we can't exclude create events, so this should be symmetrical.
#define traceTASK_DELETE( pxTaskToDelete ) \
	(void)xTraceTaskUnregisterWithoutHandle(pxTaskToDelete, (pxTaskToDelete)->uxPriority)

#if (TRC_CFG_SCHEDULING_ONLY == 0)

#if (defined(configUSE_TICKLESS_IDLE) && configUSE_TICKLESS_IDLE != 0)

#undef traceLOW_POWER_IDLE_BEGIN
#define traceLOW_POWER_IDLE_BEGIN() \
	prvTraceEventCreate1(PSF_EVENT_LOWPOWER_BEGIN, xExpectedIdleTime)

#undef traceLOW_POWER_IDLE_END
#define traceLOW_POWER_IDLE_END() \
	prvTraceEventCreate0(PSF_EVENT_LOWPOWER_END)

#endif

/* Called on vTaskSuspend */
#undef traceTASK_SUSPEND
#define traceTASK_SUSPEND( pxTaskToSuspend ) \
	prvTraceEventCreate1(PSF_EVENT_TASK_SUSPEND, pxTaskToSuspend)

/* Called on vTaskDelay - note the use of FreeRTOS variable xTicksToDelay */
#undef traceTASK_DELAY
#define traceTASK_DELAY() \
	prvTraceEventCreate1(PSF_EVENT_TASK_DELAY, xTicksToDelay)

/* Called on vTaskDelayUntil - note the use of FreeRTOS variable xTimeToWake */
#undef traceTASK_DELAY_UNTIL
#if TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_9_0_0

#define traceTASK_DELAY_UNTIL(xTimeToWake) \
	prvTraceEventCreate1(PSF_EVENT_TASK_DELAY_UNTIL, (xTimeToWake))

#else

#define traceTASK_DELAY_UNTIL() \
	prvTraceEventCreate1(PSF_EVENT_TASK_DELAY_UNTIL, xTimeToWake)

#endif

#if defined(queueQUEUE_TYPE_SET) && (queueQUEUE_TYPE_BASE != queueQUEUE_TYPE_SET)
/* Queue and QueueSet are different, so we add a case for them */
#define traceQUEUE_SET_CASE_HELPER() case queueQUEUE_TYPE_SET:
#else
#define traceQUEUE_SET_CASE_HELPER()
#endif

#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_9_0_0)

#define traceQUEUE_CREATE_HELPER() \
		case queueQUEUE_TYPE_MUTEX: \
			(void)xTraceObjectRegisterWithoutHandle(PSF_EVENT_MUTEX_CREATE, (void*)pxNewQueue, "", 0); \
			break; \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			(void)xTraceObjectRegisterWithoutHandle(PSF_EVENT_MUTEX_RECURSIVE_CREATE, (void*)pxNewQueue, "", 0); \
			break;

#else

#define traceQUEUE_CREATE_HELPER()

#endif

/* Called in xQueueCreate, and thereby for all other object based on queues, such as semaphores. */
#undef traceQUEUE_CREATE
#define traceQUEUE_CREATE( pxNewQueue )\
	switch ((pxNewQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			(void)xTraceObjectRegisterWithoutHandle(PSF_EVENT_QUEUE_CREATE, (void*)(pxNewQueue), "", (uint32_t)uxQueueLength); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
			(void)xTraceObjectRegisterWithoutHandle(PSF_EVENT_SEMAPHORE_BINARY_CREATE, (void*)(pxNewQueue), "", 0); \
			break; \
		traceQUEUE_CREATE_HELPER() \
	}

#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_9_0_0)

#define traceQUEUE_CREATE_FAILED_HELPER() \
		case queueQUEUE_TYPE_MUTEX: \
			prvTraceEventCreate2(PSF_EVENT_MUTEX_CREATE_FAILED, 0, 0); \
			break; \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceEventCreate2(PSF_EVENT_MUTEX_RECURSIVE_CREATE_FAILED, 0, 0); \
			break;

#else

#define traceQUEUE_CREATE_FAILED_HELPER()

#endif

/* Called in xQueueCreate, if the queue creation fails */
#undef traceQUEUE_CREATE_FAILED
#define traceQUEUE_CREATE_FAILED( queueType ) \
	switch (queueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			prvTraceEventCreate2(PSF_EVENT_QUEUE_CREATE_FAILED, 0, uxQueueLength); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
			prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_BINARY_CREATE_FAILED, 0, 0); \
			break; \
		traceQUEUE_CREATE_FAILED_HELPER() \
	}

#undef traceQUEUE_DELETE			// We don't allow for filtering out "delete" events. They are important and not very frequent. Moreover, we can't exclude create events, so this should be symmetrical.
#define traceQUEUE_DELETE( pxQueue ) \
	switch ((pxQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			(void)xTraceObjectUnregisterWithoutHandle(PSF_EVENT_QUEUE_DELETE, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			(void)xTraceObjectUnregisterWithoutHandle(PSF_EVENT_MUTEX_DELETE, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
			(void)xTraceObjectUnregisterWithoutHandle(PSF_EVENT_SEMAPHORE_DELETE, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting); \
			break; \
	}

/* Called in xQueueCreateCountingSemaphore, if the queue creation fails */
#undef traceCREATE_COUNTING_SEMAPHORE
#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_8_X_X)

#define traceCREATE_COUNTING_SEMAPHORE() \
	(void)xTraceObjectRegisterWithoutHandle(PSF_EVENT_SEMAPHORE_COUNTING_CREATE, (void*)xHandle, "", (uint32_t)uxMaxCount)

#elif (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_7_5_X)

#define traceCREATE_COUNTING_SEMAPHORE() \
	(void)xTraceObjectRegisterWithoutHandle(PSF_EVENT_SEMAPHORE_COUNTING_CREATE, (void*)xHandle, "", uxInitialCount)

#elif (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_7_4_X)

#define traceCREATE_COUNTING_SEMAPHORE() \
	(void)xTraceObjectRegisterWithoutHandle(PSF_EVENT_SEMAPHORE_COUNTING_CREATE, (void*)xHandle, "", uxCountValue)

#else

#define traceCREATE_COUNTING_SEMAPHORE() \
	(void)xTraceObjectRegisterWithoutHandle(PSF_EVENT_SEMAPHORE_COUNTING_CREATE, (void*)pxHandle, "", uxCountValue)

#endif

#undef traceCREATE_COUNTING_SEMAPHORE_FAILED
#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_8_X_X)

#define traceCREATE_COUNTING_SEMAPHORE_FAILED() \
	prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_COUNTING_CREATE_FAILED, 0, uxMaxCount)

#elif (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_7_5_X)

#define traceCREATE_COUNTING_SEMAPHORE_FAILED() \
	prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_COUNTING_CREATE_FAILED, 0, uxInitialCount)

#elif (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_7_4_X)

#define traceCREATE_COUNTING_SEMAPHORE_FAILED() \
	prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_COUNTING_CREATE_FAILED, 0, uxCountValue)

#else

#define traceCREATE_COUNTING_SEMAPHORE_FAILED() \
	prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_COUNTING_CREATE_FAILED, 0, uxCountValue)

#endif


/* This macro is not necessary as of FreeRTOS v9.0.0 */
#if (TRC_CFG_FREERTOS_VERSION < TRC_FREERTOS_VERSION_9_0_0)

/* Called in xQueueCreateMutex, and thereby also from xSemaphoreCreateMutex and xSemaphoreCreateRecursiveMutex */
#undef traceCREATE_MUTEX
#define traceCREATE_MUTEX( pxNewQueue ) \
	switch (pxNewQueue->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_MUTEX: \
			(void)xTraceObjectRegisterWithoutHandle(PSF_EVENT_MUTEX_CREATE, (void*)(pxNewQueue), "", 0); \
			break; \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			(void)xTraceObjectRegisterWithoutHandle(PSF_EVENT_MUTEX_RECURSIVE_CREATE, (void*)(pxNewQueue), "", 0); \
			break; \
	}

/* Called in xQueueCreateMutex when the operation fails (when memory allocation fails) */
#undef traceCREATE_MUTEX_FAILED
#define traceCREATE_MUTEX_FAILED() \
	prvTraceEventCreate2(PSF_EVENT_MUTEX_CREATE_FAILED, 0, 0)
#endif /* (TRC_CFG_FREERTOS_VERSION < TRC_FREERTOS_VERSION_9_0_0) */

/* Called when the Mutex can not be given, since not holder */
#undef traceGIVE_MUTEX_RECURSIVE_FAILED
#define traceGIVE_MUTEX_RECURSIVE_FAILED( pxMutex ) \
	prvTraceEventCreate1(PSF_EVENT_MUTEX_GIVE_RECURSIVE_FAILED, (void*)(pxMutex))

/* Called when a message is sent to a queue */	/* CS IS NEW ! */
#undef traceQUEUE_SEND
#define traceQUEUE_SEND( pxQueue ) \
	switch ((pxQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			prvTraceEventCreate2(xCopyPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND : PSF_EVENT_QUEUE_SEND_FRONT, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting + 1); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_GIVE, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting + 1); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
			prvTraceEventCreate1(PSF_EVENT_MUTEX_GIVE, (void*)(pxQueue)); \
			break; \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceEventCreate1(PSF_EVENT_MUTEX_GIVE_RECURSIVE, (void*)(pxQueue)); \
			break; \
	}

#undef traceQUEUE_SET_SEND
#define traceQUEUE_SET_SEND( pxQueue ) \
	prvTraceEventCreate2(PSF_EVENT_QUEUE_SEND, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting + 1)

/* Called when a message failed to be sent to a queue (timeout) */
#undef traceQUEUE_SEND_FAILED
#define traceQUEUE_SEND_FAILED( pxQueue ) \
	switch ((pxQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			prvTraceEventCreate2(xCopyPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_FAILED : PSF_EVENT_QUEUE_SEND_FRONT_FAILED, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_GIVE_FAILED, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceEventCreate1(PSF_EVENT_MUTEX_GIVE_FAILED, (void*)(pxQueue)); \
			break; \
	}

/* Called when the task is blocked due to a send operation on a full queue */
#undef traceBLOCKING_ON_QUEUE_SEND
#define traceBLOCKING_ON_QUEUE_SEND( pxQueue ) \
	switch ((pxQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			prvTraceEventCreate2(xCopyPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_BLOCK : PSF_EVENT_QUEUE_SEND_FRONT_BLOCK, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_GIVE_BLOCK, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceEventCreate1(PSF_EVENT_MUTEX_GIVE_BLOCK, (void*)(pxQueue)); \
			break; \
	}

/* Called when a message is sent from interrupt context, e.g., using xQueueSendFromISR */
#undef traceQUEUE_SEND_FROM_ISR
#define traceQUEUE_SEND_FROM_ISR( pxQueue ) \
	switch ((pxQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			prvTraceEventCreate2(xCopyPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_FROMISR : PSF_EVENT_QUEUE_SEND_FRONT_FROMISR, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting + 1); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_GIVE_FROMISR, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting + 1); \
			break; \
	}

/* Called when a message send from interrupt context fails (since the queue was full) */
#undef traceQUEUE_SEND_FROM_ISR_FAILED
#define traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue ) \
	switch ((pxQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			prvTraceEventCreate2(xCopyPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_FROMISR_FAILED : PSF_EVENT_QUEUE_SEND_FRONT_FROMISR_FAILED, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_GIVE_FROMISR_FAILED, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting); \
			break; \
	}

/* Called when a message is received from a queue */
#undef traceQUEUE_RECEIVE
#define traceQUEUE_RECEIVE( pxQueue ) \
	switch ((pxQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			if (isQueueReceiveHookActuallyPeek) \
			{ \
				prvTraceEventCreate3(PSF_EVENT_QUEUE_PEEK, (void*)(pxQueue), xTicksToWait, (pxQueue)->uxMessagesWaiting - 1); \
			} \
			else\
			{ \
				prvTraceEventCreate3(PSF_EVENT_QUEUE_RECEIVE, (void*)(pxQueue), xTicksToWait, (pxQueue)->uxMessagesWaiting - 1); \
			} \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			if (isQueueReceiveHookActuallyPeek) \
			{ \
				prvTraceEventCreate3(PSF_EVENT_SEMAPHORE_PEEK, (void*)(pxQueue), xTicksToWait, (pxQueue)->uxMessagesWaiting - 1); \
			} \
			else \
			{ \
				prvTraceEventCreate3(PSF_EVENT_SEMAPHORE_TAKE, (void*)(pxQueue), xTicksToWait, (pxQueue)->uxMessagesWaiting - 1); \
			} \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
			prvTraceEventCreate2(isQueueReceiveHookActuallyPeek ? PSF_EVENT_MUTEX_PEEK : PSF_EVENT_MUTEX_TAKE, (void*)(pxQueue), xTicksToWait); \
			break; \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceEventCreate2(isQueueReceiveHookActuallyPeek ? PSF_EVENT_MUTEX_PEEK : PSF_EVENT_MUTEX_TAKE_RECURSIVE, (void*)(pxQueue), xTicksToWait); \
			break; \
	}

/* Called when a receive operation on a queue fails (timeout) */
#undef traceQUEUE_RECEIVE_FAILED
#define traceQUEUE_RECEIVE_FAILED( pxQueue ) \
	switch ((pxQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			prvTraceEventCreate3(isQueueReceiveHookActuallyPeek ? PSF_EVENT_QUEUE_PEEK_FAILED : PSF_EVENT_QUEUE_RECEIVE_FAILED, (void*)(pxQueue), xTicksToWait, (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceEventCreate3(isQueueReceiveHookActuallyPeek ? PSF_EVENT_SEMAPHORE_PEEK_FAILED : PSF_EVENT_SEMAPHORE_TAKE_FAILED, (void*)(pxQueue), xTicksToWait, (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
			prvTraceEventCreate2(isQueueReceiveHookActuallyPeek ? PSF_EVENT_MUTEX_PEEK_FAILED : PSF_EVENT_MUTEX_TAKE_FAILED, (void*)(pxQueue), xTicksToWait); \
			break; \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceEventCreate2(isQueueReceiveHookActuallyPeek ? PSF_EVENT_MUTEX_PEEK_FAILED : PSF_EVENT_MUTEX_TAKE_RECURSIVE_FAILED, (void*)(pxQueue), xTicksToWait); \
			break; \
	}

/* Called when the task is blocked due to a receive operation on an empty queue */
#undef traceBLOCKING_ON_QUEUE_RECEIVE
#define traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue ) \
	switch ((pxQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			prvTraceEventCreate3(isQueueReceiveHookActuallyPeek ? PSF_EVENT_QUEUE_PEEK_BLOCK : PSF_EVENT_QUEUE_RECEIVE_BLOCK, (void*)(pxQueue), xTicksToWait, (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceEventCreate3(isQueueReceiveHookActuallyPeek ? PSF_EVENT_SEMAPHORE_PEEK_BLOCK : PSF_EVENT_SEMAPHORE_TAKE_BLOCK, (void*)(pxQueue), xTicksToWait, (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
			prvTraceEventCreate2(isQueueReceiveHookActuallyPeek ? PSF_EVENT_MUTEX_PEEK_BLOCK : PSF_EVENT_MUTEX_TAKE_BLOCK, (void*)(pxQueue), xTicksToWait); \
			break; \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceEventCreate2(isQueueReceiveHookActuallyPeek ? PSF_EVENT_MUTEX_PEEK_BLOCK : PSF_EVENT_MUTEX_TAKE_RECURSIVE_BLOCK, (void*)(pxQueue), xTicksToWait); \
			break; \
	}

#if (TRC_CFG_FREERTOS_VERSION > TRC_FREERTOS_VERSION_9_0_1)

/* Called when a peek operation on a queue fails (timeout) */
#undef traceQUEUE_PEEK_FAILED
#define traceQUEUE_PEEK_FAILED( pxQueue ) \
	switch ((pxQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			prvTraceEventCreate3(PSF_EVENT_QUEUE_PEEK_FAILED, (void*)(pxQueue), xTicksToWait, (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceEventCreate3(PSF_EVENT_SEMAPHORE_PEEK_FAILED, (void*)(pxQueue), xTicksToWait, (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceEventCreate2(PSF_EVENT_MUTEX_PEEK_FAILED, (void*)(pxQueue), xTicksToWait); \
			break; \
	}

/* Called when the task is blocked due to a peek operation on an empty queue */
#undef traceBLOCKING_ON_QUEUE_PEEK
#define traceBLOCKING_ON_QUEUE_PEEK( pxQueue ) \
	switch ((pxQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			prvTraceEventCreate3(PSF_EVENT_QUEUE_PEEK_BLOCK, (void*)(pxQueue), xTicksToWait, (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceEventCreate3(PSF_EVENT_SEMAPHORE_PEEK_BLOCK, (void*)(pxQueue), xTicksToWait, (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceEventCreate2(PSF_EVENT_MUTEX_PEEK_BLOCK, (void*)(pxQueue), xTicksToWait); \
			break; \
	}

#endif

/* Called when a message is received in interrupt context, e.g., using xQueueReceiveFromISR */
#undef traceQUEUE_RECEIVE_FROM_ISR
#define traceQUEUE_RECEIVE_FROM_ISR( pxQueue ) \
	switch ((pxQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			prvTraceEventCreate2(PSF_EVENT_QUEUE_RECEIVE_FROMISR, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting - 1); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_TAKE_FROMISR, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting - 1); \
			break; \
	}

/* Called when a message receive from interrupt context fails (since the queue was empty) */
#undef traceQUEUE_RECEIVE_FROM_ISR_FAILED
#define traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue ) \
	switch ((pxQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			prvTraceEventCreate2(PSF_EVENT_QUEUE_RECEIVE_FROMISR_FAILED, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceEventCreate2(PSF_EVENT_SEMAPHORE_TAKE_FROMISR_FAILED, (void*)(pxQueue), (pxQueue)->uxMessagesWaiting); \
			break; \
	}

/* Called on xQueuePeek */
#undef traceQUEUE_PEEK
#define traceQUEUE_PEEK( pxQueue ) \
	switch ((pxQueue)->ucQueueType) \
	{ \
		case queueQUEUE_TYPE_BASE: \
		traceQUEUE_SET_CASE_HELPER() \
			prvTraceEventCreate3(PSF_EVENT_QUEUE_PEEK, (void*)(pxQueue), xTicksToWait, (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_BINARY_SEMAPHORE: \
		case queueQUEUE_TYPE_COUNTING_SEMAPHORE: \
			prvTraceEventCreate3(PSF_EVENT_SEMAPHORE_PEEK, (void*)(pxQueue), xTicksToWait, (pxQueue)->uxMessagesWaiting); \
			break; \
		case queueQUEUE_TYPE_MUTEX: \
		case queueQUEUE_TYPE_RECURSIVE_MUTEX: \
			prvTraceEventCreate2(PSF_EVENT_MUTEX_PEEK, (void*)(pxQueue), xTicksToWait); \
			break; \
	}

/* Called in vTaskPrioritySet */
#undef traceTASK_PRIORITY_SET
#define traceTASK_PRIORITY_SET( pxTask, uxNewPriority ) \
	(void)xTraceTaskSetPriorityWithoutHandle(pxTask, uxNewPriority)
	
/* Called in vTaskPriorityInherit, which is called by Mutex operations */
#undef traceTASK_PRIORITY_INHERIT
#define traceTASK_PRIORITY_INHERIT( pxTask, uxNewPriority ) \
	prvTraceEventCreate2(PSF_EVENT_TASK_PRIO_INHERIT, (void*)(pxTask), uxNewPriority)

/* Called in vTaskPriorityDisinherit, which is called by Mutex operations */
#undef traceTASK_PRIORITY_DISINHERIT
#define traceTASK_PRIORITY_DISINHERIT( pxTask, uxNewPriority ) \
	prvTraceEventCreate2(PSF_EVENT_TASK_PRIO_DISINHERIT, (void*)(pxTask), uxNewPriority)

/* Called in vTaskResume */
#undef traceTASK_RESUME
#define traceTASK_RESUME( pxTaskToResume ) \
	prvTraceEventCreate1(PSF_EVENT_TASK_RESUME, (void*)(pxTaskToResume))

/* Called in vTaskResumeFromISR */
#undef traceTASK_RESUME_FROM_ISR
#define traceTASK_RESUME_FROM_ISR( pxTaskToResume ) \
	prvTraceEventCreate1(PSF_EVENT_TASK_RESUME_FROMISR, (void*)(pxTaskToResume))

#if (TRC_CFG_INCLUDE_MEMMANG_EVENTS == 1)

#undef traceMALLOC
#define traceMALLOC( pvAddress, uiSize ) \
	if (xTraceIsRecorderEnabled()) \
	{ \
		(void)xTraceHeapAlloc(xTraceKernelPortGetSystemHeapHandle(), pvAddress, uiSize); \
	}

#undef traceFREE
#define traceFREE( pvAddress, uiSize ) \
	if (xTraceIsRecorderEnabled()) \
	{ \
		(void)xTraceHeapFree(xTraceKernelPortGetSystemHeapHandle(), pvAddress, uiSize); \
	}

#endif

#if (TRC_CFG_INCLUDE_TIMER_EVENTS == 1)

/* Called in timer.c - xTimerCreate */
#undef traceTIMER_CREATE
#define traceTIMER_CREATE(tmr) \
	(void)xTraceObjectRegisterWithoutHandle(PSF_EVENT_TIMER_CREATE, (void*)(tmr), (const char*)(tmr)->pcTimerName, (uint32_t)(tmr)->xTimerPeriodInTicks)

#undef traceTIMER_CREATE_FAILED
#define traceTIMER_CREATE_FAILED() \
	prvTraceEventCreate0(PSF_EVENT_TIMER_CREATE_FAILED);

#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_8_X_X)

#define traceTIMER_COMMAND_SEND_8_0_CASES(tmr) \
		case tmrCOMMAND_RESET: \
			prvTraceEventCreate2((xReturn == pdPASS) ? PSF_EVENT_TIMER_RESET : PSF_EVENT_TIMER_RESET_FAILED, (void*)(tmr), xOptionalValue); \
			break; \
		case tmrCOMMAND_START_FROM_ISR: \
			prvTraceEventCreate2((xReturn == pdPASS) ? PSF_EVENT_TIMER_START_FROMISR : PSF_EVENT_TIMER_START_FROMISR_FAILED, (void*)(tmr), xOptionalValue); \
			break; \
		case tmrCOMMAND_RESET_FROM_ISR: \
			prvTraceEventCreate2((xReturn == pdPASS) ? PSF_EVENT_TIMER_RESET_FROMISR : PSF_EVENT_TIMER_RESET_FROMISR_FAILED, (void*)(tmr), xOptionalValue); \
			break; \
		case tmrCOMMAND_STOP_FROM_ISR: \
			prvTraceEventCreate2((xReturn == pdPASS) ? PSF_EVENT_TIMER_STOP_FROMISR : PSF_EVENT_TIMER_STOP_FROMISR_FAILED, (void*)(tmr), xOptionalValue); \
			break; \
		case tmrCOMMAND_CHANGE_PERIOD_FROM_ISR: \
			prvTraceEventCreate2((xReturn == pdPASS) ? PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR : PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR_FAILED, (void*)(tmr), xOptionalValue); \
			break;
#else

#define traceTIMER_COMMAND_SEND_8_0_CASES(tmr)

#endif

/* Note that xCommandID can never be tmrCOMMAND_EXECUTE_CALLBACK (-1) since the trace macro is not called in that case */
#undef traceTIMER_COMMAND_SEND
#define traceTIMER_COMMAND_SEND(tmr, xCommandID, xOptionalValue, xReturn) \
	switch(xCommandID) \
	{ \
		case tmrCOMMAND_START: \
			prvTraceEventCreate1(((xReturn) == pdPASS) ? PSF_EVENT_TIMER_START : PSF_EVENT_TIMER_START_FAILED, (void*)(tmr)); \
			break; \
		case tmrCOMMAND_STOP: \
			prvTraceEventCreate1(((xReturn) == pdPASS) ? PSF_EVENT_TIMER_STOP : PSF_EVENT_TIMER_STOP_FAILED, (void*)(tmr)); \
			break; \
		case tmrCOMMAND_CHANGE_PERIOD: \
			prvTraceEventCreate2(((xReturn) == pdPASS) ? PSF_EVENT_TIMER_CHANGEPERIOD : PSF_EVENT_TIMER_CHANGEPERIOD_FAILED, (void*)(tmr), xOptionalValue); \
			break; \
		case tmrCOMMAND_DELETE: \
			(void)xTraceObjectUnregisterWithoutHandle(((xReturn) == pdPASS) ? PSF_EVENT_TIMER_DELETE : PSF_EVENT_TIMER_DELETE_FAILED, (void*)(tmr), 0); \
			break; \
		traceTIMER_COMMAND_SEND_8_0_CASES(tmr) \
	}

#undef traceTIMER_EXPIRED
#define traceTIMER_EXPIRED(tmr) \
	prvTraceEventCreate2(PSF_EVENT_TIMER_EXPIRED, (void*)(tmr), (TraceUnsignedBaseType_t)((tmr)->pxCallbackFunction))

#endif


#if (TRC_CFG_INCLUDE_PEND_FUNC_CALL_EVENTS == 1)

#undef tracePEND_FUNC_CALL
#define tracePEND_FUNC_CALL(func, arg1, arg2, ret) \
	prvTraceEventCreate1(((ret) == pdPASS) ? PSF_EVENT_TIMER_PENDFUNCCALL : PSF_EVENT_TIMER_PENDFUNCCALL_FAILED, (TraceUnsignedBaseType_t)(func))

#undef tracePEND_FUNC_CALL_FROM_ISR
#define tracePEND_FUNC_CALL_FROM_ISR(func, arg1, arg2, ret) \
	prvTraceEventCreate1(((ret) == pdPASS) ? PSF_EVENT_TIMER_PENDFUNCCALL_FROMISR : PSF_EVENT_TIMER_PENDFUNCCALL_FROMISR_FAILED, (TraceUnsignedBaseType_t)(func))

#endif

#if (TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS == 1)

#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_10_1_0)
#undef traceEVENT_GROUP_CREATE
#define traceEVENT_GROUP_CREATE(eg)  \
	(void)xTraceObjectRegisterWithoutHandle(PSF_EVENT_EVENTGROUP_CREATE, (void*)(eg), 0, (uint32_t)(eg)->uxEventBits)

#undef traceEVENT_GROUP_DELETE
#define traceEVENT_GROUP_DELETE(eg) \
	(void)xTraceObjectUnregisterWithoutHandle(PSF_EVENT_EVENTGROUP_DELETE, (void*)(eg), (uint32_t)(eg)->uxEventBits)
#else
#undef traceEVENT_GROUP_CREATE
#define traceEVENT_GROUP_CREATE(eg)  \
	(void)xTraceObjectRegisterWithoutHandle(PSF_EVENT_EVENTGROUP_CREATE, (void*)(eg), 0, ((EventGroup_t *)(eg))->uxEventBits)

#undef traceEVENT_GROUP_DELETE
#define traceEVENT_GROUP_DELETE(eg) \
	(void)xTraceObjectUnregisterWithoutHandle(PSF_EVENT_EVENTGROUP_DELETE, (void*)(eg), ((EventGroup_t *)(eg))->uxEventBits)
#endif

#undef traceEVENT_GROUP_CREATE_FAILED
#define traceEVENT_GROUP_CREATE_FAILED() \
	prvTraceEventCreate0(PSF_EVENT_EVENTGROUP_CREATE_FAILED)

#undef traceEVENT_GROUP_SYNC_BLOCK
#define traceEVENT_GROUP_SYNC_BLOCK(eg, bitsToSet, bitsToWaitFor) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_SYNC_BLOCK, (void*)(eg), bitsToWaitFor)

#undef traceEVENT_GROUP_SYNC_END
#define traceEVENT_GROUP_SYNC_END(eg, bitsToSet, bitsToWaitFor, wasTimeout) \
	prvTraceEventCreate2(((wasTimeout) != pdTRUE) ? PSF_EVENT_EVENTGROUP_SYNC : PSF_EVENT_EVENTGROUP_SYNC_FAILED, (void*)(eg), bitsToWaitFor)

#undef traceEVENT_GROUP_WAIT_BITS_BLOCK
#define traceEVENT_GROUP_WAIT_BITS_BLOCK(eg, bitsToWaitFor) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_WAITBITS_BLOCK, (void*)(eg), bitsToWaitFor)

#undef traceEVENT_GROUP_WAIT_BITS_END
#define traceEVENT_GROUP_WAIT_BITS_END(eg, bitsToWaitFor, wasTimeout) \
	prvTraceEventCreate2(((wasTimeout) != pdTRUE) ? PSF_EVENT_EVENTGROUP_WAITBITS : PSF_EVENT_EVENTGROUP_WAITBITS_FAILED, (void*)(eg), bitsToWaitFor)

#undef traceEVENT_GROUP_CLEAR_BITS
#define traceEVENT_GROUP_CLEAR_BITS(eg, bitsToClear) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_CLEARBITS, (void*)(eg), bitsToClear)

#undef traceEVENT_GROUP_CLEAR_BITS_FROM_ISR
#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR(eg, bitsToClear) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR, (void*)(eg), bitsToClear)

#undef traceEVENT_GROUP_SET_BITS
#define traceEVENT_GROUP_SET_BITS(eg, bitsToSet) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_SETBITS, (void*)(eg), bitsToSet)

#undef traceEVENT_GROUP_SET_BITS_FROM_ISR
#define traceEVENT_GROUP_SET_BITS_FROM_ISR(eg, bitsToSet) \
	prvTraceEventCreate2(PSF_EVENT_EVENTGROUP_SETBITS_FROMISR, (void*)(eg), bitsToSet)

#endif

#undef traceTASK_NOTIFY
#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_10_4_0)

#define traceTASK_NOTIFY(index) \
	prvTraceEventCreate1(PSF_EVENT_TASK_NOTIFY, (void*)xTaskToNotify)

#else

#define traceTASK_NOTIFY() \
	prvTraceEventCreate1(PSF_EVENT_TASK_NOTIFY, (void*)xTaskToNotify)

#endif

#undef traceTASK_NOTIFY_FROM_ISR
#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_10_4_0)

#define traceTASK_NOTIFY_FROM_ISR(index) \
	prvTraceEventCreate1(PSF_EVENT_TASK_NOTIFY_FROM_ISR, (void*)xTaskToNotify)

#else

#define traceTASK_NOTIFY_FROM_ISR() \
	prvTraceEventCreate1(PSF_EVENT_TASK_NOTIFY_FROM_ISR, (void*)xTaskToNotify)

#endif

/* NOTIFY and NOTIFY_GIVE will be handled identically */
#undef traceTASK_NOTIFY_GIVE_FROM_ISR
#define traceTASK_NOTIFY_GIVE_FROM_ISR traceTASK_NOTIFY_FROM_ISR

#undef traceTASK_NOTIFY_WAIT
#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_10_4_0)

#define traceTASK_NOTIFY_WAIT(index) \
	prvTraceEventCreate2(pxCurrentTCB->ucNotifyState[index] == taskNOTIFICATION_RECEIVED ? PSF_EVENT_TASK_NOTIFY_WAIT : PSF_EVENT_TASK_NOTIFY_WAIT_FAILED, (void*)pxCurrentTCB, xTicksToWait)

#elif (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_9_0_0)

#define traceTASK_NOTIFY_WAIT() \
	prvTraceEventCreate2(pxCurrentTCB->ucNotifyState == taskNOTIFICATION_RECEIVED ? PSF_EVENT_TASK_NOTIFY_WAIT : PSF_EVENT_TASK_NOTIFY_WAIT_FAILED, (void*)pxCurrentTCB, xTicksToWait)

#else

#define traceTASK_NOTIFY_WAIT() \
	prvTraceEventCreate2(pxCurrentTCB->eNotifyState == eNotified ? PSF_EVENT_TASK_NOTIFY_WAIT : PSF_EVENT_TASK_NOTIFY_WAIT_FAILED, (void*)pxCurrentTCB, xTicksToWait)

#endif

/* WAIT and TAKE will be handled identically */
#undef traceTASK_NOTIFY_TAKE
#define traceTASK_NOTIFY_TAKE traceTASK_NOTIFY_WAIT

#undef traceTASK_NOTIFY_WAIT_BLOCK
#if (TRC_CFG_FREERTOS_VERSION >= TRC_FREERTOS_VERSION_10_4_0)

#define traceTASK_NOTIFY_WAIT_BLOCK(index) \
	prvTraceEventCreate2(PSF_EVENT_TASK_NOTIFY_WAIT_BLOCK, (void*)pxCurrentTCB, xTicksToWait)

#else

#define traceTASK_NOTIFY_WAIT_BLOCK() \
	prvTraceEventCreate2(PSF_EVENT_TASK_NOTIFY_WAIT_BLOCK, (void*)pxCurrentTCB, xTicksToWait)

#endif

/* WAIT_BLOCK and TAKE_BLOCK will be handled identically */
#undef traceTASK_NOTIFY_TAKE_BLOCK
#define traceTASK_NOTIFY_TAKE_BLOCK traceTASK_NOTIFY_WAIT_BLOCK

#undef traceQUEUE_REGISTRY_ADD
#define traceQUEUE_REGISTRY_ADD(object, name) \
	(void)xTraceObjectSetNameWithoutHandle(object, (const char*)(name));

#if (TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS == 1)

#undef traceSTREAM_BUFFER_CREATE
#define traceSTREAM_BUFFER_CREATE( pxStreamBuffer, xIsMessageBuffer ) \
	(void)xTraceObjectRegisterWithoutHandle((xIsMessageBuffer) == 1 ? PSF_EVENT_MESSAGEBUFFER_CREATE : PSF_EVENT_STREAMBUFFER_CREATE, (void*)(pxStreamBuffer), "", (uint32_t)xBufferSizeBytes)

#undef traceSTREAM_BUFFER_CREATE_FAILED
#define traceSTREAM_BUFFER_CREATE_FAILED( xIsMessageBuffer ) \
	prvTraceEventCreate2((xIsMessageBuffer) == 1 ? PSF_EVENT_MESSAGEBUFFER_CREATE_FAILED : PSF_EVENT_STREAMBUFFER_CREATE_FAILED, 0 , xBufferSizeBytes)

#undef traceSTREAM_BUFFER_CREATE_STATIC_FAILED
#define traceSTREAM_BUFFER_CREATE_STATIC_FAILED( xReturn, xIsMessageBuffer ) \
	traceSTREAM_BUFFER_CREATE_FAILED( xIsMessageBuffer )

#undef traceSTREAM_BUFFER_DELETE
#define traceSTREAM_BUFFER_DELETE( xStreamBuffer ) \
	(void)xTraceObjectUnregisterWithoutHandle(prvGetStreamBufferType(xStreamBuffer) > 0 ? PSF_EVENT_MESSAGEBUFFER_DELETE : PSF_EVENT_STREAMBUFFER_DELETE, (void*)(xStreamBuffer), prvBytesInBuffer(xStreamBuffer));

#undef traceSTREAM_BUFFER_RESET
#define traceSTREAM_BUFFER_RESET( xStreamBuffer ) \
	prvTraceEventCreate2(prvGetStreamBufferType(xStreamBuffer) > 0 ? PSF_EVENT_MESSAGEBUFFER_RESET : PSF_EVENT_STREAMBUFFER_RESET, (void*)(xStreamBuffer), 0)

#undef traceSTREAM_BUFFER_SEND
#define traceSTREAM_BUFFER_SEND( xStreamBuffer, xReturn ) \
	prvTraceEventCreate2(prvGetStreamBufferType(xStreamBuffer) > 0 ? PSF_EVENT_MESSAGEBUFFER_SEND : PSF_EVENT_STREAMBUFFER_SEND, (void*)(xStreamBuffer), prvBytesInBuffer(xStreamBuffer))

#undef traceBLOCKING_ON_STREAM_BUFFER_SEND
#define traceBLOCKING_ON_STREAM_BUFFER_SEND( xStreamBuffer ) \
	prvTraceEventCreate1(prvGetStreamBufferType(xStreamBuffer) > 0 ? PSF_EVENT_MESSAGEBUFFER_SEND_BLOCK : PSF_EVENT_STREAMBUFFER_SEND_BLOCK, (void*)(xStreamBuffer))

#undef traceSTREAM_BUFFER_SEND_FAILED
#define traceSTREAM_BUFFER_SEND_FAILED( xStreamBuffer ) \
	prvTraceEventCreate1(prvGetStreamBufferType(xStreamBuffer) > 0 ? PSF_EVENT_MESSAGEBUFFER_SEND_FAILED : PSF_EVENT_STREAMBUFFER_SEND_FAILED, (void*)(xStreamBuffer))

#undef traceSTREAM_BUFFER_RECEIVE
#define traceSTREAM_BUFFER_RECEIVE( xStreamBuffer, xReceivedLength ) \
	prvTraceEventCreate2(prvGetStreamBufferType(xStreamBuffer) > 0 ? PSF_EVENT_MESSAGEBUFFER_RECEIVE: PSF_EVENT_STREAMBUFFER_RECEIVE, (void*)(xStreamBuffer), prvBytesInBuffer(xStreamBuffer))

#undef traceBLOCKING_ON_STREAM_BUFFER_RECEIVE
#define traceBLOCKING_ON_STREAM_BUFFER_RECEIVE( xStreamBuffer ) \
	prvTraceEventCreate1(prvGetStreamBufferType(xStreamBuffer) > 0 ? PSF_EVENT_MESSAGEBUFFER_RECEIVE_BLOCK: PSF_EVENT_STREAMBUFFER_RECEIVE_BLOCK, (void*)(xStreamBuffer))

#undef traceSTREAM_BUFFER_RECEIVE_FAILED
#define traceSTREAM_BUFFER_RECEIVE_FAILED( xStreamBuffer ) \
	prvTraceEventCreate1(prvGetStreamBufferType(xStreamBuffer) > 0 ? PSF_EVENT_MESSAGEBUFFER_RECEIVE_FAILED: PSF_EVENT_STREAMBUFFER_RECEIVE_FAILED, (void*)(xStreamBuffer))

#undef traceSTREAM_BUFFER_SEND_FROM_ISR
#define traceSTREAM_BUFFER_SEND_FROM_ISR( xStreamBuffer, xReturn ) \
	if ( (xReturn) > ( size_t ) 0 ) \
	{ \
		prvTraceEventCreate2(prvGetStreamBufferType(xStreamBuffer) > 0 ? PSF_EVENT_MESSAGEBUFFER_SEND_FROM_ISR : PSF_EVENT_STREAMBUFFER_SEND_FROM_ISR, (void*)(xStreamBuffer), prvBytesInBuffer(xStreamBuffer)); \
	} \
	else \
	{ \
		prvTraceEventCreate1(prvGetStreamBufferType(xStreamBuffer) > 0 ? PSF_EVENT_MESSAGEBUFFER_SEND_FROM_ISR_FAILED : PSF_EVENT_STREAMBUFFER_SEND_FROM_ISR_FAILED, (void*)(xStreamBuffer)); \
	}

#undef traceSTREAM_BUFFER_RECEIVE_FROM_ISR
#define traceSTREAM_BUFFER_RECEIVE_FROM_ISR( xStreamBuffer, xReceivedLength ) \
	if ( (xReceivedLength) > ( size_t ) 0 ) \
	{ \
		prvTraceEventCreate2(prvGetStreamBufferType(xStreamBuffer) > 0 ? PSF_EVENT_MESSAGEBUFFER_RECEIVE_FROM_ISR : PSF_EVENT_STREAMBUFFER_RECEIVE_FROM_ISR, (void*)(xStreamBuffer), prvBytesInBuffer(xStreamBuffer)); \
	} \
	else \
	{ \
		prvTraceEventCreate1(prvGetStreamBufferType(xStreamBuffer) > 0 ? PSF_EVENT_MESSAGEBUFFER_RECEIVE_FROM_ISR_FAILED : PSF_EVENT_STREAMBUFFER_RECEIVE_FROM_ISR_FAILED, (void*)(xStreamBuffer)); \
	}

#endif

#endif

#else
	
/* When recorder is disabled */
#define vTraceSetQueueName(object, name)
#define vTraceSetSemaphoreName(object, name)
#define vTraceSetMutexName(object, name)
#define vTraceSetEventGroupName(object, name)
#define vTraceSetStreamBufferName(object, name)
#define vTraceSetMessageBufferName(object, name)
	
#endif

#ifdef __cplusplus
}
#endif

#endif
