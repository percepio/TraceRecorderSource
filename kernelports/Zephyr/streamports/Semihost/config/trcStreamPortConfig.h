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

//Where the trace file should be stored
#define TRC_CFG_STREAM_PORT_TRACE_FILE CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_TRACE_FILE

/**
 * @def TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER
 *
 * @brief This define will determine whether to use the internal buffer or not.
 * If file writing creates additional trace events (i.e. it uses semaphores or mutexes),
 * then the internal buffer must be enabled to avoid infinite recursion.
 */
#ifdef CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER
#define TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER
#else
#define TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER 0
#endif

/**
 * @def TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE
 *
 * @brief Configures the size of the internal buffer if used.
 */
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE

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
#ifdef CONFIG_PERCEPIO_TRC_INTERNAL_EVENT_BUFFER_OPTION_TRANSFER_MODE_ALL
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_TRANSFER_MODE TRC_INTERNAL_EVENT_BUFFER_OPTION_TRANSFER_MODE_ALL
#else
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_TRANSFER_MODE TRC_INTERNAL_EVENT_BUFFER_OPTION_TRANSFER_MODE_CHUNKED
#endif

/**
 * @def TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_SIZE
 *
 * @brief Defines the maximum chunk size when transferring
 * internal buffer events in chunks.
 */
#ifdef CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_SIZE
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_SIZE CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_SIZE
#else
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_SIZE 1000
#endif

/**
 * @def TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT
 *
 * @brief Defines the number of transferred bytes needed to trigger another transfer.
 * It also depends on TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT to set a maximum number
 * of additional transfers this loop.
 * This will increase throughput by immediately doing a transfer and not wait for another xTraceTzCtrl() loop.
 */
#ifdef CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT
#else
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT (TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_SIZE / 2UL)
#endif

/**
 * @def TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT
 *
 * @brief Defines the maximum number of times to trigger another transfer before returning to xTraceTzCtrl().
 * It also depends on TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_SIZE_LIMIT to see if a meaningful amount of data was
 * transferred in the last loop.
 * This will increase throughput by immediately doing a transfer and not wait for another xTraceTzCtrl() loop.
 */
#ifdef CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT
#else
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_CHUNK_TRANSFER_AGAIN_COUNT_LIMIT 5
#endif

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAM_PORT_CONFIG_H */
