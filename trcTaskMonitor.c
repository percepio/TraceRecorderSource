/*
 * Percepio TraceRecorder for Tracealyzer v4.12.0
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The implementation for the task monitor.
 */

#include <trcRecorder.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1) && (TRC_CFG_ENABLE_TASK_MONITOR == 1) && (TRC_KERNEL_PORT_SUPPORTS_TLS == 1)

#include <string.h>   /* For memcpy */
#include <inttypes.h> /* For PRIu32, PRIu64 */

/* When the callback is called to report an a CPU load anomaly, it might 
   cause secondary CPU load anomalies that trigger the callback again at
   the next xTraceTaskMonitorPoll() call. Even if TaskMonitor is not monitoring
   its own thread, other threads might be starved during the callback and execute
   less than expected. This may cause an endless loop of callbacks. 
   
   The ignoreDirtyPeriod flag avoids this problem by making xTraceTaskMonitorPoll()
   ignore any anomalies at "dirty periods", i.e. just after callback was called.*/
volatile int iIgnoreDirtyPeriod = 0;

TraceTaskMonitorData_t* pxTraceTaskMonitorData TRC_CFG_RECORDER_DATA_ATTRIBUTE;

traceResult xTraceTaskMonitorInitialize(TraceTaskMonitorData_t *pxBuffer)
{
	TraceUnsignedBaseType_t i;

	/* This should never fail */
	TRC_ASSERT(pxBuffer != (void*)0);

	pxTraceTaskMonitorData = pxBuffer;

	/* Clear all buffer values */
	pxTraceTaskMonitorData->uiPollTimestamp = 0;

	for (i = 0; i < (TRC_ALIGN_FLOOR(TRC_CFG_CORE_COUNT, 2) + 1); i++)
	{
		pxTraceTaskMonitorData->uiLastTimestamp[i] = 0;
	}

	pxTraceTaskMonitorData->xCallback = (void*)0;

	for (i = 0; i < TRC_CFG_TASK_MONITOR_MAX_TASKS; i++)
	{
		pxTraceTaskMonitorData->xMonitoredTasks[i].xTaskHandle = 0;
		pxTraceTaskMonitorData->xMonitoredTasks[i].uxTotal = 0;
		pxTraceTaskMonitorData->xMonitoredTasks[i].uxLow = 0;
		pxTraceTaskMonitorData->xMonitoredTasks[i].uxHigh = 0;
		pxTraceTaskMonitorData->xMonitoredTasks[i].uxWatermarkLow = 100;
		pxTraceTaskMonitorData->xMonitoredTasks[i].uxWatermarkHigh = 0;
	}

	pxTraceTaskMonitorData->xCallbackData.pvTaskAddress = (void*)0;
	pxTraceTaskMonitorData->xCallbackData.acName[0] = '\0';
	pxTraceTaskMonitorData->xCallbackData.uxCPULoad = 0;
	pxTraceTaskMonitorData->xCallbackData.uxLowLimit = 0;
	pxTraceTaskMonitorData->xCallbackData.uxHighLimit = 0;
	pxTraceTaskMonitorData->xCallbackData.uxNumberOfFailedTasks = 0;

	(void)xTraceSetComponentInitialized(TRC_RECORDER_COMPONENT_TASK_MONITOR);

	return TRC_SUCCESS;
}

traceResult xTraceTaskMonitorSetCallback(TraceTaskMonitorCallback_t xCallback)
{
	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_TASK_MONITOR));

	if (xCallback == 0)
	{
		return TRC_FAIL;
	}

	pxTraceTaskMonitorData->xCallback = xCallback;
	
	return TRC_SUCCESS;
}

traceResult xTraceTaskMonitorRegister(void* pvTask, TraceUnsignedBaseType_t uxLow, TraceUnsignedBaseType_t uxHigh)
{
	TraceTaskMonitorTaskData_t* pxData;
	TraceTaskHandle_t xTaskHandle;
	TRACE_ALLOC_CRITICAL_SECTION();

	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_TASK_MONITOR));

	if (pvTask == (void*)0)
	{
		(void)xTraceTaskGetCurrent(&pvTask);
		
		if (pvTask == (void*)0)
		{
			return TRC_FAIL;
		}
	}

	TRACE_ENTER_CRITICAL_SECTION();

	if (xTraceTaskMonitorGetEmptySlot(&pxData) == TRC_FAIL)
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return TRC_FAIL;
	}

	if (xTraceTaskFind(pvTask, &xTaskHandle) == TRC_FAIL)
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return TRC_FAIL;
	}

	if (xTraceKernelPortSetTaskMonitorData(pvTask, (void*)pxData) == TRC_FAIL)
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return TRC_FAIL;
	}

	pxData->uxLow = uxLow;
	pxData->uxHigh = uxHigh;
	pxData->xTaskHandle = xTaskHandle;
	pxData->uxWatermarkLow = 100;
	pxData->uxWatermarkHigh = 0;
	
	TRACE_EXIT_CRITICAL_SECTION();

	return TRC_SUCCESS;
}

traceResult xTraceTaskMonitorUnregister(void* pvTask)
{
	TraceTaskMonitorTaskData_t* pxData = (void*)0;
	TraceUnsignedBaseType_t i;

	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_TASK_MONITOR));

	/* Unregister current if null */
	if (pvTask == (void*)0)
	{
		(void)xTraceTaskGetCurrent(&pvTask);
		
		if (pvTask == (void*)0)
		{
			return TRC_FAIL;
		}
	}

	/* Locate used slot.
	 * We can't use TLS here since we can't be certain we are in the correct thread's context. */
	for (i = 0; i < TRC_CFG_TASK_MONITOR_MAX_TASKS; i++)
	{
		if (pxTraceTaskMonitorData->xMonitoredTasks[i].xTaskHandle == 0)
		{
			continue;
		}

		if (pvTraceTaskGetAddressReturn(pxTraceTaskMonitorData->xMonitoredTasks[i].xTaskHandle) == pvTask)
		{
			pxData = &pxTraceTaskMonitorData->xMonitoredTasks[i];
			break;
		}
	}

	if (pxData == (void*)0)
	{
		/* Nothing matching this */
		return TRC_FAIL;
	}

	pxData->uxTotal = 0;
	pxData->uxLow = 0;
	pxData->uxHigh = 0;
	pxData->xTaskHandle = 0;
	pxData->uxWatermarkLow = 100;
	pxData->uxWatermarkHigh = 0;

	return TRC_SUCCESS;
}

traceResult xTraceTaskMonitorGetEmptySlot(TraceTaskMonitorTaskData_t** ppxData)
{
	TraceUnsignedBaseType_t i;

	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_TASK_MONITOR));

	TRC_ASSERT(pxTraceTaskMonitorData != (void*)0);

	if (ppxData == (TraceTaskMonitorTaskData_t**)0)
	{
		return TRC_FAIL;
	}

	for (i = 0; i < TRC_CFG_TASK_MONITOR_MAX_TASKS; i++)
	{
		if (pxTraceTaskMonitorData->xMonitoredTasks[i].xTaskHandle != 0)
		{
			continue;
		}

		*ppxData = &pxTraceTaskMonitorData->xMonitoredTasks[i];

		return TRC_SUCCESS;
	}

	return TRC_FAIL;
}

traceResult xTraceTaskMonitorSwitchOut(void* pvTask)
{
	TraceTaskMonitorTaskData_t* pxData = (void*)0;
	uint32_t uiLastTimestamp = 0;
	uint32_t uiTimestampDiff = 0;

	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_TASK_MONITOR));

	TRC_ASSERT(xTraceTaskGetCurrentReturn() == pvTask);

	/* This shouldn't fail */
	(void)xTraceTimestampGet(&uiLastTimestamp);

	/* We calculate this early since we will overwrite pxTraceTaskMonitorData->uiLastTimestamp[...] next */
	uiTimestampDiff = uiLastTimestamp - pxTraceTaskMonitorData->uiLastTimestamp[TRC_CFG_GET_CURRENT_CORE()];

	/* We set this before any return */
	pxTraceTaskMonitorData->uiLastTimestamp[TRC_CFG_GET_CURRENT_CORE()] = uiLastTimestamp;

	if (pvTask == (void*)0)
	{
		return TRC_FAIL;
	}

	/* Get TaskMonitor TLS */
	if (xTraceKernelPortGetTaskMonitorData(pvTask, (void**)&pxData) == TRC_FAIL)
	{
		/* No TLS */
		return TRC_FAIL;
	}

	/* This task is monitored or was monitored at some point */
	if (pxData->xTaskHandle == 0)
	{
		/* Not an actively monitored task */
		return TRC_FAIL;
	}

	/* An actively monitored task */
	pxData->uxTotal += uiTimestampDiff;

	return TRC_SUCCESS;
}

traceResult xTraceTaskMonitorPoll(void)
{
	TraceUnsignedBaseType_t i, j;
	TraceUnsignedBaseType_t uxCPULoad;
	uint32_t uiLastTimestamp;
	uint32_t uiElapsedTime;
	const char* szName;

	TRACE_ALLOC_CRITICAL_SECTION();

	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_TASK_MONITOR));

	TRC_ASSERT(pxTraceTaskMonitorData != (void*)0);

	if (pxTraceTaskMonitorData->xCallback == 0)
	{
		/* No callback set yet */
		return TRC_FAIL;
	}

	/* Only check for CPU load anomalies on "clean" periods, where the callback
	  has not been called (due to previous anomalies). */    
	if (iIgnoreDirtyPeriod)
	{
		(void)xTraceTaskMonitorPollReset();

		return TRC_SUCCESS;
	}

	(void)xTraceTimestampGet(&uiLastTimestamp);

	uiElapsedTime = uiLastTimestamp - pxTraceTaskMonitorData->uiPollTimestamp;
	if (uiElapsedTime == 0U)
	{
		/* Polling too fast */
		return TRC_FAIL;
	}

	pxTraceTaskMonitorData->xCallbackData.uxNumberOfFailedTasks = 0;
	pxTraceTaskMonitorData->xCallbackData.pvTaskAddress = (void*)0;
	
	TRACE_ENTER_CRITICAL_SECTION();
	for (i = 0; i < TRC_CFG_TASK_MONITOR_MAX_TASKS; i++)
	{
		if (pxTraceTaskMonitorData->xMonitoredTasks[i].xTaskHandle == 0)
		{
			continue;
		}

		/* Since it is not guaranteed that we can get TLS data when the task isn't
		 * the one running we have to iterate over all monitored tasks and then
		 * see if it is currently running on any core in order to update uxTotal. */
		for (j = 0; j < TRC_CFG_CORE_COUNT; j++)
		{
			if (pvTraceTaskGetAddressReturn(pxTraceTaskMonitorData->xMonitoredTasks[i].xTaskHandle) == xTraceTaskGetCurrentOnCoreReturn(j))
			{
				/* This task is monitored and is currently running on this core so
				 * we update uxTotal with the time that has passed since the
				 * last xTraceTaskMonitorSwitchOut() on this core */
				pxTraceTaskMonitorData->xMonitoredTasks[i].uxTotal += uiLastTimestamp - pxTraceTaskMonitorData->uiLastTimestamp[j];
				break;
			}
		}

		uxCPULoad = (pxTraceTaskMonitorData->xMonitoredTasks[i].uxTotal * 100UL) / uiElapsedTime;
		pxTraceTaskMonitorData->xMonitoredTasks[i].uxTotal = 0;

		if ((uxCPULoad >= pxTraceTaskMonitorData->xMonitoredTasks[i].uxWatermarkLow) && (uxCPULoad <= pxTraceTaskMonitorData->xMonitoredTasks[i].uxWatermarkHigh))
		{
			/* This task is not worse than current watermarks */
			continue;
		}

		/* High watermark or low watermark has a new extreme */
		if (uxCPULoad > pxTraceTaskMonitorData->xMonitoredTasks[i].uxWatermarkHigh)
		{
			/* Always keep track of high watermark, even if within the expected range. */
			pxTraceTaskMonitorData->xMonitoredTasks[i].uxWatermarkHigh = uxCPULoad;
		}

		if (uxCPULoad < pxTraceTaskMonitorData->xMonitoredTasks[i].uxWatermarkLow)
		{
			/* Always keep track of low watermark, even if within the expected range. */
			pxTraceTaskMonitorData->xMonitoredTasks[i].uxWatermarkLow = uxCPULoad;
		}

		if ((uxCPULoad >= pxTraceTaskMonitorData->xMonitoredTasks[i].uxLow) && (uxCPULoad <= pxTraceTaskMonitorData->xMonitoredTasks[i].uxHigh))
		{
			/* This task is within the expected range, continue to next task */
			continue;
		}
		
		pxTraceTaskMonitorData->xCallbackData.uxNumberOfFailedTasks++;

		/* CPU load is out of acceptable range */
		if (pxTraceTaskMonitorData->xCallbackData.uxNumberOfFailedTasks > 1)
		{
			/* We only store data for the first failing task */
			continue;
		}

		/* Store the failed task's callback data. This data will be used after critical section has ended to call the callback. */
		pxTraceTaskMonitorData->xCallbackData.uxCPULoad = uxCPULoad;
		(void)xTraceTaskGetAddress(pxTraceTaskMonitorData->xMonitoredTasks[i].xTaskHandle, &pxTraceTaskMonitorData->xCallbackData.pvTaskAddress);
		(void)xTraceTaskGetName(pxTraceTaskMonitorData->xMonitoredTasks[i].xTaskHandle, &szName);
		for (j = 0; j < TRC_ENTRY_TABLE_SLOT_SYMBOL_SIZE; j++)
		{
			pxTraceTaskMonitorData->xCallbackData.acName[j] = szName[j];
			if (szName[j] == 0)
				break;
		}
		pxTraceTaskMonitorData->xCallbackData.uxLowLimit = pxTraceTaskMonitorData->xMonitoredTasks[i].uxLow;
		pxTraceTaskMonitorData->xCallbackData.uxHighLimit = pxTraceTaskMonitorData->xMonitoredTasks[i].uxHigh;
	}

	for (j = 0; j < TRC_CFG_CORE_COUNT; j++)
	{
		pxTraceTaskMonitorData->uiLastTimestamp[j] = uiLastTimestamp;
	}

	pxTraceTaskMonitorData->uiPollTimestamp = uiLastTimestamp;
	TRACE_EXIT_CRITICAL_SECTION();

	/* Check if callback should be performed */
	if (pxTraceTaskMonitorData->xCallbackData.uxNumberOfFailedTasks > 0)
	{		
		pxTraceTaskMonitorData->xCallback(&pxTraceTaskMonitorData->xCallbackData);		
		
		/* Make the next xTraceTaskMonitorPoll() ignore this "dirty period",
		since the callback might have affected the CPU load. 
		*/
		iIgnoreDirtyPeriod = 1; 
	}
	
	return TRC_SUCCESS;
}

traceResult xTraceTaskMonitorPollReset(void)
{
	TraceUnsignedBaseType_t i;
	uint32_t uiLastTimestamp = 0;
	TRACE_ALLOC_CRITICAL_SECTION();

	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_TASK_MONITOR));

	/* This shouldn't fail */
	(void)xTraceTimestampGet(&uiLastTimestamp);

	TRACE_ENTER_CRITICAL_SECTION();
	/* No longer a dirty period */
	iIgnoreDirtyPeriod = 0;
	
	for (i = 0; i < TRC_CFG_TASK_MONITOR_MAX_TASKS; i++)
	{
		pxTraceTaskMonitorData->xMonitoredTasks[i].uxTotal = 0;
	}

	for (i = 0; i < TRC_CFG_CORE_COUNT; i++)
	{
		pxTraceTaskMonitorData->uiLastTimestamp[i] = uiLastTimestamp;
	}
	pxTraceTaskMonitorData->uiPollTimestamp = uiLastTimestamp;
	TRACE_EXIT_CRITICAL_SECTION();

	return TRC_SUCCESS;
}

traceResult xTraceTaskMonitorPrint(void)
{
	TraceUnsignedBaseType_t i;
	TraceTaskMonitorTaskData_t xTaskData;
	const char *szName;

	TRACE_ALLOC_CRITICAL_SECTION();

	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_TASK_MONITOR));

	TRC_CFG_PRINTF("%-5s%-20s%-8s%-8s\n", "Slot", "Task name", "Low (%)", "High (%)");
	for (i = 0; i < TRC_CFG_TASK_MONITOR_MAX_TASKS; i++)
	{
		if (pxTraceTaskMonitorData->xMonitoredTasks[i].xTaskHandle == 0)
		{
			continue;
		}

		// Make a copy of the task data within a critical section, to ensure all fields are consistent.
		TRACE_ENTER_CRITICAL_SECTION();
		memcpy(&xTaskData, &pxTraceTaskMonitorData->xMonitoredTasks[i], sizeof(TraceTaskMonitorTaskData_t));
		TRACE_EXIT_CRITICAL_SECTION();

		(void)xTraceTaskGetName(xTaskData.xTaskHandle, &szName);
        TRC_CFG_PRINTF("%-5"PRIu32"%-20s%-8"PRIu32"%-8"PRIu32"\n", (uint32_t)i, szName, (uint32_t)xTaskData.uxWatermarkLow, (uint32_t)xTaskData.uxWatermarkHigh);
	}
	TRC_CFG_PRINTF("\n");

	return TRC_SUCCESS;
}

#endif
