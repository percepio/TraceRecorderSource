/*
* Percepio Trace Recorder for Tracealyzer v4.11.0
* Copyright 2025 Percepio AB
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
 * @internal Trace Core Event Data Structure
 */
typedef struct TraceCoreEventData	/* Aligned */
{
	uint32_t eventCounter;										/**< */
	uint32_t reserved;											/* alignment */
} TraceCoreEventData_t;

/** 
 * @internal Trace Event Data Table Structure.
 */
typedef struct TraceEventDataTable	/* Aligned */
{
	TraceCoreEventData_t coreEventData[TRC_CFG_CORE_COUNT]; /**< Holds data about current event for each core/isr depth */
} TraceEventDataTable_t;

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
 * Create a raw data event (i.e. not event code provided)
 * @param pxSource The source buffer which should be copied
 * @param ulSize The size of the data to be copied
 *
 * @retval TRC_FAIL
 * @retval TRC_SUCCESS
 */
traceResult xTraceEventCreateRawBlocking(const void* pxSource, uint32_t ulSize);

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
 * @brief Creates an offline event with no parameters and a payload
 *
 * @param[in] uiEventCode Event code.
 * @param[in] puxData Pointer to payload buffer
 * @param[in] uxSize Size of the payload buffer
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreateDataOffline0(uint32_t uiEventCode, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);


/**
 * @brief Creates an event with no parameters and a payload
 *
 * @param[in] uiEventCode Event code.
 * @param[in] puxData Pointer to payload buffer
 * @param[in] uxSize Size of the payload buffer
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreateData0(uint32_t uiEventCode, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);

/**
 * @brief Creates an event with 1 parameter and a payload
 *
 * @param[in] uiEventCode Event code.
 * @param[in] uxParam1 First parameter.
 * @param[in] puxData Pointer to payload buffer.
 * @param[in] uxSize Size of the payload buffer.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreateData1(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);

/**
 * @brief Creates an event with 2 parameters and a payload
 *
 * @param[in] uiEventCode Event code.
 * @param[in] uxParam1 First parameter.
 * @param[in] uxParam2 Second parameter.
 * @param[in] puxData Pointer to payload buffer.
 * @param[in] uxSize Size of the payload buffer.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreateData2(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);

/**
 * @brief Creates an event with 3 parameters and a payload
 *
 * @param[in] uiEventCode Event code.
 * @param[in] uxParam1 First parameter.
 * @param[in] uxParam2 Second parameter.
 * @param[in] uxParam3 Third parameter.
 * @param[in] puxData Pointer to payload buffer.
 * @param[in] uxSize Size of the payload buffer.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreateData3(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);

/**
 * @brief Creates an event with 4 parameters and a payload
 *
 * @param[in] uiEventCode Event code.
 * @param[in] uxParam1 First parameter.
 * @param[in] uxParam2 Second parameter.
 * @param[in] uxParam3 Third parameter.
 * @param[in] uxParam4 Fourth parameter.
 * @param[in] puxData Pointer to payload buffer.
 * @param[in] uxSize Size of the payload buffer.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreateData4(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);

/**
 * @brief Creates an event with 5 parameters and a payload
 *
 * @param[in] uiEventCode Event code.
 * @param[in] uxParam1 First parameter.
 * @param[in] uxParam2 Second parameter.
 * @param[in] uxParam3 Third parameter.
 * @param[in] uxParam4 Fourth parameter.
 * @param[in] uxParam5 Fifth parameter.
 * @param[in] puxData Pointer to payload buffer.
 * @param[in] uxSize Size of the payload buffer.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreateData5(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, TraceUnsignedBaseType_t uxParam5, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);

/**
 * @brief Creates an event with 6 parameters and a payload
 *
 * @param[in] uiEventCode Event code.
 * @param[in] uxParam1 First parameter.
 * @param[in] uxParam2 Second parameter.
 * @param[in] uxParam3 Third parameter.
 * @param[in] uxParam4 Fourth parameter.
 * @param[in] uxParam5 Fifth parameter.
 * @param[in] uxParam6 Sixth parameter.
 * @param[in] puxData Pointer to payload buffer.
 * @param[in] uxSize Size of the payload buffer.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceEventCreateData6(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, TraceUnsignedBaseType_t uxParam5, TraceUnsignedBaseType_t uxParam6, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);

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

/* Helper macros */

/**
 * @brief Helper macro that handles type casting for all parameters. Accepts no parameters.
 * 
 * @param[in] _eventID Event id
 */
#define prvTraceEventCreate0(_eventID) xTraceEventCreate0(_eventID)

/**
 * @brief Helper macro that handles type casting for all parameters. Accepts 1 parameters.
 * 
 * @param[in] _eventID Event id
 * @param[in] _param Param
 */
#define prvTraceEventCreate1(_eventID, _param) xTraceEventCreate1(_eventID, (TraceUnsignedBaseType_t)(_param))

/**
 * @brief Helper macro that handles type casting for all parameters. Accepts 2 parameters.
 * 
 * @param[in] _eventID Event id
 * @param[in] _param1 Param 1
 * @param[in] _param2 Param 2
 */
#define prvTraceEventCreate2(_eventID, _param1, _param2)  xTraceEventCreate2(_eventID, (TraceUnsignedBaseType_t)(_param1), (TraceUnsignedBaseType_t)(_param2))

/**
 * @brief Helper macro that handles type casting for all parameters. Accepts 3 parameters.
 * 
 * @param[in] _eventID Event id
 * @param[in] _param1 Param 1
 * @param[in] _param2 Param 2
 * @param[in] _param3 Param 3
 */
#define prvTraceEventCreate3(_eventID, _param1, _param2, _param3) xTraceEventCreate3(_eventID, (TraceUnsignedBaseType_t)(_param1), (TraceUnsignedBaseType_t)(_param2), (TraceUnsignedBaseType_t)(_param3))

/**
 * @brief Helper macro that handles type casting for all parameters. Accepts 4 parameters.
 * 
 * @param[in] _eventID Event id
 * @param[in] _param1 Param 1
 * @param[in] _param2 Param 2
 * @param[in] _param3 Param 3
 * @param[in] _param4 Param 4
 */
#define prvTraceEventCreate4(_eventID, _param1, _param2, _param3, _param4) xTraceEventCreate4(_eventID, (TraceUnsignedBaseType_t)(_param1), (TraceUnsignedBaseType_t)(_param2), (TraceUnsignedBaseType_t)(_param3), (TraceUnsignedBaseType_t)(_param4))

/**
 * @brief Helper macro that handles type casting for all parameters. Accepts 5 parameters.
 * 
 * @param[in] _eventID Event id
 * @param[in] _param1 Param 1
 * @param[in] _param2 Param 2
 * @param[in] _param3 Param 3
 * @param[in] _param4 Param 4
 * @param[in] _param5 Param 5
 */
#define prvTraceEventCreate5(_eventID, _param1, _param2, _param3, _param4, _param5) xTraceEventCreate5(_eventID, (TraceUnsignedBaseType_t)(_param1), (TraceUnsignedBaseType_t)(_param2), (TraceUnsignedBaseType_t)(_param3), (TraceUnsignedBaseType_t)(_param4), (TraceUnsignedBaseType_t)(_param5))

/**
 * @brief Helper macro that handles type casting for all parameters. Accepts 6 parameters.
 * 
 * @param[in] _eventID Event id
 * @param[in] _param1 Param 1
 * @param[in] _param2 Param 2
 * @param[in] _param3 Param 3
 * @param[in] _param4 Param 4
 * @param[in] _param5 Param 5
 * @param[in] _param6 Param 6
 */
#define prvTraceEventCreate6(_eventID, _param1, _param2, _param3, _param4, _param5, _param6) xTraceEventCreate6(_eventID, (TraceUnsignedBaseType_t)(_param1), (TraceUnsignedBaseType_t)(_param2), (TraceUnsignedBaseType_t)(_param3), (TraceUnsignedBaseType_t)(_param4), (TraceUnsignedBaseType_t)(_param5), (TraceUnsignedBaseType_t)(_param6))

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_EVENT_H */
