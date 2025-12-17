/*
 * Trace Recorder for Tracealyzer v4.11.0
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Supporting functions for trace streaming, used by the "stream ports" 
 * for reading and writing data to the interface.
 *
 * Note that this stream port is more complex than the typical case, since
 * the J-Link interface uses a separate RAM buffer in SEGGER_RTT.c, instead
 * of the default buffer included in the recorder core. The other stream ports 
 * offer more typical examples of how to define a custom streaming interface.
 */
 
#include <trcRecorder.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

static TraceStreamPortBuffer_t* pxStreamPortRTT TRC_CFG_RECORDER_DATA_ATTRIBUTE;

traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer)
{
	if (pxBuffer == 0)
	{
		return TRC_FAIL;
	}

	pxStreamPortRTT = (TraceStreamPortBuffer_t*)pxBuffer;

	return TRC_SUCCESS;
}

traceResult xTraceStreamPortOnEnable(uint32_t uiStartOption)
{
	(void)uiStartOption;

	/* Configure the RTT buffers. On down buffer, and one up buffer per core. */
	for (int i = 0; i < TRC_CFG_CORE_COUNT; i++)
	{
#ifndef TRC_STREAM_PORT_MULTISTREAM_SUPPORT
		/* Multistream support hasn't been enabled */
		if (i > 0)
		{
			break;
		}
#endif
		if (SEGGER_RTT_ConfigUpBuffer(TRC_CFG_STREAM_PORT_RTT_UP_BUFFER_INDEX + i, "TzData", pxStreamPortRTT->bufferUp[i], TRC_STREAM_PORT_RTT_UP_BUFFER_SIZE, TRC_CFG_STREAM_PORT_RTT_MODE) < 0)
		{
			return TRC_FAIL;
		}
	}

	if (SEGGER_RTT_ConfigDownBuffer(TRC_CFG_STREAM_PORT_RTT_DOWN_BUFFER_INDEX, "TzCtrl", pxStreamPortRTT->bufferDown, TRC_STREAM_PORT_RTT_DOWN_BUFFER_SIZE, TRC_CFG_STREAM_PORT_RTT_MODE) < 0)
	{
		return TRC_FAIL;
	}

	return TRC_SUCCESS;
}

#endif
