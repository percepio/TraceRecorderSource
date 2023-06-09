/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This stream port provides trace streaming using ESP-IDF apptrace
 * which depends on Xtensa TRAX for background memory access.
 */

#ifndef TRC_STREAM_PORT_H
#define TRC_STREAM_PORT_H

#include <trcDefines.h>
#include <trcTypes.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * TRC_CFG_APPTRACE_TRAX_BLOCKING_MODE
 * This stream port for streaming over XTensa Trax relies on apptrace that utilize
 * an internal TRAX RAM buffer read by OpenOCD during execution.
 *
 * Possible values:
 * - APPTRACE_TRAX_MODE_BLOCK_IF_FIFO_FULL
 * - APPTRACE_TRAX_MODE_NO_BLOCK_SKIP (default)
 *
 * Using APPTRACE_TRAX_MODE_BLOCK_IF_FIFO_FULL ensure that you get a
 * complete and valid trace. This may however cause blocking if your streaming
 * interface isn't fast enough, which may disturb the real-time behavior.
 *
 * We therefore recommend APPTRACE_TRAX_MODE_NO_BLOCK_SKIP. In this mode,
 * Tracealyzer will report lost events if the transfer is not
 * fast enough. If there are problems, try increasing the apptrace buffer
 * in sdkconfig.
 ******************************************************************************/
#if defined(CONFIG_PERCEPIO_RECORDER_CFG_TRAX_MODE_BLOCK_IF_FIFO_FULL)
#define TRC_CFG_APPTRACE_TRAX_BLOCKING_MODE 1
#elif defined(CONFIG_PERCEPIO_RECORDER_CFG_TRAX_MODE_NO_BLOCK_SKIP)
#define TRC_CFG_APPTRACE_TRAX_BLOCKING_MODE 0
#endif

#ifdef CONFIG_PERCEPIO_RECORDER_TRC_STREAM_PORT_USE_INTERNAL_BUFFER
#define TRC_USE_INTERNAL_BUFFER 1
/* Aligned */
#define TRC_STREAM_PORT_INTERNAL_BUFFER_SIZE ((((CONFIG_PERCEPIO_RECORDER_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t))
/* Aligned */
#else
#define TRC_USE_INTERNAL_BUFFER 0
#endif

#ifdef PERCEPIO_RECORDER_TRC_STREAM_PORT_INTERNAL_EVENT_BUFFER_TRANSFER_MODE_CHUNKED
#define TRC_INTERNAL_BUFFER_CHUNK_SIZE ((((CONFIG_PERCEPIO_RECORDER_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_SIZE) + sizeof(TraceUnsignedBaseType_t) - 1) / sizeof(TraceUnsignedBaseType_t)) * sizeof(TraceUnsignedBaseType_t))
#else
#define TRC_INTERNAL_BUFFER_CHUNK_SIZE 1024
#endif

#if CONFIG_PERCEPIO_RECORDER_TRC_STREAM_PORT_INTERNAL_BUFFER_WRITE_MODE_DIRECT == 1
#define TRC_INTERNAL_EVENT_BUFFER_WRITE_MODE TRC_INTERNAL_EVENT_BUFFER_OPTION_WRITE_MODE_DIRECT
#elif CONFIG_PERCEPIO_RECORDER_TRC_STREAM_PORT_INTERNAL_BUFFER_WRITE_MODE_COPY == 1
#define TRC_INTERNAL_EVENT_BUFFER_WRITE_MODE TRC_INTERNAL_EVENT_BUFFER_OPTION_WRITE_MODE_COPY
#endif

#if CONFIG_PERCEPIO_RECORDER_TRC_STREAM_PORT_INTERNAL_EVENT_BUFFER_TRANSFER_MODE_ALL == 1
#define TRC_INTERNAL_EVENT_BUFFER_TRANSFER_MODE TRC_INTERNAL_EVENT_BUFFER_OPTION_TRANSFER_MODE_ALL
#elif CONFIG_PERCEPIO_RECORDER_TRC_STREAM_PORT_INTERNAL_EVENT_BUFFER_TRANSFER_MODE_CHUNKED == 1
#define TRC_INTERNAL_EVENT_BUFFER_TRANSFER_MODE TRC_INTERNAL_EVENT_BUFFER_OPTION_TRANSFER_MODE_CHUNKED
#endif

#ifdef PERCEPIO_RECORDER_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT
#define TRC_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT (PERCEPIO_RECORDER_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT)
#else
#define TRC_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT 10
#endif

#ifdef PERCEPIO_RECORDER_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT
#define TRC_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT (PERCEPIO_RECORDER_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT)
#else
#define TRC_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT 10
#endif

/**
 * @brief A structure representing the trace stream port buffer.
 */
typedef struct TraceStreamPortBuffer
{																				/* Aligned */
#if (TRC_USE_INTERNAL_BUFFER == 1)
	uint8_t bufferInternal[TRC_STREAM_PORT_INTERNAL_BUFFER_SIZE];				/* Aligned */
#else
	TraceUnsignedBaseType_t dummy;												/* Aligned */
#endif
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
traceResult xTraceStreamPortWriteData(void* pvData, uint32_t uiSize, int32_t* piBytesWritten);

/**
 * @brief Reads data through the stream port interface.
 * 
 * @param[in] pvData Destination data buffer 
 * @param[in] uiSize Destination data buffer size
 * @param[out] piBytesRead Bytes read
 * 
 * @retval TRC_FAIL Read failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortReadData(void* pvData, uint32_t uiSize, int32_t* piBytesRead);

/**
 * @brief Performs initialization that should be performed when xTraceEnable(...) is called.
 * 
 * @param[in] uiStartOption The start option used when calling xTraceEnable(...)
 * 
 * @retval TRC_FAIL Fail
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortOnEnable(uint32_t uiStartOption);

#define xTraceStreamPortOnDisable() ((void)TRC_SUCCESS)

#define xTraceStreamPortOnTraceBegin() (TRC_SUCCESS)

#define xTraceStreamPortOnTraceEnd() (TRC_SUCCESS)

#ifdef __cplusplus
}
#endif

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#endif /* TRC_STREAM_PORT_H */
