/*
 * Percepio Trace Recorder SDK for Tracealyzer v4.5.2
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Percepio Tracealyzer Recorder SDK
 */

#ifndef TRC_SDK_H
#define TRC_SDK_H

#include <stdint.h>
#include "trcPortDefines.h"
#include "trcRecorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* xTraceSDKRegisterObject
*
* This function will register an object in the trace.
*
* @param uiEventCode The event code.
* @param pxObject The pointer to the object.
* @param uiData The object's initial data.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKRegisterObject(uint32_t uiEventCode, void* pxObject, uint32_t uiData);

/*******************************************************************************
* xTraceSDKUnregisterObject
*
* This function will unregister an object in the trace.
*
* @param uiEventCode The event code.
* @param pxObject The pointer to the object.
* @param uiData The object's end data.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKUnregisterObject(uint32_t uiEventCode, void* pxObject, uint32_t uiData);

/*******************************************************************************
* xTraceSDKSetObjectName
*
* This function will set an object's name.
*
* @param pxObject The pointer to the object.
* @param pszName The task name.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKSetObjectName(void* pxObject, const char* pszName);

/*******************************************************************************
* xTraceSDKSetObjectData
*
* This function will register a task in the trace.
*
* @param pxObject The pointer to the object.
* @param uiData The object's data.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKSetObjectData(void* pxObject, uint32_t uiData);

/*******************************************************************************
* xTraceSDKTaskSwitch
*
* This function will register a task switch and priority.
*
* @param pxTCB The pointer to the TCB.
* @param uiPriority The task priority.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKTaskSwitch(void* pxTCB, uint32_t uiPriority);

/*******************************************************************************
* xTraceSDKTaskReady
*
* This function will set a task to ready in the trace.
*
* @param pxTCB The pointer to the TCB.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKTaskReady(void* pxTCB);

/*******************************************************************************
* xTraceSDKEventBegin
*
* This function will begin an event and allow for payload to be added via the
* xTraceSDKEventAdd****() functions.
*
* @param uiEventCode The event code.
* @param uiPayloadSize The expected payload size in bytes.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKEventBegin(uint32_t uiEventCode, uint32_t uiPayloadSize);

/*******************************************************************************
* xTraceSDKEventEnd
*
* This function ends an event.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKEventEnd(void);

/*******************************************************************************
* xTraceSDKEventAddData
*
* This function will add a variable size of data as payload.
*
* @param pvData The pointer to the data.
* @param uiSize The data size.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKEventAddData(void* pvData, uint32_t uiSize);

/*******************************************************************************
* xTraceSDKEventAddObject
*
* This function will add an ojbect as payload.
*
* @param pxObject The pointer to the object.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKEventAddObject(void* pxObject);

/*******************************************************************************
* xTraceSDKEventAdd32
*
* This function will add an uint32_t as payload.
*
* @param value The value.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKEventAdd32(uint32_t value);

/*******************************************************************************
* xTraceSDKEventAdd16
*
* This function will add an uint16_t as payload.
*
* @param value The value.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKEventAdd16(uint16_t value);

/*******************************************************************************
* xTraceSDKEventAdd8
*
* This function will add an uint8_t as payload.
*
* @param value The value.
* @return Function result. TRACE_SUCCESS or TRACE_FAIL.
******************************************************************************/
traceResult xTraceSDKEventAdd8(uint8_t value);

#ifdef __cplusplus
}
#endif

#endif /* TRC_SDK_H */
