/*
 * Trace Recorder for Tracealyzer v4.10.2
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Tracealyzer support for Cs/OS3
 */

#ifndef TRC_KERNEL_PORT_H
#define TRC_KERNEL_PORT_H

#include <trcDefines.h>
#include <cpu_core.h>
#include <os_cfg_app.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_USE_TRACEALYZER_RECORDER (TRC_CFG_USE_TRACEALYZER_RECORDER) /* Allows for disabling the recorder */

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

extern uint32_t uiTraceTickCount;
extern volatile uint32_t uiTraceSystemState;

#undef TRC_CFG_ENABLE_STACK_MONITOR
#define TRC_CFG_ENABLE_STACK_MONITOR 0

/*** Don't change the below definitions, unless you know what you are doing! ***/

#define TRACE_KERNEL_VERSION 0x2AA2

#undef TRC_TICK_RATE_HZ
#undef TRC_HWTC_TYPE
#undef TRC_HWTC_COUNT
#undef TRC_HWTC_PERIOD
#undef TRC_HWTC_DIVISOR
#undef TRC_HWTC_FREQ_HZ

#define TRC_TICK_RATE_HZ    OS_CFG_TICK_RATE_HZ
#define TRC_HWTC_TYPE       TRC_FREE_RUNNING_32BIT_INCR
#define TRC_HWTC_COUNT      CPU_TS_TmrRd()
#define TRC_HWTC_PERIOD     1
#define TRC_HWTC_DIVISOR    1
#define TRC_HWTC_FREQ_HZ    CPU_TS_TmrFreq_Hz

/**
 * @def TRACE_CPU_CLOCK_HZ
 * @brief Trace CPU clock speed in Hz.
 */
#define TRACE_CPU_CLOCK_HZ TRC_CFG_CPU_CLOCK_HZ

#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC)
#include <stdlib.h> /* Include malloc() */

/**
 * @internal Kernel port specific heap initialization
 */
#define TRC_KERNEL_PORT_HEAP_INIT(size)

/**
 * @internal Kernel port specific heap malloc definition
 */
#define TRC_KERNEL_PORT_HEAP_MALLOC(size) malloc(size)
#endif

#define TRC_PLATFORM_CFG "Cs-OS3"
#define TRC_PLATFORM_CFG_MAJOR 3
#define TRC_PLATFORM_CFG_MINOR 9
#define TRC_PLATFORM_CFG_PATCH 4

#ifndef TRACE_ENTER_CRITICAL_SECTION
    #error "This hardware port has no definition for critical sections! See http://percepio.com/2014/10/27/how-to-define-critical-sections-for-the-recorder/"
#endif

#define TRC_KERNEL_PORT_ALLOC_CRITICAL_SECTION()    CPU_SR cpu_sr;
#define TRC_KERNEL_PORT_ENTER_CRITICAL_SECTION()    CPU_CRITICAL_ENTER()
#define TRC_KERNEL_PORT_EXIT_CRITICAL_SECTION()     CPU_CRITICAL_EXIT()

#define NOT_SET 1

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
#define TRC_KERNEL_PORT_BUFFER_SIZE (sizeof(TraceTaskHandle_t) * (TRC_CFG_CORE_COUNT))
#else
#define TRC_KERNEL_PORT_BUFFER_SIZE (sizeof(TraceUnsignedBaseType_t) * (TRC_CFG_CORE_COUNT))
#endif

/**
 * @internal The kernel port data buffer
 */
typedef struct TraceKernelPortDataBuffer    /* Aligned */
{
    uint8_t buffer[TRC_KERNEL_PORT_BUFFER_SIZE];
} TraceKernelPortDataBuffer_t;

/**
 * @internal Initializes the kernel port
 *
 * @param[in] pxBuffer Kernel port data buffer
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* const pxBuffer);

/**
 * @internal Enables the kernel port
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortEnable(void);

/**
 * @internal Not used for BareMetal
 *
 * @param[in] uiTicks Tick count to delay
 *
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceKernelPortDelay(uiTicks) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2((void)uiTicks, TRC_SUCCESS)

/**
 * @internal Query if scheduler is suspended. For Bare Metal systems this will always be false.
 *
 * @retval 1 Scheduler suspended
 * @retval 0 Scheduler not suspended
 */
#define xTraceKernelPortIsSchedulerSuspended() (0U)

/******************************************************************************/
/*** Definitions for Streaming mode *******************************************/
/******************************************************************************/
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

/*******************************************************************************
* vTraceStoreKernelObjectName
*
* Set the name for a kernel object (defined by its address).
******************************************************************************/
void vTraceStoreKernelObjectName(void* object, const char* name);

/*******************************************************************************
 * prvTraceOnBegin
 *
 * Called on trace begin.
 ******************************************************************************/
void prvTraceOnBegin(void);

/*******************************************************************************
 * prvTraceOnEnd
 *
 * Called on trace end.
 ******************************************************************************/
void prvTraceOnEnd(void);

/*************************************************************************/
/* KERNEL SPECIFIC OBJECT CONFIGURATION                                  */
/*************************************************************************/

/*******************************************************************************
 * The event codes - should match the offline config file.
 ******************************************************************************/

/*** Event codes for streaming - should match the Tracealyzer config file *****/
#define PSF_EVENT_NULL_EVENT                                0x000UL

#define PSF_EVENT_TRACE_START                               0x001UL
#define PSF_EVENT_TS_CONFIG                                 0x002UL /* Unused */
#define PSF_EVENT_OBJ_NAME                                  0x003UL
#define PSF_EVENT_TASK_PRIORITY                             0x004UL
#define PSF_EVENT_DEFINE_ISR                                0x005UL
#define PSF_EVENT_UNUSED_STACK                              0x006UL

#define PSF_EVENT_IFE_NEXT                                  0x008UL
#define PSF_EVENT_IFE_DIRECT                                0x009UL

#define PSF_EVENT_TASK_CREATE                               0x010UL
#define PSF_EVENT_TASK_DELETE                               0x011UL
#define PSF_EVENT_PROCESS_CREATE                            0x012UL /* Unused */
#define PSF_EVENT_PROCESS_DELETE                            0x013UL /* Unused */
#define PSF_EVENT_THREAD_CREATE                             0x014UL /* Unused */
#define PSF_EVENT_THREAD_DELETE                             0x015UL /* Unused */

#define PSF_EVENT_TASK_READY                                0x020UL
#define PSF_EVENT_ISR_BEGIN                                 0x021UL
#define PSF_EVENT_ISR_RESUME                                0x022UL
#define PSF_EVENT_TS_BEGIN                                  0x023UL /* Unused */
#define PSF_EVENT_TS_RESUME                                 0x024UL /* Unused */
#define PSF_EVENT_TASK_ACTIVATE                             0x025UL

#define PSF_EVENT_MALLOC                                    0x030UL
#define PSF_EVENT_FREE                                      0x031UL
#define PSF_EVENT_MALLOC_FAILED                             0x032UL
#define PSF_EVENT_FREE_FAILED                               0x033UL

#define PSF_EVENT_LOWPOWER_BEGIN                            0x038UL /* Unused */
#define PSF_EVENT_LOWPOWER_END                              0x039UL /* Unused */

#define PSF_EVENT_STATEMACHINE_STATE_CREATE                 0x040UL
#define PSF_EVENT_STATEMACHINE_CREATE                       0x041UL
#define PSF_EVENT_STATEMACHINE_STATECHANGE                  0x042UL

#define PSF_EVENT_INTERVAL_CHANNEL_CREATE                   0x043UL
#define PSF_EVENT_INTERVAL_START                            0x044UL
#define PSF_EVENT_INTERVAL_STOP                             0x045UL
#define PSF_EVENT_INTERVAL_CHANNEL_SET_CREATE               0x046UL

#define PSF_EVENT_EXTENSION_CREATE                          0x047UL

#define PSF_EVENT_HEAP_CREATE                               0x048UL

#define PSF_EVENT_COUNTER_CREATE                            0x049UL
#define PSF_EVENT_COUNTER_CHANGE                            0x04AUL
#define PSF_EVENT_COUNTER_LIMIT_EXCEEDED                    0x04BUL

#define PSF_EVENT_DEPENDENCY_REGISTER                       0x04CUL

#define PSF_EVENT_RUNNABLE_REGISTER                         0x04DUL
#define PSF_EVENT_RUNNABLE_START                            0x04EUL
#define PSF_EVENT_RUNNABLE_STOP                             0x04FUL

#define PSF_EVENT_USER_EVENT                                0x050UL

#define PSF_EVENT_USER_EVENT_FIXED                          0x058UL

#define PSF_EVENT_NEW_TIME                                  0x100UL
#define PSF_EVENT_NEW_TIME_SCHEDULER_SUSPENDED              0x101UL  /* Unused */

#define PSF_EVENT_DATA_CREATE                               0x102UL
#define PSF_EVENT_DATA_CREATE_FAILED                        0x103UL
#define PSF_EVENT_DATA_DELETE                               0x104UL
#define PSF_EVENT_DATA_DELETE_FAILED                        0x105UL
#define PSF_EVENT_DATA_POST                                 0x106UL
#define PSF_EVENT_DATA_POST_FAILED                          0x107UL
#define PSF_EVENT_DATA_POST_FRONT                           0x108UL
#define PSF_EVENT_DATA_POST_FRONT_FAILED                    0x109UL
#define PSF_EVENT_DATA_PEND                                 0x10AUL
#define PSF_EVENT_DATA_PEND_FAILED                          0x10BUL
#define PSF_EVENT_DATA_PEND_BLOCK                           0x10CUL
#define PSF_EVENT_DATA_FLUSH                                0x10DUL
#define PSF_EVENT_DATA_FLUSH_FAILED                         0x10EUL
#define PSF_EVENT_DATA_DEL_ENTER                            0x10FUL
#define PSF_EVENT_DATA_POST_ENTER                           0x110UL
#define PSF_EVENT_DATA_PEND_ENTER                           0x111UL
#define PSF_EVENT_DATA_DEL_EXIT                             0x112UL
#define PSF_EVENT_DATA_POST_EXIT                            0x113UL
#define PSF_EVENT_DATA_PEND_EXIT                            0x114UL

#define PSF_EVENT_FLAG_CREATE                               0x115UL
#define PSF_EVENT_FLAG_CREATE_FAILED                        0x116UL
#define PSF_EVENT_FLAG_DELETE                               0x117UL
#define PSF_EVENT_FLAG_DELETE_FAILED                        0x118UL
#define PSF_EVENT_FLAG_POST                                 0x119UL
#define PSF_EVENT_FLAG_POST_FAILED                          0x11AUL
#define PSF_EVENT_FLAG_PEND                                 0x11BUL
#define PSF_EVENT_FLAG_PEND_FAILED                          0x11CUL
#define PSF_EVENT_FLAG_PEND_BLOCK                           0x11DUL
#define PSF_EVENT_FLAG_DEL_ENTER                            0x11EUL
#define PSF_EVENT_FLAG_POST_ENTER                           0x11FUL
#define PSF_EVENT_FLAG_PEND_ENTER                           0x120UL
#define PSF_EVENT_FLAG_DEL_EXIT                             0x121UL
#define PSF_EVENT_FLAG_POST_EXIT                            0x122UL
#define PSF_EVENT_FLAG_PEND_EXIT                            0x123UL

#define PSF_EVENT_MEM_CREATE                                0x124UL
#define PSF_EVENT_MEM_CREATE_FAILED                         0x125UL
#define PSF_EVENT_MEM_PUT                                   0x126UL
#define PSF_EVENT_MEM_PUT_FAILED                            0x127UL
#define PSF_EVENT_MEM_GET                                   0x128UL
#define PSF_EVENT_MEM_GET_FAILED                            0x129UL
#define PSF_EVENT_MEM_PUT_ENTER                             0x12AUL
#define PSF_EVENT_MEM_GET_ENTER                             0x12BUL
#define PSF_EVENT_MEM_PUT_EXIT                              0x12CUL
#define PSF_EVENT_MEM_GET_EXIT                              0x12DUL

#define PSF_EVENT_MUTEX_CREATE                              0x12EUL
#define PSF_EVENT_MUTEX_CREATE_FAILED                       0x12FUL
#define PSF_EVENT_MUTEX_DELETE                              0x130UL
#define PSF_EVENT_MUTEX_DELETE_FAILED                       0x131UL
#define PSF_EVENT_MUTEX_POST                                0x132UL
#define PSF_EVENT_MUTEX_POST_FAILED                         0x133UL
#define PSF_EVENT_MUTEX_PEND                                0x134UL
#define PSF_EVENT_MUTEX_PEND_FAILED                         0x135UL
#define PSF_EVENT_MUTEX_PEND_BLOCK                          0x136UL
#define PSF_EVENT_MUTEX_DEL_ENTER                           0x137UL
#define PSF_EVENT_MUTEX_POST_ENTER                          0x138UL
#define PSF_EVENT_MUTEX_PEND_ENTER                          0x139UL
#define PSF_EVENT_MUTEX_DEL_EXIT                            0x13AUL
#define PSF_EVENT_MUTEX_POST_EXIT                           0x13BUL
#define PSF_EVENT_MUTEX_PEND_EXIT                           0x13CUL

#define PSF_EVENT_Q_CREATE                                  0x13DUL
#define PSF_EVENT_Q_CREATE_FAILED                           0x13EUL
#define PSF_EVENT_Q_DELETE                                  0x13FUL
#define PSF_EVENT_Q_DELETE_FAILED                           0x140UL
#define PSF_EVENT_Q_POST                                    0x141UL
#define PSF_EVENT_Q_POST_FAILED                             0x142UL
#define PSF_EVENT_Q_POST_FRONT                              0x143UL
#define PSF_EVENT_Q_POST_FRONT_FAILED                       0x144UL
#define PSF_EVENT_Q_PEND                                    0x145UL
#define PSF_EVENT_Q_PEND_FAILED                             0x146UL
#define PSF_EVENT_Q_PEND_BLOCK                              0x147UL
#define PSF_EVENT_Q_FLUSH                                   0x148UL
#define PSF_EVENT_Q_FLUSH_FAILED                            0x149UL
#define PSF_EVENT_Q_DEL_ENTER                               0x14AUL
#define PSF_EVENT_Q_POST_ENTER                              0x14BUL
#define PSF_EVENT_Q_PEND_ENTER                              0x14CUL
#define PSF_EVENT_Q_DEL_EXIT                                0x14DUL
#define PSF_EVENT_Q_POST_EXIT                               0x14EUL
#define PSF_EVENT_Q_PEND_EXIT                               0x14FUL

#define PSF_EVENT_SEM_CREATE                                0x150UL
#define PSF_EVENT_SEM_CREATE_FAILED                         0x151UL
#define PSF_EVENT_SEM_DELETE                                0x152UL
#define PSF_EVENT_SEM_DELETE_FAILED                         0x153UL
#define PSF_EVENT_SEM_POST                                  0x154UL
#define PSF_EVENT_SEM_POST_FAILED                           0x155UL
#define PSF_EVENT_SEM_PEND                                  0x156UL
#define PSF_EVENT_SEM_PEND_FAILED                           0x157UL
#define PSF_EVENT_SEM_PEND_BLOCK                            0x158UL
#define PSF_EVENT_SEM_SET                                   0x159UL
#define PSF_EVENT_SEM_SET_FAILED                            0x15AUL
#define PSF_EVENT_SEM_DEL_ENTER                             0x15BUL
#define PSF_EVENT_SEM_POST_ENTER                            0x15CUL
#define PSF_EVENT_SEM_PEND_ENTER                            0x15DUL
#define PSF_EVENT_SEM_DEL_EXIT                              0x15EUL
#define PSF_EVENT_SEM_POST_EXIT                             0x15FUL
#define PSF_EVENT_SEM_PEND_EXIT                             0x160UL

#define PSF_EVENT_TASK_PRIO_INHERIT                         0x161UL
#define PSF_EVENT_TASK_PRIO_DISINHERIT                      0x162UL
#define PSF_EVENT_TASK_CREATE_FAILED                        0x163UL
#define PSF_EVENT_TASK_DELAY                                0x164UL
#define PSF_EVENT_TASK_SUSPEND                              0x165UL
#define PSF_EVENT_TASK_SUSPEND_FAILED                       0x166UL
#define PSF_EVENT_TASK_RESUME                               0x167UL
#define PSF_EVENT_TASK_RESUME_FAILED                        0x168UL
#define PSF_EVENT_TASK_SUSPEND_ENTER                        0x169UL
#define PSF_EVENT_TASK_RESUME_ENTER                         0x16AUL
#define PSF_EVENT_TASK_SUSPEND_EXIT                         0x16BUL
#define PSF_EVENT_TASK_RESUME_EXIT                          0x16CUL

#define PSF_EVENT_TASK_Q_CREATE                             0x16DUL
#define PSF_EVENT_TASK_Q_DELETE                             0x16EUL
#define PSF_EVENT_TASK_Q_POST                               0x16FUL
#define PSF_EVENT_TASK_Q_POST_FAILED                        0x170UL
#define PSF_EVENT_TASK_Q_POST_FRONT                         0x171UL
#define PSF_EVENT_TASK_Q_POST_FRONT_FAILED                  0x172UL
#define PSF_EVENT_TASK_Q_PEND                               0x173UL
#define PSF_EVENT_TASK_Q_PEND_FAILED                        0x174UL
#define PSF_EVENT_TASK_Q_PEND_BLOCK                         0x175UL
#define PSF_EVENT_TASK_Q_FLUSH                              0x176UL
#define PSF_EVENT_TASK_Q_FLUSH_FAILED                       0x177UL
#define PSF_EVENT_TASK_Q_PEND_ENTER                         0x178UL
#define PSF_EVENT_TASK_Q_POST_ENTER                         0x179UL
#define PSF_EVENT_TASK_Q_PEND_EXIT                          0x17AUL
#define PSF_EVENT_TASK_Q_POST_EXIT                          0x17BUL

#define PSF_EVENT_TASK_SEM_CREATE                           0x17CUL
#define PSF_EVENT_TASK_SEM_DELETE                           0x17DUL
#define PSF_EVENT_TASK_SEM_POST                             0x17EUL
#define PSF_EVENT_TASK_SEM_POST_FAILED                      0x17FUL
#define PSF_EVENT_TASK_SEM_PEND                             0x180UL
#define PSF_EVENT_TASK_SEM_PEND_FAILED                      0x181UL
#define PSF_EVENT_TASK_SEM_PEND_BLOCK                       0x182UL
#define PSF_EVENT_TASK_SEM_SET                              0x183UL
#define PSF_EVENT_TASK_SEM_SET_FAILED                       0x184UL
#define PSF_EVENT_TASK_SEM_PEND_ENTER                       0x185UL
#define PSF_EVENT_TASK_SEM_POST_ENTER                       0x186UL
#define PSF_EVENT_TASK_SEM_PEND_EXIT                        0x187UL
#define PSF_EVENT_TASK_SEM_POST_EXIT                        0x188UL

#define PSF_EVENT_TMR_CREATE                                0x189UL
#define PSF_EVENT_TMR_CREATE_FAILED                         0x18AUL
#define PSF_EVENT_TMR_DELETE                                0x18BUL
#define PSF_EVENT_TMR_EXPIRED                               0x18CUL
#define PSF_EVENT_TMR_DEL_ENTER                             0x18DUL
#define PSF_EVENT_TMR_DEL_EXIT                              0x18EUL

#define TRC_EVENT_LAST_ID                                  (PSF_EVENT_TMR_DEL_EXIT)

/*** The trace macros for streaming ******************************************/
                                                                /* Trace Recorder Control Macros                        */
#undef OS_TRACE_INIT
#define OS_TRACE_INIT()                             xTraceInitialize()

#undef OS_TRACE_START
#define OS_TRACE_START()                            xTraceEnable(TRC_START)

#undef OS_TRACE_STOP
#define OS_TRACE_STOP()                             xTraceDisable()

#undef OS_TRACE_CLEAR
#define OS_TRACE_CLEAR()

                                                                /* ISR Trace Macros                                     */
#undef OS_TRACE_ISR_REGISTER
#define OS_TRACE_ISR_REGISTER(isr_name, isr_prio) \
    vTraceSetISRProperties(isr_name, isr_prio);

#undef OS_TRACE_ISR_BEGIN
#define OS_TRACE_ISR_BEGIN(isr_name) \
    vTraceStoreISRBegin(isr_name);

#undef OS_TRACE_ISR_END
#define OS_TRACE_ISR_END(flag) \
    vTraceStoreISREnd(flag);

#undef OS_TRACE_ISR_ENTER
#define OS_TRACE_ISR_ENTER()

#undef OS_TRACE_ISR_EXIT
#define OS_TRACE_ISR_EXIT()

#undef OS_TRACE_ISR_EXIT_TO_SCHEDULER
#define OS_TRACE_ISR_EXIT_TO_SCHEDULER()

                                                                /* Core Trace Macros                                    */
/* Called for each task that becomes ready */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_READY
#if (TRC_CFG_INCLUDE_READY_EVENTS == 1)
#define OS_TRACE_TASK_READY(p_tcb) \
    xTraceTaskReady(p_tcb);
#else /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */
#define OS_TRACE_TASK_READY(p_tcb)
#endif /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */

#undef OS_TRACE_TASK_PREEMPT
#define OS_TRACE_TASK_PREEMPT(p_tcb)

#undef OS_TRACE_TASK_NOT_READY
#define OS_TRACE_TASK_NOT_READY(p_tcb)

/* Called on each task-switch */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SWITCHED_IN
#define OS_TRACE_TASK_SWITCHED_IN(p_tcb) \
    xTraceTaskSwitch(p_tcb, p_tcb->Prio)

                                                                /* Data Queue Trace Macros                              */
/* Called from os_data.c. */
/* No need to protect critical section. */
#undef OS_TRACE_DATA_CREATE
#define OS_TRACE_DATA_CREATE(p_data, p_name) \
    xTraceObjectRegisterWithoutHandle(PSF_EVENT_DATA_CREATE, p_data, p_name, p_data->MaxEntries);

/* Called from os_data.c. */
/* No need to protect critical section. */
#undef OS_TRACE_DATA_CREATE_FAILED
#define OS_TRACE_DATA_CREATE_FAILED(p_data, max_entries) \
    xTraceEventCreate2(PSF_EVENT_DATA_CREATE_FAILED, (uint32_t)p_data, max_entries);

/* Called on OSDataDel */
/* No need to protect critical section. */
#undef OS_TRACE_DATA_DEL
#define OS_TRACE_DATA_DEL(p_data) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_DATA_DELETE, (void *)p_data, 0);

/* Called on OSDataDel */
/* No need to protect critical section. */
#undef OS_TRACE_DATA_DEL_FAILED
#define OS_TRACE_DATA_DEL_FAILED(p_data) \
    xTraceEventCreate1(PSF_EVENT_DATA_DELETE_FAILED, (uint32_t)p_data);

/* Called when a signal is sent to a data queue */
/* No need to protect critical section. */
#undef OS_TRACE_DATA_POST
#define OS_TRACE_DATA_POST(p_data, opt) \
    do { \
        if (opt & OS_OPT_POST_LIFO) { \
            xTraceEventCreate2(PSF_EVENT_DATA_POST_FRONT, (uint32_t)p_data, (uint8_t)(p_data->PendList.NbrEntries)); \
        } else { \
            xTraceEventCreate2(PSF_EVENT_DATA_POST, (uint32_t)p_data, (uint8_t)(p_data->PendList.NbrEntries)); \
        } \
    } while (0);

/* Called when a message failed to be sent to a data queue (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_DATA_POST_FAILED
#define OS_TRACE_DATA_POST_FAILED(p_data, opt) \
    do { \
        if (opt & OS_OPT_POST_LIFO) { \
            xTraceEventCreate1(PSF_EVENT_DATA_POST_FRONT_FAILED, (uint32_t)p_data); \
        } else { \
            xTraceEventCreate1(PSF_EVENT_DATA_POST_FAILED, (uint32_t)p_data); \
        } \
    } while (0);

/* Called when a message is received from a data queue */
/* No need to protect critical section. */
#undef OS_TRACE_DATA_PEND
#define OS_TRACE_DATA_PEND(p_data) \
    xTraceEventCreate2(PSF_EVENT_DATA_PEND, (uint32_t)p_data, (uint8_t)(p_data->PendList.NbrEntries));

/* Called when a message is received from a data queue */
/* No need to protect critical section. */
#undef OS_TRACE_DATA_PEND_FAILED
#define OS_TRACE_DATA_PEND_FAILED(p_data) \
    xTraceEventCreate1(PSF_EVENT_DATA_PEND_FAILED, (uint32_t)p_data);

/* Called when a message is received from a data queue */
/* No need to protect critical section. */
#undef OS_TRACE_DATA_PEND_BLOCK
#define OS_TRACE_DATA_PEND_BLOCK(p_data) \
    xTraceEventCreate2(PSF_EVENT_DATA_PEND_BLOCK, (uint32_t)p_data, (uint8_t)(p_data->PendList.NbrEntries));

/* Called when flushing a data queue */
/* No need to protect critical section. */
#undef OS_TRACE_DATA_FLUSH
#define OS_TRACE_DATA_FLUSH(p_data) \
    xTraceEventCreate1(PSF_EVENT_DATA_FLUSH, (uint32_t)p_data);

/* Called when flushing a data queue */
/* No need to protect critical section. */
#undef OS_TRACE_DATA_FLUSH_FAILED
#define OS_TRACE_DATA_FLUSH_FAILED(p_data) \
    xTraceEventCreate1(PSF_EVENT_DATA_FLUSH_FAILED, (uint32_t)p_data);

#if (OS_CFG_TRACE_API_ENTER_EN == 1u)
#undef OS_TRACE_DATA_DEL_ENTER
#define  OS_TRACE_DATA_DEL_ENTER(p_data, opt) \
    xTraceEventCreate2(PSF_EVENT_DATA_DEL_ENTER, (uint32_t)p_data, (uint32_t)opt);

#undef OS_TRACE_DATA_POST_ENTER
#define  OS_TRACE_DATA_POST_ENTER(p_data, p_void, msg_size, opt) \
    xTraceEventCreate4(PSF_EVENT_DATA_POST_ENTER, (uint32_t)p_data, (uint32_t)p_void, (uint32_t)msg_size, (uint32_t)opt);

#undef OS_TRACE_DATA_PEND_ENTER
#define  OS_TRACE_DATA_PEND_ENTER(p_data, timeout, opt, p_msg_size, item_size) \
    xTraceEventCreate5(PSF_EVENT_DATA_PEND_ENTER, (uint32_t)p_data, (uint32_t)timeout, (uint32_t)opt, (uint32_t)p_msg_size, (uint32_t)item_size);
#endif

#if (OS_CFG_TRACE_API_EXIT_EN == 1u)
#undef OS_TRACE_DATA_DEL_EXIT
#define  OS_TRACE_DATA_DEL_EXIT(p_data, err) \
    xTraceEventCreate2(PSF_EVENT_DATA_DEL_EXIT, (uint32_t)p_data, (uint32_t)err);

#undef OS_TRACE_DATA_POST_EXIT
#define  OS_TRACE_DATA_POST_EXIT(p_data, err) \
    xTraceEventCreate2(PSF_EVENT_DATA_POST_EXIT, (uint32_t)p_data, (uint32_t)err);

#undef OS_TRACE_DATA_PEND_EXIT
#define  OS_TRACE_DATA_PEND_EXIT(p_data, err) \
    xTraceEventCreate2(PSF_EVENT_DATA_PEND_EXIT, (uint32_t)p_data, (uint32_t)err);
#endif

                                                                /* Event Flag Trace Macros                              */
/* Called from OSFlagCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_CREATE
#define OS_TRACE_FLAG_CREATE(p_grp, p_name) \
    xTraceObjectRegisterWithoutHandle(PSF_EVENT_FLAG_CREATE, p_grp, p_name, p_grp->Flags);

/* Called from OSFlagCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_CREATE_FAILED
#define OS_TRACE_FLAG_CREATE_FAILED(p_grp, flags) \
    xTraceEventCreate2(PSF_EVENT_FLAG_CREATE_FAILED, (uint32_t)p_grp, flags);

/* Called on OSFlagDel */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_DEL
#define OS_TRACE_FLAG_DEL(p_grp) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_FLAG_DELETE, (void *)p_grp, 0);

/* Called on OSFlagDel */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_DEL_FAILED
#define OS_TRACE_FLAG_DEL_FAILED(p_grp) \
    xTraceEventCreate1(PSF_EVENT_FLAG_DELETE_FAILED, (uint32_t)p_grp);

/* Called when a signal is sent to a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_POST
#define OS_TRACE_FLAG_POST(p_grp) \
    switch (opt) \
    { \
        case OS_OPT_POST_FLAG_SET: \
        case OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED: \
        case OS_OPT_POST_FLAG_CLR: \
        case OS_OPT_POST_FLAG_CLR | OS_OPT_POST_NO_SCHED: \
            xTraceEventCreate1(PSF_EVENT_FLAG_POST, (uint32_t)p_grp); \
            break; \
        default: \
            xTraceEventCreate1(PSF_EVENT_FLAG_POST_FAILED, (uint32_t)p_grp); \
            break; \
    }

/* Called when a message failed to be sent to a flag group (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_POST_FAILED
#define OS_TRACE_FLAG_POST_FAILED(p_grp) \
    xTraceEventCreate1(PSF_EVENT_FLAG_POST_FAILED, (uint32_t)p_grp);

/* Called when a signal is received from a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_PEND
#define OS_TRACE_FLAG_PEND(p_grp) \
    xTraceEventCreate1(PSF_EVENT_FLAG_PEND, (uint32_t)p_grp);

/* Called when a signal is received from a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_PEND_FAILED
#define OS_TRACE_FLAG_PEND_FAILED(p_grp) \
    xTraceEventCreate1(PSF_EVENT_FLAG_PEND_FAILED, (uint32_t)p_grp);

/* Called when a signal is received from a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_PEND_BLOCK
#define OS_TRACE_FLAG_PEND_BLOCK(p_grp) \
    xTraceEventCreate1(PSF_EVENT_FLAG_PEND_BLOCK, (uint32_t)p_grp);

#if (OS_CFG_TRACE_API_ENTER_EN == 1u)
#undef OS_TRACE_FLAG_DEL_ENTER
#define  OS_TRACE_FLAG_DEL_ENTER(p_grp, opt) \
    xTraceEventCreate2(PSF_EVENT_FLAG_DEL_ENTER, (uint32_t)p_grp, (uint32_t)opt);

#undef OS_TRACE_FLAG_POST_ENTER
#define  OS_TRACE_FLAG_POST_ENTER(p_grp, flags, opt) \
    xTraceEventCreate3(PSF_EVENT_FLAG_POST_ENTER, (uint32_t)p_grp, (uint32_t)flags, (uint32_t)opt);

#undef OS_TRACE_FLAG_PEND_ENTER
#define  OS_TRACE_FLAG_PEND_ENTER(p_grp, flags, timeout, opt) \
    xTraceEventCreate4(PSF_EVENT_FLAG_PEND_ENTER, (uint32_t)p_grp, (uint32_t)flags, (uint32_t)timeout, (uint32_t)opt);
#endif

#if (OS_CFG_TRACE_API_EXIT_EN == 1u)
#undef OS_TRACE_FLAG_DEL_EXIT
#define  OS_TRACE_FLAG_DEL_EXIT(p_grp, err) \
    xTraceEventCreate2(PSF_EVENT_FLAG_DEL_EXIT, (uint32_t)p_grp, (uint32_t)err);

#undef OS_TRACE_FLAG_POST_EXIT
#define  OS_TRACE_FLAG_POST_EXIT(p_grp, err) \
    xTraceEventCreate2(PSF_EVENT_FLAG_POST_EXIT, (uint32_t)p_grp, (uint32_t)err);

#undef OS_TRACE_FLAG_PEND_EXIT
#define  OS_TRACE_FLAG_PEND_EXIT(p_grp, err) \
    xTraceEventCreate2(PSF_EVENT_FLAG_PEND_EXIT, (uint32_t)p_grp, (uint32_t)err);
#endif

                                                                /* Memory Partition Trace Macros                        */
/* Called from OSMemCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_CREATE
#define OS_TRACE_MEM_CREATE(p_mem, p_name) \
    xTraceObjectRegisterWithoutHandle(PSF_EVENT_MEM_CREATE, p_mem, p_name, (uint8_t)(p_mem->NbrMax));

/* Called from OSMemCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_CREATE_FAILED
#define OS_TRACE_MEM_CREATE_FAILED(p_mem, n_blks) \
    xTraceEventCreate2(PSF_EVENT_MEM_CREATE_FAILED, (uint32_t)p_mem, n_blks);

/* Called when a signal is sent to a memory partition */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_PUT
#define OS_TRACE_MEM_PUT(p_mem) \
    xTraceEventCreate1(PSF_EVENT_MEM_PUT, (uint32_t)p_mem);

/* Called when a message failed to be sent to a memory partition (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_PUT_FAILED
#define OS_TRACE_MEM_PUT_FAILED(p_mem) \
    xTraceEventCreate1(PSF_EVENT_MEM_PUT_FAILED, (uint32_t)p_mem);

/* Called when a signal is received from a memory partition */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_GET
#define OS_TRACE_MEM_GET(p_mem) \
    xTraceEventCreate1(PSF_EVENT_MEM_GET, (uint32_t)p_mem);

/* Called when a signal is received from a memory partition */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_GET_FAILED
#define OS_TRACE_MEM_GET_FAILED(p_mem) \
    xTraceEventCreate1(PSF_EVENT_MEM_GET_FAILED, (uint32_t)p_mem);

#if (OS_CFG_TRACE_API_ENTER_EN == 1u)
#undef OS_TRACE_MEM_PUT_ENTER
#define OS_TRACE_MEM_PUT_ENTER(p_mem, p_blk) \
    xTraceEventCreate2(PSF_EVENT_MEM_PUT_ENTER, (uint32_t)p_mem, (uint32_t)p_blk);

#undef OS_TRACE_MEM_GET_ENTER
#define OS_TRACE_MEM_GET_ENTER(p_mem) \
    xTraceEventCreate1(PSF_EVENT_MEM_GET_ENTER, (uint32_t)p_mem);
#endif

#if (OS_CFG_TRACE_API_EXIT_EN == 1u)
#undef OS_TRACE_MEM_PUT_EXIT
#define OS_TRACE_MEM_PUT_EXIT(p_mem, err) \
    xTraceEventCreate2(PSF_EVENT_MEM_PUT_EXIT, (uint32_t)p_mem, (uint32_t)err);

#undef OS_TRACE_MEM_GET_EXIT
#define OS_TRACE_MEM_GET_EXIT(p_mem, err) \
    xTraceEventCreate2(PSF_EVENT_MEM_GET_EXIT, (uint32_t)p_mem, (uint32_t)err);
#endif

                                                                /* Mutex Trace Macros                                   */
/* Called from OSMutexCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_CREATE
#define OS_TRACE_MUTEX_CREATE(p_mutex, p_name) \
    xTraceObjectRegisterWithoutHandle(PSF_EVENT_MUTEX_CREATE, p_mutex, p_name, 0);

/* Called from OSMutexCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_CREATE_FAILED
#define OS_TRACE_MUTEX_CREATE_FAILED(p_mutex) \
    xTraceEventCreate1(PSF_EVENT_MUTEX_CREATE_FAILED, (uint32_t)p_mutex);

/* Called on OSMutexDel */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_DEL
#define OS_TRACE_MUTEX_DEL(p_mutex) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_MUTEX_DELETE, (void *)p_mutex, 0);

/* Called on OSMutexDel */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_DEL_FAILED
#define OS_TRACE_MUTEX_DEL_FAILED(p_mutex) \
    xTraceEventCreate1(PSF_EVENT_MUTEX_DELETE_FAILED, (uint32_t)p_mutex);

/* Called when a signal is sent to a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_POST
#define OS_TRACE_MUTEX_POST(p_mutex) \
    xTraceEventCreate1(PSF_EVENT_MUTEX_POST, (uint32_t)p_mutex);

/* Called when a message failed to be sent to a mutex (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_POST_FAILED
#define OS_TRACE_MUTEX_POST_FAILED(p_mutex) \
    xTraceEventCreate1(PSF_EVENT_MUTEX_POST_FAILED, (uint32_t)p_mutex);

/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_PEND
#define OS_TRACE_MUTEX_PEND(p_mutex) \
    xTraceEventCreate1(PSF_EVENT_MUTEX_PEND, (uint32_t)p_mutex);

/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_PEND_FAILED
#define OS_TRACE_MUTEX_PEND_FAILED(p_mutex) \
    xTraceEventCreate1(PSF_EVENT_MUTEX_PEND_FAILED, (uint32_t)p_mutex);

/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_PEND_BLOCK
#define OS_TRACE_MUTEX_PEND_BLOCK(p_mutex) \
    xTraceEventCreate1(PSF_EVENT_MUTEX_PEND_BLOCK, (uint32_t)p_mutex);

/* Called in by mutex operations, when the task's priority is changed to the one from the mutex's owner */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_TASK_PRIO_INHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_INHERIT(p_tcb, prio) \
	xTraceEventCreate2(PSF_EVENT_TASK_PRIO_INHERIT, (uint32_t)p_tcb, prio);

/* Called in by mutex operations, when the task's priority is changed back to its original */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb, prio) \
	xTraceEventCreate2(PSF_EVENT_TASK_PRIO_DISINHERIT, (uint32_t)p_tcb, prio);

#if (OS_CFG_TRACE_API_ENTER_EN == 1u)
#undef OS_TRACE_MUTEX_DEL_ENTER
#define OS_TRACE_MUTEX_DEL_ENTER(p_mutex, opt) \
	xTraceEventCreate2(PSF_EVENT_MUTEX_DEL_ENTER, (uint32_t)p_mutex, (uint32_t)opt);

#undef OS_TRACE_MUTEX_POST_ENTER
#define OS_TRACE_MUTEX_POST_ENTER(p_mutex, opt) \
	xTraceEventCreate2(PSF_EVENT_MUTEX_POST_ENTER, (uint32_t)p_mutex, (uint32_t)opt);

#undef OS_TRACE_MUTEX_PEND_ENTER
#define OS_TRACE_MUTEX_PEND_ENTER(p_mutex, timeout, opt) \
	xTraceEventCreate3(PSF_EVENT_MUTEX_PEND_ENTER, (uint32_t)p_mutex, (uint32_t)timeout, (uint32_t)opt);
#endif

#if (OS_CFG_TRACE_API_EXIT_EN == 1u)
#undef OS_TRACE_MUTEX_DEL_EXIT
#define OS_TRACE_MUTEX_DEL_EXIT(p_mutex, err) \
	xTraceEventCreate2(PSF_EVENT_MUTEX_DEL_EXIT, (uint32_t)p_mutex, (uint32_t)err);

#undef OS_TRACE_MUTEX_POST_EXIT
#define OS_TRACE_MUTEX_POST_EXIT(p_mutex, err) \
	xTraceEventCreate2(PSF_EVENT_MUTEX_POST_EXIT, (uint32_t)p_mutex, (uint32_t)err);

#undef OS_TRACE_MUTEX_PEND_EXIT
#define OS_TRACE_MUTEX_PEND_EXIT(p_mutex, err) \
	xTraceEventCreate2(PSF_EVENT_MUTEX_PEND_EXIT, (uint32_t)p_mutex, (uint32_t)err);
#endif

                                                                /* Message Queue Trace Macros                           */
/* Called from os_q.c. */
/* No need to protect critical section. */
#undef OS_TRACE_Q_CREATE
#define OS_TRACE_Q_CREATE(p_q, p_name) \
    xTraceObjectRegisterWithoutHandle(PSF_EVENT_Q_CREATE, p_q, p_name, p_q->MsgQ.NbrEntriesSize);

/* Called from os_q.c. */
/* No need to protect critical section. */
#undef OS_TRACE_Q_CREATE_FAILED
#define OS_TRACE_Q_CREATE_FAILED(p_q, max_qty) \
    xTraceEventCreate2(PSF_EVENT_Q_CREATE_FAILED, (uint32_t)p_q, max_qty);

/* Called on OSQDel */
/* No need to protect critical section. */
#undef OS_TRACE_Q_DEL
#define OS_TRACE_Q_DEL(p_q) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_Q_DELETE, (void *)p_q, 0);

/* Called on OSQDel */
/* No need to protect critical section. */
#undef OS_TRACE_Q_DEL_FAILED
#define OS_TRACE_Q_DEL_FAILED(p_q) \
    xTraceEventCreate2(PSF_EVENT_Q_DELETE_FAILED, (uint32_t)p_q, 0);

/* Called when a signal is sent to a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_POST
#define OS_TRACE_Q_POST(p_q, opt, err)  \
    do { \
        if (opt & OS_OPT_POST_LIFO) { \
            if (err == OS_ERR_NONE) { \
                xTraceEventCreate2(PSF_EVENT_Q_POST_FRONT, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries)); \
            } else { \
                xTraceEventCreate2(PSF_EVENT_Q_POST_FRONT_FAILED, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries)); \
            } \
        } else { \
            if (err == OS_ERR_NONE) { \
                xTraceEventCreate2(PSF_EVENT_Q_POST, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries)); \
            } else { \
                xTraceEventCreate2(PSF_EVENT_Q_POST_FAILED, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries)); \
            } \
        } \
    } while (0);

/* Called when a message failed to be sent to a queue (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_Q_POST_FAILED
#define OS_TRACE_Q_POST_FAILED(p_q, opt) \
    do { \
        if (opt & OS_OPT_POST_LIFO) { \
            xTraceEventCreate1(PSF_EVENT_Q_POST_FRONT_FAILED, (uint32_t)p_q); \
        } else { \
            xTraceEventCreate1(PSF_EVENT_Q_POST_FAILED, (uint32_t)p_q); \
        } \
    } while (0);

/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_PEND
#define OS_TRACE_Q_PEND(p_q) \
    xTraceEventCreate2(PSF_EVENT_Q_PEND, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries));

/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_PEND_FAILED
#define OS_TRACE_Q_PEND_FAILED(p_q) \
    xTraceEventCreate1(PSF_EVENT_Q_PEND_FAILED, (uint32_t)p_q);

/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_PEND_BLOCK
#define OS_TRACE_Q_PEND_BLOCK(p_q) \
    xTraceEventCreate2(PSF_EVENT_Q_PEND_BLOCK, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries));

/* Called when flushing a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_FLUSH
#define OS_TRACE_Q_FLUSH(p_q) \
    xTraceEventCreate1(PSF_EVENT_Q_FLUSH, (uint32_t)p_q);

/* Called when flushing a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_FLUSH_FAILED
#define OS_TRACE_Q_FLUSH_FAILED(p_q) \
    xTraceEventCreate1(PSF_EVENT_Q_FLUSH_FAILED, (uint32_t)p_q);

#if (OS_CFG_TRACE_API_ENTER_EN == 1u)
#undef OS_TRACE_Q_DEL_ENTER
#define OS_TRACE_Q_DEL_ENTER(p_q, opt) \
	xTraceEventCreate2(PSF_EVENT_Q_DEL_ENTER, (uint32_t)p_q, (uint32_t)opt);

#undef OS_TRACE_Q_POST_ENTER
#define OS_TRACE_Q_POST_ENTER(p_q, p_void, msg_size, opt) \
	xTraceEventCreate4(PSF_EVENT_Q_POST_ENTER, (uint32_t)p_q,  (uint32_t)p_void,  (uint32_t)msg_size, (uint32_t)opt);

#undef OS_TRACE_Q_PEND_ENTER
#define OS_TRACE_Q_PEND_ENTER(p_q, timeout, opt, p_msg_size) \
	xTraceEventCreate4(PSF_EVENT_Q_PEND_ENTER, (uint32_t)p_q, (uint32_t)timeout, (uint32_t)opt, (uint32_t)p_msg_size);
#endif

#if (OS_CFG_TRACE_API_EXIT_EN == 1u)
#undef OS_TRACE_Q_DEL_EXIT
#define OS_TRACE_Q_DEL_EXIT(p_q, err) \
	xTraceEventCreate2(PSF_EVENT_Q_DEL_EXIT, (uint32_t)p_q, (uint32_t)err);

#undef OS_TRACE_Q_POST_EXIT
#define OS_TRACE_Q_POST_EXIT(p_q, err) \
	xTraceEventCreate2(PSF_EVENT_Q_POST_EXIT, (uint32_t)p_q, (uint32_t)err);

#undef OS_TRACE_Q_PEND_EXIT
#define OS_TRACE_Q_PEND_EXIT(p_q, err) \
	xTraceEventCreate2(PSF_EVENT_Q_PEND_EXIT, (uint32_t)p_q, (uint32_t)err);
#endif

                                                                /* Semaphore Trace Macros                               */
/* Called from OSSemCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_CREATE
#define OS_TRACE_SEM_CREATE(p_sem, p_name) \
    xTraceObjectRegisterWithoutHandle(PSF_EVENT_SEM_CREATE, p_sem, p_name, p_sem->Ctr);

/* Called from OSSemCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_CREATE_FAILED
#define OS_TRACE_SEM_CREATE_FAILED(p_sem, cnt) \
    xTraceEventCreate2(PSF_EVENT_SEM_CREATE_FAILED, (uint32_t)p_sem, cnt);

/* Called on OSSemDel */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_DEL
#define OS_TRACE_SEM_DEL(p_sem) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_SEM_DELETE, (void *)p_sem, 0);

/* Called from OSSemCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_DEL_FAILED
#define OS_TRACE_SEM_DEL_FAILED(p_sem) \
    xTraceEventCreate1(PSF_EVENT_SEM_DELETE_FAILED, (uint32_t)p_sem);

/* Called when a signal is sent to a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_POST
#define OS_TRACE_SEM_POST(p_sem) \
    if (p_sem->Ctr == (OS_SEM_CTR)-1) \
    { \
        xTraceEventCreate1(PSF_EVENT_SEM_POST_FAILED, (uint32_t)p_sem); \
    } \
    else \
    { \
        xTraceEventCreate2(PSF_EVENT_SEM_POST, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr)); \
    }

/* Called when a signal failed to be sent to a semaphore (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_POST_FAILED
#define OS_TRACE_SEM_POST_FAILED(p_tcb) \
    xTraceEventCreate1(PSF_EVENT_SEM_POST_FAILED, (uint32_t)p_sem);

/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_PEND
#define OS_TRACE_SEM_PEND(p_sem) \
    xTraceEventCreate2(PSF_EVENT_SEM_PEND, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr));

/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_PEND_FAILED
#define OS_TRACE_SEM_PEND_FAILED(p_sem) \
    xTraceEventCreate1(PSF_EVENT_SEM_PEND_FAILED, (uint32_t)p_sem);

/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_PEND_BLOCK
#define OS_TRACE_SEM_PEND_BLOCK(p_sem) \
    xTraceEventCreate2(PSF_EVENT_SEM_PEND_BLOCK, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr));

/* Called setting the semaphore count */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_SET
#define OS_TRACE_SEM_SET(p_sem) \
    xTraceEventCreate2(PSF_EVENT_SEM_SET, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr));

/* Called setting the semaphore count */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_SET_FAILED
#define OS_TRACE_SEM_SET_FAILED(p_sem, cnt) \
    xTraceEventCreate2(PSF_EVENT_SEM_SET_FAILED, (uint32_t)p_sem, cnt);

#if (OS_CFG_TRACE_API_ENTER_EN == 1u)
#undef OS_TRACE_SEM_DEL_ENTER
#define OS_TRACE_SEM_DEL_ENTER(p_sem, opt) \
	xTraceEventCreate2(PSF_EVENT_SEM_DEL_ENTER, (uint32_t)p_sem, (uint32_t)opt);

#undef OS_TRACE_SEM_POST_ENTER
#define OS_TRACE_SEM_POST_ENTER(p_sem, opt) \
	xTraceEventCreate2(PSF_EVENT_SEM_POST_ENTER, (uint32_t)p_sem, (uint32_t)opt);

#undef OS_TRACE_SEM_PEND_ENTER
#define OS_TRACE_SEM_PEND_ENTER(p_sem, timeout, opt) \
	xTraceEventCreate3(PSF_EVENT_SEM_PEND_ENTER, (uint32_t)p_sem, (uint32_t)timeout, (uint32_t)opt);
#endif

#if (OS_CFG_TRACE_API_EXIT_EN == 1u)
#undef OS_TRACE_SEM_DEL_EXIT
#define OS_TRACE_SEM_DEL_EXIT(p_sem, err) \
	xTraceEventCreate2(PSF_EVENT_SEM_DEL_EXIT, (uint32_t)p_sem, (uint32_t)err);

#undef OS_TRACE_SEM_POST_EXIT
#define OS_TRACE_SEM_POST_EXIT(p_sem, err) \
	xTraceEventCreate2(PSF_EVENT_SEM_POST_EXIT, (uint32_t)p_sem, (uint32_t)err);

#undef OS_TRACE_SEM_PEND_EXIT
#define OS_TRACE_SEM_PEND_EXIT(p_sem, err) \
	xTraceEventCreate2(PSF_EVENT_SEM_PEND_EXIT, (uint32_t)p_sem, (uint32_t)err);
#endif

                                                                /* Task Management Trace Macros                         */
/* Called in OSTaskCreate */
/* Need to protect critical section without reallocating the SR.                     */
#undef OS_TRACE_TASK_CREATE
#define OS_TRACE_TASK_CREATE(p_tcb) \
    do { \
        TRACE_ENTER_CRITICAL_SECTION(); \
        xTraceTaskRegisterWithoutHandle((void *)(p_tcb), (p_tcb)->NamePtr, (p_tcb)->Prio); \
        TRACE_EXIT_CRITICAL_SECTION(); \
    }  while (0);

/* Called in OSTaskCreate, if it fails (typically if the stack can not be allocated) */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_CREATE_FAILED
#define OS_TRACE_TASK_CREATE_FAILED(p_tcb, prio) \
    xTraceEventCreate2(PSF_EVENT_TASK_CREATE_FAILED, (uint32_t)p_tcb, prio);

/* Called on OSTaskDel */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_DEL
#define OS_TRACE_TASK_DEL(p_tcb) \
    (void)xTraceTaskUnregisterWithoutHandle(p_tcb, (p_tcb)->Prio)

/* Called on OSTimeDly */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_DLY
#define OS_TRACE_TASK_DLY(p_tcb, dly_ticks) \
    xTraceEventCreate2(PSF_EVENT_TASK_DELAY, (uint32_t)p_tcb, dly_ticks);

/* Called on OSTaskSuspend() */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SUSPEND
#define OS_TRACE_TASK_SUSPEND(p_tcb) \
    xTraceEventCreate1(PSF_EVENT_TASK_SUSPEND, (uint32_t)p_tcb);

/* Called on OSTaskSuspend() */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SUSPEND_FAILED
#define OS_TRACE_TASK_SUSPEND_FAILED(p_tcb) \
    if (p_tcb == 0) { \
        xTraceEventCreate1(PSF_EVENT_TASK_SUSPEND_FAILED, (uint32_t)OSTCBCurPtr); \
    } else { \
        xTraceEventCreate1(PSF_EVENT_TASK_SUSPEND_FAILED, (uint32_t)p_tcb); \
    }

/* Called on OSTaskResume() */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_RESUME
#define OS_TRACE_TASK_RESUME(p_tcb) \
    xTraceEventCreate1(PSF_EVENT_TASK_RESUME, (uint32_t)p_tcb);

/* Called on OSTaskResume() */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_RESUME_FAILED
#define OS_TRACE_TASK_RESUME_FAILED(p_tcb) \
    xTraceEventCreate1(PSF_EVENT_TASK_RESUME_FAILED, (uint32_t)p_tcb);

/* Called in OSTaskChangePrio */
#undef OS_TRACE_TASK_PRIO_CHANGE
#define OS_TRACE_TASK_PRIO_CHANGE(p_tcb, prio) \
    xTraceTaskSetPriorityWithoutHandle(p_tcb, prio);

/* Called in OSTaskCreate, while it creates the built-in message queue */
#undef OS_TRACE_TASK_Q_CREATE
#define OS_TRACE_TASK_Q_CREATE(p_tcb, p_name) \
    do { \
        TRACE_ENTER_CRITICAL_SECTION(); \
        if (p_tcb) \
        { \
            xTraceEventCreate2(PSF_EVENT_TASK_Q_CREATE, (uint32_t)&p_tcb->MsgQ, (uint32_t)p_tcb->MsgQ.NbrEntries); \
            xTraceObjectSetNameWithoutHandle(&p_tcb->MsgQ, p_name); \
        } \
        TRACE_EXIT_CRITICAL_SECTION(); \
    } while (0);

/* Called on OSTaskDel */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_Q_DEL
#define OS_TRACE_TASK_Q_DEL(p_tcb) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_TASK_Q_DELETE, (void *)&p_tcb->MsgQ, (uint32_t)p_tcb->MsgQ.NbrEntries);

/* Called when a message is sent to a task's queue */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_Q_POST
#define OS_TRACE_TASK_Q_POST(p_tcb, opt, err) \
    do { \
        if (opt & OS_OPT_POST_LIFO) { \
            if (err == OS_ERR_NONE) { \
                xTraceEventCreate2(PSF_EVENT_TASK_Q_POST_FRONT, (uint32_t)&p_tcb->MsgQ, (uint32_t)p_tcb->MsgQ.NbrEntries); \
            } else { \
                xTraceEventCreate1(PSF_EVENT_TASK_Q_POST_FRONT_FAILED, (uint32_t)&p_tcb->MsgQ); \
            } \
        } else { \
            if (err == OS_ERR_NONE) { \
                xTraceEventCreate2(PSF_EVENT_TASK_Q_POST, (uint32_t)&p_tcb->MsgQ, (uint32_t)p_tcb->MsgQ.NbrEntries); \
            } else { \
                xTraceEventCreate1(PSF_EVENT_TASK_Q_POST_FAILED, (uint32_t)&p_tcb->MsgQ); \
            } \
        } \
    } while (0);

/* Called when a message failed to be sent to a task's queue (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_Q_POST_FAILED
#define OS_TRACE_TASK_Q_POST_FAILED(p_tcb, opt) \
    do { \
        if (opt & OS_OPT_POST_LIFO) { \
            if (p_tcb == 0) { \
                xTraceEventCreate1(PSF_EVENT_TASK_Q_POST_FRONT_FAILED, (uint32_t)&OSTCBCurPtr->MsgQ); \
            } else { \
                xTraceEventCreate1(PSF_EVENT_TASK_Q_POST_FRONT_FAILED, (uint32_t)&p_tcb->MsgQ); \
            } \
        } else { \
            if (p_tcb == 0) { \
                xTraceEventCreate1(PSF_EVENT_TASK_Q_POST_FAILED, (uint32_t)&OSTCBCurPtr->MsgQ); \
            } else { \
                xTraceEventCreate1(PSF_EVENT_TASK_Q_POST_FAILED, (uint32_t)&p_tcb->MsgQ); \
            } \
        } \
    } while (0);

/* Called when a message is received from a task message queue */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_Q_PEND
#define OS_TRACE_TASK_Q_PEND(p_tcb) \
    xTraceEventCreate2(PSF_EVENT_TASK_Q_PEND, (uint32_t)&p_tcb->MsgQ, (uint8_t)p_tcb->MsgQ.NbrEntries);

/* Called when a message is received from a task message queue */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_Q_PEND_FAILED
#define OS_TRACE_TASK_Q_PEND_FAILED(p_tcb) \
    xTraceEventCreate1(PSF_EVENT_TASK_Q_PEND_FAILED, (uint32_t)&p_tcb->MsgQ);

/* Called when a message is received from a task message queue */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_Q_PEND_BLOCK
#define OS_TRACE_TASK_Q_PEND_BLOCK(p_tcb) \
    xTraceEventCreate2(PSF_EVENT_TASK_Q_PEND_BLOCK, (uint32_t)&p_tcb->MsgQ, (uint8_t)p_tcb->MsgQ.NbrEntries);

/* Called when flushing a task message queue */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_Q_FLUSH
#define OS_TRACE_TASK_Q_FLUSH(p_tcb) \
    xTraceEventCreate1(PSF_EVENT_TASK_Q_FLUSH, (uint32_t)&p_tcb->MsgQ);

/* Called when flushing a task message queue */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_Q_FLUSH_FAILED
#define OS_TRACE_TASK_Q_FLUSH_FAILED(p_tcb) \
    if (p_tcb == 0) { \
        xTraceEventCreate1(PSF_EVENT_TASK_Q_FLUSH_FAILED, (uint32_t)&OSTCBCurPtr->MsgQ); \
    } else { \
        xTraceEventCreate1(PSF_EVENT_TASK_Q_FLUSH_FAILED, (uint32_t)&p_tcb->MsgQ); \
    }

/* Called in OSTaskCreate, while it creates the built-in semaphore */
/* Need to protect critical section without reallocating the SR.       */
#undef OS_TRACE_TASK_SEM_CREATE
#define OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name) \
    do { \
        TRACE_ENTER_CRITICAL_SECTION(); \
        if (p_tcb) \
        { \
            xTraceEventCreate2(PSF_EVENT_TASK_SEM_CREATE, (uint32_t)(&p_tcb->SemCtr), p_tcb->SemCtr); \
            xTraceObjectSetNameWithoutHandle(&p_tcb->SemCtr, p_name); \
        } \
        TRACE_EXIT_CRITICAL_SECTION(); \
    } while (0);

/* Called on OSTaskDel */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_DEL
#define OS_TRACE_TASK_SEM_DEL(p_tcb) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_TASK_SEM_DELETE, (void *)(&p_tcb->SemCtr), (uint32_t)(p_tcb->SemCtr));

/* Called when a message is sent to a task's semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_POST
#define OS_TRACE_TASK_SEM_POST(p_tcb) \
    xTraceEventCreate2(PSF_EVENT_TASK_SEM_POST, (uint32_t)(&p_tcb->SemCtr), (uint8_t)(p_tcb->SemCtr));

/* Called when a signal failed to be sent to a task's semaphore (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_POST_FAILED
#define OS_TRACE_TASK_SEM_POST_FAILED(p_tcb) \
    if (p_tcb == 0) { \
        xTraceEventCreate1(PSF_EVENT_TASK_SEM_POST_FAILED, (uint32_t)(&OSTCBCurPtr->SemCtr)); \
    } else { \
        xTraceEventCreate1(PSF_EVENT_TASK_SEM_POST_FAILED, (uint32_t)(&p_tcb->SemCtr)); \
    }

/* Called when a message is received from a task semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_PEND
#define OS_TRACE_TASK_SEM_PEND(p_tcb) \
    xTraceEventCreate2(PSF_EVENT_TASK_SEM_PEND, (uint32_t)(&p_tcb->SemCtr), (uint8_t)(p_tcb->SemCtr));

/* Called when a message is received from a task semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_PEND_FAILED
#define OS_TRACE_TASK_SEM_PEND_FAILED(p_tcb) \
    xTraceEventCreate1(PSF_EVENT_TASK_SEM_PEND_FAILED, (uint32_t)(&p_tcb->SemCtr));

/* Called when a message is received from a task semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_PEND_BLOCK
#define OS_TRACE_TASK_SEM_PEND_BLOCK(p_tcb) \
    xTraceEventCreate2(PSF_EVENT_TASK_SEM_PEND_BLOCK, (uint32_t)(&p_tcb->SemCtr), (uint8_t)(p_tcb->SemCtr));

/* Called when setting a Task Sem's count */
#undef OS_TRACE_TASK_SEM_SET
#define OS_TRACE_TASK_SEM_SET(p_tcb) \
    xTraceEventCreate2(PSF_EVENT_TASK_SEM_SET, (uint32_t)(&p_tcb->SemCtr), (uint8_t)(p_tcb->SemCtr));

/* Called when setting a Task Sem's count */
#undef OS_TRACE_TASK_SEM_SET_FAILED
#define OS_TRACE_TASK_SEM_SET_FAILED(p_tcb, cnt) \
    if (p_tcb == 0) { \
        xTraceEventCreate2(PSF_EVENT_TASK_SEM_SET_FAILED, (uint32_t)(&OSTCBCurPtr->SemCtr), cnt); \
    } else { \
        xTraceEventCreate2(PSF_EVENT_TASK_SEM_SET_FAILED, (uint32_t)(&p_tcb->SemCtr), cnt); \
    }

#if (OS_CFG_TRACE_API_ENTER_EN == 1u)
#undef OS_TRACE_TASK_Q_PEND_ENTER
#define OS_TRACE_TASK_Q_PEND_ENTER(p_tcb, timeout, opt, p_msg_size) \
	xTraceEventCreate4(PSF_EVENT_TASK_Q_PEND_ENTER, (uint32_t)&p_tcb->MsgQ, (uint32_t)timeout, (uint32_t)opt, (uint32_t)p_msg_size);

#undef OS_TRACE_TASK_Q_POST_ENTER
#define OS_TRACE_TASK_Q_POST_ENTER(p_tcb, p_void, msg_size, opt) \
    if (p_tcb == 0) { \
        xTraceEventCreate4(PSF_EVENT_TASK_Q_POST_ENTER, (uint32_t)&OSTCBCurPtr->MsgQ, (uint32_t)p_void, (uint32_t)msg_size, (uint32_t)opt); \
    } else { \
        xTraceEventCreate4(PSF_EVENT_TASK_Q_POST_ENTER, (uint32_t)&p_tcb->MsgQ, (uint32_t)p_void, (uint32_t)msg_size, (uint32_t)opt); \
    }

#undef OS_TRACE_TASK_SEM_PEND_ENTER
#define OS_TRACE_TASK_SEM_PEND_ENTER(p_tcb, timeout, opt) \
	xTraceEventCreate3(PSF_EVENT_TASK_SEM_PEND_ENTER, (uint32_t)(&p_tcb->SemCtr), (uint32_t)timeout, (uint32_t)opt);

#undef OS_TRACE_TASK_SEM_POST_ENTER
#define OS_TRACE_TASK_SEM_POST_ENTER(p_tcb, opt) \
    if (p_tcb == 0) { \
        xTraceEventCreate2(PSF_EVENT_TASK_SEM_POST_ENTER, (uint32_t)(&OSTCBCurPtr->SemCtr), (uint32_t)opt); \
    } else { \
        xTraceEventCreate2(PSF_EVENT_TASK_SEM_POST_ENTER, (uint32_t)(&p_tcb->SemCtr), (uint32_t)opt); \
    }

#undef OS_TRACE_TASK_SUSPEND_ENTER
#define OS_TRACE_TASK_SUSPEND_ENTER(p_tcb) \
    if (p_tcb == 0) { \
        xTraceEventCreate1(PSF_EVENT_TASK_SUSPEND_ENTER, (uint32_t)OSTCBCurPtr); \
    } else { \
        xTraceEventCreate1(PSF_EVENT_TASK_SUSPEND_ENTER, (uint32_t)p_tcb); \
    }

#undef OS_TRACE_TASK_RESUME_ENTER
#define OS_TRACE_TASK_RESUME_ENTER(p_tcb) \
	xTraceEventCreate1(PSF_EVENT_TASK_RESUME_ENTER, (uint32_t)p_tcb);
#endif

#if (OS_CFG_TRACE_API_EXIT_EN == 1u)
#undef OS_TRACE_TASK_Q_PEND_EXIT
#define OS_TRACE_TASK_Q_PEND_EXIT(p_tcb, err) \
    xTraceEventCreate2(PSF_EVENT_TASK_Q_PEND_EXIT, (uint32_t)&p_tcb->MsgQ, (uint32_t)err);

#undef OS_TRACE_TASK_Q_POST_EXIT
#define OS_TRACE_TASK_Q_POST_EXIT(p_tcb, err) \
    if (p_tcb == 0) { \
        xTraceEventCreate2(PSF_EVENT_TASK_Q_POST_EXIT, (uint32_t)&OSTCBCurPtr->MsgQ, (uint32_t)err); \
    } else { \
        xTraceEventCreate2(PSF_EVENT_TASK_Q_POST_EXIT, (uint32_t)&p_tcb->MsgQ, (uint32_t)err); \
    }

#undef OS_TRACE_TASK_SEM_PEND_EXIT
#define OS_TRACE_TASK_SEM_PEND_EXIT(p_tcb, err) \
	xTraceEventCreate2(PSF_EVENT_TASK_SEM_PEND_EXIT, (uint32_t)(&p_tcb->SemCtr), (uint32_t)err);

#undef OS_TRACE_TASK_SEM_POST_EXIT
#define OS_TRACE_TASK_SEM_POST_EXIT(p_tcb, err) \
    if (p_tcb == 0) { \
        xTraceEventCreate2(PSF_EVENT_TASK_SEM_POST_EXIT, (uint32_t)(&OSTCBCurPtr->SemCtr), (uint32_t)err); \
    } else { \
        xTraceEventCreate2(PSF_EVENT_TASK_SEM_POST_EXIT, (uint32_t)(&p_tcb->SemCtr), (uint32_t)err); \
    }

#undef OS_TRACE_TASK_SUSPEND_EXIT
#define OS_TRACE_TASK_SUSPEND_EXIT(p_tcb, err) \
    if (p_tcb == 0) { \
        xTraceEventCreate2(PSF_EVENT_TASK_SUSPEND_EXIT, (uint32_t)OSTCBCurPtr, (uint32_t)err); \
    } else { \
        xTraceEventCreate2(PSF_EVENT_TASK_SUSPEND_EXIT, (uint32_t)p_tcb, (uint32_t)err); \
    }

#undef OS_TRACE_TASK_RESUME_EXIT
#define OS_TRACE_TASK_RESUME_EXIT(p_tcb, err) \
    xTraceEventCreate2(PSF_EVENT_TASK_RESUME_EXIT, (uint32_t)p_tcb, (uint32_t)err);
#endif

                                                                /* Tick Trace Macros                                     */
/* No need to protect critical section. */
#undef OS_TRACE_TICK_INCREMENT
#if (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1)
#define OS_TRACE_TICK_INCREMENT(ctr) \
    xTraceEventCreate1(PSF_EVENT_NEW_TIME, ctr);
#else /* (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1) */
#define OS_TRACE_TICK_INCREMENT(ctr)
#endif /* (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1) */

                                                                /* Tmr Trace Macros                                     */
#undef OS_TRACE_TMR_CREATE
#define OS_TRACE_TMR_CREATE(p_tmr, p_name, dly, period) \
    xTraceObjectRegisterWithoutHandle2(PSF_EVENT_TMR_CREATE, p_tmr, p_name, dly, period);

#undef  OS_TRACE_TMR_CREATE_FAILED
#define  OS_TRACE_TMR_CREATE_FAILED(p_tmr, dly, period) \
    xTraceEventCreate3(PSF_EVENT_TMR_CREATE_FAILED, (uint32_t)p_tmr, dly, period);

#undef OS_TRACE_TMR_DEL
#define OS_TRACE_TMR_DEL(p_tmr) \
    xTraceObjectUnregisterWithoutHandle(PSF_EVENT_TMR_DELETE, (void *)p_tmr, 0);

#undef OS_TRACE_TMR_EXPIRED
#define  OS_TRACE_TMR_EXPIRED(p_tmr) \
    xTraceEventCreate1(PSF_EVENT_TMR_EXPIRED, (uint32_t)p_tmr);

#undef OS_TRACE_TMR_DEL_ENTER
#define OS_TRACE_TMR_DEL_ENTER(p_tmr) \
    xTraceEventCreate1(PSF_EVENT_TMR_DEL_ENTER, (uint32_t)p_tmr);

#undef OS_TRACE_TMR_DEL_EXIT
#define OS_TRACE_TMR_DEL_EXIT(p_tmr, err) \
    xTraceEventCreate2(PSF_EVENT_TMR_DEL_EXIT, (uint32_t)p_tmr, (uint32_t)err);


#if (TRC_CFG_SCHEDULING_ONLY == 1)
/* SCHEDULING ONLY, DEFINE ALL AS EMPTY */

                                                                /* Core Trace Macros                                    */
#if 0
#undef OS_TRACE_TASK_READY
#define OS_TRACE_TASK_READY(p_tcb)

#undef OS_TRACE_TASK_PREEMPT
#define OS_TRACE_TASK_PREEMPT(p_tcb)

#undef OS_TRACE_TASK_NOT_READY
#define OS_TRACE_TASK_NOT_READY(p_tcb)

#undef OS_TRACE_TASK_SWITCHED_IN
#define OS_TRACE_TASK_SWITCHED_IN(p_tcb)
#endif

                                                                /* Data Queue Trace Macros                              */
#undef OS_TRACE_DATA_CREATE
#define OS_TRACE_DATA_CREATE(p_data, p_name)

#undef OS_TRACE_DATA_CREATE_FAILED
#define OS_TRACE_DATA_CREATE_FAILED(p_data, max_entries)

#undef OS_TRACE_DATA_DEL
#define OS_TRACE_DATA_DEL(p_data)

#undef OS_TRACE_DATA_DEL_FAILED
#define OS_TRACE_DATA_DEL_FAILED(p_data)

#undef OS_TRACE_DATA_POST
#define OS_TRACE_DATA_POST(p_data, opt)

#undef OS_TRACE_DATA_POST_FAILED
#define OS_TRACE_DATA_POST_FAILED(p_data, opt)

#undef OS_TRACE_DATA_PEND
#define OS_TRACE_DATA_PEND(p_data)

#undef OS_TRACE_DATA_PEND_FAILED
#define OS_TRACE_DATA_PEND_FAILED(p_data)

#undef OS_TRACE_DATA_PEND_BLOCK
#define OS_TRACE_DATA_PEND_BLOCK(p_data)

#undef OS_TRACE_DATA_FLUSH
#define OS_TRACE_DATA_FLUSH(p_data)

#undef OS_TRACE_DATA_FLUSH_FAILED
#define OS_TRACE_DATA_FLUSH_FAILED(p_data)

#undef OS_TRACE_DATA_DEL_ENTER
#define OS_TRACE_DATA_DEL_ENTER(p_data, opt)

#undef OS_TRACE_DATA_POST_ENTER
#define OS_TRACE_DATA_POST_ENTER(p_data, p_void, msg_size, opt)

#undef OS_TRACE_DATA_PEND_ENTER
#define OS_TRACE_DATA_PEND_ENTER(p_data, timeout, opt, p_msg_size, item_size)

#undef OS_TRACE_DATA_DEL_EXIT
#define OS_TRACE_DATA_DEL_EXIT(p_data, err)

#undef OS_TRACE_DATA_POST_EXIT
#define OS_TRACE_DATA_POST_EXIT(p_data, err)

#undef OS_TRACE_DATA_PEND_EXIT
#define OS_TRACE_DATA_PEND_EXIT(p_data, err)

                                                                /* Event Flag Trace Macros                              */
#undef OS_TRACE_FLAG_CREATE
#define OS_TRACE_FLAG_CREATE(p_grp, p_name)

#undef OS_TRACE_FLAG_CREATE_FAILED
#define OS_TRACE_FLAG_CREATE_FAILED(p_grp, flags)

#undef OS_TRACE_FLAG_DEL
#define OS_TRACE_FLAG_DEL(p_grp)

#undef OS_TRACE_FLAG_DEL_FAILED
#define OS_TRACE_FLAG_DEL_FAILED(p_grp)

#undef OS_TRACE_FLAG_POST
#define OS_TRACE_FLAG_POST(p_grp)

#undef OS_TRACE_FLAG_POST_FAILED
#define OS_TRACE_FLAG_POST_FAILED(p_grp)

#undef OS_TRACE_FLAG_PEND
#define OS_TRACE_FLAG_PEND(p_grp)

#undef OS_TRACE_FLAG_PEND_FAILED
#define OS_TRACE_FLAG_PEND_FAILED(p_grp)

#undef OS_TRACE_FLAG_PEND_BLOCK
#define OS_TRACE_FLAG_PEND_BLOCK(p_grp)

#undef OS_TRACE_FLAG_DEL_ENTER
#define OS_TRACE_FLAG_DEL_ENTER(p_grp, opt)

#undef OS_TRACE_FLAG_POST_ENTER
#define OS_TRACE_FLAG_POST_ENTER(p_grp, flags, opt)

#undef OS_TRACE_FLAG_PEND_ENTER
#define OS_TRACE_FLAG_PEND_ENTER(p_grp, flags, timeout, opt)

#undef OS_TRACE_FLAG_DEL_EXIT
#define OS_TRACE_FLAG_DEL_EXIT(p_grp, err)

#undef OS_TRACE_FLAG_POST_EXIT
#define OS_TRACE_FLAG_POST_EXIT(p_grp, err)

#undef OS_TRACE_FLAG_PEND_EXIT
#define OS_TRACE_FLAG_PEND_EXIT(p_grp, err)

                                                                /* Memory Partition Trace Macros                        */
#undef OS_TRACE_MEM_CREATE
#define OS_TRACE_MEM_CREATE(p_mem, p_name)

#undef OS_TRACE_MEM_CREATE_FAILED
#define OS_TRACE_MEM_CREATE_FAILED(p_mem, n_blks)

#undef OS_TRACE_MEM_PUT
#define OS_TRACE_MEM_PUT(p_mem)

#undef OS_TRACE_MEM_PUT_FAILED
#define OS_TRACE_MEM_PUT_FAILED(p_mem)

#undef OS_TRACE_MEM_GET
#define OS_TRACE_MEM_GET(p_mem)

#undef OS_TRACE_MEM_GET_FAILED
#define OS_TRACE_MEM_GET_FAILED(p_mem)

#undef OS_TRACE_MEM_PUT_ENTER
#define OS_TRACE_MEM_PUT_ENTER(p_mem, p_blk)

#undef OS_TRACE_MEM_GET_ENTER
#define OS_TRACE_MEM_GET_ENTER(p_mem)

#undef OS_TRACE_MEM_PUT_EXIT
#define OS_TRACE_MEM_PUT_EXIT(p_mem, err)

#undef OS_TRACE_MEM_GET_EXIT
#define OS_TRACE_MEM_GET_EXIT(p_mem, err)

                                                                /* Mutex Trace Macros                                   */
#undef OS_TRACE_MUTEX_CREATE
#define OS_TRACE_MUTEX_CREATE(p_mutex, p_name)

#undef OS_TRACE_MUTEX_CREATE_FAILED
#define OS_TRACE_MUTEX_CREATE_FAILED(p_mutex)

#undef OS_TRACE_MUTEX_DEL
#define OS_TRACE_MUTEX_DEL(p_mutex)

#undef OS_TRACE_MUTEX_DEL_FAILED
#define OS_TRACE_MUTEX_DEL_FAILED(p_mutex)

#undef OS_TRACE_MUTEX_POST
#define OS_TRACE_MUTEX_POST(p_mutex)

#undef OS_TRACE_MUTEX_POST_FAILED
#define OS_TRACE_MUTEX_POST_FAILED(p_mutex)

#undef OS_TRACE_MUTEX_PEND
#define OS_TRACE_MUTEX_PEND(p_mutex)

#undef OS_TRACE_MUTEX_PEND_FAILED
#define OS_TRACE_MUTEX_PEND_FAILED(p_mutex)

#undef OS_TRACE_MUTEX_PEND_BLOCK
#define OS_TRACE_MUTEX_PEND_BLOCK(p_mutex)

#undef OS_TRACE_MUTEX_TASK_PRIO_INHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_INHERIT(p_tcb, prio)

#undef OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb, prio)

#undef OS_TRACE_MUTEX_DEL_ENTER
#define OS_TRACE_MUTEX_DEL_ENTER(p_mutex, opt)

#undef OS_TRACE_MUTEX_POST_ENTER
#define OS_TRACE_MUTEX_POST_ENTER(p_mutex, opt)

#undef OS_TRACE_MUTEX_PEND_ENTER
#define OS_TRACE_MUTEX_PEND_ENTER(p_mutex, timeout, opt)

#undef OS_TRACE_MUTEX_DEL_EXIT
#define OS_TRACE_MUTEX_DEL_EXIT(p_mutex, err)

#undef OS_TRACE_MUTEX_POST_EXIT
#define OS_TRACE_MUTEX_POST_EXIT(p_mutex, err)

#undef OS_TRACE_MUTEX_PEND_EXIT
#define OS_TRACE_MUTEX_PEND_EXIT(p_mutex, err)

                                                                /* Message Queue Trace Macros                           */
#undef OS_TRACE_Q_CREATE
#define OS_TRACE_Q_CREATE(p_q, p_name)

#undef OS_TRACE_Q_CREATE_FAILED
#define OS_TRACE_Q_CREATE_FAILED(p_q, max_qty)

#undef OS_TRACE_Q_DEL
#define OS_TRACE_Q_DEL(p_q)

#undef OS_TRACE_Q_DEL_FAILED
#define OS_TRACE_Q_DEL_FAILED(p_q)

#undef OS_TRACE_Q_POST
#define OS_TRACE_Q_POST(p_q, opt, err)

#undef OS_TRACE_Q_POST_FAILED
#define OS_TRACE_Q_POST_FAILED(p_q, opt)

#undef OS_TRACE_Q_PEND
#define OS_TRACE_Q_PEND(p_q)

#undef OS_TRACE_Q_PEND_FAILED
#define OS_TRACE_Q_PEND_FAILED(p_q)

#undef OS_TRACE_Q_PEND_BLOCK
#define OS_TRACE_Q_PEND_BLOCK(p_q)

#undef OS_TRACE_Q_FLUSH
#define OS_TRACE_Q_FLUSH(p_q)

#undef OS_TRACE_Q_FLUSH_FAILED
#define OS_TRACE_Q_FLUSH_FAILED(p_q)

#undef OS_TRACE_Q_DEL_ENTER
#define OS_TRACE_Q_DEL_ENTER(p_q, opt)

#undef OS_TRACE_Q_POST_ENTER
#define OS_TRACE_Q_POST_ENTER(p_q, p_void, msg_size, opt)

#undef OS_TRACE_Q_PEND_ENTER
#define OS_TRACE_Q_PEND_ENTER(p_q, timeout, opt, p_msg_size)

#undef OS_TRACE_Q_DEL_EXIT
#define OS_TRACE_Q_DEL_EXIT(p_q, err)

#undef OS_TRACE_Q_POST_EXIT
#define OS_TRACE_Q_POST_EXIT(p_q, err)

#undef OS_TRACE_Q_PEND_EXIT
#define OS_TRACE_Q_PEND_EXIT(p_q, err)

                                                                /* Semaphore Trace Macros                               */
#undef OS_TRACE_SEM_CREATE
#define OS_TRACE_SEM_CREATE(p_sem, p_name)

#undef OS_TRACE_SEM_CREATE_FAILED
#define OS_TRACE_SEM_CREATE_FAILED(p_sem, cnt)

#undef OS_TRACE_SEM_DEL
#define OS_TRACE_SEM_DEL(p_sem)

#undef OS_TRACE_SEM_DEL_FAILED
#define OS_TRACE_SEM_DEL_FAILED(p_sem)

#undef OS_TRACE_SEM_POST
#define OS_TRACE_SEM_POST(p_sem)

#undef OS_TRACE_SEM_POST_FAILED
#define OS_TRACE_SEM_POST_FAILED(p_sem)

#undef OS_TRACE_SEM_PEND
#define OS_TRACE_SEM_PEND(p_sem)

#undef OS_TRACE_SEM_PEND_FAILED
#define OS_TRACE_SEM_PEND_FAILED(p_sem)

#undef OS_TRACE_SEM_PEND_BLOCK
#define OS_TRACE_SEM_PEND_BLOCK(p_sem)

#undef OS_TRACE_SEM_SET
#define OS_TRACE_SEM_SET(p_sem)

#undef OS_TRACE_SEM_SET_FAILED
#define OS_TRACE_SEM_SET_FAILED(p_sem, cnt)

#undef OS_TRACE_SEM_DEL_ENTER
#define OS_TRACE_SEM_DEL_ENTER(p_sem, opt)

#undef OS_TRACE_SEM_POST_ENTER
#define OS_TRACE_SEM_POST_ENTER(p_sem, opt)

#undef OS_TRACE_SEM_PEND_ENTER
#define OS_TRACE_SEM_PEND_ENTER(p_sem, timeout, opt)

#undef OS_TRACE_SEM_DEL_EXIT
#define OS_TRACE_SEM_DEL_EXIT(p_sem, err)

#undef OS_TRACE_SEM_POST_EXIT
#define OS_TRACE_SEM_POST_EXIT(p_sem, err)

#undef OS_TRACE_SEM_PEND_EXIT
#define OS_TRACE_SEM_PEND_EXIT(p_sem, err)

                                                                /* Task Management Trace Macros                         */
#if 0
#undef OS_TRACE_TASK_CREATE
#define OS_TRACE_TASK_CREATE(p_tcb)

#undef OS_TRACE_TASK_CREATE_FAILED
#define OS_TRACE_TASK_CREATE_FAILED(p_tcb, prio)

#undef OS_TRACE_TASK_DEL
#define OS_TRACE_TASK_DEL(p_tcb)
#endif

#undef OS_TRACE_TASK_DLY
#define OS_TRACE_TASK_DLY(p_tcb, dly_ticks)

#undef OS_TRACE_TASK_SUSPEND
#define OS_TRACE_TASK_SUSPEND(p_tcb)

#undef OS_TRACE_TASK_SUSPEND_FAILED
#define OS_TRACE_TASK_SUSPEND_FAILED(p_tcb)

#undef OS_TRACE_TASK_RESUME
#define OS_TRACE_TASK_RESUME(p_tcb)

#undef OS_TRACE_TASK_RESUME_FAILED
#define OS_TRACE_TASK_RESUME_FAILED(p_tcb)

#undef OS_TRACE_TASK_PRIO_CHANGE
#define OS_TRACE_TASK_PRIO_CHANGE(p_tcb, prio)

#undef OS_TRACE_TASK_Q_CREATE
#define OS_TRACE_TASK_Q_CREATE(p_tcb, p_name)

#undef OS_TRACE_TASK_Q_DEL
#define OS_TRACE_TASK_Q_DEL(p_tcb)

#undef OS_TRACE_TASK_Q_POST
#define OS_TRACE_TASK_Q_POST(p_tcb, opt, err)

#undef OS_TRACE_TASK_Q_POST_FAILED
#define OS_TRACE_TASK_Q_POST_FAILED(p_tcb, opt)

#undef OS_TRACE_TASK_Q_PEND
#define OS_TRACE_TASK_Q_PEND(p_tcb)

#undef OS_TRACE_TASK_Q_PEND_FAILED
#define OS_TRACE_TASK_Q_PEND_FAILED(p_tcb)

#undef OS_TRACE_TASK_Q_PEND_BLOCK
#define OS_TRACE_TASK_Q_PEND_BLOCK(p_tcb)

#undef OS_TRACE_TASK_Q_FLUSH
#define OS_TRACE_TASK_Q_FLUSH(p_tcb)

#undef OS_TRACE_TASK_Q_FLUSH_FAILED
#define OS_TRACE_TASK_Q_FLUSH_FAILED(p_tcb)

#undef OS_TRACE_TASK_SEM_CREATE
#define OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name)

#undef OS_TRACE_TASK_SEM_DEL
#define OS_TRACE_TASK_SEM_DEL(p_tcb)

#undef OS_TRACE_TASK_SEM_POST
#define OS_TRACE_TASK_SEM_POST(p_tcb)

#undef OS_TRACE_TASK_SEM_POST_FAILED
#define OS_TRACE_TASK_SEM_POST_FAILED(p_tcb)

#undef OS_TRACE_TASK_SEM_PEND
#define OS_TRACE_TASK_SEM_PEND(p_tcb)

#undef OS_TRACE_TASK_SEM_PEND_FAILED
#define OS_TRACE_TASK_SEM_PEND_FAILED(p_tcb)

#undef OS_TRACE_TASK_SEM_PEND_BLOCK
#define OS_TRACE_TASK_SEM_PEND_BLOCK(p_tcb)

#undef OS_TRACE_TASK_SEM_SET
#define OS_TRACE_TASK_SEM_SET(p_tcb)

#undef OS_TRACE_TASK_SEM_SET_FAILED
#define OS_TRACE_TASK_SEM_SET_FAILED(p_tcb, cnt)

#undef OS_TRACE_TASK_Q_PEND_ENTER
#define OS_TRACE_TASK_Q_PEND_ENTER(p_tcb, timeout, opt, p_msg_size)

#undef OS_TRACE_TASK_Q_POST_ENTER
#define OS_TRACE_TASK_Q_POST_ENTER(p_tcb, p_void, msg_size, opt)

#undef OS_TRACE_TASK_SEM_PEND_ENTER
#define OS_TRACE_TASK_SEM_PEND_ENTER(p_tcb, timeout, opt)

#undef OS_TRACE_TASK_SEM_POST_ENTER
#define OS_TRACE_TASK_SEM_POST_ENTER(p_tcb, opt)

#undef OS_TRACE_TASK_SUSPEND_ENTER
#define OS_TRACE_TASK_SUSPEND_ENTER(p_tcb)

#undef OS_TRACE_TASK_RESUME_ENTER
#define OS_TRACE_TASK_RESUME_ENTER(p_tcb)

#undef OS_TRACE_TASK_Q_PEND_EXIT
#define OS_TRACE_TASK_Q_PEND_EXIT(p_tcb, err)

#undef OS_TRACE_TASK_Q_POST_EXIT
#define OS_TRACE_TASK_Q_POST_EXIT(p_tcb, err)

#undef OS_TRACE_TASK_SEM_PEND_EXIT
#define OS_TRACE_TASK_SEM_PEND_EXIT(p_tcb, err)

#undef OS_TRACE_TASK_SEM_POST_EXIT
#define OS_TRACE_TASK_SEM_POST_EXIT(p_tcb, err)

#undef OS_TRACE_TASK_SUSPEND_EXIT
#define OS_TRACE_TASK_SUSPEND_EXIT(p_tcb, err)

#undef OS_TRACE_TASK_RESUME_EXIT
#define OS_TRACE_TASK_RESUME_EXIT(p_tcb, err)

                                                                /* Tick Trace Macros                                     */
#if 0
#undef OS_TRACE_TICK_INCREMENT
#define OS_TRACE_TICK_INCREMENT(ctr)
#endif

                                                                /* Tmr Trace Macros                                     */
#undef OS_TRACE_TMR_CREATE
#define OS_TRACE_TMR_CREATE(p_tmr, p_name, dly, period)

#undef OS_TRACE_TMR_CREATE_FAILED
#define OS_TRACE_TMR_CREATE_FAILED(p_tmr, dly, period)

#undef OS_TRACE_TMR_DEL
#define OS_TRACE_TMR_DEL(p_tmr)

#undef OS_TRACE_TMR_EXPIRED
#define OS_TRACE_TMR_EXPIRED(p_tmr)

#undef OS_TRACE_TMR_DEL_ENTER
#define OS_TRACE_TMR_DEL_ENTER(p_tmr)

#undef OS_TRACE_TMR_DEL_EXIT
#define OS_TRACE_TMR_DEL_EXIT(p_tmr, err)

#endif /* (TRC_CFG_SCHEDULING_ONLY == 0) */

#else
#error "Only TRC_RECORDER_MODE_STREAMING is supported."
#endif /* TRC_RECORDER_MODE_STREAMING */

#endif /* TRC_USE_TRACEALYZER_RECORDER */

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_H */
