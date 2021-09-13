/*
 * Trace Recorder for Tracealyzer v4.5.1
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Main configuration parameters for the trace recorder library.
 * More settings can be found in trcStreamingConfig.h.
 */

/**
 * @file
 *
 * @brief Main configuration parameters for the trace recorder library.
 * More settings can be found in trcStreamingConfig.h and trcSnapshotConfig.h.
 *
 * Read more at http://percepio.com/2016/10/05/rtos-tracing/
 */

#ifndef TRC_CONFIG_H
#define TRC_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_USE_TRACEALYZER_RECORDER 1

#include "trcPortDefines.h"
#include <kernel.h>

/**
 * @brief Recorder Config Macros
 * @defgroup tracing_config Tracing Config Macros
 * @{
 */

/**
 * @def TRC_CFG_HARDWARE_PORT
 *
 * @brief Specify what hardware port to use (i.e., the "timestamping driver").
 *
 * All ARM Cortex-M MCUs are supported by "TRC_HARDWARE_PORT_ARM_Cortex_M".
 * This port uses the DWT cycle counter for Cortex-M3/M4/M7 devices, which is
 * available on most such devices. In case your device don't have DWT support,
 * you will get an error message opening the trace. In that case, you may
 * force the recorder to use SysTick timestamping instead, using this define:
 *
 * #define TRC_CFG_ARM_CM_USE_SYSTICK
 *
 * For ARM Cortex-M0/M0+ devices, SysTick mode is used automatically.
 *
 * See trcHardwarePort.h for available ports and information on how to
 * define your own port, if not already present.
 */
#define TRC_CFG_HARDWARE_PORT TRC_HARDWARE_PORT_ZEPHYR

/**
 * @def TRC_CFG_RECORDER_MODE
 *
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
#ifdef CONFIG_PERCEPIO_TRC_RECORDER_MODE_SNAPSHOT
    #define TRC_CFG_RECORDER_MODE TRC_RECORDER_MODE_SNAPSHOT
#else
    #define TRC_CFG_RECORDER_MODE TRC_RECORDER_MODE_STREAMING
#endif

/**
 * @def TRC_CFG_SCHEDULING_ONLY
 * 
 * @brief Macro which should be defined as an integer value.
 *
 * If this setting is enabled (= 1), only scheduling events are recorded.
 * If disabled (= 0), all events are recorded (unless filtered in other ways).
 *
 * Default value is 0 (= include additional events).
 */
#define TRC_CFG_SCHEDULING_ONLY 0

/**
 * @def TRC_CFG_INCLUDE_USER_EVENTS
 *
 * @brief Macro which should be defined as either zero (0) or one (1).
 *
 * If this is zero (0), all code related to User Events is excluded in order 
 * to reduce code size. Any attempts of storing User Events are then silently
 * ignored.
 *
 * User Events are application-generated events, like "printf" but for the 
 * trace log, generated using vTracePrint and vTracePrintF. 
 * The formatting is done on host-side, by Tracealyzer. User Events are 
 * therefore much faster than a console printf and can often be used
 * in timing critical code without problems.
 *
 * Note: In streaming mode, User Events are used to provide error messages
 * and warnings from the recorder (in case of incorrect configuration) for
 * display in Tracealyzer. Disabling user events will also disable these
 * warnings. You can however still catch them by calling xTraceGetLastError
 * or by putting breakpoints in prvTraceError and prvTraceWarning.
 *
 * Default value is 1.
 */
#define TRC_CFG_INCLUDE_USER_EVENTS 1

/*****************************************************************************
 * TRC_CFG_INCLUDE_READY_EVENTS
 *
 * Macro which should be defined as either zero (0) or one (1).
 *
 * If one (1), events are recorded when tasks enter scheduling state "ready".
 * This allows Tracealyzer to show the initial pending time before tasks enter
 * the execution state, and present accurate response times.
 * If zero (0), "ready events" are not created, which allows for recording
 * longer traces in the same amount of RAM.
 *
 * Default value is 1.
 *****************************************************************************/
//#define TRC_CFG_INCLUDE_READY_EVENTS 1

/**
 * @def TRC_CFG_ENABLE_STACK_MONITOR
 *
 * @brief If enabled (1), the recorder periodically reports the unused stack space of
 * all active tasks.
 * The stack monitoring runs in the Tracealyzer Control task, TzCtrl. This task
 * is always created by the recorder when in streaming mode. 
 * In snapshot mode, the TzCtrl task is only used for stack monitoring and is
 * not created unless this is enabled.
 */
#ifdef CONFIG_PERCEPIO_RECORDER_CFG_STACK_MONITOR
#define TRC_CFG_ENABLE_STACK_MONITOR CONFIG_PERCEPIO_RECORDER_CFG_STACK_MONITOR
#endif

/**
 * @def TRC_CFG_STACK_MONITOR_MAX_TASKS
 *
 * @brief Macro which should be defined as a non-zero integer value.
 *
 * This controls how many tasks that can be monitored by the stack monitor.
 * If this is too small, some tasks will be excluded and a warning is shown.
 */
#ifdef CONFIG_PERCEPIO_RECORDER_CFG_STACK_MONITOR_MAX_TASKS
#define TRC_CFG_STACK_MONITOR_MAX_TASKS CONFIG_PERCEPIO_RECORDER_CFG_STACK_MONITOR_MAX_TASKS
#else
#define TRC_CFG_STACK_MONITOR_MAX_TASKS 10
#endif

/**
 * @def TRC_CFG_STACK_MONITOR_MAX_REPORTS
 *
 * @brief Macro which should be defined as a non-zero integer value.
 *
 * This defines how many tasks that will be subject to stack usage analysis for
 * each execution of the Tracealyzer Control task (TzCtrl). Note that the stack
 * monitoring cycles between the tasks, so this does not affect WHICH tasks that
 * are monitored, but HOW OFTEN each task stack is analyzed. 
 *
 * This setting can be combined with TRC_CFG_CTRL_TASK_DELAY_MS to tune the
 * frequency of the stack monitoring. This is motivated since the stack analysis
 * can take some time to execute.
 * However, note that the stack analysis runs in a separate task (TzCtrl) that
 * can be executed on low priority. This way, you can avoid that the stack
 * analysis disturbs any time-sensitive tasks.
 */
#ifdef CONFIG_PERCEPIO_RECORDER_CFG_STACK_MONITOR_MAX_REPORTS
#define TRC_CFG_STACK_MONITOR_MAX_REPORTS CONFIG_PERCEPIO_RECORDER_CFG_STACK_MONITOR_MAX_REPORTS
#else
#define TRC_CFG_STACK_MONITOR_MAX_REPORTS 1
#endif

/**
 * @def TRC_CFG_CTRL_TASK_PRIORITY
 *
 * @brief The scheduling priority of the Tracealyzer Control (TzCtrl) task. 
 *
 * In streaming mode, TzCtrl is used to receive start/stop commands from 
 * Tracealyzer and in some cases also to transmit the trace data (for stream
 * ports that uses the internal buffer, like TCP/IP). For such stream ports,
 * make sure the TzCtrl priority is high enough to ensure reliable periodic
 * execution and transfer of the data, but low enough to avoid disturbing any 
 * time-sensitive functions.
 *
 * In Snapshot mode, TzCtrl is only used for the stack usage monitoring and is
 * not created if stack monitoring is disabled. TRC_CFG_CTRL_TASK_PRIORITY should
 * be low, to avoid disturbing any time-sensitive tasks.
 */
#ifdef CONFIG_PERCEPIO_RECORDER_CFG_CTRL_TASK_PRIORITY
#define TRC_CFG_CTRL_TASK_PRIORITY CONFIG_PERCEPIO_RECORDER_CFG_CTRL_TASK_PRIORITY
#else
#define TRC_CFG_CTRL_TASK_PRIORITY 1
#endif

/**
 * @def TRC_CFG_CTRL_TASK_DELAY_MS
 *
 * @brief The delay between loops of the TzCtrl task (see TRC_CFG_CTRL_TASK_PRIORITY), 
 * which affects the frequency of the stack monitoring. 
 * 
 * In streaming mode, this also affects the trace data transfer if you are using
 * a stream port leveraging the internal buffer (like TCP/IP). A shorter delay
 * increases the CPU load of TzCtrl somewhat, but may improve the performance of
 * of the trace streaming, especially if the trace buffer is small.
 */
#ifdef CONFIG_PERCEPIO_RECORDER_CFG_CTRL_TASK_DELAY
#define TRC_CFG_CTRL_TASK_DELAY_MS CONFIG_PERCEPIO_RECORDER_CFG_CTRL_TASK_DELAY
#else
#define TRC_CFG_CTRL_TASK_DELAY_MS 10
#endif

/**
 * @def TRC_CFG_CTRL_TASK_STACK_SIZE
 *
 * @brief The stack size of the Tracealyzer Control (TzCtrl) task.
 * See TRC_CFG_CTRL_TASK_PRIORITY for further information about TzCtrl.
 */
#ifdef CONFIG_PERCEPIO_RECORDER_CFG_CTRL_TASK_STACK_SIZE
#define TRC_CFG_CTRL_TASK_STACK_SIZE CONFIG_PERCEPIO_RECORDER_CFG_CTRL_TASK_STACK_SIZE
#else
#define TRC_CFG_CTRL_TASK_STACK_SIZE (512)
#endif

/**
 * @def TRC_CFG_RECORDER_BUFFER_ALLOCATION
 *
 * @brief Specifies how the recorder buffer is allocated (also in case of streaming, in
 * port using the recorder's internal temporary buffer)
 *
 * Values:
 * TRC_RECORDER_BUFFER_ALLOCATION_STATIC  - Static allocation (internal)
 * TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC - Malloc in vTraceEnable
 * TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM  - Use vTraceSetRecorderDataBuffer
 *
 * Static and dynamic mode does the allocation for you, either in compile time
 * (static) or in runtime (malloc).
 * The custom mode allows you to control how and where the allocation is made,
 * for details see TRC_ALLOC_CUSTOM_BUFFER and vTraceSetRecorderDataBuffer().
 */
#ifdef CONFIG_PERCEPIO_RECORDER_TRC_RECORDER_BUFFER_ALLOCATION_STATIC
    #define TRC_CFG_RECORDER_BUFFER_ALLOCATION TRC_RECORDER_BUFFER_ALLOCATION_STATIC
#elif CONFIG_PERCEPIO_RECORDER_TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC
    #define TRC_CFG_RECORDER_BUFFER_ALLOCATION TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC
#else
    #define TRC_CFG_RECORDER_BUFFER_ALLOCATION TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM
#endif

/**
 * @def TRC_CFG_MAX_ISR_NESTING
 *
 * @brief Defines how many levels of interrupt nesting the recorder can handle, in
 * case multiple ISRs are traced and ISR nesting is possible. If this
 * is exceeded, the particular ISR will not be traced and the recorder then
 * logs an error message. This setting is used to allocate an internal stack
 * for keeping track of the previous execution context (4 byte per entry).
 *
 * This value must be a non-zero positive constant, at least 1.
 */
#ifdef CONFIG_PERCEPIO_RECORDER_CFG_MAX_ISR_NESTING
#define TRC_CFG_MAX_ISR_NESTING CONFIG_PERCEPIO_RECORDER_CFG_MAX_ISR_NESTING
#else
#define TRC_CFG_MAX_ISR_NESTING 8
#endif

/**
 * @def TRC_CFG_RECORDER_DATA_ATTRIBUTE
 *
 * @brief When setting TRC_CFG_RECORDER_DATA_INIT to 0, you might also need to make
 * sure certain recorder data is placed in a specific RAM section to avoid being
 * zeroed out after initialization. Define TRC_CFG_RECORDER_DATA_ATTRIBUTE as
 * that attribute.
 *
 * Example:
 * #define TRC_CFG_RECORDER_DATA_ATTRIBUTE __attribute__((section(".bss.trace_recorder_data")))
 *
 * Default value is empty.
 */
#define TRC_CFG_RECORDER_DATA_ATTRIBUTE

/**
 * @}
 */


/* Specific configuration, depending on Streaming/Snapshot mode */
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
#include "trcSnapshotConfig.h"
#elif (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
#include "trcStreamingConfig.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* _TRC_CONFIG_H */
