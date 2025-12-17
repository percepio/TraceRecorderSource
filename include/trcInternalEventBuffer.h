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
 * @brief Public internal event buffer APIs.
 */

#ifndef TRC_INTERNAL_BUFFER_H
#define TRC_INTERNAL_BUFFER_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#include <trcDefines.h>

#ifdef TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER
#define TRC_USE_INTERNAL_BUFFER TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER
#else
#define TRC_USE_INTERNAL_BUFFER 1
#endif

#if (TRC_USE_INTERNAL_BUFFER == 1)

/* Aligned */
#define TRC_INTERNAL_BUFFER_SIZE ((((TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t))

/**
 * @brief A structure representing the trace stream port buffer.
 */
typedef struct TraceInternalEventBufferData	/* Aligned */
{
	uint8_t aubBuffer[TRC_INTERNAL_BUFFER_SIZE];
} TraceInternalEventBufferData_t;

#include <trcTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_TRANSFER_MODE
#define TRC_INTERNAL_EVENT_BUFFER_TRANSFER_MODE TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_TRANSFER_MODE
#else
#define TRC_INTERNAL_EVENT_BUFFER_TRANSFER_MODE TRC_INTERNAL_EVENT_BUFFER_OPTION_TRANSFER_MODE_ALL
#endif

#ifdef TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_SIZE
#define TRC_INTERNAL_BUFFER_CHUNK_SIZE TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_SIZE
#else
#define TRC_INTERNAL_BUFFER_CHUNK_SIZE (1024UL)
#endif

#ifdef TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT
#define TRC_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT
#else
#define TRC_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT (512UL)
#endif

#ifdef TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT
#define TRC_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT
#else
#define TRC_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT (5UL)
#endif

/**
 * @defgroup trace_internal_event_buffer_apis Trace Internal Event Buffer APIs
 * @ingroup trace_recorder_apis
 * @{
 */

/**
 * @internal Initializes the internal trace event buffer used by certain stream ports.
 * 
 * @param[in] pxBuffer Pointer to previously allocated memory buffer
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceInternalEventBufferInitialize(TraceInternalEventBufferData_t* pxBuffer);

/**
 * @brief Allocates a data slot directly from the internal event buffer.
 *
 * @param[in] uiSize Allocation size
 * @param[out] ppvData Pointer that will hold the area from the buffer.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceInternalEventBufferAlloc(uint32_t uiSize, void **ppvData);

/**
 * @brief Commits the last allocated block to the internal event buffer.
 *
 * For the sake of conformity this function accepts the same arguments as
 * xTraceInternalEventBufferPush. These arguments are not used in the
 * traditional sense since the alloc already managed these parts. This
 * function simply marks the allocate stage as complete and ready
 * for transfer.
 *
 * @param[in] pvData Pointer to data
 * @param[in] uiSize Size of data
 * @param[out] piBytesWritten Bytes written.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceInternalEventBufferAllocCommit(void *pvData, uint32_t uiSize, int32_t *piBytesWritten);

/**
 * @brief Pushes data to the internal trace event buffer.
 * 
 * @param[in] pvData Pointer to data
 * @param[in] uiSize Size of data
 * @param[out] piBytesWritten Bytes written.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceInternalEventBufferPush(void *pvData, uint32_t uiSize, int32_t *piBytesWritten);

#if (TRC_INTERNAL_EVENT_BUFFER_TRANSFER_MODE == TRC_INTERNAL_EVENT_BUFFER_OPTION_TRANSFER_MODE_ALL)
#define xTraceInternalEventBufferTransfer xTraceInternalEventBufferTransferAll
#else
#define xTraceInternalEventBufferTransfer xTraceInternalEventBufferTransferChunk
#endif

/**
 * @brief Transfers all internal trace event buffer data using the function 
 * xTraceStreamPortWriteData(...) as defined in trcStreamPort.h.
 *
 * This function is intended to be called by the periodic TzCtrl task with a 
 * suitable delay (e.g. 10-100 ms).
 *
 * In case of errors from the streaming interface, it registers a warning
 * (TRC_WARNING_STREAM_PORT_WRITE) provided by xTraceErrorGetLast().
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceInternalEventBufferTransferAll(void);

/**
 * @brief Transfer internal trace event buffer data through streamport.
 *
 * This routine will attempt to transfer a chunk of existing data in the trace
 * event buffer through the streamport. New data pushed to the trace event buffer
 * during the execution of this routine will not be transfered.
 *
 * When transferring a chunk which wraps the buffer, a singular transfer
 * is made to avoid issuing dual writes. This configuration means that
 * during wrapping, the chunk might be reduced in size even if there is
 * more data at the start of the buffer. To transfer more data check
 * piBytesWritten and issue multiple transfers if required.
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceInternalEventBufferTransferChunk(void);

/**
 * @brief Clears all trace events in the internal trace event buffer.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceInternalEventBufferClear(void);

/** @} */

#ifdef __cplusplus
}
#endif

#else /* (TRC_USE_INTERNAL_BUFFER == 1)*/

#define xTraceInternalEventBufferInitialize(pxBuffer) ((pxBuffer) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceInternalEventBufferAlloc(ppvData, uiSize) ((void)(uiSize), (ppvData) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceInternalEventBufferAllocCommit(pvData, uiSize, piBytesWritten) ((void)(pvData), (void)(uiSize), (void)(piBytesWritten), TRC_SUCCESS)
#define xTraceInternalEventBufferPush(pvData, uiSize, piBytesWritten) ((void)(uiSize), (void)(piBytesWritten), (pvData) != 0 ? TRC_SUCCESS : TRC_FAIL)
#define xTraceInternalEventBufferTransfer() (void)(TRC_SUCCESS)
#define xTraceInternalEventBufferTransferChunk(piBytesWritten, uiChunkSize) ((void)(piBytesWritten), (void)(uiChunkSize), TRC_SUCCESS)
#define xTraceInternalEventBufferClear() (void)(TRC_SUCCESS)

#endif /* (TRC_USE_INTERNAL_BUFFER == 1)*/

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#endif /* TRC_INTERNAL_BUFFER_H */
