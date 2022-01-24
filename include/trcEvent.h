/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC1)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for events.
*/

#ifndef TRC_EVENT_H
#define TRC_EVENT_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif


#define TRC_EVENT_SET_PARAM_COUNT(id, n) (((uint16_t)(id)) | ((((uint16_t)(n)) & 0xF) << 12))
#define TRC_EVENT_GET_PARAM_COUNT(id) (((id) >> 12) & 0xF)

#if (TRC_CFG_CORE_COUNT > 1)
#define TRC_EVENT_SET_EVENT_COUNT(c)  (((TRC_CFG_GET_CURRENT_CORE() & 0xF) << 12) | ((uint16_t)(c) & 0xFFF))
#else
#define TRC_EVENT_SET_EVENT_COUNT(c) (uint16_t)(c)
#endif

#define SET_BASE_EVENT_DATA(pxEvent, eventId, paramCount, eventCount) \
	( \
		(pxEvent)->EventID = TRC_EVENT_SET_PARAM_COUNT(eventId, paramCount), \
		(pxEvent)->EventCount = TRC_EVENT_SET_EVENT_COUNT(eventCount), \
		xTraceTimestampGet(&(pxEvent)->TS) \
	)

#define RESET_EVENT_DATA(p) \
	( \
		(p)->pvBlob = 0, \
		(p)->size = 0, \
		(p)->offset = 0 \
	)

#define TRC_EVENT_GET_SIZE(pvAddress, puiSize) (*(uint32_t*)(puiSize) = sizeof(TraceBaseEvent_t) + (TRC_EVENT_GET_PARAM_COUNT(((TraceBaseEvent_t*)(pvAddress))->EventID)) * sizeof(uint32_t), TRC_SUCCESS)

/* Retrieves a data pointer with an offset. Also verifies the size so it won't go outside buffer. */
#define TRC_EVENT_GET_RAW_DATA(xEventHandle, uiOffset, uiSize, ppvData) ((void)(uiSize), *(void**)(ppvData) = (void*)&((uint8_t*)((TraceEventData_t*)(xEventHandle))->pvBlob)[uiOffset], TRC_SUCCESS)

/* Retrieves a payload pointer with an offset. Also verifies the size so it won't go outside payload buffer. */
#define TRC_EVENT_GET_PAYLOAD(xEventHandle, uiOffset, uiSize, ppvData) ((void)(uiSize), *(void**)(ppvData) = (void*)&((uint8_t*)((TraceEventData_t*)(xEventHandle))->pvBlob)[sizeof(TraceBaseEvent_t) + (uiOffset)], TRC_SUCCESS)

/* Get the amount of remaining payload. */
#define TRC_EVENT_PAYLOAD_REMAINING(xEventHandle, puiValue) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(uint32_t*)(puiValue) = ((TraceEventData_t*)(xEventHandle))->size - ((TraceEventData_t*)(xEventHandle))->offset, TRC_SUCCESS)

/* Get the amount of used payload. */
#define TRC_EVENT_PAYLOAD_USED(xEventHandle, puiValue) (*(uint32_t*)(puiValue) = ((TraceEventData_t*)(xEventHandle))->offset - sizeof(TraceBaseEvent_t), TRC_SUCCESS)

#define TRC_EVENT_PAYLOAD_SIZE(xEventHandle, puiValue) (*(uint32_t*)(puiValue) = ((TraceEventData_t*)(xEventHandle))->size - sizeof(TraceBaseEvent_t), TRC_SUCCESS)

#define TRC_EVENT_ADD_POINTER(xEventHandle, value) \
	TRC_COMMA_EXPR_TO_STATEMENT_EXPR_3( \
		((void**)((TraceEventData_t*)(xEventHandle))->pvBlob)[((TraceEventData_t*)(xEventHandle))->offset / sizeof(void*)] = (value), \
		((TraceEventData_t*)(xEventHandle))->offset += sizeof(void*), \
		TRC_SUCCESS \
	)

#define TRC_EVENT_ADD_UNSIGNED_BASE_TYPE(xEventHandle, value) \
	TRC_COMMA_EXPR_TO_STATEMENT_EXPR_3( \
		((TraceUnsignedBaseType_t*)((TraceEventData_t*)(xEventHandle))->pvBlob)[((TraceEventData_t*)(xEventHandle))->offset / sizeof(TraceUnsignedBaseType_t)] = (value), \
		((TraceEventData_t*)(xEventHandle))->offset += sizeof(TraceUnsignedBaseType_t), \
		TRC_SUCCESS \
	)

#define TRC_EVENT_ADD_32(xEventHandle, value) \
	TRC_COMMA_EXPR_TO_STATEMENT_EXPR_3( \
		((uint32_t*)((TraceEventData_t*)(xEventHandle))->pvBlob)[((TraceEventData_t*)(xEventHandle))->offset / sizeof(uint32_t)] = (value), \
		((TraceEventData_t*)(xEventHandle))->offset += sizeof(uint32_t), \
		TRC_SUCCESS \
	)

#define TRC_EVENT_ADD_16(xEventHandle, value) \
	TRC_COMMA_EXPR_TO_STATEMENT_EXPR_3( \
		((uint16_t*)((TraceEventData_t*)(xEventHandle))->pvBlob)[((TraceEventData_t*)(xEventHandle))->offset / sizeof(uint16_t)] = (value), \
		((TraceEventData_t*)(xEventHandle))->offset += sizeof(uint16_t), \
		TRC_SUCCESS \
	)

#define TRC_EVENT_ADD_8(xEventHandle, value) \
	TRC_COMMA_EXPR_TO_STATEMENT_EXPR_3( \
		((uint8_t*)((TraceEventData_t*)(xEventHandle))->pvBlob)[((TraceEventData_t*)(xEventHandle))->offset / sizeof(uint8_t)] = (value), \
		((TraceEventData_t*)(xEventHandle))->offset += sizeof(uint8_t), \
		TRC_SUCCESS \
	)

#define TRC_EVENT_BEGIN_OFFLINE(uiEventCode, uiPayloadSize, pxEventHandle) \
	( \
		(xTraceEventBeginRawOffline(sizeof(TraceBaseEvent_t) + (uiPayloadSize), pxEventHandle)) == TRC_SUCCESS ? \
		( \
			pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter++, \
			SET_BASE_EVENT_DATA((TraceBaseEvent_t*)(((TraceEventData_t*)*(pxEventHandle))->pvBlob), \
				uiEventCode, \
				(((TraceEventData_t*)*(pxEventHandle))->size - sizeof(TraceBaseEvent_t)) / sizeof(uint32_t), \
				pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter), \
			((TraceEventData_t*)*(pxEventHandle))->offset += sizeof(TraceBaseEvent_t), \
			TRC_SUCCESS \
		) : TRC_FAIL \
	)

#define TRC_EVENT_END_OFFLINE(xEventHandle) \
	TRC_COMMA_EXPR_TO_STATEMENT_EXPR_4( \
		xTraceStreamPortCommit(((TraceEventData_t*)(xEventHandle))->pvBlob, \
		((TraceEventData_t*)(xEventHandle))->size, &DUMMY_iTraceBytesCommitted), \
		RESET_EVENT_DATA((TraceEventData_t*)(xEventHandle)), \
		TRC_SUCCESS \
	)

typedef struct {
	uint16_t EventID;
	uint16_t EventCount;
	uint32_t TS;
} TraceBaseEvent_t;

/* This type contains information about, and pointer to, the current event being written */
typedef struct TraceEventData
{
	void* pvBlob;
	uint32_t size;
	uint32_t offset;
} TraceEventData_t;

typedef struct TraceCoreEventData
{
	TraceEventData_t eventData[(TRC_CFG_MAX_ISR_NESTING)+1];
	uint32_t eventCounter;
} TraceCoreEventData_t;

typedef struct TraceEventDataTable
{
	/* These contain data about current event for each core/isr depth*/
	TraceCoreEventData_t coreEventData[TRC_CFG_CORE_COUNT];
} TraceEventDataTable_t;

#define TRC_EVENT_DATA_BUFFER_SIZE (sizeof(TraceEventDataTable_t))

typedef struct TraceEventDataBuffer
{
	uint8_t buffer[TRC_EVENT_DATA_BUFFER_SIZE];
} TraceEventDataBuffer_t;

extern TraceEventDataTable_t* pxTraceEventDataTable;

/* Initialize the internal event data structures */
traceResult xTraceEventInitialize(TraceEventDataBuffer_t* pxBuffer);

/* Get event size */
traceResult xTraceEventGetSize(void* pvAddress, uint32_t* puiSize);

/* Begins an event with specified size. Must call xTraceEventEnd() to finalize event creation. Does not care about RecorderEnabled. */
traceResult xTraceEventBeginRawOffline(uint32_t uiSize, TraceEventHandle_t* pxEventHandle);

/* Begins an event with specified size. Must call xTraceEventEnd() to finalize event creation. Does not care about RecorderEnabled. */
traceResult xTraceEventBeginRawOfflineBlocking(uint32_t uiSize, TraceEventHandle_t* pxEventHandle);

/* Begins an event with defined specified payload size. Must call xTraceEventEnd() to finalize event creation. Does not care about uiRecorderEnabled. */
#define xTraceEventBeginOffline TRC_EVENT_BEGIN_OFFLINE

/* Begins an event with defined specified payload size. Must call xTraceEventEnd() to finalize event creation. */
#define xTraceEventBegin(uiEventCode, uiTotalPayloadSize, pxEventHandle) \
	(xTraceIsRecorderEnabled() ? xTraceEventBeginOffline(uiEventCode, uiTotalPayloadSize, pxEventHandle) : TRC_FAIL)

/* Ends the event that was begun by calling on xTraceEventBegin(). Does not care about uiRecorderEnabled. */
traceResult xTraceEventEndOffline(TraceEventHandle_t xEventHandle);

/* Ends the event that was begun by calling on xTraceEventBegin() */
traceResult xTraceEventEndOfflineBlocking(TraceEventHandle_t xEventHandle);

/* Ends the event that was begun by calling on xTraceEventBegin() */
#define xTraceEventEnd(xEventHandle) \
	(xTraceIsRecorderEnabled() == 0 ? TRC_FAIL : xTraceEventEndOffline(xEventHandle))

/* Adds data of size uiSize as event payload */
traceResult xTraceEventAddData(TraceEventHandle_t xEventHandle, void* pvData, uint32_t uiSize);

#if ((TRC_CFG_USE_TRACE_ASSERT) == 1)

/* Retrieves a data pointer with an offset. Also verifies the size so it won't go outside buffer. */
traceResult xTraceEventGetRawData(TraceEventHandle_t xEventHandle, uint32_t uiOffset, uint32_t uiSize, void** ppvData);

/* Retrieves a payload pointer with an offset. Also verifies the size so it won't go outside payload buffer. */
traceResult xTraceEventGetPayload(TraceEventHandle_t xEventHandle, uint32_t uiOffset, uint32_t uiSize, void** ppvData);

/* Get the amount of remaining payload. */
traceResult xTraceEventPayloadRemaining(TraceEventHandle_t xEventHandle, uint32_t* puiValue);

/* Get the amount of used payload. */
traceResult xTraceEventPayloadUsed(TraceEventHandle_t xEventHandle, uint32_t* puiValue);

/* Get the payload size. */
traceResult xTraceEventPayloadSize(TraceEventHandle_t xEventHandle, uint32_t* puiValue);

/* Adds an unsigned base type value as event payload */
traceResult xTraceEventAddUnsignedBaseType(TraceEventHandle_t xEventHandle, TraceUnsignedBaseType_t uxValue);

/* Adds a pointer as event payload */
traceResult xTraceEventAddPointer(TraceEventHandle_t xEventHandle, void *pvAddress);

/* Adds an uint32_t as event payload */
traceResult xTraceEventAdd32(TraceEventHandle_t xEventHandle, uint32_t value);

/* Adds an uint16_t as event payload */
traceResult xTraceEventAdd16(TraceEventHandle_t xEventHandle, uint16_t value);

/* Adds an uint8_t as event payload */
traceResult xTraceEventAdd8(TraceEventHandle_t xEventHandle, uint8_t value);

#else /* ((TRC_CFG_USE_TRACE_ASSERT) == 1) */

/* These are highly optimized for when asserts aren't used */

#define xTraceEventGetSize(pvAddress, puiSize) (*(uint32_t*)(puiSize) = sizeof(TraceBaseEvent_t) + (TRC_EVENT_GET_PARAM_COUNT(((TraceBaseEvent_t*)(pvAddress))->EventID)) * sizeof(uint32_t), TRC_SUCCESS)

/* Retrieves a data pointer with an offset. Also verifies the size so it won't go outside buffer. */
#define xTraceEventGetRawData TRC_EVENT_GET_RAW_DATA

/* Retrieves a payload pointer with an offset. Also verifies the size so it won't go outside payload buffer. */
#define xTraceEventGetPayload TRC_EVENT_GET_PAYLOAD

/* Get the amount of remaining payload. */
#define xTraceEventPayloadRemaining TRC_EVENT_PAYLOAD_REMAINING

/* Get the amount of used payload. */
#define xTraceEventPayloadUsed TRC_EVENT_PAYLOAD_USED

/* Get the payload size. */
#define xTraceEventPayloadSize TRC_EVENT_PAYLOAD_SIZE

/* Adds a pointer as event payload with no errors checks */
#define xTraceEventAddPointer TRC_EVENT_ADD_POINTER

/* Adds a unsigned base type value as event payload with no errors checks */
#define xTraceEventAddUnsignedBaseType TRC_EVENT_ADD_UNSIGNED_BASE_TYPE

/* Adds an uint32_t as event payload with no errors checks */
#define xTraceEventAdd32 TRC_EVENT_ADD_32

/* Adds an uint16_t as event payload with no errors checks */
#define xTraceEventAdd16 TRC_EVENT_ADD_16

/* Adds an uint8_t as event payload with no errors checks */
#define xTraceEventAdd8 TRC_EVENT_ADD_8

#endif /* ((TRC_CFG_USE_TRACE_ASSERT) == 1) */

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_EVENT_H */
