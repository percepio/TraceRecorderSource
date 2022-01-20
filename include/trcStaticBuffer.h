/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC0)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for the static buffer.
*/

#ifndef TRC_STATIC_BUFFER_H
#define TRC_STATIC_BUFFER_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

	/* A buffer type that is maximum size */
	typedef uint8_t TraceStaticBuffer_t[TRC_MAX_BLOB_SIZE];

	typedef struct TraceCoreStaticBufferCore
	{
		TraceStaticBuffer_t dummyEvents[(TRC_CFG_MAX_ISR_NESTING)+1];
	} TraceCoreStaticBuffer_t;

	typedef struct TraceStaticBufferTable
	{
		/* These are buffers that are used temporarily in event or blob creation */
		TraceCoreStaticBuffer_t coreDummyEvents[TRC_CFG_CORE_COUNT];
	} TraceStaticBufferTable_t;

#define TRC_STATIC_BUFFER_BUFFER_SIZE (sizeof(TraceStaticBufferTable_t))

typedef struct TraceStaticBufferBuffer
{
	uint8_t buffer[TRC_STATIC_BUFFER_BUFFER_SIZE];
} TraceStaticBufferBuffer_t;

extern TraceStaticBufferTable_t* pxTraceStaticBufferTable;

traceResult xTraceStaticBufferInitialize(TraceStaticBufferBuffer_t* pxBuffer);

#if ((TRC_CFG_USE_TRACE_ASSERT) == 1)

/* Returns a pointer to a maximum sized static buffer */
traceResult xTraceStaticBufferGet(void **ppvBuffer);

#else /* ((TRC_CFG_USE_TRACE_ASSERT) == 1) */

/* Tasks at index 0, ISRs from index 1. xTraceISRGetCurrentNestingReturned() returns -1 for tasks and 0+ for ISRs so we always add 1. */
#define xTraceStaticBufferGet(ppvBuffer) (*ppvBuffer = (void*)&pxTraceStaticBufferTable->coreDummyEvents[TRC_CFG_GET_CURRENT_CORE()].dummyEvents[xTraceISRGetCurrentNestingReturned() + 1], TRC_SUCCESS)

#endif /* ((TRC_CFG_USE_TRACE_ASSERT) == 1) */

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_STATIC_BUFFER_H */
