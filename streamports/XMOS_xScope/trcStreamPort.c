/*
 * Trace Recorder for Tracealyzer v4.11.0
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Supporting functions for trace streaming, used by the "stream ports" 
 * for reading and writing data to the interface.
 */
 
#include <trcRecorder.h>
#include <xscope.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

static TraceStreamPortBuffer_t* pxStreamPortXS TRC_CFG_RECORDER_DATA_ATTRIBUTE;

traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer)
{
	if (pxBuffer == 0)
	{
		return TRC_FAIL;
	}

	pxStreamPortXS = pxBuffer;

	return TRC_SUCCESS;
}

traceResult xTraceStreamPortOnBegin(void)
{
	return TRC_SUCCESS;
}

traceResult xTraceStreamPortOnEnd(void)
{
	return TRC_SUCCESS;
}

traceResult xTraceStreamPortWriteData(void* pvData, uint32_t uiSize, uint32_t uiChannel, int32_t* piBytesWritten)
{
	(void)uiChannel; /* uiChannel is not used in this implementation yet */

	/* xscope_bytes is supposed to be thread safe, so we do not bother with any
	 * critical sections here. */
	xscope_bytes(0, uiSize, (unsigned char*)pvData);

	if (piBytesWritten != 0) {
		/* Since xScope always write all bytes (not all might be received) we flag this as
		 * a full write */
		*piBytesWritten = (int32_t)uiSize;
	}

	return TRC_SUCCESS;
}

traceResult xTraceStreamPortReadData(void* pvData, uint32_t uiSize, int32_t* piBytesRead)
{
	return TRC_SUCCESS;
}

#endif
