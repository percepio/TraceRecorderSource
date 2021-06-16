/*
 * Trace Recorder for Tracealyzer v4.5.0
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The interface definitions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to stream the trace to file.
 */

#ifndef TRC_STREAMING_PORT_H
#define TRC_STREAMING_PORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t writeToFile(void* data, uint32_t size, int32_t *ptrBytesWritten);

void closeFile(void);

void openFile(char* fileName);

/* This define will determine whether to use the internal PagedEventBuffer or not.
If file writing creates additional trace events (i.e. it uses semaphores or mutexes), 
then the paged event buffer must be enabled to avoid infinite recursion. */
#define TRC_STREAM_PORT_USE_INTERNAL_BUFFER 1

#define TRC_STREAM_PORT_READ_DATA(_ptrData, _size, _ptrBytesRead) 0 /* Does not read commands from Tz (yet) */

#define TRC_STREAM_PORT_WRITE_DATA(_ptrData, _size, _ptrBytesSent) writeToFile(_ptrData, _size, _ptrBytesSent)

#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC)
#define TRC_STREAM_PORT_MALLOC() \
			_TzTraceData = TRC_PORT_MALLOC((TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT) * (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE));
extern char* _TzTraceData;
#else
#define TRC_STREAM_PORT_MALLOC()  /* Custom or static allocation. Not used. */
#endif
#define TRC_STREAM_PORT_INIT() \
		TRC_STREAM_PORT_MALLOC(); \
		openFile("trace.psf")

#define TRC_STREAM_PORT_ON_TRACE_END() closeFile()

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAMING_PORT_H */
