/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC1)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for strings.
*/

#ifndef TRC_OBJECT_H
#define TRC_OBJECT_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

traceResult xTraceObjectRegisterInternal(uint32_t uiEventCode, void* pvObject, const char* szName, TraceUnsignedBaseType_t uxStateCount, TraceUnsignedBaseType_t uxStates[], TraceUnsignedBaseType_t uxOptions, TraceObjectHandle_t* pxObjectHandle);

traceResult xTraceObjectRegister(uint32_t uiEventCode, void *pvObject, const char* szName, TraceUnsignedBaseType_t uxState, TraceObjectHandle_t *pxObjectHandle);

traceResult xTraceObjectUnregister(TraceObjectHandle_t xObjectHandle, uint32_t uiEventCode, TraceUnsignedBaseType_t uxState);

traceResult xTraceObjectSetName(TraceObjectHandle_t xObjectHandle, const char *szName);

#define xTraceObjectSetState(xObjectHandle, uxState) xTraceObjectSetSpecificState(xObjectHandle, 0, uxState)

#define xTraceObjectSetSpecificState(xObjectHandle, uiIndex, uxState) xTraceEntrySetState((TraceEntryHandle_t)(xObjectHandle), uiIndex, uxState)

#define xTraceObjectSetOptions(xObjectHandle, uiOptions) xTraceEntrySetOptions((TraceEntryHandle_t)(xObjectHandle), uiOptions)

traceResult xTraceObjectRegisterWithoutHandle(uint32_t uiEventCode, void* pvObject, const char* szName, TraceUnsignedBaseType_t uxState);

traceResult xTraceObjectUnregisterWithoutHandle(uint32_t uiEventCode, void* pvObject, TraceUnsignedBaseType_t uxState);

traceResult xTraceObjectSetNameWithoutHandle(void* pvObject, const char* szName);

#define xTraceObjectSetStateWithoutHandle(pvObject, uxState) xTraceObjectSetSpecificStateWithoutHandle(pvObject, 0, uxState)

traceResult xTraceObjectSetSpecificStateWithoutHandle(void* pvObject, uint32_t uiIndex, TraceUnsignedBaseType_t uxState);

traceResult xTraceObjectSetOptionsWithoutHandle(void* pvObject, uint32_t uiOptions);

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_OBJECT_H */
