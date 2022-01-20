/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC0)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for extensions.
*/

#ifndef TRC_EXTENSION_H
#define TRC_EXTENSION_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

traceResult xTraceExtensionCreate(const char *szName, uint8_t uiMajor, uint8_t uiMinor, uint16_t uiPatch, uint32_t uiEventCount, TraceExtensionHandle_t *pxExtensionHandle);
traceResult xTraceExtensionGetBaseEventId(TraceExtensionHandle_t xExtensionHandle, uint32_t *puiBaseEventId);
traceResult xTraceExtensionGetEventId(TraceExtensionHandle_t xExtensionHandle, uint32_t uiLocalEventId, uint32_t *puiGlobalEventId);
traceResult xTraceExtensionGetConfigName(TraceExtensionHandle_t xExtensionHandle, const char **pszName);

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_EXTENSION_H */
