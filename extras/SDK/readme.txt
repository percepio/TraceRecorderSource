Percepio Trace Recorder SDK v4.5.2
Copyright 2021 Percepio AB
www.percepio.com

The SDK is intended to simplify Trace Recorder integration for third party software.

API:
traceResult xTraceSDKRegisterObject(uint32_t uiEventCode, void* pxObject, uint32_t uiData)

Parameters:
uiEventCode - The event code.
pxObject - The pointer to the object.
uiData - The object's initial data.

Returns:
TRACE_SUCCESS or TRACE_FAIL.


traceResult xTraceSDKUnregisterObject(uint32_t uiEventCode, void* pxObject, uint32_t uiData)

This function will unregister an object in the trace.

Parameters:
uiEventCode - The event code.
pxObject - The pointer to the object.
uiData - The object's end data.

Returns:
TRACE_SUCCESS or TRACE_FAIL.


traceResult xTraceSDKSetObjectName(void* pxObject, const char* pszName)

This function will set an object's name.

Parameters:
pxObject - The pointer to the object.
pszName - The task name.

Returns:
TRACE_SUCCESS or TRACE_FAIL.


traceResult xTraceSDKSetObjectData(void* pxObject, uint32_t uiData)

This function will set an object's data.

Parameters:
pxObject - The pointer to the object.
uiData - The object's data.

Returns:
TRACE_SUCCESS or TRACE_FAIL.


traceResult xTraceSDKTaskSwitch(void* pxTCB, uint32_t uiPriority)

This function will register a task switch and the task's priority.

Parameters:
pxTCB - The pointer to the TCB.
uiPriority - The task priority.

Returns:
TRACE_SUCCESS or TRACE_FAIL.

traceResult xTraceSDKTaskReady(void* pxTCB)

This function will set a task to ready in the trace.

Parameters:
pxTCB - The pointer to the TCB.

Returns:
TRACE_SUCCESS or TRACE_FAIL.


traceResult xTraceSDKEventBegin(uint32_t uiEventCode, uint32_t uiPayloadSize)

This function will begin an event and allow for payload to be added via the
xTraceSDKEventAdd****() functions.

Parameters:
uiEventCode - The event code.
uiPayloadSize - The expected payload size in bytes.

Returns:
TRACE_SUCCESS or TRACE_FAIL.


traceResult xTraceSDKEventEnd(void)

This function ends an event.

Returns:
TRACE_SUCCESS or TRACE_FAIL.


traceResult xTraceSDKEventAddData(void* pvData, uint32_t uiSize)

This function will add data as payload.

Parameters:
pvData - The pointer to the data.
uiSize - The data size.

Returns:
TRACE_SUCCESS or TRACE_FAIL.


traceResult xTraceSDKEventAddObject(void* pxObject)

This function will add an object as payload.

Parameters:
pxObject - The pointer to the object.

Returns:
TRACE_SUCCESS or TRACE_FAIL.


traceResult xTraceSDKEventAdd32(uint32_t value)

This function will add an uint32_t as payload.

Parameters:
value - The value.

Returns:
TRACE_SUCCESS or TRACE_FAIL.


traceResult xTraceSDKEventAdd16(uint16_t value)

This function will add an uint16_t as payload.

Parameters:
value - The value.

Returns:
TRACE_SUCCESS or TRACE_FAIL.


traceResult xTraceSDKEventAdd8(uint8_t value)

This function will add an uint8_t as payload.

Parameters:
value - The value.

Returns:
TRACE_SUCCESS or TRACE_FAIL.
