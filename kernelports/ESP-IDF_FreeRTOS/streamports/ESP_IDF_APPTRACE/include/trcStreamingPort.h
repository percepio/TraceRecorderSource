/*
 * Trace Recorder for Tracealyzer v4.5.1
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This stream port provides trace streaming using ESP-IDF apptrace
 * which depends on Xtensa TRAX for background memory access.
 */

#ifndef TRC_STREAMING_PORT_H
#define TRC_STREAMING_PORT_H

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
#if CONFIG_PERCEPIO_RECORDER_CFG_TRAX_MODE_BLOCK_IF_FIFO_FULL == 1
    #define TRC_CFG_APPTRACE_TRAX_BLOCKING_MODE 1
#else
    #define TRC_CFG_APPTRACE_TRAX_BLOCKING_MODE 0
#endif

void initTrax();
int readFromTrax(void* ptrData, uint32_t size, int32_t* ptrBytesRead);
int writeToTrax(void* ptrData, uint32_t size, int32_t* ptrBytesWritten);

#define TRC_STREAM_PORT_INIT() initTrax()

#ifdef CONFIG_PERCEPIO_RECORDER_TRC_STREAM_PORT_USE_INTERNAL_BUFFER
#define TRC_STREAM_PORT_USE_INTERNAL_BUFFER 1
#else
#define TRC_STREAM_PORT_USE_INTERNAL_BUFFER 0
#endif

#if (TRC_STREAM_PORT_USE_INTERNAL_BUFFER == 1)
extern char _TzIntBuf[(TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT) * (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE)] __attribute__((aligned (4)));
#define TRC_STREAM_PORT_INTERNAL_BUFFER_INIT() prvPagedEventBufferInit(_TzIntBuf)
#endif

#define TRC_STREAM_PORT_WRITE_DATA(_ptrData, _size, _ptrBytesWritten) writeToTrax(_ptrData, _size, _ptrBytesWritten)
#define TRC_STREAM_PORT_READ_DATA(_ptrData, _size, _ptrBytesRead) readFromTrax(_ptrData, _size, _ptrBytesRead)

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAMING_PORT_H */
