/*
 * Trace Recorder for Tracealyzer v4.11.0
 * Copyright 2025 Percepio AB
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

#ifdef __cplusplus
extern "C" {
#endif

#include <trcTypes.h>
#include <trcStreamPortConfig.h>

/**
 * @brief A structure representing the trace stream port buffer.
 */
typedef struct TraceStreamPortBuffer	/* Aligned */
{
	uint8_t uiBuffer[8];
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
 * @brief Writes data through the stream port interface.
 * 
 * @param[in] pvData Data to write
 * @param[in] uiSize Data to write size
 * @param[in] uiChannel Channel (0 for the first core, 1 for the second core, etc.)
 * @param[out] piBytesWritten Bytes written
 * 
 * @retval TRC_FAIL Write failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortWriteData(void* pvData, uint32_t uiSize, uint32_t uiChannel, int32_t* piBytesWritten);

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

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#endif /* TRC_STREAMING_PORT_H */
