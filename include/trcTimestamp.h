/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC0)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for timestamps.
*/

#ifndef TRC_TIMESTAMP_H
#define TRC_TIMESTAMP_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Timestamp definitions
 * Select a TYPE wih a DIRECTION to configure the timestamp source.
 */

typedef struct TraceTimestamp
{
	uint32_t type;
	TraceUnsignedBaseType_t frequency;
	uint32_t period;
	uint32_t wraparounds;
	uint32_t osTickHz;
	uint32_t latestTimestamp;
	uint32_t osTickCount;
} TraceTimestamp_t;

extern TraceTimestamp_t* pxTraceTimestamp;

#define TRC_TIMESTAMP_RECORD_SIZE (sizeof(TraceTimestamp_t))

typedef struct TraceTimestampBuffer
{
	uint32_t buffer[(TRC_TIMESTAMP_RECORD_SIZE) / sizeof(uint32_t)];
} TraceTimestampBuffer_t;

traceResult xTraceTimestampInitialize(TraceTimestampBuffer_t *pxBuffer);

#if ((TRC_CFG_USE_TRACE_ASSERT) == 1)

traceResult xTraceTimestampGet(uint32_t* puiTimestamp);

traceResult xTraceTimestampGetWraparounds(uint32_t* puiTimerWraparounds);

traceResult xTraceTimestampSetFrequency(TraceUnsignedBaseType_t uxFrequency);

traceResult xTraceTimestampGetFrequency(TraceUnsignedBaseType_t* puxFrequency);

traceResult xTraceTimestampSetPeriod(uint32_t uiPeriod);

traceResult xTraceTimestampGetPeriod(uint32_t* puiPeriod);

traceResult xTraceTimestampSetOsTickCount(uint32_t uiOsTickCount);

traceResult xTraceTimestampGetOsTickCount(uint32_t *puiOsTickCount);

#else /* ((TRC_CFG_USE_TRACE_ASSERT) == 1) */

#if ((TRC_HWTC_TYPE == TRC_FREE_RUNNING_32BIT_INCR) || (TRC_HWTC_TYPE == TRC_CUSTOM_TIMER_INCR))
#define xTraceTimestampGet(puiTimestamp) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_4(*(puiTimestamp) = TRC_HWTC_COUNT, (*(puiTimestamp) < pxTraceTimestamp->latestTimestamp) ? pxTraceTimestamp->wraparounds++ : 0, pxTraceTimestamp->latestTimestamp = *(puiTimestamp), TRC_SUCCESS)
#elif ((TRC_HWTC_TYPE == TRC_FREE_RUNNING_32BIT_DECR) || (TRC_HWTC_TYPE == TRC_CUSTOM_TIMER_DECR))
#define xTraceTimestampGet(puiTimestamp) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_4(*(puiTimestamp) = TRC_HWTC_COUNT, (*(puiTimestamp) > pxTraceTimestamp->latestTimestamp) ? pxTraceTimestamp->wraparounds++ : 0, pxTraceTimestamp->latestTimestamp = *(puiTimestamp), TRC_SUCCESS)
#elif ((TRC_HWTC_TYPE == TRC_OS_TIMER_INCR) || (TRC_HWTC_TYPE == TRC_OS_TIMER_DECR))
#define xTraceTimestampGet(puiTimestamp) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_4(*(puiTimestamp) = ((TRC_HWTC_COUNT) & 0x00FFFFFFU) + ((pxTraceTimestamp->osTickCount & 0x000000FFU) << 24), pxTraceTimestamp->wraparounds = pxTraceTimestamp->osTickCount, pxTraceTimestamp->latestTimestamp = *(puiTimestamp), TRC_SUCCESS)
#endif

#define xTraceTimestampGetWraparounds(puiTimerWraparounds) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(puiTimerWraparounds) = pxTraceTimestamp->wraparounds, TRC_SUCCESS)

#define xTraceTimestampSetFrequency(uxFrequency) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(pxTraceTimestamp->frequency = uxFrequency, TRC_SUCCESS)

#define xTraceTimestampSetPeriod(uiPeriod) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(pxTraceTimestamp->period = uiPeriod, TRC_SUCCESS)

#define xTraceTimestampSetOsTickCount(uiOsTickCount) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(pxTraceTimestamp->osTickCount = uiOsTickCount, TRC_SUCCESS)

#define xTraceTimestampGetFrequency(puxFrequency) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(puxFrequency) = pxTraceTimestamp->frequency, TRC_SUCCESS)

#define xTraceTimestampGetPeriod(puiPeriod) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(puiPeriod) = pxTraceTimestamp->period, TRC_SUCCESS)

#define xTraceTimestampGetOsTickCount(puiOsTickCount) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(puiOsTickCount) = pxTraceTimestamp->osTickCount, TRC_SUCCESS)

#endif /* ((TRC_CFG_USE_TRACE_ASSERT) == 1) */

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_TIMESTAMP_H */
