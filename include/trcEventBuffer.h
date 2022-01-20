/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC0)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for the event buffer.
*/

#ifndef TRC_EVENT_BUFFER_H
#define TRC_EVENT_BUFFER_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Type flags */
#define TRC_EVENT_BUFFER_OPTION_SKIP		(0U)
#define TRC_EVENT_BUFFER_OPTION_OVERWRITE	(1U)

/* TODO: MOVE THIS TO .c FILE */
typedef struct TraceEventBuffer
{
	uint32_t uiHead;
	uint32_t uiTail;
	uint32_t uiSize;
	uint32_t uiOptions;
	uint32_t uiDroppedEvents;
	uint32_t uiFree;
	uint32_t uiTimerWraparounds;
	uint8_t* puiBuffer;
} TraceEventBuffer_t;

/**
 * @brief
 *
 * @param
 * @param
 * @param
 * @param
 *
 * @return
 */
traceResult xTraceEventBufferInitialize(TraceEventBuffer_t * pxTraceEventBuffer, uint32_t uiOptions,
		uint8_t *puiBuffer, uint32_t uiSize);

/**
 * @brief
 *
 * @param
 * @param
 * @param
 * @param
 *
 * @return
 */
traceResult xTraceEventBufferPush(TraceEventBuffer_t *pxTraceEvtBuffer, void *pxData, uint32_t uiSize, int32_t *piBytesWritten);

/**
 * @brief
 * 
 * @param
 * @param
 * 
 * @return
 */
traceResult xTraceEventBufferTransfer(TraceEventBuffer_t* pxTraceEventBuffer, int32_t* piBytesWritten);

/**
 * @brief Clears all data from event buffer.
 * 
 * @param
 * @param
 * 
 * @return
 */
traceResult xTraceEventBufferClear(TraceEventBuffer_t* pxTraceEventBuffer);

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_EVENT_BUFFER_H */
