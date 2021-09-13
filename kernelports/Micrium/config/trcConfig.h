/*
 * Trace Recorder for Tracealyzer v4.5.1
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Main configuration parameters for the trace recorder library.
 * More settings can be found in trcStreamingConfig.h and trcSnapshotConfig.h.
 */
 
#ifndef TRC_CONFIG_H
#define TRC_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "trcPortDefines.h"

/******************************************************************************
 * Include of processor header file
 * 
 * Here you may need to include the header file for your processor. This is 
 * required at least for the ARM Cortex-M port, that uses the ARM CMSIS API.
 * Try that in case of build problems. Otherwise, remove the #error line below.
 *****************************************************************************/
#error "Trace Recorder: Please include your processor?s header file here and remove this line."

/*******************************************************************************
 * Configuration Macro: TRC_CFG_HARDWARE_PORT
 *
 * Specify what hardware port to use (i.e., the "timestamping driver").
 * All ARM Cortex-M MCUs are supported by "TRC_HARDWARE_PORT_ARM_Cortex_M".
 *
 * See trcSnapshotHardwarePort.h or trcStreamingHardwarePort.h for available
 * ports and information on how to define your own port, if not already present.
 ******************************************************************************/
#define TRC_CFG_HARDWARE_PORT TRC_HARDWARE_PORT_NOT_SET

/*******************************************************************************
 * Configuration Macro: TRC_CFG_RECORDER_MODE
 *
 * Specify what recording mode to use. Snapshot means that the data is saved in
 * an internal RAM buffer, for later upload. Streaming means that the data is
 * transferred continuously to the host PC. 
 *
 * For more information, see http://percepio.com/2016/10/05/rtos-tracing/
 * and the Tracealyzer User Manual.
 *
 * Values:
 * TRC_RECORDER_MODE_SNAPSHOT
 * TRC_RECORDER_MODE_STREAMING
 ******************************************************************************/
#define TRC_CFG_RECORDER_MODE TRC_RECORDER_MODE_SNAPSHOT

/*******************************************************************************
 * TRC_BSP_CLK_SOURCE
 *
 * Macro which should be defined as an integervalue.
 *
 * If this setting is enabled (= 1), the recorder will use the default functions
 * (BSP_ClkFreqGet(CLK_ID_SYSCLK) or BSP_CPU_ClkFreq()) from bsp.h.
 *
 * If you are using a custom BSP package you need to dissable (= 0) this setting
 * and set your own clock source or the frequency.
 *
 * The default value is 1.
 ******************************************************************************/
#define TRC_BSP_CLK_SOURCE 1

#if(TRC_BSP_CLK_SOURCE == 0)

/*******************************************************************************
 * If TRC_BSP_CLK_SOURCE is set to 0 a function or the frequency value in Hz 
 * need to be set by TRACE_CPU_CLOCK_HZ.
 * ****************************************************************************/
#define TRACE_CPU_CLOCK_HZ 0

#if(TRACE_CPU_CLOCK_HZ == 0)

#error "Trace Recorder: A clock source need to be set, either by BSP function or manualy."

#endif
#endif
/*******************************************************************************
 * TRC_CFG_SCHEDULING_ONLY
 *
 * Macro which should be defined as an integer value.
 *
 * If this setting is enabled (= 1), only scheduling events are recorded.
 * If disabled (= 0), all events are recorded (unless filtered in other ways).
 *
 * Default value is 0 (= include additional events).
 ******************************************************************************/
#define TRC_CFG_SCHEDULING_ONLY 0

 /******************************************************************************
 * TRC_CFG_INCLUDE_USER_EVENTS
 *
 * Macro which should be defined as either zero (0) or one (1).
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
 *****************************************************************************/
#define TRC_CFG_INCLUDE_USER_EVENTS 1

 /*****************************************************************************
 * TRC_CFG_INCLUDE_OSTICK_EVENTS
 *
 * Macro which should be defined as either zero (0) or one (1).
 *
 * If this is one (1), events will be generated whenever the OS clock is
 * increased. If zero (0), OS tick events are not generated, which allows for
 * recording longer traces in the same amount of RAM.
 *
 * Default value is 1.
 *****************************************************************************/
#define TRC_CFG_INCLUDE_OSTICK_EVENTS 1

 /*****************************************************************************
 * TRC_CFG_INCLUDE_ISR_TRACING
 *
 * Macro which should be defined as either zero (0) or one (1).
 *
 * If this is zero (0), the code for recording Interrupt Service Routines is
 * excluded, in order to reduce code size. This means that any calls to
 * vTraceStoreISRBegin/vTraceStoreISREnd will be ignored.
 * This does not completely disable ISR tracing, in cases where an ISR is
 * calling a traced kernel service. These events will still be recorded and
 * show up in anonymous ISR instances in Tracealyzer, with names such as
 * "ISR sending to <queue name>".
 * To disable such tracing, please refer to vTraceSetFilterGroup and 
 * vTraceSetFilterMask.
 *
 * Default value is 1.
 *
 * Note: tracing ISRs requires that you insert calls to vTraceStoreISRBegin
 * and vTraceStoreISREnd in your interrupt handlers.
 *****************************************************************************/
#define TRC_CFG_INCLUDE_ISR_TRACING 1

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
#define TRC_CFG_INCLUDE_READY_EVENTS 1

/*****************************************************************************
* TRC_CFG_INCLUDE_TICK_TASK
*
* Macro which should be defined as either zero (0) or one (1).
*
* If one (1), events related to the kernel tick task are recorded.
* We filter out this task by default since it creates a lot of data that is
* uninteresting for most users.
*
* Default value is 0.
*****************************************************************************/
#define TRC_CFG_INCLUDE_TICK_TASK 0

 /******************************************************************************
 * TRC_CFG_ENABLE_STACK_MONITOR
 *
 * If enabled (1), the recorder periodically reports the unused stack space of
 * all active tasks.
 * The stack monitoring runs in the Tracealyzer Control task, TzCtrl. This task
 * is always created by the recorder when in streaming mode. 
 * In snapshot mode, the TzCtrl task is only used for stack monitoring and is
 * not created unless this is enabled.
 *
 * Note: vTraceStartStackMonitor() must be called after OSInit();
 *****************************************************************************/
#define TRC_CFG_ENABLE_STACK_MONITOR 0

 /******************************************************************************
 * TRC_CFG_STACK_MONITOR_MAX_TASKS
 *
 * Macro which should be defined as a non-zero integer value.
 *
 * This controls how many tasks that can be monitored by the stack monitor.
 * If this is too small, some tasks will be excluded and a warning is shown.
 *
 * Default value is 10.
 *****************************************************************************/
#define TRC_CFG_STACK_MONITOR_MAX_TASKS 10

 /******************************************************************************
 * TRC_CFG_STACK_MONITOR_MAX_REPORTS
 *
 * Macro which should be defined as a non-zero integer value.
 *
 * This defines how many tasks that will be subject to stack usage analysis for
 * each execution of the Tracealyzer Control task (TzCtrl). Note that the stack
 * monitoring cycles between the tasks, so this does not affect WHICH tasks that
 * are monitored, but HOW OFTEN each task stack is analyzed. 
 *
 * This setting can be combined with TRC_CFG_CTRL_TASK_DELAY to tune the
 * frequency of the stack monitoring. This is motivated since the stack analysis
 * can take some time to execute.
 * However, note that the stack analysis runs in a separate task (TzCtrl) that
 * can be executed on low priority. This way, you can avoid that the stack
 * analysis disturbs any time-sensitive tasks.
 *
 *
 * Default value is 1.
 *****************************************************************************/
#define TRC_CFG_STACK_MONITOR_MAX_REPORTS 1

/*******************************************************************************
 * Configuration Macro: TRC_CFG_CTRL_TASK_PRIORITY
 *
 * The scheduling priority of the Tracealyzer Control (TzCtrl) task. 
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
 ******************************************************************************/
#define TRC_CFG_CTRL_TASK_PRIORITY 1

/*******************************************************************************
 * Configuration Macro: TRC_CFG_CTRL_TASK_DELAY
 *
 * The delay between loops of the TzCtrl task (see TRC_CFG_CTRL_TASK_PRIORITY), 
 * which affects the frequency of the stack monitoring. 
 * 
 * In streaming mode, this also affects the trace data transfer if you are using
 * a stream port leveraging the internal buffer (like TCP/IP). A shorter delay
 * increases the CPU load of TzCtrl somewhat, but may improve the performance of
 * of the trace streaming, especially if the trace buffer is small.
 ******************************************************************************/
#define TRC_CFG_CTRL_TASK_DELAY 10

/*******************************************************************************
 * Configuration Macro: TRC_CFG_CTRL_TASK_STACK_SIZE
 *
 * The stack size of the Tracealyzer Control (TzCtrl) task.
 * See TRC_CFG_CTRL_TASK_PRIORITY for further information about TzCtrl.
 ******************************************************************************/
#define TRC_CFG_CTRL_TASK_STACK_SIZE (512)

/*******************************************************************************
 * Configuration Macro: TRC_CFG_RECORDER_BUFFER_ALLOCATION
 *
 * Specifies how the recorder's internal buffer is allocated (snapshot or
 * streaming). Note that CUSTOM is only supported in snapshot mode.
 *
 * TRC_RECORDER_BUFFER_ALLOCATION_STATIC  - Static allocation 
 * TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC - Allocated in vTraceEnable
 * TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM  - Use vTraceSetRecorderDataBuffer
 ******************************************************************************/
#define TRC_CFG_RECORDER_BUFFER_ALLOCATION TRC_RECORDER_BUFFER_ALLOCATION_STATIC

/******************************************************************************
 * TRC_CFG_MAX_ISR_NESTING
 * 
 * Defines how many levels of interrupt nesting the recorder can handle, in
 * case multiple ISRs are traced and ISR nesting is possible. If this
 * is exceeded, the particular ISR will not be traced and the recorder then 
 * logs an error message. This setting is used to allocate an internal stack
 * for keeping track of the previous execution context (4 byte per entry). 
 *
 * This value must be a non-zero positive constant, at least 1.
 * 
 * Default value: 8
 *****************************************************************************/
#define TRC_CFG_MAX_ISR_NESTING 8

 /*******************************************************************************
 * TRC_CFG_RECORDER_DATA_INIT
 *
 * Macro which states wether the recorder data should have an initial value.
 *
 * In very specific cases where traced objects are created before main(),
 * the recorder will need to be started even before that. In these cases,
 * the recorder would be initialized by vTraceEnable(TRC_INIT) but could then
 * later be overwritten by the initialization value because initialization order
 * is unspecified by compilers.
 * If this is an issue for you, set TRC_CFG_RECORDER_DATA_INIT to 0.
 * The following code can then be used before any traced objects are created:
 *
 *    extern uint32_t RecorderInitialized;
 *    RecorderInitialized = 0;
 *    vTraceEnable(TRC_INIT);
 *
 * After the hardware and clocks are properly initialized, use 
 * vTraceEnable(TRC_START) to start the tracing.
 *
 * Default value is 1.
 ******************************************************************************/
#define TRC_CFG_RECORDER_DATA_INIT 1

 /******************************************************************************
 * TRC_CFG_RECORDER_DATA_ATTRIBUTE
 *
 * When setting TRC_CFG_RECORDER_DATA_INIT to 0, you might also need to make
 * sure certain recorder data is placed in a specific RAM section to avoid being
 * zeroed out after initialization. Define TRC_CFG_RECORDER_DATA_ATTRIBUTE as
 * that attribute.
 *
 * Example:
 * #define TRC_CFG_RECORDER_DATA_ATTRIBUTE __attribute__((section(".bss.trace_recorder_data")))
 *
 * Default value is empty.
 *****************************************************************************/
#define TRC_CFG_RECORDER_DATA_ATTRIBUTE 

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
