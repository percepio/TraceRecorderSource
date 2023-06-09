/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
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

#define TRC_INTERNAL_EVENT_BUFFER_WRITE_MODE (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_WRITE_MODE)

#define TRC_INTERNAL_EVENT_BUFFER_TRANSFER_MODE (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_TRANSFER_MODE)

#define TRC_INTERNAL_BUFFER_CHUNK_SIZE (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_SIZE)

#define TRC_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT)

#define TRC_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT)

/* Aligned */
#define TRC_STREAM_PORT_INTERNAL_BUFFER_SIZE ((((TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t))

/**
 * @brief A structure representing the trace stream port buffer.
 */
typedef struct TraceStreamPortBuffer	/* Aligned */
{
#if (TRC_USE_INTERNAL_BUFFER == 1)
	uint8_t uiBufferInternal[TRC_STREAM_PORT_INTERNAL_BUFFER_SIZE];
#endif
	uint8_t uiBuffer[sizeof(TraceUnsignedBaseType_t)];
} TraceStreamPortBuffer_t;

/**
 * @internal Stream port initialize callback.
 * 
 * This function is called by the recorder as part of its initialization phase.
 * 
 * @param[in] pxBuffer Buffer
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
 * @param[in] uiSize Allocation size
 * @param[out] ppvData Allocation data pointer
 * 
 * @retval TRC_FAIL Allocate failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortAllocate(uint32_t uiSize, void** ppvData);

/**
 * @brief Commits data to the stream port, depending on the implementation/configuration of the
 * stream port this data might be directly written to the stream port interface, buffered, or
 * something else.
 * 
 * @param[in] pvData Data to commit
 * @param[in] uiSize Data to commit size
 * @param[out] piBytesCommitted Bytes commited
 * 
 * @retval TRC_FAIL Commit failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortCommit(void* pvData, uint32_t uiSize, int32_t* piBytesCommitted);

/**
 * @brief Writes data through the stream port interface.
 * 
 * @param[in] pvData Data to write
 * @param[in] uiSize Data to write size
 * @param[out] piBytesWritten Bytes written
 * 
 * @retval TRC_FAIL Write failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortWriteData(void* pvData, uint32_t uiSize, int32_t* piBytesWritten);

/**
 * @brief Reads data through the stream port interface.
 * 
 * @param[in] pvData Destination data buffer 
 * @param[in] uiSize Destination data buffer size
 * @param[out] piBytesRead Bytes read
 * 
 * @retval TRC_FAIL Read failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortReadData(void* pvData, uint32_t uiSize, int32_t* piBytesRead);

#define xTraceStreamPortOnEnable(uiStartOption) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2((void)(uiStartOption), TRC_SUCCESS)

#define xTraceStreamPortOnDisable() TRC_COMMA_EXPR_TO_STATEMENT_EXPR_1(TRC_SUCCESS)

#define xTraceStreamPortOnTraceBegin() TRC_COMMA_EXPR_TO_STATEMENT_EXPR_1(TRC_SUCCESS)

#define xTraceStreamPortOnTraceEnd() TRC_COMMA_EXPR_TO_STATEMENT_EXPR_1(TRC_SUCCESS)

#ifdef __cplusplus
}
#endif

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#endif /* TRC_STREAMING_PORT_H */
