/*
 * Trace Recorder for Tracealyzer v4.7.0
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
 * @def TRC_CFG_STREAM_PORT_ITM_PORT
 * @brief What ITM port to use for the ITM software events. Make sure the IDE is
 * configured for the same channel.
 *
 * Default: 1 (0 is typically terminal output and 31 is used by Keil)
 *
 */
#ifdef CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_ITM
#define TRC_CFG_STREAM_PORT_ITM_PORT CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_ITM
#else
#define TRC_CFG_STREAM_PORT_ITM_PORT 1
#endif

/**
 * @def TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER
 * @brief determines whether to use the internal buffer or not.
 * If file writing creates additional trace events (i.e. it uses semaphores or mutexes),
 * then the internal buffer must be enabled to avoid infinite recursion.
 */
#ifdef CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER
#define TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER
#else
#define TRC_CFG_STREAM_PORT_USE_INTERNAL_BUFFER 0
#endif

/**
 * @def TRC_CFG_INTERNAL_BUFFER_SIZE
 *
 * @brief Configures the size of the internal buffer if used.
 * is enabled.
 */
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_SIZE

/**
 * @def TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_WRITE_MODE
 *
 * @brief
 */
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_WRITE_MODE TRC_INTERNAL_EVENT_BUFFER_OPTION_WRITE_MODE_DIRECT

/**
 * @def TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_TRANSFER_MODE
 *
 * @brief
 */
#ifdef CONFIG_PERCEPIO_TRC_INTERNAL_EVENT_BUFFER_OPTION_TRANSFER_MODE_ALL
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_TRANSFER_MODE TRC_INTERNAL_EVENT_BUFFER_OPTION_TRANSFER_MODE_ALL
#else
#define TRC_CFG_STREAM_PORT_INTERNAL_BUFFER_TRANSFER_MODE TRC_INTERNAL_EVENT_BUFFER_OPTION_TRANSFER_MODE_CHUNK
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

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAM_PORT_CONFIG_H */
