/*
 * Trace Recorder for Tracealyzer v4.11.0
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The interface definitions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to use UDP as streaming channel.
 * The example is for lwIP.
 */

#ifndef TRC_STREAM_PORT_H
#define TRC_STREAM_PORT_H

#include <stdint.h>
#include <trcTypes.h>
#include <trcStreamPortConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TraceStreamPortBuffer	/* Aligned */
{
	TraceUnsignedBaseType_t dummy;
} TraceStreamPortBuffer_t;

#define TRC_STREAM_PORT_MULTISTREAM_SUPPORT

int32_t prvTraceUdpWrite(void* pvData, uint32_t uiSize, uint32_t uiChannel, int32_t* piBytesWritten);

int32_t prvTraceUdpRead(void* pvData, uint32_t uiSize, int32_t* piBytesRead);

traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer);

#define xTraceStreamPortWriteData(pvData, uiSize, uiChannel, piBytesWritten) (prvTraceUdpWrite(pvData, uiSize, uiChannel, piBytesWritten) == 0 ? TRC_SUCCESS : TRC_FAIL)

#define xTraceStreamPortReadData(pvData, uiSize, piBytesRead) (prvTraceUdpRead(pvData, uiSize, piBytesRead) == 0 ? TRC_SUCCESS : TRC_FAIL)

#define xTraceStreamPortOnEnable(uiStartOption) ((void)(uiStartOption), TRC_SUCCESS)

#define xTraceStreamPortOnDisable() (TRC_SUCCESS)

#define xTraceStreamPortOnTraceBegin() (TRC_SUCCESS)

traceResult xTraceStreamPortOnTraceEnd(void);

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAM_PORT_H */

