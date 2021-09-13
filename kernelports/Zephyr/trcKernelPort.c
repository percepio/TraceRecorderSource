/*
 * Trace Recorder for Tracealyzer v4.5.1
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The Zephyr specific parts of the trace recorder
 */

#include <init.h>
#include <kernel.h>
#include <string.h>
#include "trcKernelPort.h"
#include "trcRecorder.h"
#include "trcSDK.h"
#include "trcInternalBuffer.h"


/* Streaming specific macros */
#if ((TRC_CFG_RECORDER_MODE) == TRC_RECORDER_MODE_STREAMING)
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

		#if !defined(CONFIG_HEAP_MEM_POOL_SIZE) || ((CONFIG_HEAP_MEM_POOL_SIZE) < ((TRC_CFG_RTT_BUFFER_SIZE_UP) + (TRC_CFG_RTT_BUFFER_SIZE_DOWN)))
			#error "Tracerecorder: You have choosen the TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC option without allocating enough memory for the KERNEL_MEM_POOL in Zephyr"
		#endif

		#define TRC_PORT_MALLOC(size) k_malloc(size)
	#endif
#endif

#if ((TRC_CFG_RECORDER_MODE) == TRC_RECORDER_MODE_STREAMING || ((TRC_CFG_ENABLE_STACK_MONITOR) == 1) && (TRC_CFG_SCHEDULING_ONLY == 0))

/* Trace recorder controll thread stack */
static K_THREAD_STACK_DEFINE(TzCtrl_thread_stack, (TRC_CFG_CTRL_TASK_STACK_SIZE));

/* Trace recorder controll thread entry function */
extern void TzCtrl_thread_entry(void *_args);

/* Trace recorder controll thread handle */
static struct k_thread HandleTzCtrl;

/* Trace recorder current thread handle */
static struct k_thread *CurrentThread = NULL;

/* Generic Zephyr ISR handle */
static traceHandle HandleISR;

#endif /* ((TRC_CFG_RECORDER_MODE) == TRC_RECORDER_MODE_STREAMING || ((TRC_CFG_ENABLE_STACK_MONITOR) == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)) */

/* Monitored by TzCtrl task, that give warnings as User Events */
extern volatile uint32_t NoRoomForSymbol;
extern volatile uint32_t NoRoomForObjectData;
extern volatile uint32_t LongestSymbolName;
extern volatile uint32_t MaxBytesTruncated;
#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)
extern volatile uint32_t TaskStacksNotIncluded;
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */

/* User Event Channel for giving warnings regarding NoRoomForSymbol etc. */
traceString trcWarningChannel;

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && ((TRC_CFG_ENABLE_STACK_MONITOR) == 1)

uint32_t prvTraceGetStackHighWaterMark(void* task)
{
	size_t unusedStackSpace = 0;
	k_thread_stack_space_get((struct k_thread*)task, &unusedStackSpace);
	
	return unusedStackSpace;
}

#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && ((TRC_CFG_ENABLE_STACK_MONITOR) == 1) */

/*
 * prvCheckRecorderStatus
 *
 * Called by TzCtrl task periodically (every 100 ms - seems reasonable).
 * Checks a number of diagnostic variables and give warnings as user events,
 * in most cases including a suggested solution.
 */
static void prvCheckRecorderStatus(void)
{
#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)
	if (TaskStacksNotIncluded > 0)
	{
		prvTraceWarning(PSF_WARNING_STACKMON_NO_SLOTS);
		TaskStacksNotIncluded = 0;
	}
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */

	if (NoRoomForSymbol > 0)
	{
		prvTraceWarning(PSF_WARNING_SYMBOL_TABLE_SLOTS);
		NoRoomForSymbol = 0;
	}

	if (NoRoomForObjectData > 0)
	{
		prvTraceWarning(PSF_WARNING_OBJECT_DATA_SLOTS);
		NoRoomForObjectData = 0;
	}

	if (LongestSymbolName > (TRC_CFG_SYMBOL_MAX_LENGTH))
	{
		prvTraceWarning(PSF_WARNING_SYMBOL_MAX_LENGTH);
		LongestSymbolName = 0;
	}

	if (MaxBytesTruncated > 0)
	{
		prvTraceWarning(PSF_WARNING_STRING_TOO_LONG);
		MaxBytesTruncated = 0;
	}
}


/* Snapshot kernel port */
#if ((TRC_CFG_RECORDER_MODE) == TRC_RECORDER_MODE_SNAPSHOT)
#endif // TRC_RECORDER_MODE_SNAPSHOT == 1


/* Streaming kernel port */
#if ((TRC_CFG_RECORDER_MODE) == TRC_RECORDER_MODE_STREAMING)

/* Thread trace function definitions */
void sys_trace_k_thread_foreach_enter(k_thread_user_cb_t user_cb, void *user_data) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_FOREACH_ENTER, 8);
	xTraceSDKEventAdd32((uint32_t)user_cb);
	xTraceSDKEventAdd32((uint32_t)user_data);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_foreach_exit(k_thread_user_cb_t user_cb, void *user_data) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_FOREACH_EXIT, 8);
	xTraceSDKEventAdd32((uint32_t)user_cb);
	xTraceSDKEventAdd32((uint32_t)user_data);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_foreach_unlocked_enter(k_thread_user_cb_t user_cb, void *user_data) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_FOREACH_UNLOCKED_ENTER, 8);
	xTraceSDKEventAdd32((uint32_t)user_cb);
	xTraceSDKEventAdd32((uint32_t)user_data);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_foreach_unlocked_exit(k_thread_user_cb_t user_cb, void *user_data) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_FOREACH_UNLOCKED_EXIT, 8);
	xTraceSDKEventAdd32((uint32_t)user_cb);
	xTraceSDKEventAdd32((uint32_t)user_data);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_create(struct k_thread *thread, size_t stack_size, int prio) {
#ifdef CONFIG_THREAD_NAME
	if (thread->name != NULL && strlen(thread->name) > 0) {
		xTraceSDKSetObjectName(thread, thread->name);
	}
#endif
	xTraceSDKSetObjectData((void*)thread, prio);

	prvAddTaskToStackMonitor((void*)thread);

	xTraceSDKEventBegin(PSF_EVENT_THREAD_INIT, 12);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventAdd32((uint32_t)stack_size);
	xTraceSDKEventAdd32((uint32_t)prio);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_user_mode_enter(k_thread_entry_t entry, void *p1, void *p2, void *p3) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_USER_MODE_ENTER, 20);
	xTraceSDKEventAddObject((void*)k_current_get());
	xTraceSDKEventAdd32((uint32_t)entry);
	xTraceSDKEventAdd32((uint32_t)p1);
	xTraceSDKEventAdd32((uint32_t)p2);
	xTraceSDKEventAdd32((uint32_t)p3);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_heap_assign(struct k_thread *thread, struct k_heap *heap) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_HEAP_ASSIGN, 8);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventAdd32((uint32_t)heap);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_join_blocking(struct k_thread *thread, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_JOIN_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_join_exit(struct k_thread *thread, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_THREAD_JOIN_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_THREAD_JOIN_TIMEOUT, 12);
	}

	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_sleep_enter(k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_SLEEP_ENTER, 8);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_sleep_exit(k_timeout_t timeout, int ret) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_SLEEP_EXIT, 8);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_msleep_enter(int32_t ms) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_MSLEEP_ENTER, 4);
	xTraceSDKEventAdd32(ms);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_msleep_exit(int32_t ms, int ret) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_MSLEEP_EXIT, 4);
	xTraceSDKEventAdd32(ms);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_usleep_enter(int32_t us) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_USLEEP_ENTER, 8);
	xTraceSDKEventAdd32(us);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_usleep_exit(int32_t us, int ret) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_USLEEP_EXIT, 8);
	xTraceSDKEventAdd32(us);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_busy_wait_enter(uint32_t usec_to_wait) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_BUSY_WAIT_ENTER, 4);
	xTraceSDKEventAdd32(usec_to_wait);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_busy_wait_exit(uint32_t usec_to_wait) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_BUSY_WAIT_EXIT, 4);
	xTraceSDKEventAdd32(usec_to_wait);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_yield() {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_YIELD, 0);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_wakeup(struct k_thread *thread) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_WAKEUP, 4);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_abort(struct k_thread *thread) {
	prvRemoveTaskFromStackMonitor(thread);

	xTraceSDKEventBegin(PSF_EVENT_THREAD_ABORT, 4);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_start(struct k_thread *thread) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_START, 4);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_priority_set(struct k_thread *thread) {
	xTraceSDKSetObjectData((void*)thread, k_thread_priority_get(thread));

	xTraceSDKEventBegin(PSF_EVENT_THREAD_SET_PRIORITY, 8);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventAdd32(k_thread_priority_get(thread));
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_suspend(struct k_thread *thread) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_SUSPEND, 4);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_resume(struct k_thread *thread) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_RESUME, 4);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_name_set(struct k_thread *thread, int ret) {
	if (ret == 0) {
		xTraceSDKSetObjectName(thread, thread->name);
	}
}

void sys_trace_k_thread_switched_out(void) {
}

void sys_trace_k_thread_switched_in(void) {
	/* Due to how Zephyr implements this trace hook it gets called every time that
	 * an interrupt is disabled. As such, when we enable/disable interrupts for tracing
	 * it results in switched in calls for every event that we trace. To solve this we use
	 * this hack which is unsafe in a multicore environment
	 */
	if (k_current_get() != CurrentThread) {
		CurrentThread = k_current_get();
		xTraceSDKEventBegin(PSF_EVENT_TASK_ACTIVATE, 8);
		xTraceSDKEventAddObject((void*)k_current_get());
		xTraceSDKEventAdd32((uint32_t)k_thread_priority_get(k_current_get()));
		xTraceSDKEventEnd();
	}
}

void sys_trace_k_thread_info(struct k_thread *thread) {
}


/* Thread sceduler trace function definitions */
void sys_trace_k_thread_sched_lock() {
}

void sys_trace_k_thread_sched_unlock() {
}

void sys_trace_k_thread_sched_wakeup(struct k_thread *thread) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_SCHED_WAKEUP, 4);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_sched_abort(struct k_thread *thread) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_SCHED_ABORT, 4);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_sched_set_priority(struct k_thread *thread, int prio) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_SCHED_PRIORITY_SET, 8);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventAdd32(prio);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_sched_ready(struct k_thread *thread) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_SCHED_READY, 4);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_sched_pend(struct k_thread *thread) {
	
}

void sys_trace_k_thread_sched_resume(struct k_thread *thread) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_SCHED_RESUME, 4);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventEnd();
}

void sys_trace_k_thread_sched_suspend(struct k_thread *thread) {
	xTraceSDKEventBegin(PSF_EVENT_THREAD_SCHED_SUSPEND, 4);
	xTraceSDKEventAddObject((void*)thread);
	xTraceSDKEventEnd();
}


/* Work trace function definitions */
void sys_trace_k_work_init(struct k_work *work, k_work_handler_t handler) {
	xTraceSDKEventBegin(PSF_EVENT_WORK_INIT, 8);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventAdd32((uint32_t)handler);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_submit_to_queue_enter(struct k_work_q *queue, struct k_work *work) {
	xTraceSDKEventBegin(PSF_EVENT_WORK_SUBMIT_TO_QUEUE_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_submit_to_queue_exit(struct k_work_q *queue, struct k_work *work, int ret) {
	if (ret >= 0) {
		xTraceSDKEventBegin(PSF_EVENT_WORK_SUBMIT_TO_QUEUE_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_WORK_SUBMIT_TO_QUEUE_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_submit_enter(struct k_work *work) {
	xTraceSDKEventBegin(PSF_EVENT_WORK_SUBMIT_BLOCKING, 4);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_submit_exit(struct k_work *work, int ret) {
	if (ret >= 0) {
		xTraceSDKEventBegin(PSF_EVENT_WORK_SUBMIT_SUCCESS, 8);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_WORK_SUBMIT_FAILURE, 8);
	}

	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_flush_enter(struct k_work *work, struct k_work_sync *sync) {
	xTraceSDKEventBegin(PSF_EVENT_WORK_FLUSH_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventAdd32((uint32_t)sync);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_flush_blocking(struct k_work *work, struct k_work_sync *sync, k_timeout_t timeout) {
	
}

void sys_trace_k_work_flush_exit(struct k_work *work, struct k_work_sync *sync, bool ret) {
	xTraceSDKEventBegin(PSF_EVENT_WORK_FLUSH_SUCCESS, 12);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventAdd32((uint32_t)sync);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_cancel_enter(struct k_work *work) {
	xTraceSDKEventBegin(PSF_EVENT_WORK_CANCEL_BLOCKING, 4);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_cancel_exit(struct k_work *work, int ret) {
	xTraceSDKEventBegin(PSF_EVENT_WORK_CANCEL_SUCCESS, 8);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_cancel_sync_enter(struct k_work *work, struct k_work_sync *sync) {
	xTraceSDKEventBegin(PSF_EVENT_WORK_CANCEL_SYNC_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventAdd32((uint32_t)sync);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_cancel_sync_blocking(struct k_work *work, struct k_work_sync *sync) {
	
}

void sys_trace_k_work_cancel_sync_exit(struct k_work *work, struct k_work_sync *sync, bool ret) {
	xTraceSDKEventBegin(PSF_EVENT_WORK_CANCEL_SYNC_SUCCESS, 12);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventAdd32((uint32_t)sync);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}


/* Work queue trace function definitions */
void sys_trace_k_work_queue_start_enter(struct k_work_q *queue, k_thread_stack_t *stack, size_t stack_size, int prio, const struct k_work_queue_config *cfg) {
	xTraceSDKEventBegin(PSF_EVENT_WORK_QUEUE_START_BLOCKING, 20);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAddObject((void*)stack);
	xTraceSDKEventAdd32(stack_size);
	xTraceSDKEventAdd32(prio);
	xTraceSDKEventAdd32((uint32_t)cfg);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_queue_start_exit(struct k_work_q *queue, k_thread_stack_t *stack, size_t stack_size, int prio, const struct k_work_queue_config *cfg) {
	xTraceSDKEventBegin(PSF_EVENT_WORK_QUEUE_START_SUCCESS, 20);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAddObject((void*)stack);
	xTraceSDKEventAdd32(stack_size);
	xTraceSDKEventAdd32(prio);
	xTraceSDKEventAdd32((uint32_t)cfg);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_queue_drain_enter(struct k_work_q *queue, bool plug) {
	xTraceSDKEventBegin(PSF_EVENT_WORK_QUEUE_DRAIN_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32(plug);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_queue_drain_exit(struct k_work_q *queue, bool plug, int ret) {
	if (ret >= 0) {
		xTraceSDKEventBegin(PSF_EVENT_WORK_QUEUE_DRAIN_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_WORK_QUEUE_DRAIN_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32(plug);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_queue_unplug_enter(struct k_work_q *queue) {
	xTraceSDKEventBegin(PSF_EVENT_WORK_QUEUE_UNPLUG_BLOCKING, 4);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_queue_unplug_exit(struct k_work_q *queue, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_WORK_QUEUE_UNPLUG_SUCCESS, 8);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_WORK_QUEUE_UNPLUG_FAILURE, 8);
	}

	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}


/* Work delayable trace function definitions */
void sys_trace_k_work_delayable_init(struct k_work_delayable *dwork, k_work_handler_t handler) {
	xTraceSDKEventBegin(PSF_EVENT_DWORK_INIT, 8);
	xTraceSDKEventAddObject((void*)dwork);
	xTraceSDKEventAdd32((uint32_t)handler);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_schedule_for_queue_enter(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay) {
	//prvTraceStoreEvent3(PSF_EVENT_DWORK_SCHEDULE_FOR_QUEUE_BLOCKING, (uint32_t)queue, (uint32_t)dwork, delay.ticks);
}

void sys_trace_k_work_schedule_for_queue_exit(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay, int ret) {
	xTraceSDKEventBegin(PSF_EVENT_DWORK_SCHEDULE_FOR_QUEUE_SUCCESS, 16);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAddObject((void*)dwork);
	xTraceSDKEventAdd32(delay.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_schedule_enter(struct k_work_delayable *dwork, k_timeout_t delay) {
	xTraceSDKEventBegin(PSF_EVENT_DWORK_SCHEDULE_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)dwork);
	xTraceSDKEventAdd32(delay.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_schedule_exit(struct k_work_delayable *dwork, k_timeout_t delay, int ret) {
	xTraceSDKEventBegin(PSF_EVENT_DWORK_SCHEDULE_SUCCESS, 12);
	xTraceSDKEventAddObject((void*)dwork);
	xTraceSDKEventAdd32(delay.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_reschedule_for_queue_enter(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay) {
	//prvTraceStoreEvent3(PSF_EVENT_DWORK_RESCHEDULE_FOR_QUEUE_BLOCKING, (uint32_t)queue, (uint32_t)dwork, delay.ticks);
}

void sys_trace_k_work_reschedule_for_queue_exit(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay, int ret) {
	xTraceSDKEventBegin(PSF_EVENT_DWORK_RESCHEDULE_FOR_QUEUE_SUCCESS, 16);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAddObject((void*)dwork);
	xTraceSDKEventAdd32(delay.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_reschedule_enter(struct k_work_delayable *dwork, k_timeout_t delay) {
	xTraceSDKEventBegin(PSF_EVENT_DWORK_RESCHEDULE_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)dwork);
	xTraceSDKEventAdd32(delay.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_reschedule_exit(struct k_work_delayable *dwork, k_timeout_t delay, int ret) {
	xTraceSDKEventBegin(PSF_EVENT_DWORK_RESCHEDULE_SUCCESS, 12);
	xTraceSDKEventAddObject((void*)dwork);
	xTraceSDKEventAdd32(delay.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_flush_delayable_enter(struct k_work_delayable *dwork, struct k_work_sync *sync) {
	xTraceSDKEventBegin(PSF_EVENT_DWORK_FLUSH_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)dwork);
	xTraceSDKEventAdd32((uint32_t)sync);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_flush_delayable_exit(struct k_work_delayable *dwork, struct k_work_sync *sync, bool ret) {
	xTraceSDKEventBegin(PSF_EVENT_DWORK_FLUSH_SUCCESS, 12);
	xTraceSDKEventAddObject((void*)dwork);
	xTraceSDKEventAdd32((uint32_t)sync);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_cancel_delayable_enter(struct k_work_delayable *dwork) {
	//prvTraceStoreEvent1(PSF_EVENT_DWORK_CANCEL_DELAYABLE_BLOCKING, (uint32_t)dwork);
}

void sys_trace_k_work_cancel_delayable_exit(struct k_work_delayable *dwork, int ret) {
	xTraceSDKEventBegin(PSF_EVENT_DWORK_CANCEL_DELAYABLE_SUCCESS, 8);
	xTraceSDKEventAddObject((void*)dwork);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_cancel_delayable_sync_enter(struct k_work_delayable *dwork, struct k_work_sync *sync) {
	xTraceSDKEventBegin(PSF_EVENT_DWORK_CANCEL_DELAYABLE_SYNC_BLOCKING, 12);
	xTraceSDKEventAddObject((void*)dwork);
	xTraceSDKEventAdd32((uint32_t)sync);
	xTraceSDKEventEnd();
}

void sys_trace_cancel_delayable_sync_exit(struct k_work_delayable *dwork, struct k_work_sync *sync, bool ret) {
	xTraceSDKEventBegin(PSF_EVENT_DWORK_CANCEL_DELAYABLE_SYNC_SUCCESS, 12);
	xTraceSDKEventAddObject((void*)dwork);
	xTraceSDKEventAdd32((uint32_t)sync);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}


/* Work poll trace function definitions */
void sys_trace_k_work_poll_init_enter(struct k_work_poll *work, k_work_handler_t handler) {
	
}

void sys_trace_k_work_poll_init_exit(struct k_work_poll *work, k_work_handler_t handler) {
	xTraceSDKEventBegin(PSF_EVENT_PWORK_INIT, 8);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventAdd32((uint32_t)handler);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_poll_submit_to_queue_enter(struct k_work_q *work_q, struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_PWORK_SUBMIT_TO_QUEUE_BLOCKING, 20);
	xTraceSDKEventAddObject((void*)work_q);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventAdd32((uint32_t)events);
	xTraceSDKEventAdd32(num_events);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_poll_submit_to_queue_blocking(struct k_work_q *work_q, struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout) {

}

void sys_trace_k_work_poll_submit_to_queue_exit(struct k_work_q *work_q, struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_PWORK_SUBMIT_TO_QUEUE_SUCCESS, 24);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_PWORK_SUBMIT_TO_QUEUE_FAILURE, 24);
	}

	xTraceSDKEventAddObject((void*)work_q);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventAdd32((uint32_t)events);
	xTraceSDKEventAdd32(num_events);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_poll_submit_enter(struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_PWORK_SUBMIT_BLOCKING, 16);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventAdd32((uint32_t)events);
	xTraceSDKEventAdd32(num_events);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_poll_submit_exit(struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_PWORK_SUBMIT_SUCCESS, 20);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_PWORK_SUBMIT_FAILURE, 20);
	}

	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventAdd32((uint32_t)events);
	xTraceSDKEventAdd32(num_events);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_poll_cancel_enter(struct k_work_poll *work) {
	xTraceSDKEventBegin(PSF_EVENT_PWORK_CANCEL_BLOCKING, 4);
	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventEnd();
}

void sys_trace_k_work_poll_cancel_exit(struct k_work_poll *work, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_PWORK_CANCEL_SUCCESS, 8);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_PWORK_CANCEL_FAILURE, 8);
	}

	xTraceSDKEventAddObject((void*)work);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}


/* Poll API trace function definitions */
void sys_trace_k_poll_api_event_init(struct k_poll_event *event, uint32_t type, int mode, void *obj) {
	xTraceSDKEventBegin(PSF_EVENT_POLL_EVENT_INIT, 16);
	xTraceSDKEventAddObject((void*)event);
	xTraceSDKEventAdd32(type);
	xTraceSDKEventAdd32(mode);
	xTraceSDKEventAddObject((void*)obj);
	xTraceSDKEventEnd();
}

void sys_trace_k_poll_api_event_poll_enter(struct k_poll_event *events, int num_events, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_POLL_POLL_BLOCKING, 12);
	xTraceSDKEventAdd32((uint32_t)events);
	xTraceSDKEventAdd32(num_events);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_poll_api_event_poll_exit(struct k_poll_event *events, int num_events, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_POLL_POLL_SUCCESS, 16);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_POLL_POLL_FAILURE, 16);
	}

	xTraceSDKEventAdd32((uint32_t)events);
	xTraceSDKEventAdd32(num_events);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_poll_api_signal_init(struct k_poll_signal *signal) {
	xTraceSDKEventBegin(PSF_EVENT_POLL_SIGNAL_INIT, 4);
	xTraceSDKEventAddObject((void*)signal);
	xTraceSDKEventEnd();
}

void sys_trace_k_poll_api_signal_reset(struct k_poll_signal *signal) {
	xTraceSDKEventBegin(PSF_EVENT_POLL_SIGNAL_RESET, 4);
	xTraceSDKEventAddObject((void*)signal);
	xTraceSDKEventEnd();
}

void sys_trace_k_poll_api_signal_check(struct k_poll_signal *signal, unsigned int *signaled, int *result) {
	xTraceSDKEventBegin(PSF_EVENT_POLL_SIGNAL_CHECK, 12);
	xTraceSDKEventAddObject((void*)signal);
	xTraceSDKEventAdd32((uint32_t)signaled);
	xTraceSDKEventAdd32((uint32_t)result);
	xTraceSDKEventEnd();
}

void sys_trace_k_poll_api_signal_raise(struct k_poll_signal *signal, int result, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_POLL_SIGNAL_RAISE_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_POLL_SIGNAL_RAISE_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)signal);
	xTraceSDKEventAdd32(result);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}


/* Semaphore trace function definitions */
void sys_trace_k_sem_init(struct k_sem *sem, uint32_t initial_count, uint32_t limit, int ret) {
	if (ret == 0) {
		//prvTraceSaveObjectData(sem, initial_count);
		xTraceSDKEventBegin(PSF_EVENT_SEMAPHORE_CREATE_SUCCESS, 20);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_SEMAPHORE_CREATE_TIMEOUT, 20);
	}

	xTraceSDKEventAddObject((void*)sem);
	xTraceSDKEventAdd32(initial_count);
	xTraceSDKEventAdd32(limit);
	xTraceSDKEventAdd32(sem->count);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_sem_give_enter(struct k_sem *sem) {
	xTraceSDKEventBegin(PSF_EVENT_SEMAPHORE_GIVE, 8);
	xTraceSDKEventAddObject((void*)sem);
	xTraceSDKEventAdd32(sem->count);
	xTraceSDKEventEnd();
}

void sys_trace_k_sem_take_enter(struct k_sem *sem, k_timeout_t timeout) {

}

void sys_trace_k_sem_take_blocking(struct k_sem *sem, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_SEMAPHORE_TAKE_BLOCK, 12);
	xTraceSDKEventAddObject((void*)sem);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(sem->count);
	xTraceSDKEventEnd();
}

void sys_trace_k_sem_take_exit(struct k_sem *sem, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_SEMAPHORE_TAKE, 16);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_SEMAPHORE_TAKE_FAILED, 16);
	}

	xTraceSDKEventAddObject((void*)sem);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(sem->count);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_sem_reset(struct k_sem *sem) {
	xTraceSDKEventBegin(PSF_EVENT_SEMAPHORE_RESET, 4);
	xTraceSDKEventAddObject((void*)sem);
	xTraceSDKEventEnd();
}


/* Mutex trace function definitions */
void sys_trace_k_mutex_init(struct k_mutex *mutex, int ret) {
	xTraceSDKEventBegin(PSF_EVENT_MUTEX_CREATE, 8);
	xTraceSDKEventAddObject((void*)mutex);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_mutex_lock_enter(struct k_mutex *mutex, k_timeout_t timeout) {
}

void sys_trace_k_mutex_lock_blocking(struct k_mutex *mutex, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_MUTEX_TAKE_BLOCK, 8);
	xTraceSDKEventAddObject((void*)mutex);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_mutex_lock_exit(struct k_mutex *mutex, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_MUTEX_TAKE, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_MUTEX_TAKE_FAILED, 12);
	}

	xTraceSDKEventAddObject((void*)mutex);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_mutex_unlock_enter(struct k_mutex *mutex) {
}

void sys_trace_k_mutex_unlock_exit(struct k_mutex *mutex, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_MUTEX_GIVE, 8);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_MUTEX_GIVE_FAILED, 8);
	}

	xTraceSDKEventAddObject((void*)mutex);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}


/* Conditional variable trace function definitions */
void sys_trace_k_condvar_init(struct k_condvar *condvar, int ret) {
	xTraceSDKEventBegin(PSF_EVENT_CONDVAR_INIT, 8);
	xTraceSDKEventAddObject((void*)condvar);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_condvar_signal_enter(struct k_condvar *condvar) {

}

void sys_trace_k_condvar_signal_blocking(struct k_condvar *condvar) {
	xTraceSDKEventBegin(PSF_EVENT_CONDVAR_SIGNAL_BLOCKING, 4);
	xTraceSDKEventAddObject((void*)condvar);
	xTraceSDKEventEnd();
}

void sys_trace_k_condvar_signal_exit(struct k_condvar *condvar, int ret) {
	xTraceSDKEventBegin(PSF_EVENT_CONDVAR_SIGNAL_SUCCESS, 8);
	xTraceSDKEventAddObject((void*)condvar);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_condvar_broadcast_enter(struct k_condvar *condvar) {
	xTraceSDKEventBegin(PSF_EVENT_CONDVAR_BROADCAST_ENTER, 4);
	xTraceSDKEventAddObject((void*)condvar);
	xTraceSDKEventEnd();
}

void sys_trace_k_condvar_broadcast_exit(struct k_condvar *condvar, int ret) {
	xTraceSDKEventBegin(PSF_EVENT_CONDVAR_BROADCAST_EXIT, 8);
	xTraceSDKEventAddObject((void*)condvar);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_condvar_wait_enter(struct k_condvar *condvar, struct k_mutex *mutex, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_CONDVAR_WAIT_BLOCKING, 12);
	xTraceSDKEventAddObject((void*)condvar);
	xTraceSDKEventAddObject((void*)mutex);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_condvar_wait_exit(struct k_condvar *condvar, struct k_mutex *mutex, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_CONDVAR_WAIT_SUCCESS, 16);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_CONDVAR_WAIT_FAILURE, 16);
	}

	xTraceSDKEventAddObject((void*)condvar);
	xTraceSDKEventAddObject((void*)mutex);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}


/* Queue trace function definitions */
void sys_trace_k_queue_init(struct k_queue *queue) {
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_INIT, 4);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_cancel_wait(struct k_queue *queue) {
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_CANCEL_WAIT, 4);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_queue_insert_enter(struct k_queue *queue, bool alloc, void *data) {
	// Ignore non alloc tracing of this event
	if (!alloc) {
		return;
	}
}

void sys_trace_k_queue_queue_insert_blocking(struct k_queue *queue, bool alloc, void *data) {
	// Ignore non alloc tracing of this event
	if (!alloc) {
		return;
	}
	
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_QUEUE_INSERT_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_queue_insert_exit(struct k_queue *queue, bool alloc, void *data, int ret) {
	// Ignore non alloc tracing of this event
	if (!alloc) {
		return;
	}

	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_QUEUE_INSERT_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_QUEUE_INSERT_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_append_enter(struct k_queue *queue, void *data) {
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_APPEND, 8);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_append_exit(struct k_queue *queue, void *data) {
}

void sys_trace_k_queue_alloc_append_enter(struct k_queue *queue, void *data) {
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_ALLOC_APPEND_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_alloc_append_exit(struct k_queue *queue, void *data, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_ALLOC_APPEND_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_ALLOC_APPEND_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_prepend_enter(struct k_queue *queue, void *data) {
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_PREPEND, 8);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_prepend_exit(struct k_queue *queue, void *data) {
}

void sys_trace_k_queue_alloc_prepend_enter(struct k_queue *queue, void *data) {
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_ALLOC_PREPEND_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_alloc_prepend_exit(struct k_queue *queue, void *data, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_ALLOC_PREPEND_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_ALLOC_PREPEND_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_insert_enter(struct k_queue *queue, void *prev, void *data) {
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_INSERT, 12);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)prev);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_insert_exit(struct k_queue *queue, void *prev, void *data) {
}

void sys_trace_k_queue_append_list_enter(struct k_queue *queue, void *head, void *tail) {
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_APPEND_LIST_BLOCKING, 12);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)head);
	xTraceSDKEventAdd32((uint32_t)tail);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_append_list_exit(struct k_queue *queue, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_APPEND_LIST_SUCCESS, 16);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_APPEND_LIST_FAILURE, 16);
	}
	
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_merge_slist_enter(struct k_queue *queue, sys_slist_t *list) {
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_MERGE_SLIST_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)list);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_merge_slist_exit(struct k_queue *queue, sys_slist_t *list, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_MERGE_SLIST_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_MERGE_SLIST_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)list);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_get_blocking(struct k_queue *queue, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_GET_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_get_exit(struct k_queue *queue, k_timeout_t timeout, void *ret) {
	if (ret != NULL) {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_GET_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_GET_TIMEOUT, 12);
	}

	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32((uint32_t)ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_remove_enter(struct k_queue *queue, void *data) {
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_REMOVE_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_remove_exit(struct k_queue *queue, void *data, bool ret) {
	if (ret) {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_REMOVE_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_REMOVE_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_unique_append_enter(struct k_queue *queue, void *data) {
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_UNIQUE_APPEND_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_unique_append_exit(struct k_queue *queue, void *data, bool ret) {
	if (ret) {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_UNIQUE_APPEND_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_QUEUE_UNIQUE_APPEND_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_peek_head(struct k_queue *queue, void *ret) {
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_PEEK_HEAD, 8);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_queue_peek_tail(struct k_queue *queue, void *ret) {
	xTraceSDKEventBegin(PSF_EVENT_QUEUE_PEEK_TAIL, 8);
	xTraceSDKEventAddObject((void*)queue);
	xTraceSDKEventAdd32((uint32_t)ret);
	xTraceSDKEventEnd();
}


/* FIFO trace function definitions */
void sys_trace_k_fifo_init_enter(struct k_fifo *fifo) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_INIT_ENTER, 4);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_init_exit(struct k_fifo *fifo) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_INIT_EXIT, 4);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_cancel_wait_enter(struct k_fifo *fifo) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_CANCEL_WAIT_ENTER, 4);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_cancel_wait_exit(struct k_fifo *fifo) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_CANCEL_WAIT_EXIT, 4);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_put_enter(struct k_fifo *fifo, void *data) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_PUT_ENTER, 8);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_put_exit(struct k_fifo *fifo, void *data) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_PUT_EXIT, 8);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_alloc_put_enter(struct k_fifo *fifo, void *data) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_ALLOC_PUT_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_alloc_put_exit(struct k_fifo *fifo, void *data, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_FIFO_ALLOC_PUT_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_FIFO_ALLOC_PUT_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_put_list_enter(struct k_fifo *fifo, void *head, void *tail) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_PUT_LIST_ENTER, 12);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventAdd32((uint32_t)head);
	xTraceSDKEventAdd32((uint32_t)tail);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_put_list_exit(struct k_fifo *fifo, void *head, void *tail) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_PUT_LIST_EXIT, 12);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventAdd32((uint32_t)head);
	xTraceSDKEventAdd32((uint32_t)tail);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_put_slist_enter(struct k_fifo *fifo, sys_slist_t *list) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_PUT_SLIST_ENTER, 8);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventAdd32((uint32_t)list);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_put_slist_exit(struct k_fifo *fifo, sys_slist_t *list) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_PUT_SLIST_EXIT, 8);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventAdd32((uint32_t)list);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_get_enter(struct k_fifo *fifo, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_GET_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_get_exit(struct k_fifo *fifo, k_timeout_t timeout, void *ret) {
	if (ret != NULL) {
		xTraceSDKEventBegin(PSF_EVENT_FIFO_GET_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_FIFO_GET_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32((uint32_t)ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_peek_head_enter(struct k_fifo *fifo) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_PEEK_HEAD_ENTER, 4);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_peek_head_exit(struct k_fifo *fifo, void *ret) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_PEEK_HEAD_EXIT, 8);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventAdd32((uint32_t)ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_peek_tail_enter(struct k_fifo *fifo) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_PEEK_TAIL_ENTER, 4);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventEnd();
}

void sys_trace_k_fifo_peek_tail_exit(struct k_fifo *fifo, void *ret) {
	xTraceSDKEventBegin(PSF_EVENT_FIFO_PEEK_TAIL_EXIT, 8);
	xTraceSDKEventAddObject((void*)fifo);
	xTraceSDKEventAdd32((uint32_t)ret);
	xTraceSDKEventEnd();
}


/* LIFO trace function definitions */
void sys_trace_k_lifo_init_enter(struct k_lifo *lifo) {
	xTraceSDKEventBegin(PSF_EVENT_LIFO_INIT_ENTER, 4);
	xTraceSDKEventAddObject((void*)lifo);
	xTraceSDKEventEnd();
}

void sys_trace_k_lifo_init_exit(struct k_lifo *lifo) {
	xTraceSDKEventBegin(PSF_EVENT_LIFO_INIT_EXIT, 4);
	xTraceSDKEventAddObject((void*)lifo);
	xTraceSDKEventEnd();
}

void sys_trace_k_lifo_put_enter(struct k_lifo *lifo, void *data) {
	xTraceSDKEventBegin(PSF_EVENT_LIFO_PUT_ENTER, 8);
	xTraceSDKEventAddObject((void*)lifo);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_lifo_put_exit(struct k_lifo *lifo, void *data) {
	xTraceSDKEventBegin(PSF_EVENT_LIFO_PUT_EXIT, 8);
	xTraceSDKEventAddObject((void*)lifo);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_lifo_alloc_put_enter(struct k_lifo *lifo, void *data) {
	xTraceSDKEventBegin(PSF_EVENT_LIFO_ALLOC_PUT_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)lifo);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_lifo_alloc_put_exit(struct k_lifo *lifo, void *data, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_LIFO_ALLOC_PUT_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_LIFO_ALLOC_PUT_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)lifo);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_lifo_get_enter(struct k_lifo *lifo, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_LIFO_GET_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)lifo);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_lifo_get_exit(struct k_lifo *lifo, k_timeout_t timeout, void *ret) {
	if (ret != NULL) {
		xTraceSDKEventBegin(PSF_EVENT_LIFO_GET_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_LIFO_GET_FAILURE, 12);
	}
	
	xTraceSDKEventAddObject((void*)lifo);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32((uint32_t)ret);
	xTraceSDKEventEnd();
}


/* Stack trace function definitions */
void sys_trace_k_stack_init(struct k_stack *stack, stack_data_t *buffer, uint32_t num_entries) {
	xTraceSDKEventBegin(PSF_EVENT_STACK_INIT, 12);
	xTraceSDKEventAddObject((void*)stack);
	xTraceSDKEventAdd32((uint32_t)buffer);
	xTraceSDKEventAdd32(num_entries);
	xTraceSDKEventEnd();
}

void sys_trace_k_stack_alloc_init_enter(struct k_stack *stack, uint32_t num_entries) {
	xTraceSDKEventBegin(PSF_EVENT_STACK_ALLOC_INIT_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)stack);
	xTraceSDKEventAdd32(num_entries);
	xTraceSDKEventEnd();
}

void sys_trace_k_stack_alloc_init_exit(struct k_stack *stack, uint32_t num_entries, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_STACK_ALLOC_INIT_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_STACK_ALLOC_INIT_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)stack);
	xTraceSDKEventAdd32(num_entries);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_stack_cleanup_enter(struct k_stack *stack) {
	xTraceSDKEventBegin(PSF_EVENT_STACK_CLEANUP_BLOCKING, 4);
	xTraceSDKEventAddObject((void*)stack);
	xTraceSDKEventEnd();
}

void sys_trace_k_stack_cleanup_exit(struct k_stack *stack, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_STACK_CLEANUP_SUCCESS, 8);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_STACK_CLEANUP_FAILURE, 8);
	}

	xTraceSDKEventAddObject((void*)stack);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_stack_push_enter(struct k_stack *stack, stack_data_t data) {
	xTraceSDKEventBegin(PSF_EVENT_STACK_PUSH_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)stack);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventEnd();
}

void sys_trace_k_stack_push_exit(struct k_stack *stack, stack_data_t data, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_STACK_PUSH_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_STACK_PUSH_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)stack);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_stack_pop_blocking(struct k_stack *stack, stack_data_t *data, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_STACK_POP_BLOCKING, 12);
	xTraceSDKEventAddObject((void*)stack);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_stack_pop_exit(struct k_stack *stack, stack_data_t *data, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_STACK_POP_SUCCESS, 16);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_STACK_POP_FAILURE, 16);
	}

	xTraceSDKEventAddObject((void*)stack);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}


/* Message queue trace function definitions */
void sys_trace_k_msgq_init(struct k_msgq *msgq) {
	xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_INIT, 16);
	xTraceSDKEventAddObject((void*)msgq);
	xTraceSDKEventAdd32((uint32_t)msgq->buffer_start);
	xTraceSDKEventAdd32((uint32_t)msgq->msg_size);
	xTraceSDKEventAdd32((uint32_t)msgq->max_msgs);
	xTraceSDKEventEnd();
}

void sys_trace_k_msgq_alloc_init_enter(struct k_msgq *msgq, size_t msg_size, uint32_t max_msgs) {
	xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_ALLOC_INIT_BLOCKING, 12);
	xTraceSDKEventAddObject((void*)msgq);
	xTraceSDKEventAdd32((uint32_t)msg_size);
	xTraceSDKEventAdd32((uint32_t)max_msgs);
	xTraceSDKEventEnd();
}

void sys_trace_k_msgq_alloc_init_exit(struct k_msgq *msgq, size_t msg_size, uint32_t max_msgs, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_ALLOC_INIT_SUCCESS, 16);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_ALLOC_INIT_TIMEOUT, 16);
	}

	xTraceSDKEventAddObject((void*)msgq);
	xTraceSDKEventAdd32((uint32_t)msg_size);
	xTraceSDKEventAdd32((uint32_t)max_msgs);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_msgq_cleanup_enter(struct k_msgq *msgq) {
	xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_CLEANUP_BLOCKING, 4);
	xTraceSDKEventAddObject((void*)msgq);
	xTraceSDKEventEnd();
}

void sys_trace_k_msgq_cleanup_exit(struct k_msgq *msgq, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_CLEANUP_SUCCESS, 8);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_CLEANUP_TIMEOUT, 8);
	}

	xTraceSDKEventAddObject((void*)msgq);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_msgq_put_enter(struct k_msgq *msgq, const void *data, k_timeout_t timeout) {
}

void sys_trace_k_msgq_put_blocking(struct k_msgq *msgq, const void *data, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_PUT_BLOCKING, 12);
	xTraceSDKEventAddObject((void*)msgq);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_msgq_put_exit(struct k_msgq *msgq, const void *data, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_PUT_SUCCESS, 16);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_PUT_TIMEOUT, 16);
	}

	xTraceSDKEventAddObject((void*)msgq);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_msgq_get_enter(struct k_msgq *msgq, const void *data, k_timeout_t timeout) {
}

void sys_trace_k_msgq_get_blocking(struct k_msgq *msgq, const void *data, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_GET_BLOCKING, 12);
	xTraceSDKEventAddObject((void*)msgq);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_msgq_get_exit(struct k_msgq *msgq, const void *data, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_GET_SUCCESS, 16);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_GET_TIMEOUT, 16);
	}

	xTraceSDKEventAddObject((void*)msgq);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_msgq_peek(struct k_msgq *msgq, void *data, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_PEEK_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_PEEK_FAILED, 12);
	}

	xTraceSDKEventAddObject((void*)msgq);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32((uint32_t)ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_msgq_purge(struct k_msgq *msgq) {
	xTraceSDKEventBegin(PSF_EVENT_MESSAGEQUEUE_PURGE, 4);
	xTraceSDKEventAddObject((void*)msgq);
	xTraceSDKEventEnd();
}


/* Mailbox trace function definitions */
void sys_trace_k_mbox_init(struct k_mbox *mbox) {
	xTraceSDKEventBegin(PSF_EVENT_MAILBOX_INIT, 4);
	xTraceSDKEventAddObject((void*)mbox);
	xTraceSDKEventEnd();
}

void sys_trace_k_mbox_message_put_enter(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout) {
}

void sys_trace_k_mbox_message_put_blocking(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_MAILBOX_MESSAGE_PUT_BLOCKING, 12);
	xTraceSDKEventAddObject((void*)mbox);
	xTraceSDKEventAdd32((uint32_t)tx_msg);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_mbox_message_put_exit(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_MAILBOX_MESSAGE_PUT_SUCCESS, 16);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_MAILBOX_MESSAGE_PUT_FAILURE, 16);
	}

	xTraceSDKEventAddObject((void*)mbox);
	xTraceSDKEventAdd32((uint32_t)tx_msg);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_mbox_put_enter(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_MAILBOX_PUT_BLOCKING, 12);
	xTraceSDKEventAddObject((void*)mbox);
	xTraceSDKEventAdd32((uint32_t)tx_msg);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_mbox_put_exit(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_MAILBOX_PUT_SUCCESS, 16);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_MAILBOX_PUT_FAILURE, 16);
	}

	xTraceSDKEventAddObject((void*)mbox);
	xTraceSDKEventAdd32((uint32_t)tx_msg);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_mbox_async_put_enter(struct k_mbox *mbox, struct k_sem *sem) {
	xTraceSDKEventBegin(PSF_EVENT_MAILBOX_ASYNC_PUT_ENTER, 8);
	xTraceSDKEventAddObject((void*)mbox);
	xTraceSDKEventAdd32((uint32_t)sem);
	xTraceSDKEventEnd();
}

void sys_trace_k_mbox_async_put_exit(struct k_mbox *mbox, struct k_sem *sem) {
	xTraceSDKEventBegin(PSF_EVENT_MAILBOX_ASYNC_PUT_EXIT, 8);
	xTraceSDKEventAddObject((void*)mbox);
	xTraceSDKEventAdd32((uint32_t)sem);
	xTraceSDKEventEnd();
}

void sys_trace_k_mbox_get_enter(struct k_mbox *mbox, struct k_mbox_msg *rx_msg, void *buffer, k_timeout_t timeout) {
}

void sys_trace_k_mbox_get_blocking(struct k_mbox *mbox, struct k_mbox_msg *rx_msg, void *buffer, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_MAILBOX_GET_BLOCKING, 16);
	xTraceSDKEventAddObject((void*)mbox);
	xTraceSDKEventAdd32((uint32_t)rx_msg);
	xTraceSDKEventAdd32((uint32_t)buffer);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_mbox_get_exit(struct k_mbox *mbox, struct k_mbox_msg *rx_msg, void *buffer, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_MAILBOX_GET_SUCCESS, 20);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_MAILBOX_GET_TIMEOUT, 20);
	}

	xTraceSDKEventAddObject((void*)mbox);
	xTraceSDKEventAdd32((uint32_t)rx_msg);
	xTraceSDKEventAdd32((uint32_t)buffer);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_mbox_data_get(struct k_mbox *mbox, struct k_mbox_msg *rx_msg, void *buffer) {
	xTraceSDKEventBegin(PSF_EVENT_MAILBOX_DATA_GET, 12);
	xTraceSDKEventAddObject((void*)mbox);
	xTraceSDKEventAdd32((uint32_t)rx_msg);
	xTraceSDKEventAdd32((uint32_t)buffer);
	xTraceSDKEventEnd();
}


/* Pipe trace function definitions */
void sys_trace_k_pipe_init(struct k_pipe *pipe, unsigned char *buffer, size_t size) {
	xTraceSDKEventBegin(PSF_EVENT_PIPE_INIT, 12);
	xTraceSDKEventAddObject((void*)pipe);
	xTraceSDKEventAdd32((uint32_t)buffer);
	xTraceSDKEventAdd32((uint32_t)size);
	xTraceSDKEventEnd();
}

void sys_trace_k_pipe_cleanup_enter(struct k_pipe *pipe) {
	xTraceSDKEventBegin(PSF_EVENT_PIPE_CLEANUP_BLOCKING, 4);
	xTraceSDKEventAddObject((void*)pipe);
	xTraceSDKEventEnd();
}

void sys_trace_k_pipe_cleanup_exit(struct k_pipe *pipe, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_PIPE_CLEANUP_SUCCESS, 8);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_PIPE_CLEANUP_FAILURE, 8);
	}

	xTraceSDKEventAddObject((void*)pipe);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_pipe_alloc_init_enter(struct k_pipe *pipe, size_t size) {
	xTraceSDKEventBegin(PSF_EVENT_PIPE_ALLOC_INIT_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)pipe);
	xTraceSDKEventAdd32((uint32_t)size);
	xTraceSDKEventEnd();
}

void sys_trace_k_pipe_alloc_init_exit(struct k_pipe *pipe, size_t size, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_PIPE_ALLOC_INIT_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_PIPE_ALLOC_INIT_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)pipe);
	xTraceSDKEventAdd32((uint32_t)size);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_pipe_put_enter(struct k_pipe *pipe, void *data, size_t bytes_to_write, size_t *bytes_written, size_t min_xfer, k_timeout_t timeout) {

}

void sys_trace_k_pipe_put_blocking(struct k_pipe *pipe, void *data, size_t bytes_to_write, size_t *bytes_written, size_t min_xfer, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_PIPE_PUT_BLOCKING, 24);
	xTraceSDKEventAddObject((void*)pipe);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32((uint32_t)bytes_to_write);
	xTraceSDKEventAdd32((uint32_t)bytes_written);
	xTraceSDKEventAdd32((uint32_t)min_xfer);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_pipe_put_exit(struct k_pipe *pipe, void *data, size_t bytes_to_write, size_t *bytes_written, size_t min_xfer, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_PIPE_PUT_SUCCESS, 28);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_PIPE_PUT_TIMEOUT, 28);
	}

	xTraceSDKEventAddObject((void*)pipe);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32((uint32_t)bytes_to_write);
	xTraceSDKEventAdd32((uint32_t)bytes_written);
	xTraceSDKEventAdd32((uint32_t)min_xfer);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_pipe_get_enter(struct k_pipe *pipe, void *data, size_t bytes_to_read, size_t *bytes_read, size_t min_xfer, k_timeout_t timeout) {

}

void sys_trace_k_pipe_get_blocking(struct k_pipe *pipe, void *data, size_t bytes_to_read, size_t *bytes_read, size_t min_xfer, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_PIPE_GET_BLOCKING, 24);
	xTraceSDKEventAddObject((void*)pipe);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32((uint32_t)bytes_to_read);
	xTraceSDKEventAdd32((uint32_t)bytes_read);
	xTraceSDKEventAdd32((uint32_t)min_xfer);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_pipe_get_exit(struct k_pipe *pipe, void *data, size_t bytes_to_read, size_t *bytes_read, size_t min_xfer, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_PIPE_GET_SUCCESS, 28);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_PIPE_GET_TIMEOUT, 28);
	}

	xTraceSDKEventAddObject((void*)pipe);
	xTraceSDKEventAdd32((uint32_t)data);
	xTraceSDKEventAdd32((uint32_t)bytes_to_read);
	xTraceSDKEventAdd32((uint32_t)bytes_read);
	xTraceSDKEventAdd32((uint32_t)min_xfer);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_pipe_block_put_enter(struct k_pipe *pipe, struct k_mem_block *block, size_t size, struct k_sem *sem) {
	xTraceSDKEventBegin(PSF_EVENT_PIPE_BLOCK_PUT_ENTER, 16);
	xTraceSDKEventAddObject((void*)pipe);
	xTraceSDKEventAdd32((uint32_t)block);
	xTraceSDKEventAdd32((uint32_t)size);
	xTraceSDKEventAdd32((uint32_t)sem);
	xTraceSDKEventEnd();
}

void sys_trace_k_pipe_block_put_exit(struct k_pipe *pipe, struct k_mem_block *block, size_t size, struct k_sem *sem) {
	xTraceSDKEventBegin(PSF_EVENT_PIPE_BLOCK_PUT_EXIT, 16);
	xTraceSDKEventAddObject((void*)pipe);
	xTraceSDKEventAdd32((uint32_t)block);
	xTraceSDKEventAdd32((uint32_t)size);
	xTraceSDKEventAdd32((uint32_t)sem);
	xTraceSDKEventEnd();
}


/* Memory heap trace function definitions */
void sys_trace_k_heap_init(struct k_heap *h, void *mem, size_t bytes) {
	xTraceSDKEventBegin(PSF_EVENT_KHEAP_INIT, 12);
	xTraceSDKEventAddObject((void*)h);
	xTraceSDKEventAdd32((uint32_t)mem);
	xTraceSDKEventAdd32((uint32_t)bytes);
	xTraceSDKEventEnd();
}

void sys_trace_k_heap_alloc_enter(struct k_heap *h, size_t bytes, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_KHEAP_ALLOC_BLOCKING, 12);
	xTraceSDKEventAddObject((void*)h);
	xTraceSDKEventAdd32(bytes);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_heap_alloc_exit(struct k_heap *h, size_t bytes, k_timeout_t timeout, void *ret) {
	if (ret != NULL) {
		xTraceSDKEventBegin(PSF_EVENT_KHEAP_ALLOC_SUCCESS, 16);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_KHEAP_ALLOC_FAILURE, 16);
	}

	xTraceSDKEventAddObject((void*)h);
	xTraceSDKEventAdd32(bytes);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32((uint32_t)ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_heap_aligned_alloc_enter(struct k_heap *h, size_t bytes, k_timeout_t timeout) {

}

void sys_trace_k_heap_aligned_alloc_blocking(struct k_heap *h, size_t bytes, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_KHEAP_ALIGNED_ALLOC_BLOCKING, 16);
	xTraceSDKEventAddObject((void*)h);
	xTraceSDKEventAdd32(bytes);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32(0);
	xTraceSDKEventEnd();
}

void sys_trace_k_heap_aligned_alloc_exit(struct k_heap *h, size_t bytes, k_timeout_t timeout, void *ret) {
	/**
	 * There seems to be an issue with k_heap and forever timeouts where if there is not enough memory for direct allocation
	 * the system stops trying instantly and returns NULL.
	 */
	if (ret == NULL && K_TIMEOUT_EQ(timeout, K_FOREVER)) {
		vTracePrint(trcWarningChannel, "Potential Zephyr Bug: Why did it not wait?");
		xTraceSDKEventBegin(PSF_EVENT_KHEAP_ALIGNED_ALLOC_FAILURE, 16);
	} else if (ret == NULL) {
		xTraceSDKEventBegin(PSF_EVENT_KHEAP_ALIGNED_ALLOC_FAILURE, 16);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_KHEAP_ALIGNED_ALLOC_SUCCESS, 16);
	}

	xTraceSDKEventAddObject((void*)h);
	xTraceSDKEventAdd32((uint32_t)bytes);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventAdd32((uint32_t)ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_heap_free(struct k_heap *h, void *mem) {
	xTraceSDKEventBegin(PSF_EVENT_KHEAP_FREE, 8);
	xTraceSDKEventAddObject((void*)h);
	xTraceSDKEventAdd32((uint32_t)mem);
	xTraceSDKEventEnd();
}

void sys_trace_k_heap_sys_k_aligned_alloc_enter(struct k_heap *h, size_t align, size_t size) {
	xTraceSDKEventBegin(PSF_EVENT_KHEAP_SYS_ALIGNED_ALLOC_BLOCKING, 12);
	xTraceSDKEventAddObject((void*)h);
	xTraceSDKEventAdd32(align);
	xTraceSDKEventAdd32(size);
	xTraceSDKEventEnd();
}

void sys_trace_k_heap_sys_k_aligned_alloc_exit(struct k_heap *h, size_t align, size_t size, void *ret) {
	if (ret != NULL) {
		xTraceSDKEventBegin(PSF_EVENT_KHEAP_SYS_ALIGNED_ALLOC_SUCCESS, 16);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_KHEAP_SYS_ALIGNED_ALLOC_FAILURE, 16);
	}

	xTraceSDKEventAddObject((void*)h);
	xTraceSDKEventAdd32(align);
	xTraceSDKEventAdd32(size);
	xTraceSDKEventAdd32((uint32_t)ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_heap_sys_k_malloc_enter(struct k_heap *h, size_t size) {
	xTraceSDKEventBegin(PSF_EVENT_KHEAP_SYS_MALLOC_BLOCKING, 8);
	xTraceSDKEventAddObject((void*)h);
	xTraceSDKEventAdd32(size);
	xTraceSDKEventEnd();
}

void sys_trace_k_heap_sys_k_malloc_exit(struct k_heap *h, size_t size, void *ret) {
	if (ret != NULL) {
		xTraceSDKEventBegin(PSF_EVENT_KHEAP_SYS_MALLOC_SUCCESS, 12);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_KHEAP_SYS_MALLOC_FAILURE, 12);
	}

	xTraceSDKEventAddObject((void*)h);
	xTraceSDKEventAdd32(size);
	xTraceSDKEventAdd32((uint32_t)ret);
	xTraceSDKEventEnd();
}

void sys_trace_k_heap_sys_k_free_enter(struct k_heap *h) {
	xTraceSDKEventBegin(PSF_EVENT_KHEAP_SYS_FREE_ENTER, 4);
	xTraceSDKEventAddObject((void*)h);
	xTraceSDKEventEnd();
}

void sys_trace_k_heap_sys_k_free_exit(struct k_heap *h) {
	xTraceSDKEventBegin(PSF_EVENT_KHEAP_SYS_FREE_EXIT, 4);
	xTraceSDKEventAddObject((void*)h);
	xTraceSDKEventEnd();
}

void sys_trace_k_heap_sys_k_enter(struct k_heap *h, size_t nmemb, size_t size) {

}

void sys_trace_k_heap_sys_k_exit(struct k_heap *h, size_t nmemb, size_t size, void *ret) {

}

void sys_trace_k_heap_sys_k_calloc_enter(struct k_heap *h, size_t nmemb, size_t size) {
	xTraceSDKEventBegin(PSF_EVENT_KHEAP_SYS_CALLOC_BLOCKING, 12);
	xTraceSDKEventAddObject((void*)h);
	xTraceSDKEventAdd32(nmemb);
	xTraceSDKEventAdd32(size);
	xTraceSDKEventEnd();
}

void sys_trace_k_heap_sys_k_calloc_exit(struct k_heap *h, size_t nmemb, size_t size, void *ret) {
	if (ret != NULL) {
		xTraceSDKEventBegin(PSF_EVENT_KHEAP_SYS_CALLOC_SUCCESS, 16);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_KHEAP_SYS_CALLOC_FAILURE, 16);
	}

	xTraceSDKEventAddObject((void*)h);
	xTraceSDKEventAdd32(nmemb);
	xTraceSDKEventAdd32(size);
	xTraceSDKEventAdd32((uint32_t)ret);
	xTraceSDKEventEnd();
}


/* Memory slab trace function definitions */
void sys_trace_k_mem_slab_init(struct k_mem_slab *slab, void *buffer, size_t block_size, uint32_t num_blocks, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_MEMORY_SLAB_INIT_SUCCESS, 24);
	} else {
		xTraceSDKEventBegin(PSF_EVENT_MEMORY_SLAB_INIT_FAILURE, 24);
	}

	xTraceSDKEventAddObject((void*)slab);
	xTraceSDKEventAdd32((uint32_t)slab->buffer);
	xTraceSDKEventAdd32((uint32_t)slab->block_size);
	xTraceSDKEventAdd32((uint32_t)slab->num_blocks);
	xTraceSDKEventAdd32(ret);
	xTraceSDKEventAdd32((uint32_t)slab->num_blocks);
	xTraceSDKEventEnd();
}

void sys_trace_k_mem_slab_alloc_enter(struct k_mem_slab *slab, void **mem, k_timeout_t timeout) {

}

void sys_trace_k_mem_slab_alloc_blocking(struct k_mem_slab *slab, void **mem, k_timeout_t timeout) {
	xTraceSDKEventBegin(PSF_EVENT_MEMORY_SLAB_ALLOC_BLOCKING, 12);
	xTraceSDKEventAddObject((void*)slab);
	xTraceSDKEventAdd32((uint32_t)mem);
	xTraceSDKEventAdd32(timeout.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_mem_slab_alloc_exit(struct k_mem_slab *slab, void **mem, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		xTraceSDKEventBegin(PSF_EVENT_MEMORY_SLAB_ALLOC_SUCCESS, 20);
		xTraceSDKEventAddObject((void*)slab);
		xTraceSDKEventAdd32((uint32_t)mem);
		xTraceSDKEventAdd32(timeout.ticks);
		xTraceSDKEventAdd32(ret);
		xTraceSDKEventAdd32(slab->num_blocks);
		xTraceSDKEventEnd();
	} else if (ret == -ENOMEM || ret == -EAGAIN) {
		xTraceSDKEventBegin(PSF_EVENT_MEMORY_SLAB_ALLOC_TIMEOUT, 16);
		xTraceSDKEventAddObject((void*)slab);
		xTraceSDKEventAdd32((uint32_t)mem);
		xTraceSDKEventAdd32(timeout.ticks);
		xTraceSDKEventAdd32(ret);
		xTraceSDKEventEnd();
	} else {
		xTraceSDKEventBegin(PSF_EVENT_MEMORY_SLAB_ALLOC_ERROR, 16);
		xTraceSDKEventAddObject((void*)slab);
		xTraceSDKEventAdd32((uint32_t)mem);
		xTraceSDKEventAdd32(timeout.ticks);
		xTraceSDKEventAdd32(ret);
		xTraceSDKEventEnd();
	}
}

void sys_trace_k_mem_slab_free_exit(struct k_mem_slab *slab, void **mem) {
	xTraceSDKEventBegin(PSF_EVENT_MEMORY_SLAB_FREE, 12);
	xTraceSDKEventAddObject((void*)slab);
	xTraceSDKEventAdd32((uint32_t)mem);
	xTraceSDKEventAdd32(slab->num_blocks);
	xTraceSDKEventEnd();
}


/* Timer trace function definitions */
void sys_trace_k_timer_init(struct k_timer *timer, k_timer_expiry_t expiry_fn, k_timer_expiry_t stop_fn) {
	xTraceSDKEventBegin(PSF_EVENT_TIMER_INIT, 12);
	xTraceSDKEventAddObject((void*)timer);
	xTraceSDKEventAdd32((uint32_t)expiry_fn);
	xTraceSDKEventAdd32((uint32_t)stop_fn);
	xTraceSDKEventEnd();
}

void sys_trace_k_timer_start(struct k_timer *timer, k_timeout_t duration, k_timeout_t period) {
	xTraceSDKEventBegin(PSF_EVENT_TIMER_START, 12);
	xTraceSDKEventAddObject((void*)timer);
	xTraceSDKEventAdd32(duration.ticks);
	xTraceSDKEventAdd32(period.ticks);
	xTraceSDKEventEnd();
}

void sys_trace_k_timer_stop(struct k_timer *timer) {
	xTraceSDKEventBegin(PSF_EVENT_TIMER_STOP, 4);
	xTraceSDKEventAddObject((void*)timer);
	xTraceSDKEventEnd();
}

void sys_trace_k_timer_status_sync_blocking(struct k_timer *timer) {
	xTraceSDKEventBegin(PSF_EVENT_TIMER_STATUS_SYNC_AWAIT, 4);
	xTraceSDKEventAddObject((void*)timer);
	xTraceSDKEventEnd();
}

void sys_trace_k_timer_status_sync_exit(struct k_timer *timer, uint32_t result) {
	xTraceSDKEventBegin(PSF_EVENT_TIMER_STATUS_SYNC_EXIT, 8);
	xTraceSDKEventAddObject((void*)timer);
	xTraceSDKEventAdd32(result);
	xTraceSDKEventEnd();
}


/* Syscall trace function definitions */
void sys_trace_syscall_enter() {
	xTraceSDKEventBegin(PSF_EVENT_SYSTEM_SYSCALL_ENTER, 0);
	xTraceSDKEventEnd();
}

void sys_trace_syscall_exit() {
	xTraceSDKEventBegin(PSF_EVENT_SYSTEM_SYSCALL_EXIT, 0);
	xTraceSDKEventEnd();
}


/* Legacy trace functions that are pending refactoring/removal by
 * the Zephyr team.
 */
void sys_trace_isr_enter(void) {
	vTraceStoreISRBegin(HandleISR);
}

void sys_trace_isr_exit(void) {
	vTraceStoreISREnd(0);
}

void sys_trace_isr_exit_to_scheduler(void) {
}

void sys_trace_idle(void) {
}

void sys_trace_void(unsigned int id) {
}

#endif /*TRC_RECORDER_MODE_STREAMING == 1*/




/**
 * @brief TzCtrl_thread_entry
 *
 * Task for sending the trace data from the internal buffer to the stream 
 * interface (assuming TRC_STREAM_PORT_USE_INTERNAL_BUFFER == 1) and for
 * receiving commands from Tracealyzer. Also does some diagnostics.
 * 
 * @param _args
 */
void TzCtrl_thread_entry(void *_args)
{
	TracealyzerCommandType msg;
	int32_t bytes = 0;
	int32_t status = 0;

	while (1)
	{
		do
		{
			/* Listen for new commands */
			bytes = 0;
			status = TRC_STREAM_PORT_READ_DATA(&msg, sizeof(TracealyzerCommandType), (int32_t*)&bytes);

			if (status != 0)
			{
				/* The connection has failed, stop tracing */
				vTraceStop();
			}

			if ((status == 0) && (bytes == sizeof(TracealyzerCommandType)))
			{
				if (prvIsValidCommand(&msg))
				{
					prvProcessCommand(&msg); /* Start or Stop currently... */
				}
			}

/* If the internal buffer is disabled, the COMMIT macro instead sends the data directly 
   from the "event functions" (using TRC_STREAM_PORT_WRITE_DATA). */			
#if (TRC_STREAM_PORT_USE_INTERNAL_BUFFER == 1)
			/* If there is a buffer page, this sends it to the streaming interface using TRC_STREAM_PORT_WRITE_DATA. */
			bytes = prvPagedEventBufferTransfer();
#endif			
			
		/* If there was data sent or received (bytes != 0), loop around and repeat, if there is more data to send or receive.
		Otherwise, step out of this loop and sleep for a while. */		
		
		} while (bytes != 0);

		if (xTraceIsRecordingEnabled())
		{
			prvCheckRecorderStatus();
			prvReportStackUsage();
		}

		k_msleep((TRC_CFG_CTRL_TASK_DELAY_MS));
	}
}


/**
 * 
 */
TRC_STREAM_PORT_ALLOCATE_FIELDS()


/**
 * @brief vTraceEnable
 *
 * Function that enables the tracing and creates the control task. It will halt
 * execution until a Start command has been received if haltUntilStart is true.
 */
void vTraceEnable(int startOption)
{
	int32_t bytes = 0;
	int32_t status;
	TracealyzerCommandType msg;
	extern uint32_t RecorderEnabled;

	/* Make sure recorder data is initialized */
	vTraceInitialize();

	/* Since we might perform heap allocation as part of the port 
	 * initialization phase we disable all tracing macros for this
	 * section of code. If we didn't disable tracing here we would
	 * end up producing trace events before the recorder is enabled.
	 */
	#undef SYS_PORT_TRACING_TYPE_MASK
	#define SYS_PORT_TRACING_TYPE_MASK(type, trace_call)

	TRC_STREAM_PORT_INIT();

	/* Re-enable tracing
	 */
	#undef SYS_PORT_TRACING_TYPE_MASK
	#define SYS_PORT_TRACING_TYPE_MASK(type, trace_call) \
		_SYS_PORT_TRACING_TYPE_MASK(type)(trace_call)

   /* The #WFR channel means "Warnings from Recorder" and
	* is used to store warnings and errors from the recorder.
	* The abbreviation #WFR is used instead of the longer full name,
	* to avoid truncation by small slots in the symbol table. 
	* This is translated in Tracealyzer and shown as the full name,
	* "Warnings from Recorder".
	*
	* Note: Requires that TRC_CFG_INCLUDE_USER_EVENTS is 1. */
	trcWarningChannel = xTraceRegisterString("#WFR");

	if (startOption == TRC_START_AWAIT_HOST)
	{
		/* We keep trying to read commands until the recorder has been started */
		do
		{
			bytes = 0;
			
			status = TRC_STREAM_PORT_READ_DATA(&msg, sizeof(TracealyzerCommandType), (int32_t*)&bytes);
			
			if (status != 0)
			{
				//prvTraceWarning(PSF_WARNING_STREAM_PORT_READ);
			}

			if ((status == 0) && (bytes == sizeof(TracealyzerCommandType)))
			{
				if (prvIsValidCommand(&msg))
				{
					if (msg.cmdCode == CMD_SET_ACTIVE && msg.param1 == 1)
					{
						/* On start, init and reset the timestamping */
						TRC_PORT_SPECIFIC_INIT();
					}
					
					prvProcessCommand(&msg);
				}
			}
		}
		while (RecorderEnabled == 0);
	}
	else if (startOption == TRC_START)
	{
		/* We start streaming directly - this assumes that the interface is ready! */
		TRC_PORT_SPECIFIC_INIT();
		
		msg.cmdCode = CMD_SET_ACTIVE;
		msg.param1 = 1;
		prvProcessCommand(&msg);
	}
	else
	{
		/* On TRC_INIT */
		TRC_PORT_SPECIFIC_INIT();
	}
}

/**
 * @brief Initialize aspects of the recorder that must preceed the
 * kernel initialization (scheduling, threads, etc.).
 * 
 * @param arg
 */
static int tracelyzer_pre_kernel_init(const struct device *arg)
{
	ARG_UNUSED(arg);

#ifdef CONFIG_PERCEPIO_RECORDER_CFG_START_MODE_START
	vTraceEnable(TRC_START);
#elif CONFIG_PERCEPIO_RECORDER_CFG_START_MODE_START_AWAIT_HOST
	vTraceEnable(TRC_START_AWAIT_HOST);
#else
	vTraceEnable(TRC_INIT);
#endif

	/* Create ISR handle */
	HandleISR = xTraceSetISRProperties("Zephyr ISR", -32);

	return 0;
}

/**
 * @brief Initialize aspects of the recorder that depends on the kernel
 * being initialized.
 * 
 * @param arg
 */
static int tracealyzer_post_kernel_init(const struct device *arg)
{
	ARG_UNUSED(arg);

	/* Create controller task */
	k_thread_create(&HandleTzCtrl, TzCtrl_thread_stack,
	 	K_THREAD_STACK_SIZEOF(TzCtrl_thread_stack),
	 	(k_thread_entry_t)TzCtrl_thread_entry,
	 	NULL, NULL, NULL,
	 	(TRC_CFG_CTRL_TASK_PRIORITY),
	 	0,
	 	K_NO_WAIT);
	
	/* Set controller task name */
	k_thread_name_set(&HandleTzCtrl, "TzCtrl");

	return 0;
}

/* Specify recorder module initialization stages */
SYS_INIT(tracelyzer_pre_kernel_init, PRE_KERNEL_2, 0);
SYS_INIT(tracealyzer_post_kernel_init, POST_KERNEL, 0);