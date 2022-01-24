/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC1)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for intervals.
*/

#ifndef TRC_COUNTER_H
#define TRC_COUNTER_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

traceResult xTraceCounterSetCallback(TraceCounterCallback_t xCallback);
traceResult xTraceCounterCreate(const char* szName, TraceBaseType_t xInitialValue, TraceBaseType_t xLowerLimit, TraceBaseType_t xUpperLimit, TraceCounterHandle_t* pxCounterHandle);
traceResult xTraceCounterAdd(TraceCounterHandle_t xCounterHandle, TraceBaseType_t xValue);
traceResult xTraceCounterSet(TraceCounterHandle_t xCounterHandle, TraceBaseType_t xValue);
traceResult xTraceCounterGet(TraceCounterHandle_t xCounterHandle, TraceBaseType_t* pxValue);
traceResult xTraceCounterIncrease(TraceCounterHandle_t xCounterHandle);
traceResult xTraceCounterDecrease(TraceCounterHandle_t xCounterHandle);
traceResult xTraceCounterGetUpperLimit(TraceCounterHandle_t xCounterHandle, TraceBaseType_t* pxValue);
traceResult xTraceCounterGetLowerLimit(TraceCounterHandle_t xCounterHandle, TraceBaseType_t* pxValue);
traceResult xTraceCounterGetName(TraceCounterHandle_t xCounterHandle, const char** pszName);

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_COUNTER_H */
