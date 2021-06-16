/*
 * Percepio Trace Recorder SDK for Tracealyzer v4.5.0
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The interface for the internal buffer.
 */

#ifndef TRC_INTERNAL_BUFFER_H
#define TRC_INTERNAL_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "trcRecorder.h"


#if (TRC_USE_TRACEALYZER_RECORDER == 1) && (TRC_STREAM_PORT_USE_INTERNAL_BUFFER == 1)

/**
 * @brief Initializes the paged event buffer used by certain stream ports.
 * 
 * @param buffer Pointer to previously allocated memory buffer
 */
void prvPagedEventBufferInit(char* buffer);

/**
 * @brief Retrieve a pointer to an available location in the buffer able
 * to store the requested size.
 * 
 * @param sizeOfEvent Size of the event that is to be placed in the buffer.
 * @return void* Pointer to free buffer memory area.
 */
void* prvPagedEventBufferGetWritePointer(int sizeOfEvent);

/**
 * @brief Transfers one buffer page of trace data, if a full page is available,
 * using the macro TRC_STREAM_PORT_WRITE_DATA as defined in trcStreamingPort.h.
 *
 * This function is intended to be called the periodic TzCtrl task with a 
 * suitable delay (e.g. 10-100 ms).
 *
 * In case of errors from the streaming interface, it registers a warning
 * (PSF_WARNING_STREAM_PORT_WRITE) provided by xTraceGetLastError().
 * 
 * @return uint32_t The number of bytes sent. If non-zero, it is good to call this 
 * again, in order to send any additional data waiting in the buffer.
 * If zero, wait a while before calling again.
 */
uint32_t prvPagedEventBufferTransfer(void);

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1) && (TRC_STREAM_PORT_USE_INTERNAL_BUFFER == 1)*/

#ifdef __cplusplus
}
#endif

#endif /* TRC_INTERNAL_BUFFER_H */
