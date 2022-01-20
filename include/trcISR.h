/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC0)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for ISR tagging.
*/

#ifndef TRC_ISR_H
#define TRC_ISR_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TraceISRCoreInfo
{
	TraceISRHandle_t handleStack[TRC_CFG_MAX_ISR_NESTING];
	int32_t stackIndex;
	int32_t isPendingContextSwitch;
} TraceISRCoreInfo_t;

typedef struct TraceISRInfo
{
	TraceISRCoreInfo_t coreInfos[TRC_CFG_CORE_COUNT]; /* ISR handles */
} TraceISRInfo_t;

/* We expose this to enable faster access */
extern TraceISRInfo_t* pxTraceISRInfo;

#define TRACE_ISR_INFO_BUFFER_SIZE (sizeof(TraceISRInfo_t))

typedef struct TraceISRInfoBuffer
{
	uint8_t buffer[(TRACE_ISR_INFO_BUFFER_SIZE)];
} TraceISRInfoBuffer_t;

/**
 * @brief Initializes the ISR stack tracing system.
 */
traceResult xTraceISRInitialize(TraceISRInfoBuffer_t *pxBuffer);

/*******************************************************************************
 * xTraceISRRegister
 *
 * Stores a name and priority level for an Interrupt Service Routine, to allow
 * for better visualization. Returns a traceHandle used by vTraceStoreISRBegin.
*
* Example:
*	 #define PRIO_OF_ISR_TIMER1 3 // the hardware priority of the interrupt
*	 TraceISRHandle_t xISRTimer1Handle = 0; // The ID set by the recorder
*	 ...
*	 xTraceISRRegister("ISRTimer1", PRIO_OF_ISR_TIMER1, &xISRTimer1Handle);
*	 ...
*	 void ISR_handler()
*	 {
*		 xTraceISRBegin(xISRTimer1Handle);
*		 ...
*		 xTraceISREnd(0);
*	 }
 ******************************************************************************/
traceResult xTraceISRRegister(const char* szName, uint32_t uiPriority, TraceISRHandle_t* pxISRHandle);

/*******************************************************************************
* xTraceISRBegin
*
* Registers the beginning of an Interrupt Service Routine, using a
* TraceISRHandle_t provided by xTraceISRRegister(...).
*
* Example:
*	 #define PRIO_OF_ISR_TIMER1 3 // the hardware priority of the interrupt
*	 TraceISRHandle_t xISRTimer1Handle = 0; // The ID set by the recorder
*	 ...
*	 xTraceISRRegister("ISRTimer1", PRIO_OF_ISR_TIMER1, &xISRTimer1Handle);
*	 ...
*	 void ISR_handler()
*	 {
*		 xTraceISRBegin(xISRTimer1Handle);
*		 ...
*		 xTraceISREnd(0);
*	 }
******************************************************************************/
traceResult xTraceISRBegin(TraceISRHandle_t xISRHandle);

/*******************************************************************************
* xTraceISREnd
*
* Registers the end of an Interrupt Service Routine.
*
* The parameter uxIsTaskSwitchRequired indicates if the interrupt has requested
* a task-switch (= 1), e.g., by signaling a semaphore. Otherwise (= 0) the
* interrupt is assumed to return to the previous context.
*
* Example:
*	 #define PRIO_OF_ISR_TIMER1 3 // the hardware priority of the interrupt
*	 TraceISRHandle_t xISRTimer1Handle = 0; // The ID set by the recorder
*	 ...
*	 xTraceISRRegister("ISRTimer1", PRIO_OF_ISR_TIMER1, &xISRTimer1Handle);
*	 ...
*	 void ISR_handler()
*	 {
*		 xTraceISRBegin(xISRTimer1Handle);
*		 ...
*		 xTraceISREnd(0);
*	 }
******************************************************************************/
traceResult xTraceISREnd(TraceBaseType_t xIsTaskSwitchRequired);

#if ((TRC_CFG_USE_TRACE_ASSERT) == 1)

traceResult xTraceISRGetCurrentNesting(int32_t* puiValue);

int32_t xTraceISRGetCurrentNestingReturned(void);

traceResult xTraceISRGetCurrent(TraceISRHandle_t* pxISRHandle);

#else /* ((TRC_CFG_USE_TRACE_ASSERT) == 1) */

#define xTraceISRGetCurrentNesting(puiValue) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(puiValue) = pxTraceISRInfo->coreInfos[TRC_CFG_GET_CURRENT_CORE()].stackIndex, TRC_SUCCESS)

#define xTraceISRGetCurrentNestingReturned() (pxTraceISRInfo->coreInfos[TRC_CFG_GET_CURRENT_CORE()].stackIndex)

#define xTraceISRGetCurrent(pxISRHandle) (xTraceISRGetCurrentNestingReturned() >= 0 ? (*(pxISRHandle) = pxTraceISRInfo->coreInfos[TRC_CFG_GET_CURRENT_CORE()].handleStack[xTraceISRGetCurrentNestingReturned()], TRC_SUCCESS) : TRC_FAIL)

#endif /* ((TRC_CFG_USE_TRACE_ASSERT) == 1) */

/* DEPRECATED */
TraceISRHandle_t xTraceSetISRProperties(const char* szName, uint32_t uiPriority);

/* DEPRECATED */
#define xTraceGetCurrentISRNesting(puiValue) xTraceISRGetCurrentNesting(puiValue)

/* DEPRECATED */
#define vTraceStoreISRBegin(xISRHandle) xTraceISRBegin(xISRHandle)

/* DEPRECATED */
#define vTraceStoreISREnd(xIsTaskSwitchRequired) xTraceISREnd(xIsTaskSwitchRequired)

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_ISR_H */
