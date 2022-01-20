/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC0)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for the multi-core event buffer.
*/

#ifndef TRC_MULTI_CORE_EVENT_BUFFER_H
#define TRC_MULTI_CORE_EVENT_BUFFER_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: MOVE THIS TO .c FILE. THEN EXPOSE ANONYMOUS BUFFER FOR INITIALIZE */
typedef struct TraceMultiCoreEventBuffer
{
	TraceEventBuffer_t *xEventBuffer[TRC_CFG_CORE_COUNT];
} TraceMultiCoreEventBuffer_t;

traceResult xTraceMultiCoreEventBufferInitialize(TraceMultiCoreEventBuffer_t* pxTraceMultiCoreEventBuffer, uint32_t uiOptions,
	uint8_t* puiBuffer, uint32_t uiSize);



#if ((TRC_CFG_USE_TRACE_ASSERT) == 1)

traceResult xTraceMultiCoreEventBufferPush(TraceMultiCoreEventBuffer_t* pxTraceMultiCoreEventBuffer, void* pvData, uint32_t uiSize, int32_t* piBytesWritten);

#else

#define xTraceMultiCoreEventBufferPush(pxTraceMultiCoreEventBuffer, pvData, uiSize, piBytesWritten) xTraceEventBufferPush((pxTraceMultiCoreEventBuffer)->xEventBuffer[TRC_CFG_GET_CURRENT_CORE()], pvData, uiSize, piBytesWritten)

#endif

traceResult xTraceMultiCoreEventBufferTransfer(TraceMultiCoreEventBuffer_t* pxTraceMultiCoreEventBuffer, int32_t* piBytesWritten);

traceResult xTraceMultiCoreEventBufferClear(TraceMultiCoreEventBuffer_t* pxTraceMultiCoreEventBuffer);

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_MULTI_CORE_EVENT_BUFFER_H */
