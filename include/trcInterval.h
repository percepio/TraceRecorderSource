/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC1)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for intervals.
*/

#ifndef TRC_INTERVAL_H
#define TRC_INTERVAL_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

traceResult xTraceIntervalCreate(const char *szName, TraceIntervalHandle_t *pxIntervalHandle);
traceResult xTraceIntervalStart(TraceIntervalHandle_t xIntervalHandle);
traceResult xTraceIntervalStop(TraceIntervalHandle_t xIntervalHandle);
traceResult xTraceIntervalGetState(TraceIntervalHandle_t xIntervalHandle, uint32_t *puxState);

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_INTERVAL_H */
