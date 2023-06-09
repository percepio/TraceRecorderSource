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

/**
 * @def TRC_CFG_STREAM_PORT_RTT_UP_BUFFER_SIZE 
* 
 * @brief Defines the size of the "up" RTT buffer (target -> host) to use for writing
 * the trace data, for RTT buffer 1 or higher.
 *
 * This setting is ignored for RTT buffer 0, which can't be reconfigured
 * in runtime and therefore hard-coded to use the defines in SEGGER_RTT_Conf.h.
 *
 * Default buffer size for Tracealyzer is 5000 bytes. 
 *
 * If you have a stand-alone J-Link probe, the can be decreased to around 1 KB.
 * But integrated J-Link OB interfaces are slower and needs about 5-10 KB, 
 * depending on the amount of data produced.
 */
#define TRC_CFG_STREAM_PORT_RTT_UP_BUFFER_SIZE CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_RTT_UP_BUFFER_SIZE

/**
 * @def TRC_CFG_STREAM_PORT_RTT_DOWN_BUFFER_SIZE
*
 * @brief Defines the size of the "down" RTT buffer (host -> target) to use for reading
 * commands from Tracealyzer, for RTT buffer 1 or higher.
 *
 * Default buffer size for Tracealyzer is 32 bytes.
 *
 * This setting is ignored for RTT buffer 0, which can't be reconfigured
 * in runtime and therefore hard-coded to use the defines in SEGGER_RTT_Conf.h.
 */
#define TRC_CFG_STREAM_PORT_RTT_DOWN_BUFFER_SIZE CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_RTT_DOWN_BUFFER_SIZE

/**
 * @def TRC_CFG_STREAM_PORT_RTT_UP_BUFFER_INDEX
*
 * @brief Defines the RTT buffer to use for writing the trace data. Make sure that
 * the PC application has the same setting (File->Settings).
 *
 * Default: 1
 *
 * We don't recommend using RTT buffer 0, since mainly intended for terminals.
 * If you prefer to use buffer 0, it must be configured in SEGGER_RTT_Conf.h.
 */
#define TRC_CFG_STREAM_PORT_RTT_UP_BUFFER_INDEX CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_RTT_UP_BUFFER_INDEX

/**
 * @def TRC_CFG_STREAM_PORT_RTT_DOWN_BUFFER_INDEX
*
 * @brief Defines the RTT buffer to use for reading the trace data. Make sure that
 * the PC application has the same setting (File->Settings).
 *
 * Default: 1
 *
 * We don't recommend using RTT buffer 0, since mainly intended for terminals.
 * If you prefer to use buffer 0, it must be configured in SEGGER_RTT_Conf.h.
 */
#define TRC_CFG_STREAM_PORT_RTT_DOWN_BUFFER_INDEX CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_RTT_DOWN_BUFFER_INDEX

/**
 * @def TRC_CFG_STREAM_PORT_RTT_MODE
*
 * @brief This stream port for J-Link streaming relies on SEGGER RTT, that contains an
 * internal RAM buffer read by the J-Link probes during execution.
 *
 * Possible values:
 * - SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL
 * - SEGGER_RTT_MODE_NO_BLOCK_SKIP (default)
 *
 * Using SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL ensure that you get a
 * complete and valid trace. This may however cause blocking if your streaming
 * interface isn't fast enough, which may disturb the real-time behavior.
 *
 * We therefore recommend SEGGER_RTT_MODE_NO_BLOCK_SKIP. In this mode,
 * Tracealyzer will report lost events if the transfer is not
 * fast enough. In that case, try increasing the size of the "up buffer".
 */
#ifdef CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_RTT_MODE_NO_BLOCK_SKIP
#define TRC_CFG_STREAM_PORT_RTT_MODE SEGGER_RTT_MODE_NO_BLOCK_SKIP
#else
#define TRC_CFG_STREAM_PORT_RTT_MODE SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL
#endif

/**
 * @def TRC_CFG_STREAM_PORT_RTT_NO_LOCK_WRITE
 * 
 * @brief Sets if RTT should write without locking or not when writing
 * RTT data. This should normally be disabled with an exception being
 * Zephyr, where the SEGGER RTT locks aren't necessary and causes
 * problems if enabled.
 * 
 * Default: 0
 */
#define TRC_CFG_STREAM_PORT_RTT_NO_LOCK_WRITE CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_RTT_NO_LOCK_WRITE

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAM_PORT_CONFIG_H */
