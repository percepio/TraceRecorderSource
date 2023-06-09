/*
* Percepio Trace Recorder for Tracealyzer v4.8.0.hotfix1
* Copyright 2023 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * @file 
 * 
 * @brief Public trace event APIs.
 */

#ifndef TRC_EVENT_H
#define TRC_EVENT_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup trace_event_apis Trace Event APIs
 * @ingroup trace_recorder_apis
 * @{
 */

/**
 * @internal Macro helper for setting trace event parameter count.
 */
#define TRC_EVENT_SET_PARAM_COUNT(id, n) ((uint16_t)(((uint16_t)(id)) | ((((uint16_t)(n)) & 0xF) << 12)))

/**
 * @internal Macro helper for getting trace event parameter count.
 */
#define TRC_EVENT_GET_PARAM_COUNT(id) (((id) >> 12u) & 0xFU)

#if (TRC_CFG_CORE_COUNT > 1)
#define TRC_EVENT_SET_EVENT_COUNT(c)  ((uint16_t)(((TRC_CFG_GET_CURRENT_CORE() & 0xF) << 12) | ((uint16_t)(c) & 0xFFF)))
#else
#define TRC_EVENT_SET_EVENT_COUNT(c) ((uint16_t)(c))
#endif

/**
 * @internal Macro helper for setting base event data.
 */
#define SET_BASE_EVENT_DATA(pxEvent, eventId, paramCount, eventCount) \
	( \
		(pxEvent)->EventID = TRC_EVENT_SET_PARAM_COUNT(eventId, paramCount), \
		(pxEvent)->EventCount = TRC_EVENT_SET_EVENT_COUNT(eventCount), \
		xTraceTimestampGet(&(pxEvent)->TS) \
	)

/**
 * @internal Macro helper for resetting trace event data.
 */
#define RESET_EVENT_DATA(p) \
	( \
		(p)->pvBlob = 0, \
		(p)->size = 0, \
		(p)->offset = 0 \
	)

/**
 * @internal Macro optimization for getting trace event size.
 */
#define TRC_EVENT_GET_SIZE(pvAddress, puiSize) (*(uint32_t*)(puiSize) = sizeof(TraceEvent0_t) + (TRC_EVENT_GET_PARAM_COUNT(((TraceEvent0_t*)(pvAddress))->EventID)) * sizeof(TraceBaseType_t), TRC_SUCCESS)

/**
 * @internal Macro optimization for getting trace event data pointer with an offset.
 */
#define TRC_EVENT_GET_RAW_DATA(xEventHandle, uiOffset, uiSize, ppvData) ((void)(uiSize), *(void**)(ppvData) = (void*)&((uint8_t*)((TraceEventData_t*)(xEventHandle))->pvBlob)[uiOffset], TRC_SUCCESS)

/**
 * @internal Macro optimization for getting trace event payload pointer with an offset.
 */
#define TRC_EVENT_GET_PAYLOAD(xEventHandle, uiOffset, uiSize, ppvData) ((void)(uiSize), *(void**)(ppvData) = (void*)&((uint8_t*)((TraceEventData_t*)(xEventHandle))->pvBlob)[sizeof(TraceEvent0_t) + (uiOffset)], TRC_SUCCESS)

/**
 * @internal Macro optimization for getting trace event remaining payload size.
 */
#define TRC_EVENT_PAYLOAD_REMAINING(xEventHandle, puiValue) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(uint32_t*)(puiValue) = ((TraceEventData_t*)(xEventHandle))->size - ((TraceEventData_t*)(xEventHandle))->offset, TRC_SUCCESS)

/**
 * @internal Macro optimization for getting trace event used payload size.
 */
#define TRC_EVENT_PAYLOAD_USED(xEventHandle, puiValue) (*(uint32_t*)(puiValue) = ((TraceEventData_t*)(xEventHandle))->offset - sizeof(TraceEvent0_t), TRC_SUCCESS)

/**
 * @internal Macro optimization getting trace event payload size.
 */
#define TRC_EVENT_PAYLOAD_SIZE(xEventHandle, puiValue) (*(uint32_t*)(puiValue) = ((TraceEventData_t*)(xEventHandle))->size - sizeof(TraceEvent0_t), TRC_SUCCESS)

/**
 * @internal Macro optimization for adding a pointer address to trace event.
 */
#define TRC_EVENT_ADD_POINTER(xEventHandle, value) \
	TRC_COMMA_EXPR_TO_STATEMENT_EXPR_3( \
		((void**)((TraceEventData_t*)(xEventHandle))->pvBlob)[((TraceEventData_t*)(xEventHandle))->offset / sizeof(void*)] = (value), \
		((TraceEventData_t*)(xEventHandle))->offset += sizeof(void*), \
		TRC_SUCCESS \
	)

/**
 * @internal Macro optimization for adding a unsigned base type to trace event.
 */
#define TRC_EVENT_ADD_UNSIGNED_BASE_TYPE(xEventHandle, value) \
	TRC_COMMA_EXPR_TO_STATEMENT_EXPR_3( \
		((TraceUnsignedBaseType_t*)((TraceEventData_t*)(xEventHandle))->pvBlob)[((TraceEventData_t*)(xEventHandle))->offset / sizeof(TraceUnsignedBaseType_t)] = (value), \
		((TraceEventData_t*)(xEventHandle))->offset += sizeof(TraceUnsignedBaseType_t), \
		TRC_SUCCESS \
	)

/**
 * @internal Macro optimization for adding a 32-bit value to trace event.
 */
#define TRC_EVENT_ADD_32(xEventHandle, value) \
	TRC_COMMA_EXPR_TO_STATEMENT_EXPR_3( \
		((uint32_t*)((TraceEventData_t*)(xEventHandle))->pvBlob)[((TraceEventData_t*)(xEventHandle))->offset / sizeof(uint32_t)] = (value), \
		((TraceEventData_t*)(xEventHandle))->offset += sizeof(uint32_t), \
		TRC_SUCCESS \
	)

/**
 * @internal Macro optimization for adding a 16-bit value to trace event.
 */
#define TRC_EVENT_ADD_16(xEventHandle, value) \
	TRC_COMMA_EXPR_TO_STATEMENT_EXPR_3( \
		((uint16_t*)((TraceEventData_t*)(xEventHandle))->pvBlob)[((TraceEventData_t*)(xEventHandle))->offset / sizeof(uint16_t)] = (value), \
		((TraceEventData_t*)(xEventHandle))->offset += sizeof(uint16_t), \
		TRC_SUCCESS \
	)

/**
 * @internal Macro optimization for adding a 8-bit value to trace event.
 */
#define TRC_EVENT_ADD_8(xEventHandle, value) \
	TRC_COMMA_EXPR_TO_STATEMENT_EXPR_3( \
		((uint8_t*)((TraceEventData_t*)(xEventHandle))->pvBlob)[((TraceEventData_t*)(xEventHandle))->offset / sizeof(uint8_t)] = (value), \
		((TraceEventData_t*)(xEventHandle))->offset += sizeof(uint8_t), \
		TRC_SUCCESS \
	)

/**
 * @internal Macro optimization for beginning an offline trace event.
 */
#define TRC_EVENT_BEGIN_OFFLINE(uiEventCode, uiPayloadSize, pxEventHandle) \
	( \
		(xTraceEventBeginRawOffline((uint32_t)(sizeof(TraceEvent0_t) + (uiPayloadSize)), pxEventHandle)) == TRC_SUCCESS ? \
		( \
			SET_BASE_EVENT_DATA((TraceEvent0_t*)(((TraceEventData_t*)*(pxEventHandle))->pvBlob), \
				uiEventCode, \
				(((TraceEventData_t*)*(pxEventHandle))->size - sizeof(TraceEvent0_t)) / sizeof(TraceBaseType_t), \
				pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter), \
			((TraceEventData_t*)*(pxEventHandle))->offset += sizeof(TraceEvent0_t), \
			TRC_SUCCESS \
		) : TRC_FAIL \
	)

/**
 * @internal Trace Event Structure without uTraceUnsignedBaseType_t parameters
 */
typedef struct {	/* Aligned */
	uint16_t EventID;		/**< */
	uint16_t EventCount;	/**< */
	uint32_t TS;			/**< */
} TraceEvent0_t;

/**
 * @internal Trace Event Structure with one uTraceUnsignedBaseType_t parameter
 */
typedef struct {	/* Aligned */
	uint16_t EventID;						/**< */
	uint16_t EventCount;					/**< */
	uint32_t TS;							/**< */
	TraceUnsignedBaseType_t uxParams[1];	/**< */
} TraceEvent1_t;

/**
 * @internal Trace Event Structure with two uTraceUnsignedBaseType_t parameters
 */
typedef struct {	/* Aligned */
	uint16_t EventID;						/**< */
	uint16_t EventCount;					/**< */
	uint32_t TS;							/**< */
	TraceUnsignedBaseType_t uxParams[2];	/**< */
} TraceEvent2_t;

/**
 * @internal Trace Event Structure with three uTraceUnsignedBaseType_t parameters
 */
typedef struct {	/* Aligned */
	uint16_t EventID;						/**< */
	uint16_t EventCount;					/**< */
	uint32_t TS;							/**< */
	TraceUnsignedBaseType_t uxParams[3];	/**< */
} TraceEvent3_t;

/**
 * @internal Trace Event Structure with four uTraceUnsignedBaseType_t parameters
 */
typedef struct {	/* Aligned */
	uint16_t EventID;						/**< */
	uint16_t EventCount;					/**< */
	uint32_t TS;							/**< */
	TraceUnsignedBaseType_t uxParams[4];	/**< */
} TraceEvent4_t;

/**
 * @internal Trace Event Structure with five uTraceUnsignedBaseType_t parameters
 */
typedef struct {	/* Aligned */
	uint16_t EventID;						/**< */
	uint16_t EventCount;					/**< */
	uint32_t TS;							/**< */
	TraceUnsignedBaseType_t uxParams[5];	/**< */
} TraceEvent5_t;

/**
 * @internal Trace Event Structure with six uTraceUnsignedBaseType_t parameters
 */
typedef struct {	/* Aligned */
	uint16_t EventID;						/**< */
	uint16_t EventCount;					/**< */
	uint32_t TS;							/**< */
	TraceUnsignedBaseType_t uxParams[6];	/**< */
} TraceEvent6_t;

/**
 * @internal Trace Event Structure with seven uTraceUnsignedBaseType_t parameters
 */
typedef struct {	/* Aligned */
	uint16_t EventID;						/**< */
	uint16_t EventCount;					/**< */
	uint32_t TS;							/**< */
	TraceUnsignedBaseType_t uxParams[7];	/**< */
} TraceEvent7_t;

/** 
 * @internal Trace Event Data Structure
 */
typedef struct TraceEventData	/* Aligned */
{
	void* pvBlob;		/**< */
	uint32_t size;		/**< */
	uint32_t offset;	/**< */
} TraceEventData_t;

/** 
 * @internal Trace Core Event Data Structure
 */
typedef struct TraceCoreEventData	/* Aligned */
{
	TraceEventData_t eventData[(TRC_CFG_MAX_ISR_NESTING)+1];	/* aligned */
	uint32_t eventCounter;										/**< */
	uint32_t reserved;											/* alignment */
	TRACE_ALLOC_CRITICAL_SECTION()
} TraceCoreEventData_t;

/** 
 * @internal Trace Event Data Table Structure.
 */
typedef struct TraceEventDataTable	/* Aligned */
{
	TraceCoreEventData_t coreEventData[TRC_CFG_CORE_COUNT]; /**< Holds data about current event for each core/isr depth */
} TraceEventDataTable_t;

extern TraceEventDataTable_t* pxTraceEventDataTable;

/**
 * @internal Initialize event trace system.
 * 
 * @param[in] pxBuffer Pointer to memory that will be used by the event
 * trace system.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventInitialize(TraceEventDataTable_t* pxBuffer);

/**
 * @brief Gets trace event size.
 * 
 * @param[in] pvAddress Pointer to initialized trace event.
 * @param[out] puiSize Size.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventGetSize(const void* const pvAddress, uint32_t* puiSize);

/**
 * @brief Creates an event with 0 parameters.
 *
 * @param[in] uiEventCode Event code.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreate0(uint32_t uiEventCode);

/**
 * @brief Creates an event with 1 parameters.
 *
 * @param[in] uiEventCode Event code.
 * @param[in] uxParam1 First parameter.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreate1(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1);

/**
 * @brief Creates an event with 2 parameters.
 *
 * @param[in] uiEventCode Event code.
 * @param[in] uxParam1 First parameter.
 * @param[in] uxParam2 Second parameter.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreate2(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2);

/**
 * @brief Creates an event with 3 parameters.
 *
 * @param[in] uiEventCode Event code.
 * @param[in] uxParam1 First parameter.
 * @param[in] uxParam2 Second parameter.
 * @param[in] uxParam3 Third parameter.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreate3(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3);

/**
 * @brief Creates an event with 4 parameters.
 *
 * @param[in] uiEventCode Event code.
 * @param[in] uxParam1 First parameter.
 * @param[in] uxParam2 Second parameter.
 * @param[in] uxParam3 Third parameter.
 * @param[in] uxParam4 Fourth parameter.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreate4(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4);

/**
 * @brief Creates an event with 5 parameters.
 *
 * @param[in] uiEventCode Event code.
 * @param[in] uxParam1 First parameter.
 * @param[in] uxParam2 Second parameter.
 * @param[in] uxParam3 Third parameter.
 * @param[in] uxParam4 Fourth parameter.
 * @param[in] uxParam5 Fifth parameter.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreate5(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, TraceUnsignedBaseType_t uxParam5);

/**
 * @brief Creates an event with 6 parameters.
 *
 * @param[in] uiEventCode Event code.
 * @param[in] uxParam1 First parameter.
 * @param[in] uxParam2 Second parameter.
 * @param[in] uxParam3 Third parameter.
 * @param[in] uxParam4 Fourth parameter.
 * @param[in] uxParam5 Fifth parameter.
 * @param[in] uxParam6 Sixth parameter.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreate6(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, TraceUnsignedBaseType_t uxParam5, TraceUnsignedBaseType_t uxParam6);

/**
 * @internal Begins a raw trace event offline.
 * 
 * This routine begins a trace event with specified size. Must call xTraceEventEnd()
 * to finalize event creation. Does not care about RecorderEnabled.
 * 
 * @param[in] uiSize Size.
 * @param[in] pxEventHandle Pointer to initialized trace event.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventBeginRawOffline(uint32_t uiSize, TraceEventHandle_t* pxEventHandle);

/**
 * @internal Begins a blocking trace event offline. 
 * 
 * This routine begins a trace event with specified size. Must call xTraceEventEnd()
 * to finalize event creation. Does not care about RecorderEnabled.
 * 
 * @param[in] uiSize Size.
 * @param[in] pxEventHandle Pointer to initialized trace event.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventBeginRawOfflineBlocking(uint32_t uiSize, TraceEventHandle_t* pxEventHandle);

/**
 * @internal Begins a trace event offline.
 * 
 * This routine begins a trace event with specified size. Must call xTraceEventEnd()
 * to finalize event creation. Does not care about RecorderEnabled.
 * 
 * @param[in] uiSize Size.
 * @param[in] pxEventHandle Pointer to initialized trace event.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceEventBeginOffline TRC_EVENT_BEGIN_OFFLINE

/**
 * @brief Begins a trace event.
 * 
 * This routine begins a trace event with specified size. Must call xTraceEventEnd()
 * to finalize event creation. Does not care about RecorderEnabled.
 * 
 * @param[in] uiSize Size.
 * @param[in] pxEventHandle Pointer to initialized trace event.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceEventBegin(uiEventCode, uiTotalPayloadSize, pxEventHandle) \
	(xTraceIsRecorderEnabled() ? xTraceEventBeginOffline(uiEventCode, uiTotalPayloadSize, pxEventHandle) : TRC_FAIL)

/**
 * @internal Ends a trace event offline. 
 * 
 * This routine ends the event that was begun by calling on xTraceEventBegin().
 * Does not care about uiRecorderEnabled.
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventEndOffline(TraceEventHandle_t xEventHandle);

/**
 * @internal Ends a blocking event offline.
 * 
 * Ends the event that was begun by calling on xTraceEventBegin()
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventEndOfflineBlocking(TraceEventHandle_t xEventHandle);

/**
 * @brief Ends a trace event. 
 * 
 * This routine ends the event that was begun by calling on xTraceEventBegin().
 * Does not care about uiRecorderEnabled.
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceEventEnd(xEventHandle) \
	(!xTraceIsRecorderEnabled() ? TRC_FAIL : xTraceEventEndOffline(xEventHandle))

/**
 * @brief Adds data to event payload.
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] puxData Pointer to data.
 * @param[in] uxSize Size.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventAddData(TraceEventHandle_t xEventHandle, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);

/**
 * @brief Adds string to event payload.
 *
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] szString Pointer to string.
 * @param[in] uiLength Size.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceEventAddString(xEventHandle, szString, uiLength) xTraceEventAddData(xEventHandle, (TraceUnsignedBaseType_t*)(szString), (((uiLength) + (sizeof(TraceUnsignedBaseType_t) - 1)) / sizeof(TraceUnsignedBaseType_t)))

#if ((TRC_CFG_USE_TRACE_ASSERT) == 1)

/**
 * @brief Gets trace event data pointer with an offset.
 * 
 * This routine gets a trace event data pointer with an offset. It also verifies
 * the size so it won't go outside its' buffer. 
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] uiOffset Offset.
 * @param[in] uiSize Size.
 * @param[out] ppvData Data.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventGetRawData(TraceEventHandle_t xEventHandle, uint32_t uiOffset, uint32_t uiSize, void** ppvData);

/**
 * @brief Gets trace event payload pointer with an offset.
 * 
 * This routine gets a trace event payload pointer with an offset. It also verifies
 * the size so it won't go outside its payload buffer.
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] uiOffset Offset.
 * @param[in] uiSize Size.
 * @param[out] ppvData Data.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventGetPayload(const TraceEventHandle_t xEventHandle, uint32_t uiOffset, uint32_t uiSize, void** ppvData);

/**
 * @brief Gets the amount of remaining trace event payload.
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[out] puiValue Value.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventPayloadRemaining(const TraceEventHandle_t xEventHandle, uint32_t* puiValue);

/**
 * @brief Gets the amount of used trace event payload.
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[out] puiValue Value 
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventPayloadUsed(TraceEventHandle_t xEventHandle, uint32_t* puiValue);

/**
 * @brief Gets trace event payload size. 
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[out] puiValue Value
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventPayloadSize(TraceEventHandle_t xEventHandle, uint32_t* puiValue);

/**
 * @brief Adds an unsigned base type value as trace event payload 
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] uxValue Value. 
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventAddUnsignedBaseType(TraceEventHandle_t xEventHandle, TraceUnsignedBaseType_t uxValue);

/**
 * @brief Adds a pointer address as trace event payload
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] pvAddress Address.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventAddPointer(TraceEventHandle_t xEventHandle, void *pvAddress);

/**
 * @brief Adds an uint32_t as trace event payload 
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] value Value.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventAdd32(TraceEventHandle_t xEventHandle, uint32_t value);

/**
 * @brief Adds an uint16_t as trace event payload 
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] value Value.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventAdd16(TraceEventHandle_t xEventHandle, uint16_t value);

/**
 * @brief Adds an uint8_t as trace event payload.
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] value Value. 
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventAdd8(TraceEventHandle_t xEventHandle, uint8_t value);

#else /* ((TRC_CFG_USE_TRACE_ASSERT) == 1) */

/**
 * @brief Gets trace event size.
 * 
 * @param[in] pvAddress Pointer to initialized trace event.
 * @param[out] puiSize Size.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceEventGetSize(pvAddress, puiSize) (*(uint32_t*)(puiSize) = sizeof(TraceEvent0_t) + (TRC_EVENT_GET_PARAM_COUNT(((TraceEvent0_t*)(pvAddress))->EventID)) * sizeof(uint32_t), TRC_SUCCESS)

/**
 * @brief Gets trace event data pointer with an offset.
 * 
 * This routine gets a trace event data pointer with an offset. It also verfies
 * that the size so it won't go outside its buffer. 
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] uiOffset Offset.
 * @param[in] uiSize Size.
 * @param[out] ppvData Data.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceEventGetRawData TRC_EVENT_GET_RAW_DATA

/**
 * @brief Gets trace event payload pointer with an offset.
 * 
 * This routine gets a trace event payload pointer with an offset. It also verifies
 * that the size so it won't go outside its payload buffer.
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] uiOffset Offset.
 * @param[in] uiSize Size.
 * @param[out] ppvData Data.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceEventGetPayload TRC_EVENT_GET_PAYLOAD

/**
 * @brief Gets the amount of remaining trace event payload.
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[out] puiValue Value.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceEventPayloadRemaining TRC_EVENT_PAYLOAD_REMAINING

/**
 * @brief Gets the amount of used trace event payload.
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[out] puiValue Value 
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceEventPayloadUsed TRC_EVENT_PAYLOAD_USED

/**
 * @brief Gets trace event payload size. 
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[out] puiValue Value
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceEventPayloadSize TRC_EVENT_PAYLOAD_SIZE

/* Adds a pointer as event payload with no errors checks */
#define xTraceEventAddPointer TRC_EVENT_ADD_POINTER

/**
 * @brief Adds an unsigned base type value as trace event payload 
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] uxValue Value. 
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceEventAddUnsignedBaseType TRC_EVENT_ADD_UNSIGNED_BASE_TYPE

/**
 * @brief Adds an uint32_t as trace event payload 
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] value Value.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceEventAdd32 TRC_EVENT_ADD_32

/**
 * @brief Adds an uint16_t as trace event payload 
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] value Value.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceEventAdd16 TRC_EVENT_ADD_16

/**
 * @brief Adds an uint8_t as trace event payload.
 * 
 * @param[in] xEventHandle Pointer to initialized trace event.
 * @param[in] value Value. 
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceEventAdd8 TRC_EVENT_ADD_8

#endif /* ((TRC_CFG_USE_TRACE_ASSERT) == 1) */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_EVENT_H */
