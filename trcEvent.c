/*
* Percepio Trace Recorder for Tracealyzer v4.8.0.hotfix1
* Copyright 2023 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The implementation for events.
*/

#include <trcRecorder.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

/*cstat !MISRAC2004-19.4 Suppress macro check*/
#define VERIFY_EVENT_SIZE(i) \
	if ((TraceUnsignedBaseType_t)(i) > (TraceUnsignedBaseType_t)(TRC_MAX_BLOB_SIZE)) \
	{ \
		(void)xTraceDiagnosticsSetIfHigher(TRC_DIAGNOSTICS_BLOB_MAX_BYTES_TRUNCATED, (TraceBaseType_t)(i) - (TraceBaseType_t)(TRC_MAX_BLOB_SIZE)); \
		(i) = (uint32_t)(TRC_MAX_BLOB_SIZE); \
	}

TraceEventDataTable_t *pxTraceEventDataTable TRC_CFG_RECORDER_DATA_ATTRIBUTE;

traceResult xTraceEventInitialize(TraceEventDataTable_t* pxBuffer)
{
	TraceCoreEventData_t* pxCoreEventData;
	uint32_t i, j;

	/* This should never fail */
	TRC_ASSERT(pxBuffer != (void*)0);

	pxTraceEventDataTable = pxBuffer;

	for (i = 0u; i < (uint32_t)(TRC_CFG_CORE_COUNT); i++)
	{
		pxCoreEventData = &pxTraceEventDataTable->coreEventData[i];

		pxCoreEventData->eventCounter = 0u;

		for (j = 0u; j < ((uint32_t)(TRC_CFG_MAX_ISR_NESTING) + 1u); j++)
		{
			RESET_EVENT_DATA(&pxCoreEventData->eventData[j]);
		}
	}

	xTraceSetComponentInitialized(TRC_RECORDER_COMPONENT_EVENT);

	return TRC_SUCCESS;
}

traceResult xTraceEventCreate0(uint32_t uiEventCode)
{
	TraceEvent0_t* pxEventData = (void*)0;
	int32_t iBytesCommitted = 0;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We need to check this */
	if (!xTraceIsRecorderEnabled())
	{
		return TRC_FAIL;
	}

	TRACE_ENTER_CRITICAL_SECTION();

	pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter++;

	if (xTraceStreamPortAllocate(sizeof(TraceEvent0_t), (void**)&pxEventData) == TRC_FAIL) /*cstat !MISRAC2004-11.4 !MISRAC2012-Rule-11.3 Suppress pointer checks*/
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return TRC_FAIL;
	}

	SET_BASE_EVENT_DATA(pxEventData, uiEventCode, 0, pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter); /*cstat !MISRAC2012-Rule-11.5 Suppress pointer checks*/

	(void)xTraceStreamPortCommit(pxEventData, sizeof(TraceEvent0_t), &iBytesCommitted);

	TRACE_EXIT_CRITICAL_SECTION();

	/* We need to use iBytesCommitted for the above call but do not use the value,
	 * remove potential warnings */
	(void)iBytesCommitted;

	return TRC_SUCCESS;
}

traceResult xTraceEventCreate1(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1)
{
	TraceEvent1_t* pxEventData = (void*)0;
	int32_t iBytesCommitted = 0;

	TRACE_ALLOC_CRITICAL_SECTION();

	/* We need to check this */
	if (!xTraceIsRecorderEnabled())
	{
		return TRC_FAIL;
	}

	TRACE_ENTER_CRITICAL_SECTION();

	pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter++;

	if (xTraceStreamPortAllocate(sizeof(TraceEvent1_t), (void**)&pxEventData) == TRC_FAIL) /*cstat !MISRAC2004-11.4 !MISRAC2012-Rule-11.3 Suppress pointer checks*/
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return TRC_FAIL;
	}

	SET_BASE_EVENT_DATA(pxEventData, uiEventCode, 1, pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter); /*cstat !MISRAC2012-Rule-11.5 Suppress pointer checks*/

	pxEventData->uxParams[0] = uxParam1;

	(void)xTraceStreamPortCommit(pxEventData, sizeof(TraceEvent1_t), &iBytesCommitted);

	TRACE_EXIT_CRITICAL_SECTION();

	/* We need to use iBytesCommitted for the above call but do not use the value,
	 * remove potential warnings */
	(void)iBytesCommitted;

	return TRC_SUCCESS;
}

traceResult xTraceEventCreate2(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2)
{
	TraceEvent2_t* pxEventData = (void*)0;
	int32_t iBytesCommitted = 0;

	TRACE_ALLOC_CRITICAL_SECTION();

	/* We need to check this */
	if (!xTraceIsRecorderEnabled())
	{
		return TRC_FAIL;
	}

	TRACE_ENTER_CRITICAL_SECTION();

	pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter++;

	if (xTraceStreamPortAllocate(sizeof(TraceEvent2_t), (void**)&pxEventData) == TRC_FAIL) /*cstat !MISRAC2004-11.4 !MISRAC2012-Rule-11.3 Suppress pointer checks*/
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return TRC_FAIL;
	}

	SET_BASE_EVENT_DATA(pxEventData, uiEventCode, 2, pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter); /*cstat !MISRAC2012-Rule-11.5 Suppress pointer checks*/

	pxEventData->uxParams[0] = uxParam1;
	pxEventData->uxParams[1] = uxParam2;

	(void)xTraceStreamPortCommit(pxEventData, sizeof(TraceEvent2_t), &iBytesCommitted);

	TRACE_EXIT_CRITICAL_SECTION();

	/* We need to use iBytesCommitted for the above call but do not use the value,
	 * remove potential warnings */
	(void)iBytesCommitted;

	return TRC_SUCCESS;
}

traceResult xTraceEventCreate3(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3)
{
	TraceEvent3_t* pxEventData = (void*)0;
	int32_t iBytesCommitted = 0;

	TRACE_ALLOC_CRITICAL_SECTION();

	/* We need to check this */
	if (!xTraceIsRecorderEnabled())
	{
		return TRC_FAIL;
	}

	TRACE_ENTER_CRITICAL_SECTION();

	pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter++;

	if (xTraceStreamPortAllocate(sizeof(TraceEvent3_t), (void**)&pxEventData) == TRC_FAIL) /*cstat !MISRAC2004-11.4 !MISRAC2012-Rule-11.3 Suppress pointer checks*/
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return TRC_FAIL;
	}

	SET_BASE_EVENT_DATA(pxEventData, uiEventCode, 3, pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter); /*cstat !MISRAC2012-Rule-11.5 Suppress pointer checks*/
	
	pxEventData->uxParams[0] = uxParam1;
	pxEventData->uxParams[1] = uxParam2;
	pxEventData->uxParams[2] = uxParam3;

	(void)xTraceStreamPortCommit(pxEventData, sizeof(TraceEvent3_t), &iBytesCommitted);

	TRACE_EXIT_CRITICAL_SECTION();

	/* We need to use iBytesCommitted for the above call but do not use the value,
	 * remove potential warnings */
	(void)iBytesCommitted;

	return TRC_SUCCESS;
}

traceResult xTraceEventCreate4(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4)
{
	TraceEvent4_t* pxEventData = (void*)0;
	int32_t iBytesCommitted = 0;

	TRACE_ALLOC_CRITICAL_SECTION();

	/* We need to check this */
	if (!xTraceIsRecorderEnabled())
	{
		return TRC_FAIL;
	}

	TRACE_ENTER_CRITICAL_SECTION();

	pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter++;

	if (xTraceStreamPortAllocate(sizeof(TraceEvent4_t), (void**)&pxEventData) == TRC_FAIL) /*cstat !MISRAC2004-11.4 !MISRAC2012-Rule-11.3 Suppress pointer checks*/
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return TRC_FAIL;
	}

	SET_BASE_EVENT_DATA(pxEventData, uiEventCode, 4, pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter); /*cstat !MISRAC2012-Rule-11.5 Suppress pointer checks*/

	pxEventData->uxParams[0] = uxParam1;
	pxEventData->uxParams[1] = uxParam2;
	pxEventData->uxParams[2] = uxParam3;
	pxEventData->uxParams[3] = uxParam4;

	(void)xTraceStreamPortCommit(pxEventData, sizeof(TraceEvent4_t), &iBytesCommitted);

	TRACE_EXIT_CRITICAL_SECTION();

	/* We need to use iBytesCommitted for the above call but do not use the value,
	 * remove potential warnings */
	(void)iBytesCommitted;

	return TRC_SUCCESS;
}

traceResult xTraceEventCreate5(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, TraceUnsignedBaseType_t uxParam5)
{
	TraceEvent5_t* pxEventData = (void*)0;
	int32_t iBytesCommitted = 0;

	TRACE_ALLOC_CRITICAL_SECTION();

	/* We need to check this */
	if (!xTraceIsRecorderEnabled())
	{
		return TRC_FAIL;
	}

	TRACE_ENTER_CRITICAL_SECTION();

	pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter++;

	if (xTraceStreamPortAllocate(sizeof(TraceEvent5_t), (void**)&pxEventData) == TRC_FAIL) /*cstat !MISRAC2004-11.4 !MISRAC2012-Rule-11.3 Suppress pointer checks*/
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return TRC_FAIL;
	}

	SET_BASE_EVENT_DATA(pxEventData, uiEventCode, 5, pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter); /*cstat !MISRAC2012-Rule-11.5 Suppress pointer checks*/

	pxEventData->uxParams[0] = uxParam1;
	pxEventData->uxParams[1] = uxParam2;
	pxEventData->uxParams[2] = uxParam3;
	pxEventData->uxParams[3] = uxParam4;
	pxEventData->uxParams[4] = uxParam5;

	(void)xTraceStreamPortCommit(pxEventData, sizeof(TraceEvent5_t), &iBytesCommitted);

	TRACE_EXIT_CRITICAL_SECTION();

	/* We need to use iBytesCommitted for the above call but do not use the value,
	 * remove potential warnings */
	(void)iBytesCommitted;

	return TRC_SUCCESS;
}

traceResult xTraceEventCreate6(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, TraceUnsignedBaseType_t uxParam5, TraceUnsignedBaseType_t uxParam6)
{
	TraceEvent6_t* pxEventData = (void*)0;
	int32_t iBytesCommitted = 0;

	TRACE_ALLOC_CRITICAL_SECTION();

	/* We need to check this */
	if (!xTraceIsRecorderEnabled())
	{
		return TRC_FAIL;
	}

	TRACE_ENTER_CRITICAL_SECTION();

	pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter++;

	if (xTraceStreamPortAllocate(sizeof(TraceEvent6_t), (void**)&pxEventData) == TRC_FAIL) /*cstat !MISRAC2004-11.4 !MISRAC2012-Rule-11.3 Suppress pointer checks*/
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return TRC_FAIL;
	}

	SET_BASE_EVENT_DATA(pxEventData, uiEventCode, 6, pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter); /*cstat !MISRAC2012-Rule-11.5 Suppress pointer checks*/

	pxEventData->uxParams[0] = uxParam1;
	pxEventData->uxParams[1] = uxParam2;
	pxEventData->uxParams[2] = uxParam3;
	pxEventData->uxParams[3] = uxParam4;
	pxEventData->uxParams[4] = uxParam5;
	pxEventData->uxParams[5] = uxParam6;

	(void)xTraceStreamPortCommit(pxEventData, sizeof(TraceEvent6_t), &iBytesCommitted);

	TRACE_EXIT_CRITICAL_SECTION();

	/* We need to use iBytesCommitted for the above call but do not use the value,
	 * remove potential warnings */
	(void)iBytesCommitted;

	return TRC_SUCCESS;
}

traceResult xTraceEventBeginRawOffline(uint32_t uiSize, TraceEventHandle_t* pxEventHandle)
{
	int32_t ISR_nesting = 0;
	TraceEventData_t* pxEventData;
	TraceCoreEventData_t* pxCoreEventData;

	TRACE_ALLOC_CRITICAL_SECTION();

	/* We need to check this */
	if (xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT) == 0U)
	{
		return TRC_FAIL;
	}

	/* This should never fail */
	TRC_ASSERT(pxEventHandle != (void*)0);

	TRACE_ENTER_CRITICAL_SECTION();

	pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter++;

	pxCoreEventData = &pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()];

	/* We backup the local variable to the CORE specific variable */
	pxCoreEventData->TRACE_ALLOC_CRITICAL_SECTION_NAME = TRACE_ALLOC_CRITICAL_SECTION_NAME;

	(void)xTraceISRGetCurrentNesting(&ISR_nesting);

	/* We add 1 since xTraceISRGetCurrentNesting(...) returns -1 if no ISR is active */
	pxEventData = &pxCoreEventData->eventData[ISR_nesting + 1];

	/* This should never fail */
	TRC_ASSERT_CUSTOM_ON_FAIL(pxEventData->pvBlob == 0, TRACE_EXIT_CRITICAL_SECTION(); return TRC_FAIL; );

	VERIFY_EVENT_SIZE(uiSize); /*cstat !MISRAC2012-Rule-17.8 Suppress modified function parameter check*/

	pxEventData->size = ((uiSize + (sizeof(TraceUnsignedBaseType_t) - 1u)) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t);	/* BaseType align */

	pxEventData->offset = 0u;

	/* This can fail and we should handle it */
	if (xTraceStreamPortAllocate(pxEventData->size, &pxEventData->pvBlob) == TRC_FAIL)
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return TRC_FAIL;
	}

	*pxEventHandle = (TraceEventHandle_t)pxEventData;

	return TRC_SUCCESS;
}

traceResult xTraceEventBeginRawOfflineBlocking(uint32_t uiSize, TraceEventHandle_t* pxEventHandle)
{
	TraceEventData_t* pxEventData;
	TraceCoreEventData_t* pxCoreEventData;
	int32_t ISR_nesting = 0;
	uint32_t uiAttempts = 0u;

	TRACE_ALLOC_CRITICAL_SECTION();

	/* We need to check this */
	if (xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT) == 0U)
	{
		return TRC_FAIL;
	}

	/* This should never fail */
	TRC_ASSERT(pxEventHandle != (void*)0);

	TRACE_ENTER_CRITICAL_SECTION();

	pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter++;

	pxCoreEventData = &pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()];

	/* We backup the local variable to the CORE specific variable */
	pxCoreEventData->TRACE_ALLOC_CRITICAL_SECTION_NAME = TRACE_ALLOC_CRITICAL_SECTION_NAME;

	(void)xTraceGetCurrentISRNesting(&ISR_nesting);

	/* We add 1 since xTraceISRGetCurrentNesting(...) returns -1 if no ISR is active */
	pxEventData = &pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventData[ISR_nesting + 1];

	/* This should never fail */
	TRC_ASSERT_CUSTOM_ON_FAIL(pxEventData->pvBlob == 0, TRACE_EXIT_CRITICAL_SECTION(); return TRC_FAIL; );

	VERIFY_EVENT_SIZE(uiSize); /*cstat !MISRAC2012-Rule-17.8 Suppress modified function parameter check*/

	pxEventData->size = ((uiSize + (sizeof(TraceUnsignedBaseType_t) - 1u)) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t);	/* BaseType align */

	pxEventData->offset = 0u;

	/* This can fail and we should handle it */
	while (xTraceStreamPortAllocate(pxEventData->size, &pxEventData->pvBlob) != TRC_SUCCESS)
	{
		uiAttempts++;
	}

	*pxEventHandle = (TraceEventHandle_t)pxEventData;

	return TRC_SUCCESS;
}

traceResult xTraceEventEndOffline(TraceEventHandle_t xEventHandle)
{
	TraceEventData_t* pxEventData = (TraceEventData_t*)xEventHandle;
	const TraceCoreEventData_t* pxCoreEventData;
	int32_t iBytesCommitted = 0;

	TRACE_ALLOC_CRITICAL_SECTION()

	pxCoreEventData = &pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()];

	/* We restore the CORE specific variable to the local variable before any EXIT */
	TRACE_ALLOC_CRITICAL_SECTION_NAME = pxCoreEventData->TRACE_ALLOC_CRITICAL_SECTION_NAME;

	/* This should never fail */
	TRC_ASSERT_CUSTOM_ON_FAIL(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT), TRACE_EXIT_CRITICAL_SECTION(); return TRC_FAIL; );

	/* This should never fail */
	TRC_ASSERT_CUSTOM_ON_FAIL(pxEventData != (void*)0, TRACE_EXIT_CRITICAL_SECTION(); return TRC_FAIL; );

	/* This should never fail */
	TRC_ASSERT_CUSTOM_ON_FAIL(pxEventData->pvBlob != (void*)0, TRACE_EXIT_CRITICAL_SECTION(); return TRC_FAIL; );

	(void)xTraceStreamPortCommit(pxEventData->pvBlob, pxEventData->size, &iBytesCommitted);

	/* We need to use iBytesCommitted for the above call but do not use the value,
	 * remove potential warnings */
	(void)iBytesCommitted;

	RESET_EVENT_DATA(pxEventData);

	TRACE_EXIT_CRITICAL_SECTION();

	return TRC_SUCCESS;
}

traceResult xTraceEventEndOfflineBlocking(TraceEventHandle_t xEventHandle)
{
	TraceEventData_t* pxEventData = (TraceEventData_t*)xEventHandle;
	const TraceCoreEventData_t* pxCoreEventData;
	int32_t iBytesCommitted;
	uint32_t uiByteOffset = 0u;
	uint8_t *pubBlob;

	TRACE_ALLOC_CRITICAL_SECTION()

	pxCoreEventData = &pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()];

	/* We restore the CORE specific variable to the local variable before any EXIT */
	TRACE_ALLOC_CRITICAL_SECTION_NAME = pxCoreEventData->TRACE_ALLOC_CRITICAL_SECTION_NAME;

	/* This should never fail */
	TRC_ASSERT_CUSTOM_ON_FAIL(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT), TRACE_EXIT_CRITICAL_SECTION(); return TRC_FAIL; );

	/* This should never fail */
	TRC_ASSERT_CUSTOM_ON_FAIL(pxEventData != (void*)0, TRACE_EXIT_CRITICAL_SECTION(); return TRC_FAIL; );

	/* This should never fail */
	TRC_ASSERT_CUSTOM_ON_FAIL(pxEventData->pvBlob != (void*)0, TRACE_EXIT_CRITICAL_SECTION(); return TRC_FAIL; );
	
	pubBlob = (uint8_t*)pxEventData->pvBlob; /*cstat !MISRAC2012-Rule-11.5 Suppress pointer checks*/

	while (pxEventData->size > uiByteOffset)
	{
		iBytesCommitted = 0;
		(void)xTraceStreamPortCommit((void*)&pubBlob[uiByteOffset], pxEventData->size - uiByteOffset, &iBytesCommitted); /*cstat !MISRAC2004-17.4_b We need to access a specific part of the buffer*/

		uiByteOffset += (uint32_t)iBytesCommitted;
	}

	RESET_EVENT_DATA(pxEventData);

	TRACE_EXIT_CRITICAL_SECTION();

	return TRC_SUCCESS;
}

traceResult xTraceEventAddData(TraceEventHandle_t xEventHandle, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize)
{
	TraceUnsignedBaseType_t i;

	/* This should never fail */
	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT));

	/* This should never fail */
	TRC_ASSERT(xEventHandle != 0);

	/* This should never fail */
	TRC_ASSERT(puxData != (void*)0);

	/* This should never fail */
	TRC_ASSERT((((TraceEventData_t*)xEventHandle)->offset + (uint32_t)(uxSize * sizeof(TraceUnsignedBaseType_t))) <= ((TraceEventData_t*)xEventHandle)->size);

	for (i = 0u; i < uxSize; i++)
	{
		TRC_EVENT_ADD_UNSIGNED_BASE_TYPE(xEventHandle, puxData[i]); /*cstat !MISRAC2004-17.4_b We need to access a specific part of the buffer*/
	}

	return TRC_SUCCESS;
}

#if ((TRC_CFG_USE_TRACE_ASSERT) == 1)

traceResult xTraceEventGetSize(const void* const pvAddress, uint32_t* puiSize)
{
	/* This should never fail */
	TRC_ASSERT(pvAddress != (void*)0);
	
	/* This should never fail */
	TRC_ASSERT(puiSize != (void*)0);

	/* This should never fail */
	TRC_ASSERT((sizeof(TraceEvent0_t) + ((uint32_t)(uint16_t)(TRC_EVENT_GET_PARAM_COUNT(((const TraceEvent0_t*)pvAddress)->EventID)) * sizeof(uint32_t))) <= (uint32_t)(TRC_MAX_BLOB_SIZE)); /*cstat !MISRAC2012-Rule-11.5 Suppress pointer checks*/
	
	return TRC_EVENT_GET_SIZE(pvAddress, puiSize);
}

traceResult xTraceEventGetRawData(TraceEventHandle_t xEventHandle, uint32_t uiOffset, uint32_t uiSize, void** ppvData)
{
	/* This should never fail */
	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT));

	/* This should never fail */
	TRC_ASSERT(xEventHandle != 0);

	/* This should never fail */
	TRC_ASSERT(ppvData != (void*)0);

	/* This should never fail */
	TRC_ASSERT((uiOffset + uiSize) <= ((TraceEventData_t*)xEventHandle)->size);

	return TRC_EVENT_GET_RAW_DATA(xEventHandle, uiOffset, uiSize, ppvData);
}

traceResult xTraceEventGetPayload(const TraceEventHandle_t xEventHandle, uint32_t uiOffset, uint32_t uiSize, void** ppvData)
{
	/* This should never fail */
	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT));

	/* This should never fail */
	TRC_ASSERT(xEventHandle != 0);

	/* This should never fail */
	TRC_ASSERT(ppvData != (void*)0);

	/* This should never fail */
	TRC_ASSERT((uiOffset + uiSize) <= ((const TraceEventData_t*)xEventHandle)->size);

	return TRC_EVENT_GET_PAYLOAD(xEventHandle, uiOffset, uiSize, ppvData);
}

traceResult xTraceEventPayloadRemaining(const TraceEventHandle_t xEventHandle, uint32_t* puiValue)
{
	/* This should never fail */
	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT));

	/* This should never fail */
	TRC_ASSERT(xEventHandle != 0);

	/* This should never fail */
	TRC_ASSERT(puiValue != (void*)0);

	/* This should never fail */
	TRC_ASSERT(((const TraceEventData_t*)xEventHandle)->pvBlob != (void*)0);

	return TRC_EVENT_PAYLOAD_REMAINING(xEventHandle, puiValue);
}

traceResult xTraceEventPayloadUsed(const TraceEventHandle_t xEventHandle, uint32_t* puiValue)
{
	/* This should never fail */
	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT));

	/* This should never fail */
	TRC_ASSERT(xEventHandle != 0);

	/* This should never fail */
	TRC_ASSERT(puiValue != (void*)0);

	/* This should never fail */
	TRC_ASSERT(((const TraceEventData_t*)xEventHandle)->pvBlob != (void*)0);

	return TRC_EVENT_PAYLOAD_USED(xEventHandle, puiValue);
}

traceResult xTraceEventPayloadSize(const TraceEventHandle_t xEventHandle, uint32_t* puiValue)
{
	/* This should never fail */
	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT));

	/* This should never fail */
	TRC_ASSERT(xEventHandle != 0);

	/* This should never fail */
	TRC_ASSERT(puiValue != (void*)0);

	/* This should never fail */
	TRC_ASSERT(((const TraceEventData_t*)xEventHandle)->pvBlob != (void*)0);

	return TRC_EVENT_PAYLOAD_SIZE(xEventHandle, puiValue);
}

traceResult xTraceEventAddPointer(TraceEventHandle_t xEventHandle, void* pvAddress)
{
	/* This should never fail */
	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT));

	/* This should never fail */
	TRC_ASSERT(xEventHandle != 0);

	/* This should never fail */
	TRC_ASSERT((((TraceEventData_t*)xEventHandle)->offset + sizeof(void*)) <= ((TraceEventData_t*)xEventHandle)->size);

	/* Make sure we are writing at void* aligned offset */
	/* This should never fail */
	TRC_ASSERT((((TraceEventData_t*)xEventHandle)->offset & (sizeof(void*) - 1u)) == 0u);

	return TRC_EVENT_ADD_POINTER(xEventHandle, pvAddress);
}

traceResult xTraceEventAddUnsignedBaseType(TraceEventHandle_t xEventHandle, TraceUnsignedBaseType_t uxValue)
{
	/* This should never fail */
	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT));

	/* This should never fail */
	TRC_ASSERT(xEventHandle != 0);

	/* This should never fail */
	TRC_ASSERT((((TraceEventData_t*)xEventHandle)->offset + sizeof(TraceUnsignedBaseType_t)) <= ((TraceEventData_t*)xEventHandle)->size);

	/* Make sure we are writing at TraceUnsignedBaseType_t aligned offset */
	/* This should never fail */
	TRC_ASSERT((((TraceEventData_t*)xEventHandle)->offset & (sizeof(TraceUnsignedBaseType_t) - 1u)) == 0u);

	return TRC_EVENT_ADD_UNSIGNED_BASE_TYPE(xEventHandle, uxValue);
}

traceResult xTraceEventAdd32(TraceEventHandle_t xEventHandle, uint32_t value)
{
	/* This should never fail */
	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT));

	/* This should never fail */
	TRC_ASSERT(xEventHandle != 0);

	/* This should never fail */
	TRC_ASSERT((((TraceEventData_t*)xEventHandle)->offset + sizeof(uint32_t)) <= ((TraceEventData_t*)xEventHandle)->size);

	/* Make sure we are writing at 32-bit aligned offset */
	/* This should never fail */
	TRC_ASSERT((((TraceEventData_t*)xEventHandle)->offset & 3u) == 0u);

	return TRC_EVENT_ADD_32(xEventHandle, value);
}

traceResult xTraceEventAdd16(TraceEventHandle_t xEventHandle, uint16_t value)
{
	/* This should never fail */
	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT));

	/* This should never fail */
	TRC_ASSERT(xEventHandle != 0);

	/* This should never fail */
	TRC_ASSERT((((TraceEventData_t*)xEventHandle)->offset + sizeof(uint16_t)) <= ((TraceEventData_t*)xEventHandle)->size);

	/* Make sure we are writing at 16-bit aligned offset */
	/* This should never fail */
	TRC_ASSERT((((TraceEventData_t*)xEventHandle)->offset & 1u) == 0u);

	return TRC_EVENT_ADD_16(xEventHandle, value);
}

traceResult xTraceEventAdd8(TraceEventHandle_t xEventHandle, uint8_t value)
{
	/* This should never fail */
	TRC_ASSERT(xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_EVENT));

	/* This should never fail */
	TRC_ASSERT(xEventHandle != 0);

	/* This should never fail */
	TRC_ASSERT((((TraceEventData_t*)xEventHandle)->offset + sizeof(uint8_t)) <= ((TraceEventData_t*)xEventHandle)->size);

	return TRC_EVENT_ADD_8(xEventHandle, value);
}

#endif /* ((TRC_CFG_USE_TRACE_ASSERT) == 1) */

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */
