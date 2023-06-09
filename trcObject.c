/*
* Percepio Trace Recorder for Tracealyzer v4.8.0.hotfix1
* Copyright 2023 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The implementation for strings.
*/

#include <trcRecorder.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifndef TRC_SEND_NAME_ONLY_ON_DELETE
#define TRC_SEND_NAME_ONLY_ON_DELETE 0
#endif

traceResult prvTraceObjectSendNameEvent(void* pvObject, const char* szName, uint32_t uiLength);

/*cstat !MISRAC2004-6.3 !MISRAC2012-Dir-4.6_a Suppress basic char type usage*/
traceResult xTraceObjectRegisterInternal(uint32_t uiEventCode, void* const pvObject, const char* szName, TraceUnsignedBaseType_t uxStateCount, const TraceUnsignedBaseType_t uxStates[], TraceUnsignedBaseType_t uxOptions, TraceObjectHandle_t* pxObjectHandle)
{
	TraceEntryHandle_t xEntryHandle;
	TraceEventHandle_t xEventHandle = 0;
	TraceUnsignedBaseType_t i;
	void *pvAddress;

	TRACE_ALLOC_CRITICAL_SECTION();

	/* This should never fail */
	TRC_ASSERT(pxObjectHandle != (void*)0);

	/* This should never fail */
	TRC_ASSERT(uxStateCount <= (uint32_t)(TRC_ENTRY_TABLE_STATE_COUNT));

	TRACE_ENTER_CRITICAL_SECTION();

	if (pvObject != (void*)0)
	{
		/* An address was supplied */
		if (xTraceEntryCreateWithAddress(pvObject, &xEntryHandle) == TRC_FAIL)
		{
			TRACE_EXIT_CRITICAL_SECTION();

			return TRC_FAIL;
		}
		
		pvAddress = pvObject;
	}
	else
	{
		/* No address was supplied */
		if (xTraceEntryCreate(&xEntryHandle) == TRC_FAIL)
		{
			TRACE_EXIT_CRITICAL_SECTION();

			return TRC_FAIL;
		}

		TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntryGetAddress(xEntryHandle, &pvAddress) == TRC_SUCCESS);
	}

	for (i = 0u; i < uxStateCount; i++)
	{
		/* This should never fail */
		TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntrySetState(xEntryHandle, (uint32_t)i, uxStates[i]) == TRC_SUCCESS);
	}

	/* This should never fail */
	TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntrySetOptions(xEntryHandle, (uint32_t)uxOptions) == TRC_SUCCESS);

	*pxObjectHandle = (TraceObjectHandle_t)xEntryHandle;

	TRACE_EXIT_CRITICAL_SECTION();

	if ((szName != (void*)0) && (szName[0] != (char)0)) /*cstat !MISRAC2004-6.3 !MISRAC2012-Dir-4.6_a Suppress basic char type usage*/ /*cstat !MISRAC2004-17.4_b We need to access every character in the string*/
	{
		/* Not a null or empty string */
		/* This will set the symbol and create an event for it */
		/* This should never fail */
		TRC_ASSERT_ALWAYS_EVALUATE(xTraceObjectSetName((TraceObjectHandle_t)xEntryHandle, szName) == TRC_SUCCESS);
	}

	/* Send the create event, if possible */
	/*We need to check this */
	if (xTraceEventBegin(uiEventCode, sizeof(void*) + (uxStateCount * sizeof(TraceUnsignedBaseType_t)), &xEventHandle) == TRC_SUCCESS)
	{
		(void)xTraceEventAddPointer(xEventHandle, pvAddress);
		for (i = 0u; i < uxStateCount; i++)
		{
			(void)xTraceEventAddUnsignedBaseType(xEventHandle, uxStates[i]);
		}
		(void)xTraceEventEnd(xEventHandle); /*cstat !MISRAC2012-Rule-17.7 Suppress ignored return value check (inside macro)*/
	}

	return TRC_SUCCESS;
}

/*cstat !MISRAC2004-6.3 !MISRAC2012-Dir-4.6_a Suppress basic char type usage*/
traceResult xTraceObjectRegister(uint32_t uiEventCode, void* const pvObject, const char* szName, TraceUnsignedBaseType_t uxState, TraceObjectHandle_t *pxObjectHandle)
{
	return xTraceObjectRegisterInternal(uiEventCode, pvObject, szName, 1u, &uxState, 0u, pxObjectHandle);
}

traceResult xTraceObjectUnregister(TraceObjectHandle_t xObjectHandle, uint32_t uiEventCode, TraceUnsignedBaseType_t uxState)
{
	void* pvObject = (void*)0;
	const char *szName = (void*)0; /*cstat !MISRAC2004-6.3 !MISRAC2012-Dir-4.6_a Suppress basic char type usage*/
#if (TRC_SEND_NAME_ONLY_ON_DELETE == 1)
	uint32_t uiLength;
	uint32_t i;
#endif

	/* If asserts are disabled this variable will not get used, this stops warnings. */
	(void)szName;

	/* This should never fail */
	TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntryGetAddress((TraceEntryHandle_t)xObjectHandle, &pvObject) == TRC_SUCCESS);

	/* This should never fail */
	TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntryGetSymbol((TraceEntryHandle_t)xObjectHandle, &szName) == TRC_SUCCESS);

#if (TRC_SEND_NAME_ONLY_ON_DELETE == 1)
	/* Send name event because this is a delete */

	for (i = 0u; (szName[i] != (char)0) && (i < 128u); i++) {} /*cstat !MISRAC2004-6.3 !MISRAC2012-Dir-4.6_a Suppress basic char type usage*/ /*cstat !MISRAC2004-17.4_b We need to access every character in the string*/

	uiLength = i;
	/* This should never fail */
	TRC_ASSERT_ALWAYS_EVALUATE(prvTraceObjectSendNameEvent(pvObject, szName, uiLength) == TRC_SUCCESS);
#endif /* (TRC_SEND_NAME_ONLY_ON_DELETE == 1) */

	/* Send the delete event, if possible */
	(void)xTraceEventCreate2(uiEventCode, (TraceUnsignedBaseType_t)(pvObject), uxState);  /*cstat !MISRAC2004-11.3 !MISRAC2012-Rule-11.4 !MISRAC2012-Rule-11.6 Suppress conversion from pointer to integer check*/

	return xTraceEntryDelete(xObjectHandle);
}

/*cstat !MISRAC2004-6.3 !MISRAC2012-Dir-4.6_a Suppress basic char type usage*/
traceResult xTraceObjectSetName(TraceObjectHandle_t xObjectHandle, const char* szName)
{
	void* pvObject = (void*)0;
	uint32_t uiLength;
	uint32_t i;

    /* If asserts are disabled this variable will not get used, this stops warnings. */
	(void)pvObject;

	if (szName == (void*)0)
	{
		szName = ""; /*cstat !MISRAC2012-Rule-17.8 Suppress modified function parameter check*/
	}

	/* This should never fail */
	TRC_ASSERT_ALWAYS_EVALUATE(xTraceEntryGetAddress((TraceEntryHandle_t)xObjectHandle, &pvObject) == TRC_SUCCESS);

	for (i = 0u; (szName[i] != (char)0) && (i < 128u); i++) {} /*cstat !MISRAC2004-6.3 !MISRAC2012-Dir-4.6_a Suppress basic char type usage*/ /*cstat !MISRAC2004-17.4_b We need to access every character in the string*/

	uiLength = i;

#if (TRC_SEND_NAME_ONLY_ON_DELETE == 0)
	/* Send name event now since we don't do it on delete events */
	/* This should never fail */
	TRC_ASSERT_ALWAYS_EVALUATE(prvTraceObjectSendNameEvent(pvObject, szName, uiLength) == TRC_SUCCESS);
#endif /* (TRC_SEND_NAME_ONLY_ON_DELETE == 0) */

	return xTraceEntrySetSymbol((TraceEntryHandle_t)xObjectHandle, szName, uiLength);
}

/*cstat !MISRAC2004-6.3 !MISRAC2012-Dir-4.6_a Suppress basic char type usage*/
traceResult xTraceObjectRegisterWithoutHandle(uint32_t uiEventCode, void* pvObject, const char* szName, TraceUnsignedBaseType_t uxState)
{
	TraceObjectHandle_t xObjectHandle;

	return xTraceObjectRegister(uiEventCode, pvObject, szName, uxState, &xObjectHandle);
}

traceResult xTraceObjectUnregisterWithoutHandle(uint32_t uiEventCode, void* pvObject, TraceUnsignedBaseType_t uxState)
{
	TraceEntryHandle_t xEntryHandle;
	traceResult xResult;

	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();

	if (xTraceEntryFind(pvObject, &xEntryHandle) == TRC_FAIL)
	{
		TRACE_EXIT_CRITICAL_SECTION();

		return TRC_FAIL;
	}

	xResult = xTraceObjectUnregister((TraceObjectHandle_t)xEntryHandle, uiEventCode, uxState);

	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

/*cstat !MISRAC2004-6.3 !MISRAC2012-Dir-4.6_a Suppress basic char type usage*/
traceResult xTraceObjectSetNameWithoutHandle(void* pvObject, const char* szName)
{
	TraceEntryHandle_t xEntryHandle;
	traceResult xResult;

	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();

	if (xTraceEntryFind(pvObject, &xEntryHandle) == TRC_FAIL)
	{
		/* No previous entry found. Create one. */
		if (xTraceEntryCreateWithAddress(pvObject, &xEntryHandle) == TRC_FAIL)
		{
			TRACE_EXIT_CRITICAL_SECTION();

			return TRC_FAIL;
		}
	}

	xResult = xTraceObjectSetName((TraceObjectHandle_t)xEntryHandle, szName);

	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceObjectSetSpecificStateWithoutHandle(void* pvObject, uint32_t uiIndex, TraceUnsignedBaseType_t uxState)
{
	TraceEntryHandle_t xEntryHandle;
	traceResult xResult;

	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();

	if (xTraceEntryFind(pvObject, &xEntryHandle) == TRC_FAIL)
	{
		TRACE_EXIT_CRITICAL_SECTION();

		return TRC_FAIL;
	}

	xResult = xTraceObjectSetSpecificState((TraceObjectHandle_t)xEntryHandle, uiIndex, uxState);
	
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceObjectSetOptionsWithoutHandle(void* pvObject, uint32_t uiMask)
{
	TraceEntryHandle_t xEntryHandle;
	traceResult xResult;

	TRACE_ALLOC_CRITICAL_SECTION();

	TRACE_ENTER_CRITICAL_SECTION();

	if (xTraceEntryFind(pvObject, &xEntryHandle) == TRC_FAIL)
	{
		/* No previous entry found. Create one. */
		if (xTraceEntryCreateWithAddress(pvObject, &xEntryHandle) == TRC_FAIL)
		{
			TRACE_EXIT_CRITICAL_SECTION();

			return TRC_FAIL;
		}
	}

	xResult = xTraceObjectSetOptions((TraceObjectHandle_t)xEntryHandle, uiMask);

	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

/*cstat !MISRAC2004-6.3 !MISRAC2012-Dir-4.6_a Suppress basic char type usage*/
traceResult prvTraceObjectSendNameEvent(void* const pvObject, const char* szName, uint32_t uiLength)
{
	uint32_t uiValue = 0u;
	TraceEventHandle_t xEventHandle = 0;

	if (xTraceEventBegin(PSF_EVENT_OBJ_NAME, sizeof(void*) + uiLength, &xEventHandle) == TRC_SUCCESS)
	{
		(void)xTraceEventAddPointer(xEventHandle, pvObject);
		(void)xTraceEventAddString(xEventHandle, szName, uiLength);

		/* Check if we can truncate */
		(void)xTraceEventPayloadRemaining(xEventHandle, &uiValue);
		if (uiValue > 0u)
		{
			(void)xTraceEventAdd8(xEventHandle, 0u);
		}

		(void)xTraceEventEnd(xEventHandle); /*cstat !MISRAC2012-Rule-17.7 Suppress ignored return value check (inside macro)*/
	}

	return TRC_SUCCESS;
}

#endif

#endif
