/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC1)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for tasks.
*/

#ifndef TRC_TASK_H
#define TRC_TASK_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TRC_CFG_ENABLE_STACK_MONITOR
#define TRC_CFG_ENABLE_STACK_MONITOR 0
#endif

typedef struct TraceTaskInfo
{
	void* coreTasks[TRC_CFG_CORE_COUNT];
} TraceTaskInfo_t;

extern TraceTaskInfo_t* pxTraceTaskInfo;

#define TRACE_TASK_INFO_BUFFER_SIZE (sizeof(TraceTaskInfo_t))

typedef struct TraceTaskInfoBuffer
{
	uint8_t buffer[TRACE_TASK_INFO_BUFFER_SIZE];
} TraceTaskInfoBuffer_t;

/*******************************************************************************
* xTraceTaskInitialize
*
* This function will initialize the local storage.
*
* @param pxBuffer The pointer to the buffer to use.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceTaskInitialize(TraceTaskInfoBuffer_t* pxBuffer);

/*******************************************************************************
* xTraceTaskRegister
*
* This function will register a task in the trace.
*
* @param pvTask The pointer to the task.
* @param uxPriority The task's priority.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
#define xTraceTaskRegister(pvTask, szName, uxPriority, pxTaskHandle) ((((pvTask) != 0) && (xTraceObjectRegister(PSF_EVENT_TASK_CREATE, pvTask, szName, uxPriority, (TraceObjectHandle_t*)(pxTaskHandle)) == TRC_SUCCESS)) ? (xTraceStackMonitorAdd(pvTask), TRC_SUCCESS) : TRC_FAIL)

/*******************************************************************************
* xTraceTaskUnregister
*
* This function will unregister an object in the trace.
*
* @param pvTask The pointer to the task.
* @param uxState The task's priority.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceTaskUnregister(TraceTaskHandle_t xTaskHandle, TraceUnsignedBaseType_t uxPriority);

/*******************************************************************************
* xTraceTaskSetName
*
* This function will set a task's name.
*
* @param pvTask The pointer to the task.
* @param szName The task name.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
#define xTraceTaskSetName xTraceObjectSetName

/*******************************************************************************
* xTraceTaskSetPriority
*
* This function will set the priority for a task.
*
* @param pvTask The pointer to the task.
* @param uxPriority The task's priority.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceTaskSetPriority(TraceTaskHandle_t xTaskHandle, TraceUnsignedBaseType_t uxPriority);

#define xTraceTaskRegisterWithoutHandle(pvTask, szName, uxPriority) ((((pvTask) != 0) && (xTraceObjectRegisterWithoutHandle(PSF_EVENT_TASK_CREATE, pvTask, szName, uxPriority) == TRC_SUCCESS)) ? (xTraceStackMonitorAdd(pvTask), TRC_SUCCESS) : TRC_FAIL)

#define xTraceTaskUnregisterWithoutHandle(pvTask, uxPriority) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(xTraceStackMonitorRemove(pvTask), xTraceObjectUnregisterWithoutHandle(PSF_EVENT_TASK_DELETE, pvTask, uxPriority))

#define xTraceTaskSetNameWithoutHandle xTraceObjectSetNameWithoutHandle

traceResult xTraceTaskSetPriorityWithoutHandle(void* pvTask, TraceUnsignedBaseType_t uxPriority);

/*******************************************************************************
* xTraceTaskSwitch
*
* This function will register a task switch and priority.
*
* @param pvTask The pointer to the task.
* @param uxPriority The task's priority.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceTaskSwitch(void* pvTask, TraceUnsignedBaseType_t uxPriority);

/*******************************************************************************
* xTraceTaskReady
*
* This function will set a task to ready in the trace.
*
* @param pvTask The pointer to the task.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
#if (TRC_CFG_INCLUDE_READY_EVENTS == 1)
traceResult xTraceTaskReady(void* pvTask);
#else
#define xTraceTaskReady(p) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2((void)p, TRC_SUCCESS)
#endif

#define xTraceTaskSetCurrent(pvTask) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(pxTraceTaskInfo->coreTasks[TRC_CFG_GET_CURRENT_CORE()] = (pvTask), TRC_SUCCESS)

#define xTraceTaskGetCurrent(ppvTask) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(ppvTask) = pxTraceTaskInfo->coreTasks[TRC_CFG_GET_CURRENT_CORE()], TRC_SUCCESS)

/*******************************************************************************
 * xTraceTaskInstanceFinishedNow
 *
 * Creates an event that ends the current task instance at this very instant.
 * This makes the viewer to splits the current fragment at this point and begin
 * a new actor instance, even if no task-switch has occurred.
 *****************************************************************************/
traceResult xTraceTaskInstanceFinishedNow(void);

/*******************************************************************************
 * xTraceTaskInstanceFinishedNext
 *
 * Marks the current "task instance" as finished on the next kernel call.
 *
 * If that kernel call is blocking, the instance ends after the blocking event
 * and the corresponding return event is then the start of the next instance.
 * If the kernel call is not blocking, the viewer instead splits the current
 * fragment right before the kernel call, which makes this call the first event
 * of the next instance.
 *****************************************************************************/
traceResult xTraceTaskInstanceFinishedNext(void);

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_TASK_H */
