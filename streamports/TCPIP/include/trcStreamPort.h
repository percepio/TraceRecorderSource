/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The interface definitions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to use TCP/IP as streaming channel.
 * The example is for lwIP.
 */

#ifndef TRC_STREAM_PORT_H
#define TRC_STREAM_PORT_H

#include <stdint.h>
#include <trcTypes.h>
#include <trcStreamPortConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_ALIGNED_STREAM_PORT_BUFFER_SIZE ((((TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t))

#define TRC_USE_INTERNAL_BUFFER (TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER)

#define TRC_INTERNAL_EVENT_BUFFER_WRITE_MODE (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_WRITE_MODE)

#define TRC_INTERNAL_EVENT_BUFFER_TRANSFER_MODE (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_TRANSFER_MODE)

#define TRC_INTERNAL_BUFFER_CHUNK_SIZE (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_SIZE)

#define TRC_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT)

#define TRC_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT)

typedef struct TraceStreamPortBuffer	/* Aligned */
{
#if (TRC_USE_INTERNAL_BUFFER)
	uint8_t buffer[(TRC_ALIGNED_STREAM_PORT_BUFFER_SIZE)];
#else
	TraceUnsignedBaseType_t buffer[1];
#endif
} TraceStreamPortBuffer_t;

int32_t prvTraceTcpWrite(void* pvData, uint32_t uiSize, int32_t* piBytesWritten);

int32_t prvTraceTcpRead(void* pvData, uint32_t uiSize, int32_t* piBytesRead);

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

#define xTraceStreamPortWriteData(pvData, uiSize, piBytesWritten) (prvTraceTcpWrite(pvData, uiSize, piBytesWritten) == 0 ? TRC_SUCCESS : TRC_FAIL)

#define xTraceStreamPortReadData(pvData, uiSize, piBytesRead) (prvTraceTcpRead(pvData, uiSize, piBytesRead) == 0 ? TRC_SUCCESS : TRC_FAIL)

#define xTraceStreamPortOnEnable(uiStartOption) ((void)(uiStartOption), TRC_SUCCESS)

#define xTraceStreamPortOnDisable() (TRC_SUCCESS)

#define xTraceStreamPortOnTraceBegin() (TRC_SUCCESS)

traceResult xTraceStreamPortOnTraceEnd(void);

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAM_PORT_H */

