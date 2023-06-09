/*
 * Trace Recorder for Tracealyzer v4.7.0
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Configuration parameters for the kernel port.
 * More settings can be found in trcKernelPortStreamingConfig.h and
 * trcKernelPortSnapshotConfig.h.
 */

#ifndef TRC_KERNEL_PORT_CONFIG_H
#define TRC_KERNEL_PORT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def TRC_CFG_RECORDER_MODE
 * @brief Specify what recording mode to use. Snapshot means that the data is saved in
 * an internal RAM buffer, for later upload. Streaming means that the data is
 * transferred continuously to the host PC.
 *
 * For more information, see http://percepio.com/2016/10/05/rtos-tracing/
 * and the Tracealyzer User Manual.
 *
 * Values:
 * TRC_RECORDER_MODE_SNAPSHOT
 * TRC_RECORDER_MODE_STREAMING
 */
#if CONFIG_PERCEPIO_RECORDER_TRC_RECORDER_MODE_SNAPSHOT == 1
    #define TRC_CFG_RECORDER_MODE TRC_RECORDER_MODE_SNAPSHOT
#else
    #define TRC_CFG_RECORDER_MODE TRC_RECORDER_MODE_STREAMING
#endif

/**
 * @def TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS
 * @brief Macro which should be defined as either zero (0) or one (1).
 *
 * If this is zero (0), the trace will exclude any "event group" events.
 *
 * Default value is 0 (excluded) since dependent on event_groups.c
 */
#define TRC_CFG_INCLUDE_EVENT_GROUP_EVENTS CONFIG_PERCEPIO_RECORDER_CFG_INCLUDE_EVENT_GROUP_EVENTS

/**
 * @def TRC_CFG_INCLUDE_TIMER_EVENTS
 * @brief Macro which should be defined as either zero (0) or one (1).
 *
 * If this is zero (0), the trace will exclude any Timer events.
 *
 * Default value is 0 since dependent on timers.c
 */
#define TRC_CFG_INCLUDE_TIMER_EVENTS CONFIG_PERCEPIO_RECORDER_CFG_INCLUDE_TIMER_EVENTS

/**
 * @def TRC_CFG_INCLUDE_PEND_FUNC_CALL_EVENTS
 * @brief Macro which should be defined as either zero (0) or one (1).
 *
 * If this is zero (0), the trace will exclude any "pending function call" 
 * events, such as xTimerPendFunctionCall().
 *
 * Default value is 0 since dependent on timers.c
 */
#define TRC_CFG_INCLUDE_PEND_FUNC_CALL_EVENTS CONFIG_PERCEPIO_RECORDER_CFG_INCLUDE_PEND_FUNC_CALL_EVENTS

/**
 * @def TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS
 * @brief Macro which should be defined as either zero (0) or one (1).
 *
 * If this is zero (0), the trace will exclude any stream buffer or message
 * buffer events.
 *
 * Default value is 0 since dependent on stream_buffer.c (new in FreeRTOS v10)
 */
#define TRC_CFG_INCLUDE_STREAM_BUFFER_EVENTS CONFIG_PERCEPIO_RECORDER_CFG_INCLUDE_STREAM_BUFFER_EVENTS

/**
 * @def TRC_CFG_ACKNOWLEDGE_QUEUE_SET_SEND
 * @brief When using FreeRTOS v10.3.0 or v10.3.1, please make sure that the trace
 * point in prvNotifyQueueSetContainer() in queue.c is renamed from
 * traceQUEUE_SEND to traceQUEUE_SET_SEND in order to tell them apart from
 * other traceQUEUE_SEND trace points. Then set this to TRC_ACKNOWLEDGED.
 */
#define TRC_CFG_ACKNOWLEDGE_QUEUE_SET_SEND  0 /* TRC_ACKNOWLEDGED */

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_CONFIG_H */
