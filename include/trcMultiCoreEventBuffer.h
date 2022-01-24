/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC1)
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

/**
 * @brief Initialize multicore event buffer.
 * 
 * This routine initializes a multi-core trace event buffer and assignts it
 * a memory area based on the supplied buffer.
 * 
 * Trace event buffer options specifies the buffer behavior regarding
 * old data, the alternatives are TRC_EVENT_BUFFER_OPTION_SKIP and
 * TRC_EVENT_BUFFER_OPTION_OVERWRITE (mutal exclusive).
 * 
 * @param pxTraceMultiCoreEventBuffer Pointer to unitialized multi-core trace event buffer.
 * @param uiOptions Trace event buffer options.
 * @param puiBuffer Pointer to buffer that will be used by the multi-core trace event buffer.
 * @param uiSize Size of buffer.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceMultiCoreEventBufferInitialize(TraceMultiCoreEventBuffer_t* pxTraceMultiCoreEventBuffer, uint32_t uiOptions,
	uint8_t* puiBuffer, uint32_t uiSize);



#if ((TRC_CFG_USE_TRACE_ASSERT) == 1)

/**
 * @brief Pushes data into multi-core trace event buffer.
 * 
 * This routine attempts to push data into the multi-core trace event buffer. Selection
 * of which core the data is pushed for is managed automatically through the
 * TRC_CFG_GET_CURRENT_CORE macro which is defined on an RTOS basis. 
 * 
 * @param pxTraceMultiCoreEventBuffer Pointer to initialized multi-core event buffer.
 * @param pvData Pointer to data should be pushed into multi-core event buffer.
 * @param uiSize Size of data that should be pushed into multi-core trace event buffer.
 * @param piBytesWritten Pointer to variable which the routine will write the number
 * of bytes that was pushed into the multi-core trace event buffer.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceMultiCoreEventBufferPush(TraceMultiCoreEventBuffer_t* pxTraceMultiCoreEventBuffer, void* pvData, uint32_t uiSize, int32_t* piBytesWritten);

#else

#define xTraceMultiCoreEventBufferPush(pxTraceMultiCoreEventBuffer, pvData, uiSize, piBytesWritten) xTraceEventBufferPush((pxTraceMultiCoreEventBuffer)->xEventBuffer[TRC_CFG_GET_CURRENT_CORE()], pvData, uiSize, piBytesWritten)

#endif

/**
 * @brief Transfer multi-core trace event buffer data through streamport.
 * 
 * This routine will attempt to transfer all existing data in the multi-core trace event
 * buffer through the streamport. New data pushed to the trace event buffer
 * during the execution of this routine will not be transfered to 
 * 
 * @param pxTraceMultiCoreEventBuffer Pointer to initialized multi-core event buffer.
 * @param piBytesWritten Pointer to variable which the routine will write the number
 * of bytes that was pushed into the multi-core trace event buffer.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceMultiCoreEventBufferTransfer(TraceMultiCoreEventBuffer_t* pxTraceMultiCoreEventBuffer, int32_t* piBytesWritten);

/**
 * @brief Clears all data from event buffer.
 * 
 * @param pxTraceMultiCoreEventBuffer Pointer to initialized multic-core trace event buffer.
 *  
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceMultiCoreEventBufferClear(TraceMultiCoreEventBuffer_t* pxTraceMultiCoreEventBuffer);

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_MULTI_CORE_EVENT_BUFFER_H */
