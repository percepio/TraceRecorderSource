/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC1)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for state machines.
*/

#ifndef TRC_STATE_MACHINE_H
#define TRC_STATE_MACHINE_H

#include <trcTypes.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#ifdef __cplusplus
extern "C" {
#endif

traceResult xTraceStateMachineCreate(const char *szName, TraceStateMachineHandle_t *pxStateMachineHandle);
traceResult xTraceStateMachineStateCreate(TraceStateMachineHandle_t xStateMachineHandle, const char *szName, TraceStateMachineStateHandle_t *pxStateHandle);
traceResult xTraceStateMachineSetState(TraceStateMachineHandle_t xStateMachineHandle, TraceStateMachineStateHandle_t xStateHandle);

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_STATE_MACHINE_H */
