/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC1)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for strings.
*/

#ifndef TRC_STRING_H
#define TRC_STRING_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* xTraceStringRegister
*
* Register strings in the recorder, e.g. for names of user event channels.
*
* Example:
*	 TraceStringHandle_t myEventHandle;
*	 xTraceStringRegister("MyUserEvent", &myEventHandle);
*	 ...
*	 xTracePrintF(myEventHandle, "My value is: %d", myValue);
******************************************************************************/
traceResult xTraceStringRegister(const char *szString, TraceStringHandle_t* pString);

/* DEPRECATED */
TraceStringHandle_t xTraceRegisterString(const char *name);

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_STRING_H */
