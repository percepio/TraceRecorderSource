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

traceResult xTraceIntervalChannelSetCreate(const char* szName, TraceIntervalChannelSetHandle_t* pxIntervalChannelSetHandle)
{
	TraceObjectHandle_t xObjectHandle;

	/* This should never fail */
	TRC_ASSERT(pxIntervalChannelSetHandle != 0);

	/* We need to check this */
	if (xTraceObjectRegister(PSF_EVENT_INTERVAL_CHANNEL_SET_CREATE, 0, szName, 0, &xObjectHandle) == TRC_FAIL)
	{
		return TRC_FAIL;
	}

	/* This should never fail */
	TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntrySetOptions((TraceEntryHandle_t)xObjectHandle, TRC_ENTRY_OPTION_INTERVAL_CHANNEL_SET) == TRC_SUCCESS);

	*pxIntervalChannelSetHandle = (TraceIntervalChannelSetHandle_t)xObjectHandle;

	return TRC_SUCCESS;
}

traceResult xTraceIntervalChannelCreate(const char *szName, TraceIntervalChannelSetHandle_t xIntervalChannelSetHandle, TraceIntervalChannelHandle_t *pxIntervalChannelHandle)
{
	TraceObjectHandle_t xObjectHandle;

	/* This should never fail */
	TRC_ASSERT(pxIntervalChannelHandle != 0);

	/* This should never fail */
	TRC_ASSERT(xIntervalChannelSetHandle != 0);

	/* We need to check this */
	if (xTraceObjectRegister(PSF_EVENT_INTERVAL_CHANNEL_CREATE, 0, szName, xIntervalChannelSetHandle, &xObjectHandle) == TRC_FAIL)
	{
		return TRC_FAIL;
	}

	/* This should never fail */
	TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntrySetOptions((TraceEntryHandle_t)xObjectHandle, TRC_ENTRY_OPTION_INTERVAL_CHANNEL) == TRC_SUCCESS);

	*pxIntervalChannelHandle = (TraceIntervalChannelHandle_t)xObjectHandle;
	
	return TRC_SUCCESS;
}

traceResult xTraceIntervalStart(TraceIntervalChannelHandle_t xIntervalChannelHandle, TraceUnsignedBaseType_t uxValue, TraceIntervalInstanceHandle_t *pxIntervalInstanceHandle)
{
	TraceEventHandle_t xEventHandle = 0;

	TRC_ASSERT(xIntervalChannelHandle != 0);

	TRC_ASSERT_ALWAYS_EVALUATE(xTraceTimestampGet((uint32_t*)pxIntervalInstanceHandle) == TRC_SUCCESS);

	/* We need to check this */
	if (xTraceEventBegin(PSF_EVENT_INTERVAL_START, sizeof(void*) + sizeof(TraceUnsignedBaseType_t) + sizeof(TraceUnsignedBaseType_t), &xEventHandle) == TRC_SUCCESS)
	{
		xTraceEventAddPointer(xEventHandle, (void*)xIntervalChannelHandle);
		xTraceEventAddUnsignedBaseType(xEventHandle, (TraceUnsignedBaseType_t)*pxIntervalInstanceHandle);
		xTraceEventAddUnsignedBaseType(xEventHandle, uxValue);
		xTraceEventEnd(xEventHandle);
	}

	return TRC_SUCCESS;
}

traceResult xTraceIntervalStop(TraceIntervalChannelHandle_t xIntervalChannelHandle, TraceIntervalInstanceHandle_t xIntervalInstanceHandle)
{
	TraceEventHandle_t xEventHandle = 0;

	TRC_ASSERT(xIntervalChannelHandle != 0);

	/* We need to check this */
	if (xTraceEventBegin(PSF_EVENT_INTERVAL_STOP, sizeof(void*) + sizeof(TraceUnsignedBaseType_t), &xEventHandle) == TRC_SUCCESS)
	{
		xTraceEventAddPointer(xEventHandle, (void*)xIntervalChannelHandle);
		xTraceEventAddUnsignedBaseType(xEventHandle, (TraceUnsignedBaseType_t)xIntervalInstanceHandle);
		xTraceEventEnd(xEventHandle);
	}
	
	return TRC_SUCCESS;
}

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */
