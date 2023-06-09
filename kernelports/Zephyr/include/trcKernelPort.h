/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The Zephyr specific parts needed by the trace recorder
 */

#ifndef TRC_KERNEL_PORT_H
#define TRC_KERNEL_PORT_H

#include <zephyr/kernel.h>
#include <version.h>
#include <trcRecorder.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @def TRACE_KERNEL_VERSION
 * @brief This defines the kernel version/identity. 
 */
#define TRACE_KERNEL_VERSION 0x9AA9

/**
 * @def TRC_PLATFORM_CFG
 * @brief This defines the basis for version specific lookup of
 * platform configuration files. If left empty the default
 * RTOS XML files are used.
 */
#define TRC_PLATFORM_CFG "Zephyr"

/**
 * @def TRC_PLATFORM_CFG_MAJOR
 * @brief Major release version for recorder.
 */
#define TRC_PLATFORM_CFG_MAJOR 3

/**
 * @def TRC_PLATFORM_CFG_MINOR
 * @brief Minor release version for recorder.
 */
#define TRC_PLATFORM_CFG_MINOR 3

/**
 * @def TRC_PLATFORM_CFG_PATCH
 * @brief Patchlevel release version for recorder.
 */
#define TRC_PLATFORM_CFG_PATCH 0

/**
 * @def TRC_TICK_RATE_HZ
 * @brief Trace RTOS tick rate in Hz.
 */
#define TRC_TICK_RATE_HZ CONFIG_SYS_CLOCK_TICKS_PER_SEC

/**
 * @def TraceKernelPortTaskHandle_t
 * @brief RTOS data type for tasks/threads.
 */
#define TraceKernelPortTaskHandle_t struct k_thread

extern TraceHeapHandle_t xSystemHeapHandle;

/**
 * @brief A structure representing the kernel port buffer.
 */
typedef struct TraceKernelPortDataBuffer	/* Aligned */
{
	uint8_t buffer[sizeof(TraceHeapHandle_t) + sizeof(TraceKernelPortTaskHandle_t) + sizeof(TraceExtensionHandle_t) + 8];
} TraceKernelPortDataBuffer_t;

/**
 * @internal Kernel port initialize callback.
 * 
 * This function is called by the recorder as part of its initialization phase.
 * 
 * @param[in] pxBuffer Buffer
 * 
 * @retval TRC_FAIL Initialization failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* pxBuffer);

/**
 * @brief Kernel port begin callback.
 * 
 * This function is called by the recorder as part of its begin phase.
 * 
 * @retval TRC_FAIL Enabling failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortEnable(void);

/**
 * @brief Get unused stack size for kernel port thread.
 * 
 * @param[in] pvThread Thread
 * @param[out] puxUnusedStack Unused stack
 * 
 * @retval TRC_FAIL Failed to get size
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceKernelPortGetUnusedStack(void* pvThread, TraceUnsignedBaseType_t* puxUnusedStack);

/**
 * @brief Get kernel port system heap handle.
 * 
 * @return TraceHeapHandle_t 
 */
TraceHeapHandle_t xTraceKernelPortGetSystemHeapHandle(void);

/**
 * @brief Get kernel port scheduler suspended state
 * 
 * @retval 0 Scheduler not suspended
 * @retval 1 Scheduler suspended
 */
unsigned char xTraceKernelPortIsSchedulerSuspended(void);

/**
 * @brief Sets kernel object name for display in Tracealyzer.
 * 
 * @param[in] object Kernel object
 * @param[in] name name
 */
void vTraceSetKernelObjectName(void* object, const char* name);

/**
 * @brief Sets Work Queue name for display in Tracealyzer.
 * 
 * @param[in] object Work queue object
 * @param[in] name name
 */
void vTraceSetWorkQueueName(void* object, const char* name);

/**
 * @brief Sets Heap name for display in Tracealyzer.
 * 
 * @param[in] object Heap object
 * @param[in] name name
 */
void vTraceSetHeapName(void* object, const char* name);

/**
 * @brief Sets Semaphore name for display in Tracealyzer.
 * 
 * @param[in] object Semaphore object
 * @param[in] name name
 */
void vTraceSetSemaphoreName(void* object, const char* name);

/**
 * @brief Sets Mutex name for display in Tracealyzer.
 * 
 * @param[in] object Mutex object
 * @param[in] name name
 */
void vTraceSetMutexName(void* object, const char* name);

/**
 * @brief Sets Condvar name for display in Tracealyzer.
 * 
 * @param[in] object Condvar object
 * @param[in] name name
 */
void vTraceSetCondvarName(void* object, const char* name);

/**
 * @brief Sets Queue name for display in Tracealyzer.
 * 
 * @param[in] object Queue object
 * @param[in] name name
 */
void vTraceSetQueueName(void* object, const char* name);

/**
 * @brief Sets FIFO Queue name for display in Tracealyzer.
 * 
 * @param[in] object FIFO Queue object
 * @param[in] name name
 */
void vTraceSetFIFOQueueName(void* object, const char* name);

/**
 * @brief Sets LIFO Queue name for display in Tracealyzer.
 * 
 * @param[in] object LIFO Queue object
 * @param[in] name name
 */
void vTraceSetLIFOQueueName(void* object, const char* name);

/**
 * @brief Sets Stack name for display in Tracealyzer.
 * 
 * @param[in] object Stack object
 * @param[in] name name
 */
void vTraceSetStackName(void* object, const char* name);

/**
 * @brief Sets Message Queue name for display in Tracealyzer.
 * 
 * @param[in] object Message Queue object
 * @param[in] name name
 */
void vTraceSetMessageQueueName(void* object, const char* name);

/**
 * @brief Sets Mailbox name for display in Tracealyzer.
 * 
 * @param[in] object Mailbox object
 * @param[in] name name
 */
void vTraceSetMailboxName(void* object, const char* name);

/**
 * @brief Sets Pipe name for display in Tracealyzer.
 * 
 * @param[in] object Pipe object
 * @param[in] name name
 */
void vTraceSetPipeName(void* object, const char* name);

/**
 * @brief Sets Memory Heap name for display in Tracealyzer.
 * 
 * @param[in] object Memory Heap object
 * @param[in] name name
 */
void vTraceSetMemoryHeapName(void* object, const char* name);

/**
 * @brief Sets Memory Slab name for display in Tracealyzer.
 * 
 * @param[in] object Memory Slab object
 * @param[in] name name
 */
void vTraceSetMemorySlabName(void* object, const char* name);

/**
 * @brief Sets Timer name for display in Tracealyzer.
 * 
 * @param[in] object Timer object
 * @param[in] name name
 */
void vTraceSetTimerName(void* object, const char* name);


/* Flag an error if the user is trying to use the tracerecorder in a version of
 * Zephyr that predates the new tracing system on which the recorder relies
 * (Zephyr 2.6.0).
 */
#if ((KERNEL_VERSION_MAJOR) < 2)
	#error "Tracerecorder is not compatible with Zephyr versions older than v2.6.0"
#elif (((KERNEL_VERSION_MAJOR) == 2) && ((KERNEL_VERSION_MINOR) < 6))
	#error "Tracerecorder is not compatible with Zephyr versions lower than v2.6.0"
#endif

/* Ensure that CONFIG_MEM_POOL has been set when the user selects dynamic
 * allocation of the recorder buffer.
 */
#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC)
    /* While we could add CONFIG_KERNEL_MEM_POOL as a dependency for the 
	 * dynamic allocation option, we have opted to output and error if 
	 * the user have forgotten this since they also have to specify an
	 * appropriate size for the kernel memory pool.
	 */
    #ifndef CONFIG_KERNEL_MEM_POOL
        #error "Tracerecorder: You have choosen the TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC option without enabling KERNEL_MEM_POOL in Zephyr. Enable this option and allocate an appropriate size."
    #endif

	/**
	 * @internal Kernel port specific heap initialization
	 */
	#define TRC_KERNEL_PORT_HEAP_INIT(size)

	/**
	 * @internal Kernel port specific heap malloc definition
	 */
	#define TRC_KERNEL_PORT_HEAP_MALLOC(size) k_malloc(size)
#endif


/**
 * Define streaming event codes - should match the Tracealyzer config file
 */
#define PSF_EVENT_NULL_EVENT								0x00

#define PSF_EVENT_TRACE_START								0x01
#define PSF_EVENT_TS_CONFIG									0x02
#define PSF_EVENT_OBJ_NAME									0x03

#define PSF_EVENT_TASK_PRIO_INHERIT							0x05
#define PSF_EVENT_TASK_PRIO_DISINHERIT						0x06
#define PSF_EVENT_DEFINE_ISR								0x07

#define PSF_EVENT_THREAD_SCHED_WAKEUP						0xB6
#define PSF_EVENT_THREAD_SCHED_ABORT						0x20
#define PSF_EVENT_THREAD_SCHED_PRIORITY_SET					0x04
#define PSF_EVENT_THREAD_SCHED_READY						0x30
#define PSF_EVENT_THREAD_SCHED_PEND
#define PSF_EVENT_THREAD_SCHED_RESUME						0x7C
#define PSF_EVENT_THREAD_SCHED_SUSPEND						0x7B

#define PSF_EVENT_THREAD_FOREACH_ENTER						0x09
#define PSF_EVENT_THREAD_FOREACH_EXIT						0x0B
#define PSF_EVENT_THREAD_FOREACH_UNLOCKED_ENTER				0x0A
#define PSF_EVENT_THREAD_FOREACH_UNLOCKED_EXIT				0xF2
#define PSF_EVENT_THREAD_INIT								0x10
#define PSF_EVENT_THREAD_USER_MODE_ENTER					0x0C
#define PSF_EVENT_THREAD_HEAP_ASSIGN						0xFC
#define PSF_EVENT_THREAD_JOIN_SUCCESS						0xB0
#define PSF_EVENT_THREAD_JOIN_BLOCKING						0xB2
#define PSF_EVENT_THREAD_JOIN_TIMEOUT						0xB1
#define PSF_EVENT_THREAD_SLEEP_ENTER						0x2A
#define PSF_EVENT_THREAD_SLEEP_EXIT							0x2B
#define PSF_EVENT_THREAD_MSLEEP_ENTER						0x2C
#define PSF_EVENT_THREAD_MSLEEP_EXIT						0x2D
#define PSF_EVENT_THREAD_USLEEP_ENTER						0x2E
#define PSF_EVENT_THREAD_USLEEP_EXIT						0x2F
#define PSF_EVENT_THREAD_BUSY_WAIT_ENTER					0xB4
#define PSF_EVENT_THREAD_BUSY_WAIT_EXIT						0xF3
#define PSF_EVENT_THREAD_YIELD								0xB5
#define PSF_EVENT_THREAD_WAKEUP								0x15D
#define PSF_EVENT_THREAD_ABORT								0x15E	
#define PSF_EVENT_THREAD_START								0xB3
#define PSF_EVENT_THREAD_SET_PRIORITY						0x15F
#define PSF_EVENT_THREAD_SUSPEND							0x161
#define PSF_EVENT_THREAD_RESUME								0x160
#define PSF_EVENT_THREAD_SCHED_LOCK							0xB7
#define PSF_EVENT_THREAD_SCHED_UNLOCK						0xB8

#define PSF_EVENT_WORK_INIT									0x11E
#define PSF_EVENT_WORK_SUBMIT_TO_QUEUE_SUCCESS				0x11F
#define PSF_EVENT_WORK_SUBMIT_TO_QUEUE_BLOCKING				0x120
#define PSF_EVENT_WORK_SUBMIT_TO_QUEUE_FAILURE				0x121
#define PSF_EVENT_WORK_SUBMIT_SUCCESS						0x122
#define PSF_EVENT_WORK_SUBMIT_BLOCKING						0x123
#define PSF_EVENT_WORK_SUBMIT_FAILURE						0x124
#define PSF_EVENT_WORK_FLUSH_SUCCESS						0x125
#define PSF_EVENT_WORK_FLUSH_BLOCKING						0x126
#define PSF_EVENT_WORK_FLUSH_FAILURE						0x127
#define PSF_EVENT_WORK_CANCEL_SUCCESS						0x128
#define PSF_EVENT_WORK_CANCEL_BLOCKING						0x129
#define PSF_EVENT_WORK_CANCEL_FAILURE						0x12A
#define PSF_EVENT_WORK_CANCEL_SYNC_SUCCESS					0x12B
#define PSF_EVENT_WORK_CANCEL_SYNC_BLOCKING					0x12C
#define PSF_EVENT_WORK_CANCEL_SYNC_FAILURE					0x12D

#define PSF_EVENT_WORK_QUEUE_START_SUCCESS					0x12E
#define PSF_EVENT_WORK_QUEUE_START_BLOCKING					0x12F
#define PSF_EVENT_WORK_QUEUE_START_FAILURE					0x130
#define PSF_EVENT_WORK_QUEUE_DRAIN_SUCCESS					0x131
#define PSF_EVENT_WORK_QUEUE_DRAIN_BLOCKING					0x132
#define PSF_EVENT_WORK_QUEUE_DRAIN_FAILURE					0x133
#define PSF_EVENT_WORK_QUEUE_UNPLUG_SUCCESS					0x134
#define PSF_EVENT_WORK_QUEUE_UNPLUG_BLOCKING				0x135
#define PSF_EVENT_WORK_QUEUE_UNPLUG_FAILURE					0x136

#define PSF_EVENT_DWORK_INIT								0x137
#define PSF_EVENT_DWORK_SCHEDULE_FOR_QUEUE_SUCCESS 			0x138
#define PSF_EVENT_DWORK_SCHEDULE_FOR_QUEUE_BLOCKING 		0x139
#define PSF_EVENT_DWORK_SCHEDULE_FOR_QUEUE_FAILURE			0x13A
#define PSF_EVENT_DWORK_SCHEDULE_SUCCESS					0x13B
#define PSF_EVENT_DWORK_SCHEDULE_BLOCKING					0x13C
#define PSF_EVENT_DWORK_SCHEDULE_FAILURE					0x13D
#define PSF_EVENT_DWORK_RESCHEDULE_FOR_QUEUE_SUCCESS		0x13E
#define PSF_EVENT_DWORK_RESCHEDULE_FOR_QUEUE_BLOCKING		0x13F
#define PSF_EVENT_DWORK_RESCHEDULE_FOR_QUEUE_FAILURE		0x140
#define PSF_EVENT_DWORK_RESCHEDULE_SUCCESS					0x141
#define PSF_EVENT_DWORK_RESCHEDULE_BLOCKING					0x142
#define PSF_EVENT_DWORK_RESCHEDULE_FAILURE					0x143
#define PSF_EVENT_DWORK_FLUSH_SUCCESS						0x144
#define PSF_EVENT_DWORK_FLUSH_BLOCKING						0x145
#define PSF_EVENT_DWORK_FLUSH_FAILURE						0x146
#define PSF_EVENT_DWORK_CANCEL_DELAYABLE_SUCCESS			0x147
#define PSF_EVENT_DWORK_CANCEL_DELAYABLE_BLOCKING			0x148
#define PSF_EVENT_DWORK_CANCEL_DELAYABLE_FAILURE			0x149
#define PSF_EVENT_DWORK_CANCEL_DELAYABLE_SYNC_SUCCESS		0x14A
#define PSF_EVENT_DWORK_CANCEL_DELAYABLE_SYNC_BLOCKING		0x14B
#define PSF_EVENT_DWORK_CANCEL_DELAYABLE_SYNC_FAILURE		0x14C

#define PSF_EVENT_UWORK_INIT								0x14D
#define PSF_EVENT_UWORK_SUBMIT_TO_QUEUE_SUCCESS				0x14E
#define PSF_EVENT_UWORK_SUBMIT_TO_QUEUE_BLOCKING			0x14F
#define PSF_EVENT_UWORK_SUBMIT_TO_QUEUE_FAILURE				0x150

#define PSF_EVENT_UWORK_QUEUE_START_ENTER					0x151
#define PSF_EVENT_UWORK_QUEUE_START_EXIT					0x152

#define PSF_EVENT_PWORK_INIT_ENTER							0x153
#define PSF_EVENT_PWORK_INIT_EXIT							0x166
#define PSF_EVENT_PWORK_SUBMIT_TO_QUEUE_SUCCESS				0x154
#define PSF_EVENT_PWORK_SUBMIT_TO_QUEUE_BLOCKING			0x155
#define PSF_EVENT_PWORK_SUBMIT_TO_QUEUE_FAILURE				0x156
#define PSF_EVENT_PWORK_SUBMIT_SUCCESS						0x157
#define PSF_EVENT_PWORK_SUBMIT_BLOCKING						0x158
#define PSF_EVENT_PWORK_SUBMIT_FAILURE						0x159
#define PSF_EVENT_PWORK_CANCEL_SUCCESS						0x15A
#define PSF_EVENT_PWORK_CANCEL_BLOCKING						0x15B
#define PSF_EVENT_PWORK_CANCEL_FAILURE						0x15C

#define PSF_EVENT_ISR_ENTER									0x162
#define PSF_EVENT_ISR_EXIT									0x163
#define PSF_EVENT_ISR_EXIT_TO_SCHEDULER						0x164

#define PSF_EVENT_POLL_EVENT_INIT							0x82
#define PSF_EVENT_POLL_POLL_SUCCESS							0x8F
#define PSF_EVENT_POLL_POLL_BLOCKING						0x11
#define PSF_EVENT_POLL_POLL_FAILURE							0x0D
#define PSF_EVENT_POLL_SIGNAL_INIT							0x83
#define PSF_EVENT_POLL_SIGNAL_RESET							0x8D
#define PSF_EVENT_POLL_SIGNAL_CHECK							0xF0
#define PSF_EVENT_POLL_SIGNAL_RAISE_SUCCESS					0x8E
#define PSF_EVENT_POLL_SIGNAL_RAISE_FAILURE					0xEF

#define PSF_EVENT_QUEUE_INIT								0xC0
#define PSF_EVENT_QUEUE_CANCEL_WAIT							0xC1
#define PSF_EVENT_QUEUE_QUEUE_INSERT_SUCCESS				0xBC		// Only tracked when there is a queue allocation event
#define PSF_EVENT_QUEUE_QUEUE_INSERT_BLOCKING				0xBD		// Only tracked when there is a queue allocation event
#define PSF_EVENT_QUEUE_QUEUE_INSERT_FAILURE				0xBE		// Only tracked when there is a queue allocation event
#define PSF_EVENT_QUEUE_APPEND								0xC2
#define PSF_EVENT_QUEUE_ALLOC_APPEND_SUCCESS				0xC3
#define PSF_EVENT_QUEUE_ALLOC_APPEND_BLOCKING				0xF1
#define PSF_EVENT_QUEUE_ALLOC_APPEND_FAILURE				0xC4
#define PSF_EVENT_QUEUE_PREPEND								0xC5
#define PSF_EVENT_QUEUE_ALLOC_PREPEND_SUCCESS				0xC6
#define PSF_EVENT_QUEUE_ALLOC_PREPEND_BLOCKING				0xBF
#define PSF_EVENT_QUEUE_ALLOC_PREPEND_FAILURE				0xC7
#define PSF_EVENT_QUEUE_INSERT								0xC8
#define PSF_EVENT_QUEUE_APPEND_LIST_SUCCESS					0xC9
#define PSF_EVENT_QUEUE_APPEND_LIST_BLOCKING				0x165
#define PSF_EVENT_QUEUE_APPEND_LIST_FAILURE					0xCA
#define PSF_EVENT_QUEUE_MERGE_SLIST_SUCCESS					0xCB
#define PSF_EVENT_QUEUE_MERGE_SLIST_BLOCKING				0x6E
#define PSF_EVENT_QUEUE_MERGE_SLIST_FAILURE					0xCC
#define PSF_EVENT_QUEUE_GET_SUCCESS							0xCD
#define PSF_EVENT_QUEUE_GET_BLOCKING						0xCE
#define PSF_EVENT_QUEUE_GET_TIMEOUT							0xCF
#define PSF_EVENT_QUEUE_REMOVE_SUCCESS						0xD0
#define PSF_EVENT_QUEUE_REMOVE_BLOCKING						0xDB
#define PSF_EVENT_QUEUE_REMOVE_FAILURE						0xD1
#define PSF_EVENT_QUEUE_UNIQUE_APPEND_SUCCESS				0xD2
#define PSF_EVENT_QUEUE_UNIQUE_APPEND_BLOCKING				0xEE
#define PSF_EVENT_QUEUE_UNIQUE_APPEND_FAILURE				0xB9
#define PSF_EVENT_QUEUE_PEEK_HEAD							0xBA
#define PSF_EVENT_QUEUE_PEEK_TAIL							0xBB

#define PSF_EVENT_FIFO_INIT_ENTER							0x100
#define PSF_EVENT_FIFO_INIT_EXIT							0x101
#define PSF_EVENT_FIFO_CANCEL_WAIT_ENTER					0x102
#define PSF_EVENT_FIFO_CANCEL_WAIT_EXIT						0x103
#define PSF_EVENT_FIFO_PUT_ENTER							0x104
#define PSF_EVENT_FIFO_PUT_EXIT								0x105
#define PSF_EVENT_FIFO_ALLOC_PUT_SUCCESS					0x106
#define PSF_EVENT_FIFO_ALLOC_PUT_BLOCKING					0x107
#define PSF_EVENT_FIFO_ALLOC_PUT_FAILURE					0x108
#define PSF_EVENT_FIFO_PUT_LIST_ENTER						0x109
#define PSF_EVENT_FIFO_PUT_LIST_EXIT						0x10A
#define PSF_EVENT_FIFO_PUT_SLIST_ENTER						0x10B
#define PSF_EVENT_FIFO_PUT_SLIST_EXIT						0x10C
#define PSF_EVENT_FIFO_GET_SUCCESS							0x10D
#define PSF_EVENT_FIFO_GET_BLOCKING							0x10E
#define PSF_EVENT_FIFO_GET_FAILURE							0x10F
#define PSF_EVENT_FIFO_PEEK_HEAD_ENTER						0x110
#define PSF_EVENT_FIFO_PEEK_HEAD_EXIT						0x111
#define PSF_EVENT_FIFO_PEEK_TAIL_ENTER						0x112
#define PSF_EVENT_FIFO_PEEK_TAIL_EXIT						0x113

#define PSF_EVENT_LIFO_INIT_ENTER							0x114
#define PSF_EVENT_LIFO_INIT_EXIT							0x115
#define PSF_EVENT_LIFO_PUT_ENTER							0x116
#define PSF_EVENT_LIFO_PUT_EXIT								0x117
#define PSF_EVENT_LIFO_ALLOC_PUT_SUCCESS					0x118
#define PSF_EVENT_LIFO_ALLOC_PUT_BLOCKING					0x119
#define PSF_EVENT_LIFO_ALLOC_PUT_FAILURE					0x11A
#define PSF_EVENT_LIFO_GET_SUCCESS							0x11B
#define PSF_EVENT_LIFO_GET_BLOCKING							0x11C
#define PSF_EVENT_LIFO_GET_FAILURE							0x11D

#define PSF_EVENT_STACK_INIT								0x4A
#define PSF_EVENT_STACK_ALLOC_INIT_SUCCESS					0x4B
#define PSF_EVENT_STACK_ALLOC_INIT_BLOCKING					0x4C
#define PSF_EVENT_STACK_ALLOC_INIT_FAILURE					0x4D
#define PSF_EVENT_STACK_CLEANUP_SUCCESS						0x4E
#define PSF_EVENT_STACK_CLEANUP_BLOCKING					0x4F
#define PSF_EVENT_STACK_CLEANUP_FAILURE						0x26
#define PSF_EVENT_STACK_PUSH_SUCCESS						0x27
#define PSF_EVENT_STACK_PUSH_BLOCKING						0x3E
#define PSF_EVENT_STACK_PUSH_FAILURE						0x3F
#define PSF_EVENT_STACK_POP_SUCCESS							0x5B
#define PSF_EVENT_STACK_POP_BLOCKING						0x5E
#define PSF_EVENT_STACK_POP_FAILURE							0x5F

#define PSF_EVENT_QUEUE_CREATE								0x11
#define PSF_EVENT_MUTEX_CREATE								0x13
#define PSF_EVENT_TIMER_CREATE								0x14
#define PSF_EVENT_EVENTGROUP_CREATE							0x15
#define PSF_EVENT_SEMAPHORE_CREATE_SUCCESS					0x16
#define PSF_EVENT_SEMAPHORE_CREATE_TIMEOUT					0x12
#define PSF_EVENT_MUTEX_RECURSIVE_CREATE					0x17
#define PSF_EVENT_STREAMBUFFER_CREATE						0x18
#define PSF_EVENT_MEMORY_SLAB_INIT_SUCCESS                  0x1C
#define PSF_EVENT_MEMORY_SLAB_INIT_FAILURE					0x29
#define PSF_EVENT_KHEAP_INIT                                0x1D

#define PSF_EVENT_QUEUE_DELETE								0x21
#define PSF_EVENT_SEMAPHORE_DELETE							0x22
#define PSF_EVENT_MUTEX_DELETE								0x23
#define PSF_EVENT_TIMER_DELETE								0x24
#define PSF_EVENT_EVENTGROUP_DELETE							0x25
#define PSF_EVENT_STREAMBUFFER_DELETE						0x28

#define PSF_EVENT_NEW_TIME									0x31
#define PSF_EVENT_NEW_TIME_SCHEDULER_SUSPENDED				0x32
#define PSF_EVENT_ISR_BEGIN									0x33
#define PSF_EVENT_ISR_RESUME								0x34
#define PSF_EVENT_TS_BEGIN									0x35
#define PSF_EVENT_TS_RESUME									0x36
#define PSF_EVENT_TASK_ACTIVATE								0x37

#define PSF_EVENT_MALLOC									0x38
#define PSF_EVENT_FREE										0x39

#define PSF_EVENT_LOWPOWER_BEGIN							0x3A
#define PSF_EVENT_LOWPOWER_END								0x3B

#define PSF_EVENT_IFE_NEXT									0x3C
#define PSF_EVENT_IFE_DIRECT								0x3D

#define PSF_EVENT_TASK_CREATE_FAILED						0x40
#define PSF_EVENT_QUEUE_CREATE_FAILED						0x41
#define PSF_EVENT_SEMAPHORE_BINARY_CREATE_FAILED			0x42
#define PSF_EVENT_MUTEX_CREATE_FAILED						0x43
#define PSF_EVENT_TIMER_CREATE_FAILED						0x44
#define PSF_EVENT_EVENTGROUP_CREATE_FAILED					0x45
#define PSF_EVENT_SEMAPHORE_COUNTING_CREATE_FAILED			0x46
#define PSF_EVENT_MUTEX_RECURSIVE_CREATE_FAILED				0x47
#define PSF_EVENT_STREAMBUFFER_CREATE_FAILED				0x49

#define PSF_EVENT_TIMER_DELETE_FAILED						0x48

#define PSF_EVENT_SEMAPHORE_RESET							0x6A
#define PSF_EVENT_SEMAPHORE_GIVE_SUCCESS					0x51
#define PSF_EVENT_SEMAPHORE_GIVE_FAILED						0x54
#define PSF_EVENT_SEMAPHORE_GIVE_BLOCKING					0x57
#define PSF_EVENT_SEMAPHORE_GIVE_FROMISR					0x5A
#define PSF_EVENT_SEMAPHORE_GIVE_FROMISR_FAILED				0x5D
#define PSF_EVENT_SEMAPHORE_TAKE_SUCCESS					0x61
#define PSF_EVENT_SEMAPHORE_TAKE_BLOCKING					0x67
#define PSF_EVENT_SEMAPHORE_TAKE_FAILED						0x64
#define PSF_EVENT_SEMAPHORE_TAKE_FROMISR_FAILED				0x6D
#define PSF_EVENT_SEMAPHORE_PEEK_SUCCESS					0x71
#define PSF_EVENT_SEMAPHORE_PEEK_FAILED						0x74
#define PSF_EVENT_SEMAPHORE_PEEK_BLOCKING					0x77

#define PSF_EVENT_MUTEX_GIVE_SUCCESS						0x52
#define PSF_EVENT_MUTEX_GIVE_FAILED							0x55
#define PSF_EVENT_MUTEX_GIVE_BLOCKING						0x58
#define PSF_EVENT_MUTEX_TAKE_SUCCESS						0x62
#define PSF_EVENT_MUTEX_TAKE_FAILED							0x65
#define PSF_EVENT_MUTEX_TAKE_BLOCKING						0x68
#define PSF_EVENT_MUTEX_PEEK_SUCCESS						0x72
#define PSF_EVENT_MUTEX_PEEK_FAILED							0x75
#define PSF_EVENT_MUTEX_PEEK_BLOCKING						0x78

#define PSF_EVENT_CONDVAR_INIT								0x6F
#define PSF_EVENT_CONDVAR_SIGNAL_SUCCESS					0x76
#define PSF_EVENT_CONDVAR_SIGNAL_BLOCKING					0x79
#define PSF_EVENT_CONDVAR_SIGNAL_FAILURE					0x7A
#define PSF_EVENT_CONDVAR_BROADCAST_ENTER					0x7D
#define PSF_EVENT_CONDVAR_BROADCAST_EXIT					0x7E
#define PSF_EVENT_CONDVAR_WAIT_SUCCESS						0x7F
#define PSF_EVENT_CONDVAR_WAIT_BLOCKING						0x80
#define PSF_EVENT_CONDVAR_WAIT_FAILURE						0x81

#define PSF_EVENT_USER_EVENT								0x90
#define PSF_EVENT_USER_EVENT_FIXED							0x98

#define PSF_EVENT_TIMER_INIT								0xA0
#define PSF_EVENT_TIMER_START								0xA1
#define PSF_EVENT_TIMER_STOP								0xA2
#define PSF_EVENT_TIMER_STATUS_SYNC_AWAIT					0xA3
#define PSF_EVENT_TIMER_STATUS_SYNC_EXIT					0xA4

#define PSF_EVENT_MAILBOX_INIT                              0x1B
#define PSF_EVENT_MAILBOX_MESSAGE_PUT_SUCCESS				0xF7
#define PSF_EVENT_MAILBOX_MESSAGE_PUT_BLOCKING				0xF8
#define PSF_EVENT_MAILBOX_MESSAGE_PUT_FAILURE				0xF9
#define PSF_EVENT_MAILBOX_PUT_SUCCESS                      	0xD3
#define PSF_EVENT_MAILBOX_PUT_BLOCKING                     	0xD5
#define PSF_EVENT_MAILBOX_PUT_FAILURE                      	0xD4
#define PSF_EVENT_MAILBOX_ASYNC_PUT_ENTER                   0xD6
#define PSF_EVENT_MAILBOX_ASYNC_PUT_EXIT					0xF6
#define PSF_EVENT_MAILBOX_GET_SUCCESS                       0xD7
#define PSF_EVENT_MAILBOX_GET_BLOCKING                      0xD9
#define PSF_EVENT_MAILBOX_GET_TIMEOUT                       0xD8
#define PSF_EVENT_MAILBOX_DATA_GET							0x89

#define PSF_EVENT_PIPE_INIT									0xA5
#define PSF_EVENT_PIPE_CLEANUP_SUCCESS						0xA6
#define PSF_EVENT_PIPE_CLEANUP_BLOCKING						0xF5
#define PSF_EVENT_PIPE_CLEANUP_FAILURE						0xA7
#define PSF_EVENT_PIPE_ALLOC_INIT_SUCCESS					0xA8
#define PSF_EVENT_PIPE_ALLOC_INIT_BLOCKING					0xF4
#define PSF_EVENT_PIPE_ALLOC_INIT_FAILURE					0xA9
#define PSF_EVENT_PIPE_PUT_SUCCESS							0xAA
#define PSF_EVENT_PIPE_PUT_BLOCKING							0xAB
#define PSF_EVENT_PIPE_PUT_TIMEOUT							0xAC
#define PSF_EVENT_PIPE_GET_SUCCESS							0xAD
#define PSF_EVENT_PIPE_GET_BLOCKING							0xAE
#define PSF_EVENT_PIPE_GET_TIMEOUT							0xAF
#define PSF_EVENT_PIPE_BLOCK_PUT_ENTER						0xDA
#define PSF_EVENT_PIPE_BLOCK_PUT_EXIT						0xED

#define PSF_EVENT_MESSAGEQUEUE_INIT                 		0x1A
#define PSF_EVENT_MESSAGEQUEUE_ALLOC_INIT_SUCCESS			0x1E
#define PSF_EVENT_MESSAGEQUEUE_ALLOC_INIT_BLOCKING			0x6B
#define PSF_EVENT_MESSAGEQUEUE_ALLOC_INIT_TIMEOUT			0x1F
#define PSF_EVENT_MESSAGEQUEUE_CLEANUP_SUCCESS				0xDC
#define PSF_EVENT_MESSAGEQUEUE_CLEANUP_BLOCKING				0x6C
#define PSF_EVENT_MESSAGEQUEUE_CLEANUP_TIMEOUT				0xDD
#define PSF_EVENT_MESSAGEQUEUE_PUT_SUCCESS                  0xDE
#define PSF_EVENT_MESSAGEQUEUE_PUT_TIMEOUT                  0xDF
#define PSF_EVENT_MESSAGEQUEUE_PUT_BLOCKING                 0xE0
#define PSF_EVENT_MESSAGEQUEUE_GET_SUCCESS                  0xE1
#define PSF_EVENT_MESSAGEQUEUE_GET_TIMEOUT                  0xE2
#define PSF_EVENT_MESSAGEQUEUE_GET_BLOCKING                 0xE3
#define PSF_EVENT_MESSAGEQUEUE_PEEK_SUCCESS					0xE4
#define PSF_EVENT_MESSAGEQUEUE_PEEK_FAILED					0xE5
#define PSF_EVENT_MESSAGEQUEUE_PURGE                        0xE6

#define PSF_EVENT_MEMORY_SLAB_ALLOC_SUCCESS                 0xE7
#define PSF_EVENT_MEMORY_SLAB_ALLOC_TIMEOUT                 0xE8
#define PSF_EVENT_MEMORY_SLAB_ALLOC_BLOCKING                0xE9
#define PSF_EVENT_MEMORY_SLAB_ALLOC_ERROR                   0xEB
#define PSF_EVENT_MEMORY_SLAB_FREE                          0xEC

#define PSF_EVENT_KHEAP_ALIGNED_ALLOC_SUCCESS               0x84
#define PSF_EVENT_KHEAP_ALIGNED_ALLOC_BLOCKING              0x86    
#define PSF_EVENT_KHEAP_ALIGNED_ALLOC_FAILURE               0x85
#define PSF_EVENT_KHEAP_ALLOC_SUCCESS						0x87						
#define PSF_EVENT_KHEAP_ALLOC_BLOCKING						0xFA
#define PSF_EVENT_KHEAP_ALLOC_FAILURE						0xFB
#define PSF_EVENT_KHEAP_FREE                                0x88
#define PSF_EVENT_KHEAP_SYS_ALIGNED_ALLOC_SUCCESS			0x50
#define PSF_EVENT_KHEAP_SYS_ALIGNED_ALLOC_BLOCKING			0x53
#define PSF_EVENT_KHEAP_SYS_ALIGNED_ALLOC_FAILURE			0x56
#define PSF_EVENT_KHEAP_SYS_MALLOC_SUCCESS					0x59
#define PSF_EVENT_KHEAP_SYS_MALLOC_BLOCKING					0x5C
#define PSF_EVENT_KHEAP_SYS_MALLOC_FAILURE					0x60
#define PSF_EVENT_KHEAP_SYS_FREE_ENTER						0x63
#define PSF_EVENT_KHEAP_SYS_FREE_EXIT						0x66
#define PSF_EVENT_KHEAP_SYS_CALLOC_SUCCESS					0x69
#define PSF_EVENT_KHEAP_SYS_CALLOC_BLOCKING					0x73
#define PSF_EVENT_KHEAP_SYS_CALLOC_FAILURE					0x70

#define PSF_EVENT_HEAP_MALLOC								0x8A
#define PSF_EVENT_HEAP_FREE									0x8B
#define PSF_EVENT_HEAP_CALLOC								0x8C

#define PSF_EVENT_UNUSED_STACK								0xEA

#define PSF_EVENT_SYSTEM_SYSCALL_ENTER						0xFE
#define PSF_EVENT_SYSTEM_SYSCALL_EXIT						0xFF

/* Forced compliance due to task API implementation */
#define PSF_EVENT_TASK_CREATE								0x0
#define PSF_EVENT_TASK_READY								PSF_EVENT_THREAD_SCHED_READY
#define PSF_EVENT_TASK_PRIORITY								PSF_EVENT_THREAD_SET_PRIORITY
#define PSF_EVENT_TASK_DELETE								PSF_EVENT_THREAD_ABORT

#define PSF_EVENT_RUNNABLE_REGISTER							0x167
#define PSF_EVENT_RUNNABLE_START							0x168
#define PSF_EVENT_RUNNABLE_STOP								0x169

#define PSF_EVENT_DEPENDENCY_REGISTER						0x16A

#define PSF_EVENT_STATEMACHINE_STATE_CREATE					0x170
#define PSF_EVENT_STATEMACHINE_CREATE						0x171
#define PSF_EVENT_STATEMACHINE_STATECHANGE					0x172
#define PSF_EVENT_INTERVAL_CREATE							0x173
#define PSF_EVENT_INTERVAL_CHANNEL_CREATE					0x174
#define PSF_EVENT_INTERVAL_CHANNEL_SET_CREATE				0x175
#define PSF_EVENT_INTERVAL_STATECHANGE						0x176
#define PSF_EVENT_INTERVAL_START							0x177
#define PSF_EVENT_INTERVAL_STOP								0x178
#define PSF_EVENT_COUNTER_CREATE							0x179
#define PSF_EVENT_COUNTER_CHANGE							0x17A
#define PSF_EVENT_COUNTER_LIMIT_EXCEEDED					0x17B

#define PSF_EVENT_MALLOC_FAILED 							0x17C
#define PSF_EVENT_FREE_FAILED 								0x17D
#define PSF_EVENT_EXTENSION_CREATE							0x17E
#define PSF_EVENT_HEAP_CREATE								0x17F

#define TRC_EVENT_LAST_ID									(PSF_EVENT_HEAP_CREATE)


#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_H */
