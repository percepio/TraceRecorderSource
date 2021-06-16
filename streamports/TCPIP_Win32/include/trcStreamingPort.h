/*
 * Trace Recorder for Tracealyzer v4.5.0
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The interface definitions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to use TCP/IP as streaming channel.
 * The example is for Windows sockets (Winsock), for use with Windows ports.
 */

#ifndef TRC_STREAMING_PORT_H
#define TRC_STREAMING_PORT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t readFromSocket(void* data, uint32_t size, int32_t *ptrBytesRead);
int32_t writeToSocket(void* data, uint32_t size, int32_t *ptrBytesWritten);

/* This port supports both direct write and buffered mode ...*/
#define TRC_STREAM_PORT_USE_INTERNAL_BUFFER 0

#define TRC_STREAM_PORT_READ_DATA(_ptrData, _size, _ptrBytesRead) readFromSocket(_ptrData, _size, _ptrBytesRead)

#if (TRC_STREAM_PORT_USE_INTERNAL_BUFFER == 1)    
	#define TRC_STREAM_PORT_WRITE_DATA(_ptrData, _size, _ptrBytesWritten) writeToSocket(_ptrData, _size, _ptrBytesWritten)
#else
	/* In the direct mode, _ptrBytesWritten is not used, so it is assumed that "all or nothing" is written. */
	#define TRC_STREAM_PORT_WRITE_DATA(_ptrData, _size, UNUSED) writeToSocket(_ptrData, _size, NULL)
#endif

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAMING_PORT_H */
