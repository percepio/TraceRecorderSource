/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The interface definitions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to stream the trace to file.
 */

#ifndef TRC_STREAM_PORT_H
#define TRC_STREAM_PORT_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <stdint.h>
#include <trcTypes.h>
#include <trcStreamPortConfig.h>
#include <stdio.h>
#include <zephyr/arch/common/semihost.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_USE_INTERNAL_BUFFER (TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER)

#define TRC_INTERNAL_EVENT_BUFFER_WRITE_MODE (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_WRITE_MODE)

#define TRC_INTERNAL_EVENT_BUFFER_TRANSFER_MODE (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_TRANSFER_MODE)

#define TRC_INTERNAL_BUFFER_CHUNK_SIZE (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_SIZE)

#define TRC_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT)

#define TRC_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT)

/* Aligned */
#define TRC_STREAM_PORT_INTERNAL_BUFFER_SIZE ((((TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t))

typedef struct TraceStreamPortFile	/* Aligned */
{
	long pxFileDescriptor;
#if (TRC_USE_INTERNAL_BUFFER)
	uint8_t buffer[TRC_STREAM_PORT_INTERNAL_BUFFER_SIZE];
#endif
} TraceStreamPortFile_t;

extern TraceStreamPortFile_t* pxStreamPortFile;

#define TRC_STREAM_PORT_BUFFER_SIZE (sizeof(TraceStreamPortFile_t))

typedef struct TraceStreamPortBuffer
{
	uint8_t buffer[TRC_STREAM_PORT_BUFFER_SIZE];
} TraceStreamPortBuffer_t;

/**
 * @internal Stream port initialize callback.
 *
 * This function is called by the recorder as part of its initialization phase.
 *
 * @param[in] pxBuffer Buffer
 *
 * @retval TRC_FAIL Initialization failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer);

/**
 * @brief Allocates data from the stream port.
 *
 * @param[in] uiSize Allocation size
 * @param[out] ppvData Allocation data pointer
 *
 * @retval TRC_FAIL Allocate failed
 * @retval TRC_SUCCESS Success
 */
#if (TRC_USE_INTERNAL_BUFFER == 1)
#if (TRC_INTERNAL_EVENT_BUFFER_WRITE_MODE == TRC_INTERNAL_EVENT_BUFFER_OPTION_WRITE_MODE_COPY)
#define xTraceStreamPortAllocate(uiSize, ppvData) ((void)(uiSize), xTraceStaticBufferGet(ppvData))
#else
#define xTraceStreamPortAllocate(uiSize, ppvData) ((void)(uiSize), xTraceInternalEventBufferAlloc(uiSize, ppvData))
#endif
#else
#define xTraceStreamPortAllocate(uiSize, ppvData) ((void)(uiSize), xTraceStaticBufferGet(ppvData))
#endif

/**
 * @brief Commits data to the stream port, depending on the implementation/configuration of the
 * stream port this data might be directly written to the stream port interface, buffered, or
 * something else.
 *
 * @param[in] pvData Data to commit
 * @param[in] uiSize Data to commit size
 * @param[out] piBytesCommitted Bytes committed
 *
 * @retval TRC_FAIL Commit failed
 * @retval TRC_SUCCESS Success
 */
#if (TRC_USE_INTERNAL_BUFFER == 1)
#if (TRC_INTERNAL_EVENT_BUFFER_WRITE_MODE == TRC_INTERNAL_EVENT_BUFFER_OPTION_WRITE_MODE_COPY)
#define xTraceStreamPortCommit xTraceInternalEventBufferPush
#else
#define xTraceStreamPortCommit xTraceInternalEventBufferAllocCommit
#endif
#else
#define xTraceStreamPortCommit xTraceStreamPortWriteData
#endif

/**
 * @brief Writes data through the stream port interface.
 *
 * @param[in] pvData Data to write
 * @param[in] uiSize Data to write size
 * @param[out] piBytesWritten Bytes written
 *
 * @retval TRC_FAIL Write failed
 * @retval TRC_SUCCESS Success
 */
#define xTraceStreamPortReadData(pvData, uiSize, piBytesRead) ((void)(pvData), (void)(uiSize), (void)(piBytesRead), TRC_SUCCESS)

#define xTraceStreamPortOnEnable(uiStartOption) ((void)(uiStartOption), TRC_SUCCESS)

#define xTraceStreamPortOnDisable() (TRC_SUCCESS)

traceResult xTraceStreamPortOnTraceBegin(void);

traceResult xTraceStreamPortOnTraceEnd(void);

//TODO: Might want to change these to non-32bit types, however this depends on what Tracealyzer expects
traceResult xTraceStreamPortWriteData(void* pvData, uint32_t uiSize, int32_t* piBytesWritten);

#ifdef __cplusplus
}
#endif

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#endif /* TRC_STREAM_PORT_H */
