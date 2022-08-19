/*
* Trace Recorder for Tracealyzer v4.6.6
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The implementation of intervals.
*/

#include <trcRecorder.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

static TraceCounterCallback_t xCallbackFunction;

traceResult xTraceCounterSetCallback(TraceCounterCallback_t xCallback)
{
	TRC_ASSERT(xCallback != 0);

	xCallbackFunction = xCallback;

	/* We only set this component as initialized when the callback has been set */
	xTraceSetComponentInitialized(TRC_RECORDER_COMPONENT_COUNTER);

	return TRC_SUCCESS;
}

traceResult xTraceCounterCreate(const char *szName, TraceBaseType_t xInitialValue, TraceBaseType_t xLowerLimit, TraceBaseType_t xUpperLimit, TraceCounterHandle_t *pxCounterHandle)
{
	TraceObjectHandle_t xObjectHandle;
	TraceUnsignedBaseType_t uxStates[3];

	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_COUNTER));

	/* This should never fail */
	TRC_ASSERT(pxCounterHandle != 0);

	TRC_ASSERT(xInitialValue >= xLowerLimit && xInitialValue <= xUpperLimit);

	uxStates[TRC_COUNTER_VALUE_INDEX] = (TraceUnsignedBaseType_t)xInitialValue;
	uxStates[TRC_COUNTER_LOWER_LIMIT_INDEX] = (TraceUnsignedBaseType_t)xLowerLimit;
	uxStates[TRC_COUNTER_UPPER_LIMIT_INDEX] = (TraceUnsignedBaseType_t)xUpperLimit;

	/* We need to check this */
	if (xTraceObjectRegisterInternal(PSF_EVENT_COUNTER_CREATE, 0, szName, 3, uxStates, TRC_ENTRY_OPTION_COUNTER, &xObjectHandle) == TRC_FAIL)
	{
		return TRC_FAIL;
	}

	*pxCounterHandle = (TraceCounterHandle_t)xObjectHandle;
	
	return TRC_SUCCESS;
}

traceResult xTraceCounterSet(TraceCounterHandle_t xCounterHandle, TraceBaseType_t xValue)
{
	TraceEventHandle_t xEventHandle = 0;
	TraceBaseType_t xLowerLimit;
	TraceBaseType_t xUpperLimit;

	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_COUNTER));

	/* This should never fail */
	TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntrySetState((TraceEntryHandle_t)xCounterHandle, TRC_COUNTER_VALUE_INDEX, (TraceUnsignedBaseType_t)xValue) == TRC_SUCCESS);

	/* We need to check this */
	if (xTraceEventBegin(PSF_EVENT_COUNTER_CHANGE, sizeof(void*) + sizeof(uint32_t), &xEventHandle) == TRC_SUCCESS)
	{
		xTraceEventAddPointer(xEventHandle, (void*)xCounterHandle);
		xTraceEventAdd32(xEventHandle, (uint32_t)xValue);
		xTraceEventEnd(xEventHandle);
	}

	/* These should never fail */
	TRC_ASSERT_ALWAYS_EVALUATE(xTraceCounterGetLowerLimit(xCounterHandle, &xLowerLimit) == TRC_SUCCESS);
	TRC_ASSERT_ALWAYS_EVALUATE(xTraceCounterGetUpperLimit(xCounterHandle, &xUpperLimit) == TRC_SUCCESS);

	if (xValue < xLowerLimit || xValue > xUpperLimit)
	{
		if (xTraceEventBegin(PSF_EVENT_COUNTER_LIMIT_EXCEEDED, sizeof(void*), &xEventHandle) == TRC_SUCCESS)
		{
			xTraceEventAddPointer(xEventHandle, (void*)xCounterHandle);
			xTraceEventEnd(xEventHandle);
		}

		xCallbackFunction(xCounterHandle);
	}

	return TRC_SUCCESS;
}

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */
