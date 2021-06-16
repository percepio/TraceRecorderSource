/*
 * Trace Recorder for Tracealyzer v4.5.0
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The interface definitions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to use TCP/IP as streaming channel.
 * The example is for lwIP.
 */

#ifndef TRC_STREAMING_PORT_H
#define TRC_STREAMING_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_STREAM_PORT_USE_INTERNAL_BUFFER 1

int32_t trcTcpRead(void* data, uint32_t size, int32_t *ptrBytesRead);

int32_t trcTcpWrite(void* data, uint32_t size, int32_t *ptrBytesWritten);

#define TRC_STREAM_PORT_READ_DATA(_ptrData, _size, _ptrBytesRead) trcTcpRead(_ptrData, _size, _ptrBytesRead)

#define TRC_STREAM_PORT_WRITE_DATA(_ptrData, _size, _ptrBytesSent) trcTcpWrite(_ptrData, _size, _ptrBytesSent)

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAMING_PORT_H */
