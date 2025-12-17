/*
 * Trace Recorder for Tracealyzer v4.11.0
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The interface definitions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to use USB CDC as streaming channel.
 * The example is for STM32 using STM32Cube.
 */

#ifndef TRC_STREAM_PORT_H
#define TRC_STREAM_PORT_H

#include <trcTypes.h>
#include <trcStreamPortConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_STREAM_PORT_USB_BUFFER_SIZE ((((TRC_CFG_STREAM_PORT_USB_BUFFER_SIZE) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t))

typedef struct TraceStreamPortBuffer	/* Aligned */
{
	TraceUnsignedBaseType_t idx;
	uint8_t bufferUSB[TRC_STREAM_PORT_USB_BUFFER_SIZE];
} TraceStreamPortBuffer_t;

traceResult prvTraceCDCReceive(void* data, uint32_t uiSize, int32_t* piBytesReceived);

traceResult prvTraceCDCTransmit(void* pvData, uint32_t uiSize, int32_t* piBytesSent);

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
 * @param[in] uiChannel Channel (not used in this implementation)
 * @param[out] piBytesWritten Bytes written
 * 
 * @retval TRC_FAIL Write failed
 * @retval TRC_SUCCESS Success
 */
#define xTraceStreamPortWriteData(pvData, uiSize, uiChannel, piBytesWritten) prvTraceCDCTransmit(pvData, uiSize, piBytesWritten)

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
#define xTraceStreamPortReadData prvTraceCDCReceive

#define xTraceStreamPortOnEnable(uiStartOption) ((void)(uiStartOption), TRC_SUCCESS)

#define xTraceStreamPortOnDisable() (TRC_SUCCESS)

#define xTraceStreamPortOnTraceBegin() (TRC_SUCCESS)

#define xTraceStreamPortOnTraceEnd() (TRC_SUCCESS)

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAM_PORT_H */
