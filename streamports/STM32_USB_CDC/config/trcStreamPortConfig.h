/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The configuration for trace streaming ("stream ports").
 */

#ifndef TRC_STREAM_PORT_CONFIG_H
#define TRC_STREAM_PORT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def TRC_CFG_STREAM_PORT_DELAY_ON_BUSY
 *
 * @brief The time to wait if the USB interface is busy.
 */
#define TRC_CFG_STREAM_PORT_DELAY_ON_BUSY 1

/**
 * @def TRC_CFG_STREAM_PORT_USB_BUFFER_SIZE
 *
 * @brief Specifies the size of the usb buffer.
 */
#define TRC_CFG_STREAM_PORT_USB_BUFFER_SIZE 64

/**
 * @def TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE
 *
 * @brief Configures the size of the internal buffer if used.
 */
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE 10240

/**
 * @def TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_WRITE_MODE
 *
 * @brief This should be set to TRC_INTERNAL_EVENT_BUFFER_OPTION_WRITE_MODE_DIRECT for best performance.
 */
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_WRITE_MODE TRC_INTERNAL_EVENT_BUFFER_OPTION_WRITE_MODE_DIRECT

/**
 * @def TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_TRANSFER_MODE
 *
 * @brief Defines if the internal buffer will attempt to transfer all data each time or limit it to a chunk size.
 */
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_TRANSFER_MODE TRC_INTERNAL_EVENT_BUFFER_OPTION_TRANSFER_MODE_ALL

/**
 * @def TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_SIZE
 *
 * @brief Defines the maximum chunk size when transferring
 * internal buffer events in chunks.
 */
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_SIZE 64

/**
 * @def TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT
 *
 * @brief Defines the number of transferred bytes needed to trigger another transfer.
 * It also depends on TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT to set a maximum number
 * of additional transfers this loop.
 * This will increase throughput by immediately doing a transfer and not wait for another xTraceTzCtrl() loop.
 */
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT 64

/**
 * @def TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT
 *
 * @brief Defines the maximum number of times to trigger another transfer before returning to xTraceTzCtrl().
 * It also depends on TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT to see if a meaningful amount of data was
 * transferred in the last loop.
 * This will increase throughput by immediately doing a transfer and not wait for another xTraceTzCtrl() loop.
 */
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT 5

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAM_PORT_CONFIG_H */
