/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC0)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The implementation for tasks.
*/

#include <trcRecorder.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

/* Code used for "task address" when no task has started, to indicate "(startup)".
 * This value was used since NULL/0 was already reserved for the idle task. */
#define TRACE_HANDLE_NO_TASK ((void*)2)

#define TRC_TASK_STATE_INDEX_PRIORITY		0
#define TRC_TASK_STATE_INDEX_UNUSED_STACK	1

TraceTaskInfo_t* pxTraceTaskInfo;

/*******************************************************************************
* xTraceTaskInitialize
*
* This function will initialize the local storage.
*
* @param pxBuffer The pointer to the buffer to use.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceTaskInitialize(TraceTaskInfoBuffer_t *pxBuffer)
{
	uint32_t i;

	TRC_ASSERT_EQUAL_SIZE(TraceTaskInfoBuffer_t, TraceTaskInfo_t);

	/* This should never fail */
	TRC_ASSERT(pxBuffer != 0);

	pxTraceTaskInfo = (TraceTaskInfo_t*)pxBuffer;

	for (i = 0; i < TRC_CFG_CORE_COUNT; i++)
	{
		pxTraceTaskInfo->coreTasks[i] = TRACE_HANDLE_NO_TASK;
	}

	xTraceSetComponentInitialized(TRC_RECORDER_COMPONENT_TASK);

	return TRC_SUCCESS;
}

/*******************************************************************************
* xTraceTaskUnregister
*
* This function will unregister an object in the trace.
*
* @param pvTask The pointer to the task.
* @param uxState The task's priority.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceTaskUnregister(TraceTaskHandle_t xTaskHandle, TraceUnsignedBaseType_t uxPriority)
{
	void* pvTask;
	
	/* This should never fail */
	TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntryGetAddress((TraceEntryHandle_t)xTaskHandle, &pvTask) == TRC_SUCCESS);
	
	xTraceStackMonitorRemove(pvTask);
	
	return xTraceObjectUnregister((TraceObjectHandle_t)xTaskHandle, PSF_EVENT_TASK_DELETE, uxPriority);
}

/*******************************************************************************
* xTraceTaskSetPriority
*
* This function will set the priority for a task.
*
* @param pvTask The pointer to the task.
* @param uxPriority The task's priority.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceTaskSetPriority(TraceTaskHandle_t xTaskHandle, TraceUnsignedBaseType_t uxPriority)
{
	TraceEventHandle_t xEventHandle = 0;
	void *pvTask;

	/* This should never fail */
	TRC_ASSERT_ALWAYS_EVALUATE(xTraceObjectSetState((TraceObjectHandle_t)xTaskHandle, uxPriority) == TRC_SUCCESS);
	
	/* This should never fail */
	TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntryGetAddress((TraceEntryHandle_t)xTaskHandle, &pvTask) == TRC_SUCCESS);

	/* We need to check this */
	if (xTraceEventBegin(PSF_EVENT_TASK_PRIORITY, sizeof(void*) + sizeof(uint32_t), &xEventHandle) == TRC_SUCCESS)
	{
		xTraceEventAddPointer(xEventHandle, pvTask);
		xTraceEventAdd32(xEventHandle, (uint32_t)uxPriority);
		xTraceEventEnd(xEventHandle);
	}
	
	return TRC_SUCCESS;
}

traceResult xTraceTaskSetPriorityWithoutHandle(void* pvTask, TraceUnsignedBaseType_t uxPriority)
{
	TraceEventHandle_t xEventHandle = 0;
	TraceEntryHandle_t xEntryHandle;
	
	if (xTraceEntryFind(pvTask, &xEntryHandle) == TRC_FAIL)
	{
		return TRC_FAIL;
	}

	/* This should never fail */
	TRC_ASSERT_ALWAYS_EVALUATE(xTraceObjectSetState((TraceObjectHandle_t)xEntryHandle, uxPriority) == TRC_SUCCESS);

	/* We need to check this */
	if (xTraceEventBegin(PSF_EVENT_TASK_PRIORITY, sizeof(void*) + sizeof(uint32_t), &xEventHandle) == TRC_SUCCESS)
	{
		xTraceEventAddPointer(xEventHandle, pvTask);
		xTraceEventAdd32(xEventHandle, (uint32_t)uxPriority);
		xTraceEventEnd(xEventHandle);
	}
	
	return TRC_SUCCESS;
}

/*******************************************************************************
* xTraceTaskSwitch
*
* This function will register a task switch and priority.
*
* @param pvTask The pointer to the task.
* @param uxPriority The task's priority.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceTaskSwitch(void *pvTask, TraceUnsignedBaseType_t uxPriority)
{
	traceResult xResult = TRC_FAIL;
	TraceEventHandle_t xEventHandle = 0;
	void* pvCurrent = 0;
	
	(void)pvTask;
	(void)uxPriority;

	TRACE_ALLOC_CRITICAL_SECTION();

	if (xTraceIsRecorderEnabled() == 0)
	{
		return xResult;
	}

	TRACE_ENTER_CRITICAL_SECTION();

	xTraceStateSet(TRC_STATE_IN_TASKSWITCH);

	xTraceTaskGetCurrent(&pvCurrent);
	if (pvCurrent != pvTask)
	{
		xTraceTaskSetCurrent(pvTask);
		
		if (xTraceEventBegin(PSF_EVENT_TASK_ACTIVATE, sizeof(void*) + sizeof(uint32_t), &xEventHandle) == TRC_SUCCESS)
		{
			xTraceEventAddPointer(xEventHandle, pvTask);
			xTraceEventAdd32(xEventHandle, (uint32_t)uxPriority);
			xTraceEventEnd(xEventHandle);
			xResult = TRC_SUCCESS;
		}
	}

	xTraceStateSet(TRC_STATE_IN_APPLICATION);

	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

/*******************************************************************************
* xTraceTaskReady
*
* This function will set a task to ready in the trace.
*
* @param pvTask The pointer to the task.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
#if (TRC_CFG_INCLUDE_READY_EVENTS == 1)
traceResult xTraceTaskReady(void *pvTask)
{
	traceResult xResult = TRC_FAIL;
	TraceEventHandle_t xEventHandle = 0;

	if (xTraceEventBegin(PSF_EVENT_TASK_READY, sizeof(void*), &xEventHandle) == TRC_SUCCESS)
	{
		xTraceEventAddPointer(xEventHandle, pvTask);
		xTraceEventEnd(xEventHandle);
		xResult = TRC_SUCCESS;
	}

	return xResult;
}
#endif /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */

/*******************************************************************************
 * xTraceTaskInstanceFinishedNow
 *
 * Creates an event that ends the current task instance at this very instant.
 * This makes the viewer to splits the current fragment at this point and begin
 * a new actor instance, even if no task-switch has occurred.
 *****************************************************************************/
traceResult xTraceTaskInstanceFinishedNow(void)
{
	TraceEventHandle_t xEventHandle = 0;

	if (xTraceEventBegin(PSF_EVENT_IFE_DIRECT, 0, &xEventHandle) == TRC_FAIL)
	{
		return TRC_FAIL;
	}

	xTraceEventEnd(xEventHandle);

	return TRC_SUCCESS;
}

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
traceResult xTraceTaskInstanceFinishedNext(void)
{
	TraceEventHandle_t xEventHandle = 0;

	if (xTraceEventBegin(PSF_EVENT_IFE_NEXT, 0, &xEventHandle) == TRC_FAIL)
	{
		return TRC_FAIL;
	}

	xTraceEventEnd(xEventHandle);

	return TRC_SUCCESS;
}

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */
