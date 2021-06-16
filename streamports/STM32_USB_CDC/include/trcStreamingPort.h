/*
 * Trace Recorder for Tracealyzer v4.5.0
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The interface definitions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to use USB CDC as streaming channel.
 * The example is for STM32 using STM32Cube.
 */

#ifndef TRC_STREAMING_PORT_H
#define TRC_STREAMING_PORT_H

#include "usb_device.h"
#include "usbd_CDC_if.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The time to wait if the USB interface is busy. */
#define TRC_CFG_DELAY_ON_BUSY 3

/* For FreeRTOS. Modify for other RTOSes. */
#define TRC_OS_DELAY(x) vTaskDelay(x)

void trcCDCInit(void);

int32_t trcCDCReceive(void *data, uint32_t size, int32_t* NumBytes);

int32_t trcCDCTransmit(void* data, uint32_t size, int32_t * noOfBytesSent );

#define TRC_STREAM_PORT_INIT() \
        trcCDCInit();\
        TRC_STREAM_PORT_MALLOC();

#define TRC_STREAM_PORT_READ_DATA(_ptrData, _size, _ptrBytesRead) trcCDCReceive(_ptrData, _size, _ptrBytesRead)

#define TRC_STREAM_PORT_WRITE_DATA(_ptrData, _size, _ptrBytesSent) trcCDCTransmit(_ptrData, _size, _ptrBytesSent)


#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAMING_PORT_H */
