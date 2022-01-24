/*
 * Trace Recorder for Tracealyzer v4.6.0(RC1)
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The interface definitions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to use XMOS xScope as a streaming channel.
 */

#ifndef TRC_STREAMING_PORT_H
#define TRC_STREAMING_PORT_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#ifdef __cplusplus
extern "C" {
#endif

#include <trcTypes.h>
#include <trcStreamPortConfig.h>

#define TRC_USE_INTERNAL_BUFFER (TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER)

/* Aligned */
#define TRC_STREAM_PORT_INTERNAL_BUFFER_SIZE ((((TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t))

/**
 * @brief A structure representing the trace stream port buffer.
 */
typedef struct TraceStreamPortBuffer
{
#if (TRC_USE_INTERNAL_BUFFER == 1)
	uint8_t uiBufferInternal[TRC_STREAM_PORT_INTERNAL_BUFFER_SIZE];
#endif
	uint8_t uiBuffer[4];
} TraceStreamPortBuffer_t;

/**
 * @brief Stream port initialize callback.
 * 
 * This function is called by the recorder as part of its initialization phase.
 * 
 * @param pxBuffer Buffer
 * @retval TRC_FAIL Initialization failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer);

/**
 * @brief Stream port begin callback.
 * 
 * This function is called by the recorder as part of its begin phase.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortOnBegin(void);

/**
 * @brief Stream port end callback.
 * 
 * This function is called by the recorder as part of its end phase.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortOnEnd(void);

/**
 * @brief Allocates data from the stream port.
 * 
 * @param uiSize Allocation size
 * @param ppvData Allocation data pointer
 * @retval TRC_FAIL Allocate failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortAllocate(uint32_t uiSize, void** ppvData);

/**
 * @brief Commits data to the stream port, depending on the implementation/configuration of the
 * stream port this data might be directly written to the stream port interface, buffered, or
 * something else.
 * 
 * @param pvData Data to commit
 * @param uiSize Data to commit size
 * @param piBytesCommitted Bytes commited
 * @retval TRC_FAIL Commit failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortCommit(void* pvData, uint32_t uiSize, int32_t* piBytesCommitted);

/**
 * @brief Writes data through the stream port interface.
 * 
 * @param pvData Data to write
 * @param uiSize Data to write size
 * @param piBytesWritten Bytes written
 * @retval TRC_FAIL Write failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortWriteData(void* pvData, uint32_t uiSize, int32_t* piBytesWritten);

/**
 * @brief Reads data through the stream port interface.
 * 
 * @param pvData Destination data buffer 
 * @param uiSize Destination data buffer size
 * @param piBytesRead Bytes read
 * @retval TRC_FAIL Read failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortReadData(void* pvData, uint32_t uiSize, int32_t* piBytesRead);

#ifdef __cplusplus
}
#endif

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#endif /* TRC_STREAMING_PORT_H */
