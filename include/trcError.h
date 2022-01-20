/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC0)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for errors.
*/

#ifndef TRC_ERROR_H
#define TRC_ERROR_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_ERROR_BUFFER_SIZE (sizeof(uint32_t) + sizeof(uint32_t) + sizeof(TraceStringHandle_t))

typedef struct TraceErrorBuffer
{
	uint32_t buffer[(TRC_ERROR_BUFFER_SIZE) / sizeof(uint32_t)];
} TraceErrorBuffer_t;

traceResult xTraceErrorInitialize(TraceErrorBuffer_t* pxBuffer);
traceResult xTraceWarning(uint32_t uiErrorCode);
traceResult xTraceError(uint32_t uiErrorCode);
traceResult xTraceErrorGetLast(const char** pszError);
traceResult xTraceErrorClear(void);

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_ERROR_H*/
