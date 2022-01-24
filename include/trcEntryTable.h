/*
* Percepio Trace Recorder for Tracealyzer v4.6.0(RC1)
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*
* The interface for the entry table.
*/

#ifndef TRC_ENTRY_TABLE_H
#define TRC_ENTRY_TABLE_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_ENTRY_CREATE_WITH_ADDRESS(_pvAddress, _pxEntryHandle) (xTraceEntryCreate(_pxEntryHandle) == TRC_SUCCESS ? (((TraceEntry_t*)*(_pxEntryHandle))->pvAddress = (_pvAddress), TRC_SUCCESS) : TRC_FAIL)
#define TRC_ENTRY_SET_STATE(xEntryHandle, uiStateIndex, uxState) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(((TraceEntry_t*)(xEntryHandle))->xStates[uiStateIndex] = (uxState), TRC_SUCCESS)
#define TRC_ENTRY_SET_OPTIONS(xEntryHandle, uiMask) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(((TraceEntry_t*)(xEntryHandle))->uiOptions |= (uiMask), TRC_SUCCESS)
#define TRC_ENTRY_CLEAR_OPTIONS(xEntryHandle, uiMask) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(((TraceEntry_t*)(xEntryHandle))->uiOptions &= ~(uiMask), TRC_SUCCESS)
#define TRC_ENTRY_GET_ADDRESS(xEntryHandle, ppvAddress) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(ppvAddress) = ((TraceEntry_t*)(xEntryHandle))->pvAddress, TRC_SUCCESS)
#define TRC_ENTRY_GET_SYMBOL(xEntryHandle, pszSymbol) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(pszSymbol) = ((TraceEntry_t*)(xEntryHandle))->szSymbol, TRC_SUCCESS)
#define TRC_ENTRY_GET_STATE(xEntryHandle, uiStateIndex, puxState) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(puxState) = ((TraceEntry_t*)(xEntryHandle))->xStates[uiStateIndex], TRC_SUCCESS)
#define TRC_ENTRY_GET_OPTIONS(xEntryHandle, puiOptions) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(puiOptions) = ((TraceEntry_t*)(xEntryHandle))->uiOptions, TRC_SUCCESS)

#define TRC_ENTRY_TABLE_SLOTS (TRC_CFG_ENTRY_SLOTS)
#define TRC_ENTRY_TABLE_STATE_COUNT (3)
#define TRC_ENTRY_TABLE_SYMBOL_LENGTH (TRC_CFG_ENTRY_SYMBOL_MAX_LENGTH)
#define TRC_ENTRY_TABLE_SLOT_SYMBOL_SIZE ((((sizeof(char) * TRC_ENTRY_TABLE_SYMBOL_LENGTH) + (sizeof(uint32_t) - 1)) / sizeof(uint32_t)) * sizeof(uint32_t))

typedef struct TraceEntry
{
	void* pvAddress;
	TraceUnsignedBaseType_t xStates[TRC_ENTRY_TABLE_STATE_COUNT];
	uint32_t uiOptions;
	char szSymbol[TRC_ENTRY_TABLE_SLOT_SYMBOL_SIZE];
} TraceEntry_t;

#define TRC_ENTRY_TABLE_SIZE (sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + (sizeof(TraceEntry_t) * (TRC_ENTRY_TABLE_SLOTS)))

typedef struct TraceEntryTableBuffer
{
	uint8_t buffer[(TRC_ENTRY_TABLE_SIZE)];
} TraceEntryTableBuffer_t;

traceResult xTraceEntryTableInitialize(TraceEntryTableBuffer_t* pxBuffer);

traceResult xTraceEntryCreate(TraceEntryHandle_t *pxEntryHandle);
traceResult xTraceEntryDelete(TraceEntryHandle_t xEntryHandle);

traceResult xTraceEntryFind(void* pvAddress, TraceEntryHandle_t* pxEntryHandle);

traceResult xTraceEntryGetCount(uint32_t* puiCount);
traceResult xTraceEntryGetAtIndex(uint32_t index, TraceEntryHandle_t* pxEntryHandle);

traceResult xTraceEntrySetSymbol(TraceEntryHandle_t xEntryHandle, const char* szSymbol);

#if ((TRC_CFG_USE_TRACE_ASSERT) == 1)

traceResult xTraceEntryCreateWithAddress(void* pvAddress, TraceEntryHandle_t* pxEntryHandle);

traceResult xTraceEntrySetState(TraceEntryHandle_t xEntryHandle, uint32_t uiStateIndex, TraceUnsignedBaseType_t uxState);
traceResult xTraceEntrySetOptions(TraceEntryHandle_t xEntryHandle, uint32_t uiMask);
traceResult xTraceEntryClearOptions(TraceEntryHandle_t xEntryHandle, uint32_t uiMask);

traceResult xTraceEntryGetAddress(TraceEntryHandle_t xEntryHandle, void **ppvAddress);
traceResult xTraceEntryGetSymbol(TraceEntryHandle_t xEntryHandle, const char** pszSymbol);
traceResult xTraceEntryGetState(TraceEntryHandle_t xEntryHandle, uint32_t uiStateIndex, TraceUnsignedBaseType_t *puxState);
traceResult xTraceEntryGetOptions(TraceEntryHandle_t xEntryHandle, uint32_t *puiOptions);

#else

#define xTraceEntryCreateWithAddress TRC_ENTRY_CREATE_WITH_ADDRESS

#define xTraceEntrySetState TRC_ENTRY_SET_STATE
#define xTraceEntrySetOptions TRC_ENTRY_SET_OPTIONS
#define xTraceEntryClearOptions TRC_ENTRY_CLEAR_OPTIONS

#define xTraceEntryGetAddress TRC_ENTRY_GET_ADDRESS
#define xTraceEntryGetSymbol TRC_ENTRY_GET_SYMBOL
#define xTraceEntryGetState TRC_ENTRY_GET_STATE
#define xTraceEntryGetOptions TRC_ENTRY_GET_OPTIONS

#endif /* ((TRC_CFG_USE_TRACE_ASSERT) == 1) */

#ifdef __cplusplus
}
#endif

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_ENTRY_TABLE_H */
