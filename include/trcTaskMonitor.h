/*
* Percepio Trace Recorder for Tracealyzer v4.11.0
* Copyright 2025 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * @file 
 * 
 * @brief Public trace task monitor APIs.
 */

#ifndef TRC_TASK_MONITOR_H
#define TRC_TASK_MONITOR_H

#ifndef TRC_CFG_ENABLE_TASK_MONITOR
#define TRC_CFG_ENABLE_TASK_MONITOR 0
#endif

#ifndef TRC_KERNEL_PORT_SUPPORTS_TLS
#define TRC_KERNEL_PORT_SUPPORTS_TLS 0
#endif

typedef struct TraceTaskMonitorCallbackData
{
	void* pvTaskAddress;
	char acName[TRC_ENTRY_TABLE_SLOT_SYMBOL_SIZE];
	TraceUnsignedBaseType_t uxCPULoad;
	TraceUnsignedBaseType_t uxLowLimit;
	TraceUnsignedBaseType_t uxHighLimit;
	TraceUnsignedBaseType_t uxNumberOfFailedTasks;
} TraceTaskMonitorCallbackData_t;

typedef void (*TraceTaskMonitorCallback_t)(TraceTaskMonitorCallbackData_t *pxData);

#if (TRC_USE_TRACEALYZER_RECORDER == 1) && (TRC_CFG_ENABLE_TASK_MONITOR == 1) && (TRC_KERNEL_PORT_SUPPORTS_TLS == 1)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup trace_task_monitor_apis Trace Task Monitor APIs
 * @ingroup trace_recorder_apis
 * @{
 */

/**
 * @internal Trace Task Monitor Data Instance Structure
 */
typedef struct TraceTaskMonitorTaskData
{
	TraceTaskHandle_t xTaskHandle;
	TraceUnsignedBaseType_t uxTotal;
	TraceUnsignedBaseType_t uxLow;
	TraceUnsignedBaseType_t uxHigh;
    TraceUnsignedBaseType_t uxWatermarkLow;
    TraceUnsignedBaseType_t uxWatermarkHigh;
} TraceTaskMonitorTaskData_t;

/**
 * @internal Trace Task Monitor Data Structure
 */
typedef struct TraceTaskMonitorData	/* Aligned */
{
	uint32_t uiPollTimestamp;
	uint32_t uiLastTimestamp[TRC_ALIGN_FLOOR(TRC_CFG_CORE_COUNT, 2) + 1];	/* Will FLOOR the core count to a multiple of 2, then add 1 to ensure aligned struct with uiPollTimestamp */
	TraceTaskMonitorCallback_t xCallback;
	TraceTaskMonitorTaskData_t xMonitoredTasks[TRC_CFG_TASK_MONITOR_MAX_TASKS];
	TraceTaskMonitorCallbackData_t xCallbackData; /* Data that will be used for callback */
} TraceTaskMonitorData_t;

/**
 * @internal Initialize trace task monitor system.
 * 
 * @param[in] pxBuffer Pointer to memory that will be used by the
 * trace task monitor system.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceTaskMonitorInitialize(TraceTaskMonitorData_t* pxBuffer);

/**
 * @brief Set a callback function to be called when a task's CPU load is outside the accepted range.
 * The function should accept a TraceTaskMonitorCallbackData_t* parameter.
 * The number of failing tasks can be accessed through pxData->uxNumberOfFailedTasks, though only
 * the information for the first failing task will be stored in pxData.
 * The task's address can be retrieved using pxData->pvTaskAddress.
 * The name is in pxData->acName. Note that the name may not be null terminated if it uses the entire buffer!
 * The task's CPU load, low limit, and high limit can be accessed through
 * pxData->uxCPULoad, pxData->uxLowLimit, and pxData->uxHighLimit.
 * 
 * Callback example:
 * void my_callback(TraceTaskMonitorCallbackData_t* pxData);
 * ...
 * xTraceTaskMonitorSetCallback(my_callback);
 * 
 * @param[in] xCallback Callback function.
 */
traceResult xTraceTaskMonitorSetCallback(TraceTaskMonitorCallback_t xCallback);

/**
 * @brief Registers the task to the trace task monitor.
 * 
 * @param[in] pvTask Task. If NULL, the currently executing task is registered.
 * @param[in] uxLow Minimum accepted percentage CPU load.
 * @param[in] uxHigh Maximum accepted percentage CPU load.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceTaskMonitorRegister(void* pvTask, TraceUnsignedBaseType_t uxLow, TraceUnsignedBaseType_t uxHigh);

/**
 * @brief Unregister task from the trace task monitor.
 * 
 * @param[in] pvTask Task.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceTaskMonitorUnregister(void* pvTask);

/**
 * @internal Get an empty slot for writing.
 * 
 * @param[out] ppxData Pointer where task's data pointer will be stored.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceTaskMonitorGetEmptySlot(TraceTaskMonitorTaskData_t** ppxData);

/**
 * @brief Update a task's load. Should be called when task is switched out.
 * 
 * @param[in] pvTask Task.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceTaskMonitorSwitchOut(void* pvTask);

/**
 * @brief Call this regularly to poll the system and check if any tasks are
 * outside the accepted range.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceTaskMonitorPoll(void);

/**
 * @brief This will reset all timestamps to start a new poll interval.
 * Only need to call this if xTraceTaskMonitorPoll() hasn't been called regularly.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceTaskMonitorPollReset(void);

/**
 * @brief Prints the high and low watermark for the CPU load of each monitored task.
 * 
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceTaskMonitorPrint(void);

/** @} */

#ifdef __cplusplus
}
#endif

#else

typedef struct TraceTaskMonitorData	/* Aligned */
{
	TraceUnsignedBaseType_t dummy;
} TraceTaskMonitorData_t;

/* Empty defines */
#define xTraceTaskMonitorInitialize(_pxBuffer) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2((void)(_pxBuffer), TRC_SUCCESS)
#define xTraceTaskMonitorSetCallback(_xCallback) (TRC_FAIL)
#define xTraceTaskMonitorRegister(_pvTask, _uxLow, _uxHigh) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_4((void)(_pvTask), (void)(_uxLow), (void)(_uxHigh), TRC_FAIL)
#define xTraceTaskMonitorUnregister(_pvTask) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2((void)(_pvTask), TRC_FAIL)
#define xTraceTaskMonitorGetData(_pvTask, _ppxData) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_3((void)(_pvTask), (void)(_ppxData), TRC_FAIL)
#define xTraceTaskMonitorSwitchOut(_pvTask) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2((void)(_pvTask), TRC_FAIL)
#define xTraceTaskMonitorPoll() (TRC_FAIL)
#define xTraceTaskMonitorPrint() (TRC_FAIL)

#endif

#endif
