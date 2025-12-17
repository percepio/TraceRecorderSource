/*
 * Trace Recorder for Tracealyzer v4.11.0
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The interface definitions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to use SEGGER RTT as streaming channel.
 *
 * Note that this stream port is more complex than the typical case, since
 * the J-Link interface uses a separate RAM buffer in SEGGER_RTT.c, instead
 * of the default buffer included in the recorder core. The other stream ports 
 * offer more typical examples of how to define a custom streaming interface.
 */

#ifndef TRC_STREAM_PORT_H
#define TRC_STREAM_PORT_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#ifdef __cplusplus
extern "C" {
#endif

#include <trcDefines.h>
#include <trcTypes.h>
#include <trcStreamPortConfig.h>

#include <SEGGER_RTT_Conf.h>
#include <SEGGER_RTT.h>

/* For now we disable multistream support for this streamport.
#define TRC_STREAM_PORT_MULTISTREAM_SUPPORT
*/

/* Aligned */
#define TRC_STREAM_PORT_RTT_UP_BUFFER_SIZE ((((TRC_CFG_STREAM_PORT_RTT_UP_BUFFER_SIZE) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t))

/* Aligned */
#define TRC_STREAM_PORT_RTT_DOWN_BUFFER_SIZE ((((TRC_CFG_STREAM_PORT_RTT_DOWN_BUFFER_SIZE) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t))

#if (defined(TRC_CFG_STREAM_PORT_RTT_NO_LOCK_WRITE) && TRC_CFG_STREAM_PORT_RTT_NO_LOCK_WRITE == 1)
#define TRC_STREAM_PORT_SEGGER_RTT_WRITE SEGGER_RTT_WriteNoLock
#else
#define TRC_STREAM_PORT_SEGGER_RTT_WRITE SEGGER_RTT_Write
#endif

#ifndef TRC_STREAM_PORT_MULTISTREAM_SUPPORT
/* Multistream support is disabled */
#define TRC_STREAM_PORT_MULTISTREAM_GET_CHANNEL(uiChannel) 0
#else
#define TRC_STREAM_PORT_MULTISTREAM_GET_CHANNEL(uiChannel) uiChannel
#endif

/**
 * @brief A structure representing the trace stream port buffer.
 */
typedef struct TraceStreamPortBuffer	/* Aligned */
{
	uint8_t bufferUp[TRC_CFG_CORE_COUNT][TRC_STREAM_PORT_RTT_UP_BUFFER_SIZE];
	uint8_t bufferDown[TRC_STREAM_PORT_RTT_DOWN_BUFFER_SIZE];
} TraceStreamPortBuffer_t;

/**
 * @internal Stream port initialize callback.
 * 
 * This function is called by the recorder as part of its initialization phase.
 * 
 * @param[in] pxBuffer Buffer
 * 
 * @retval TRC_FAIL Initialization failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer);

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
#define xTraceStreamPortWriteData(pvData, uiSize, uiChannel, piBytesWritten) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(piBytesWritten) = (int32_t)TRC_STREAM_PORT_SEGGER_RTT_WRITE((TRC_CFG_STREAM_PORT_RTT_UP_BUFFER_INDEX) + TRC_STREAM_PORT_MULTISTREAM_GET_CHANNEL(uiChannel), (const char*)pvData, uiSize), TRC_SUCCESS)

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
#define xTraceStreamPortReadData(pvData, uiSize, piBytesRead) ((SEGGER_RTT_HASDATA(TRC_CFG_STREAM_PORT_RTT_DOWN_BUFFER_INDEX)) ? (*(piBytesRead) = (int32_t)SEGGER_RTT_Read((TRC_CFG_STREAM_PORT_RTT_DOWN_BUFFER_INDEX), (char*)(pvData), uiSize), TRC_SUCCESS) : TRC_SUCCESS)

traceResult xTraceStreamPortOnEnable(uint32_t uiStartOption);

#define xTraceStreamPortOnDisable() (void)(TRC_SUCCESS)

#define xTraceStreamPortOnTraceBegin() (void)(TRC_SUCCESS)

#define xTraceStreamPortOnTraceEnd() (void)(TRC_SUCCESS)

#ifdef __cplusplus
}
#endif

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#endif /* TRC_STREAM_PORT_H */
