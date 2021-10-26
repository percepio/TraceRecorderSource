/*
 * Percepio Trace Recorder SDK for Tracealyzer v4.5.2
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Percepio Tracealyzer Recorder SDK
 */

#include <stdio.h>
#include "trcSDK.h"

extern uint16_t CurrentFilterMask;

extern uint16_t CurrentFilterGroup;

TRACE_ALLOC_CRITICAL_SECTION();

void vTraceSDKSetObjectFilter();

/*******************************************************************************
* xTraceSDKRegisterObject
*
* This function will register an object in the trace.
*
* @param uiEventCode The event code.
* @param pxObject The pointer to the object.
* @param uiData The object's initial data.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKRegisterObject(uint32_t uiEventCode, void* pxObject, uint32_t uiData)
{
	traceResult xResult = TRACE_FAIL;

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
	prvTraceSaveObjectData(pxObject, uiData); \
	if (prvTraceBeginStoreEvent(uiEventCode, sizeof(uint32_t) + sizeof(uint32_t)) == TRACE_SUCCESS)
	{
		prvTraceStoreEventPayload32((uint32_t)pxObject);
		prvTraceStoreEventPayload32(uiData);
		prvTraceEndStoreEvent();
		xResult = TRACE_SUCCESS;
	}
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
	(void)uiEventCode;
	(void)pxObject;
	(void)uiData;
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

	return xResult;
}

/*******************************************************************************
* xTraceSDKUnregisterObject
*
* This function will unregister an object in the trace.
*
* @param uiEventCode The event code.
* @param pxObject The pointer to the object.
* @param uiData The object's end data.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKUnregisterObject(uint32_t uiEventCode, void* pxObject, uint32_t uiData)
{
	traceResult xResult = TRACE_FAIL;

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
	if (prvTraceBeginStoreEvent(uiEventCode, sizeof(uint32_t) + sizeof(uint32_t)) == TRACE_SUCCESS)
	{
		prvTraceStoreEventPayload32((uint32_t)pxObject);
		prvTraceStoreEventPayload32(uiData);
		prvTraceEndStoreEvent();
		xResult = TRACE_SUCCESS;
	}
	/* Only remove the symbol entry */
	prvTraceDeleteSymbol(pxObject);
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
	(void)uiEventCode;
	(void)pxObject;
	(void)uiData;
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

	return xResult;
}

/*******************************************************************************
* xTraceSDKSetObjectName
*
* This function will set an object's name.
*
* @param pxObject The pointer to the object.
* @param pszName The task name.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKSetObjectName(void* pxObject, const char* pszName)
{
	traceResult xResult = TRACE_FAIL;

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
	prvTraceSaveObjectSymbol(pxObject, (const char*)pszName);
	prvTraceStoreStringEvent(1, PSF_EVENT_OBJ_NAME, (const char*)pszName, (uint32_t)pxObject);
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
	(void)pxObject;
	(void)pszName;
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

	return xResult;
}

/*******************************************************************************
* xTraceSDKSetObjectData
*
* This function will register a task in the trace.
*
* @param pxObject The pointer to the object.
* @param uiData The object's data.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKSetObjectData(void* pxObject, uint32_t uiData)
{
	traceResult xResult = TRACE_FAIL;

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
	prvTraceSaveObjectData(pxObject, uiData);
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
	(void)pxObject;
	(void)uiData;
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

	return xResult;
}

/*******************************************************************************
* xTraceSDKTaskSwitch
*
* This function will register a task switch and priority.
*
* @param pxTCB The pointer to the TCB.
* @param uiPriority The task priority.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKTaskSwitch(void* pxTCB, uint32_t uiPriority)
{
	traceResult xResult = TRACE_FAIL;

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
	extern volatile uint32_t uiTraceSystemState;
	uiTraceSystemState = TRC_STATE_IN_TASKSWITCH;
	if (prvTraceGetCurrentTask() != (uint32_t)pxTCB)
	{
		prvTraceSetCurrentTask((uint32_t)pxTCB);
		if (prvTraceBeginStoreEvent(PSF_EVENT_TASK_ACTIVATE, sizeof(uint32_t)) == TRACE_SUCCESS)
		{
			prvTraceStoreEventPayload32((uint32_t)pxTCB);
			prvTraceStoreEventPayload32(uiPriority);
			prvTraceEndStoreEvent();
			xResult = TRACE_SUCCESS;
		}
	}
	uiTraceSystemState = TRC_STATE_IN_APPLICATION;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
	(void)pxTCB;
	(void)uiPriority;
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

	return xResult;
}

/*******************************************************************************
* xTraceSDKTaskReady
*
* This function will set a task to ready in the trace.
*
* @param pxTCB The pointer to the TCB.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKTaskReady(void* pxTCB)
{
	traceResult xResult = TRACE_FAIL;
	
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
	if (prvTraceBeginStoreEvent(PSF_EVENT_TASK_READY, sizeof(uint32_t)) == TRACE_SUCCESS)
	{
		prvTraceStoreEventPayload32((uint32_t)pxTCB);
		prvTraceEndStoreEvent();
		xResult = TRACE_SUCCESS;
	}
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
	(void)pxTCB;
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

	return xResult;
}

/*******************************************************************************
* xTraceSDKEventBegin
*
* This function will begin an event and allow for payload to be added via the
* xTraceSDKEventAdd****() functions.
*
* @param uiEventCode The event code.
* @param uiPayloadSize The expected payload size in bytes.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKEventBegin(uint32_t uiEventCode, uint32_t uiPayloadSize)
{
	TRACE_ENTER_CRITICAL_SECTION();
	
	traceResult xResult = TRACE_FAIL;
	
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
	xResult = prvTraceBeginStoreEvent(uiEventCode, uiPayloadSize);

	if (xResult == TRACE_FAIL)
	{
		TRACE_EXIT_CRITICAL_SECTION();
	}
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
	(void)uiEventCode;
	(void)uiPayloadSize;
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

	return xResult;
}

/*******************************************************************************
* xTraceSDKEventEnd
*
* This function ends an event.
*
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKEventEnd()
{
	traceResult xResult = TRACE_FAIL;

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
	xResult = prvTraceEndStoreEvent();
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

/*******************************************************************************
* xTraceSDKEventAddData
*
* This function will add a variable size of data as payload.
*
* @param pvData The pointer to the data.
* @param uiSize The data size.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKEventAddData(void* pvData, uint32_t uiSize)
{
	traceResult xResult = TRACE_FAIL;

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
	xResult = prvTraceStoreEventPayload(pvData, uiSize);
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
	(void)pvData;
	(void)uiSize;
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

	return xResult;
}

/*******************************************************************************
* xTraceSDKEventAddObject
*
* This function will add an object as payload.
*
* @param pxObject The pointer to the object.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKEventAddObject(void* pxObject)
{
	traceResult xResult = TRACE_FAIL;

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
	xResult = xTraceSDKEventAdd32((uint32_t)pxObject);
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
	(void)pxObject;
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

	return xResult;
}

/*******************************************************************************
* xTraceSDKEventAdd32
*
* This function will add a 32-bit value as payload.
*
* @param value The value.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKEventAdd32(uint32_t value)
{
	traceResult xResult = TRACE_FAIL;

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
	xResult = prvTraceStoreEventPayload32(value);
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
	(void)value;
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

	return xResult;
}

/*******************************************************************************
* xTraceSDKEventAdd16
*
* This function will add an uint16_t as payload.
*
* @param value The value.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKEventAdd16(uint16_t value)
{
	traceResult xResult = TRACE_FAIL;

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
	xResult = prvTraceStoreEventPayload16(value);
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
	(void)value;
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

	return xResult;
}

/*******************************************************************************
* xTraceSDKEventAdd8
*
* This function will add an uint8_t as payload.
*
* @param value The value.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKEventAdd8(uint8_t value)
{
	traceResult xResult = TRACE_FAIL;

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
	xResult = prvTraceStoreEventPayload8(value);
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
	(void)value;
	xResult = TRACE_SUCCESS;
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

	return xResult;
}
