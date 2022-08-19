/*
 * Trace Recorder for Tracealyzer v4.6.6
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The Zephyr specific parts of the trace recorder
 */

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <string.h>
#include <trcRecorder.h>


/* Generic Zephyr system heap handle */
TraceHeapHandle_t xSystemHeapHandle;

/* Generic Zephyr ISR handle used for all Zephyr ISRs that the user haven't
 * manually added tracing for. */
static TraceISRHandle_t xHandleISR;

/* Trace recorder controll thread stack */
static K_THREAD_STACK_DEFINE(TzCtrl_thread_stack, (TRC_CFG_CTRL_TASK_STACK_SIZE));


/**
 * @brief TzCtrl_thread_entry
 *
 * Task for sending the trace data from the internal buffer to the stream 
 * interface (assuming TRC_STREAM_PORT_USE_INTERNAL_BUFFER == 1) and for
 * receiving commands from Tracealyzer. Also does some diagnostics.
 * 
 * @param[in] _args
 */
void TzCtrl_thread_entry(void *_args)
{
	while (1)
	{
		xTraceTzCtrl();

		k_msleep((TRC_CFG_CTRL_TASK_DELAY));
	}
}


/**
 * @brief 
 */
typedef struct TraceKernelPortData
{
	TraceHeapHandle_t xSystemHeapHandle;
	TraceKernelPortTaskHandle_t xTzCtrlHandle;
} TraceKernelPortData_t;

static TraceKernelPortData_t* pxKernelPortData;

traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* pxBuffer)
{
	TRC_ASSERT_EQUAL_SIZE(TraceKernelPortDataBuffer_t, TraceKernelPortData_t);
	
	if (pxBuffer == 0)
	{
		return TRC_FAIL;
	}
	
	pxKernelPortData = (TraceKernelPortData_t*)pxBuffer;

	pxKernelPortData->xSystemHeapHandle = 0;

	return TRC_SUCCESS;
}

traceResult xTraceKernelPortEnable(void)
{
	return TRC_SUCCESS;
}

TraceHeapHandle_t xTraceKernelPortGetSystemHeapHandle(void)
{
	return 0;
}

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)
traceResult xTraceKernelPortGetUnusedStack(void* thread, TraceUnsignedBaseType_t* puxUnusedStack)
{
	return k_thread_stack_space_get(thread, puxUnusedStack);
}
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */

unsigned char xTraceKernelPortIsSchedulerSuspended(void)
{
	return 0;
}

void vTraceSetKernelObjectName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetWorkQueueName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetHeapName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetSemaphoreName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetMutexName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetCondvarName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetQueueName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetFIFOQueueName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetLIFOQueueName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetStackName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetMessageQueueName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetMailboxName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetPipeName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetMemoryHeapName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetMemorySlabName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetTimerName(void* object, const char* name)
{
	xTraceObjectSetNameWithoutHandle(object, name);
}

/**
 * @brief Initialize aspects of the recorder that must preceed the
 * kernel initialization (scheduling, threads, etc.).
 * 
 * @param[in] arg
 */
static int tracelyzer_pre_kernel_init(const struct device *arg)
{
	ARG_UNUSED(arg);

#ifdef CONFIG_PERCEPIO_TRC_START_MODE_START
	xTraceEnable(TRC_START);
#elif CONFIG_PERCEPIO_TRC_START_MODE_START_AWAIT_HOST
	xTraceEnable(TRC_START_AWAIT_HOST);
#else
	xTraceEnable(TRC_START_FROM_HOST);
#endif

	/* Create ISR handle */
	xTraceISRRegister("Zephyr ISR", -32, &xHandleISR);

	return 0;
}

/**
 * @brief Initialize aspects of the recorder that depends on the kernel
 * being initialized.
 * 
 * @param[in] arg
 */
static int tracealyzer_post_kernel_init(const struct device *arg)
{
	ARG_UNUSED(arg);

	/* Create controller task */
	k_thread_create(&pxKernelPortData->xTzCtrlHandle, TzCtrl_thread_stack,
	 	K_THREAD_STACK_SIZEOF(TzCtrl_thread_stack),
	 	(k_thread_entry_t)TzCtrl_thread_entry,
	 	NULL, NULL, NULL,
	 	(TRC_CFG_CTRL_TASK_PRIORITY),
	 	0,
	 	K_NO_WAIT);
	
	/* Set controller task name */
	k_thread_name_set(&pxKernelPortData->xTzCtrlHandle, "TzCtrl");

	return 0;
}

/* Specify recorder module initialization stages */
SYS_INIT(tracelyzer_pre_kernel_init, PRE_KERNEL_2, 0);
SYS_INIT(tracealyzer_post_kernel_init, POST_KERNEL, 0);




/* Thread trace function definitions */
void sys_trace_k_thread_foreach_enter(k_thread_user_cb_t user_cb, void *user_data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_FOREACH_ENTER, sizeof(void*) + sizeof(void*), 
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, user_cb);
		xTraceEventAddPointer(xTraceHandle, user_data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_foreach_exit(k_thread_user_cb_t user_cb, void *user_data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_FOREACH_EXIT, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, user_cb);
		xTraceEventAddPointer(xTraceHandle, user_data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_foreach_unlocked_enter(k_thread_user_cb_t user_cb, void *user_data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_FOREACH_UNLOCKED_ENTER, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, user_cb);
		xTraceEventAddPointer(xTraceHandle, user_data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_foreach_unlocked_exit(k_thread_user_cb_t user_cb, void *user_data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_FOREACH_UNLOCKED_EXIT, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, user_cb);
		xTraceEventAddPointer(xTraceHandle, user_data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_create(struct k_thread *thread, size_t stack_size, int prio) {
	TraceEventHandle_t xTraceHandle;

	/* Since we have a slightly different task/thread register event
	 * we manually update the entry table here */
	TraceEntryHandle_t xEntryHandle;

	TRACE_ALLOC_CRITICAL_SECTION();
	TRACE_ENTER_CRITICAL_SECTION();
	
	if (xTraceEntryCreateWithAddress((void*)thread, &xEntryHandle) == TRC_FAIL)
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return;
	}

	xTraceEntrySetState(xEntryHandle, 0, prio);
	TRACE_EXIT_CRITICAL_SECTION();

	/* Register task with stack monitor */
	xTraceStackMonitorAdd((void*)thread);

	if (xTraceEventBegin(PSF_EVENT_THREAD_INIT, sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)thread);
		xTraceEventAdd32(xTraceHandle, (uint32_t)stack_size);
		xTraceEventAdd32(xTraceHandle, (uint32_t)prio);
		xTraceEventEnd(xTraceHandle);
	}

#ifdef CONFIG_THREAD_NAME
	if (thread->name != NULL && strlen(thread->name) > 0) {
		xTraceObjectSetName(xEntryHandle, thread->name);
	}
#endif
}

void sys_trace_k_thread_user_mode_enter(k_thread_entry_t entry, void *p1, void *p2, void *p3) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_USER_MODE_ENTER, sizeof(void*) * 5,
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)k_current_get());
		xTraceEventAddPointer(xTraceHandle, (void*)entry);
		xTraceEventAddPointer(xTraceHandle, (void*)p1);
		xTraceEventAddPointer(xTraceHandle, (void*)p2);
		xTraceEventAddPointer(xTraceHandle, (void*)p3);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_heap_assign(struct k_thread *thread, struct k_heap *heap) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_HEAP_ASSIGN, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)thread);
		xTraceEventAddPointer(xTraceHandle, (void*)heap);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_join_blocking(struct k_thread *thread, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_JOIN_BLOCKING, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)thread);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_join_exit(struct k_thread *thread, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_THREAD_JOIN_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_THREAD_JOIN_TIMEOUT,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)thread);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_sleep_enter(k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_SLEEP_ENTER, sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_sleep_exit(k_timeout_t timeout, int ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_SLEEP_EXIT, sizeof(uint32_t) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_msleep_enter(int32_t ms) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_MSLEEP_ENTER, sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAdd32(xTraceHandle, ms);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_msleep_exit(int32_t ms, int ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_MSLEEP_EXIT, sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAdd32(xTraceHandle, ms);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_usleep_enter(int32_t us) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_USLEEP_ENTER, sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAdd32(xTraceHandle, us);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_usleep_exit(int32_t us, int ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_USLEEP_EXIT, sizeof(uint32_t) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAdd32(xTraceHandle, us);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_busy_wait_enter(uint32_t usec_to_wait) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_BUSY_WAIT_ENTER, sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAdd32(xTraceHandle, usec_to_wait);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_busy_wait_exit(uint32_t usec_to_wait) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_BUSY_WAIT_EXIT, sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAdd32(xTraceHandle, usec_to_wait);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_yield() {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_YIELD, 0, &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_wakeup(struct k_thread *thread) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_WAKEUP, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)thread);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_abort(struct k_thread *thread) {
	/* Intentionally left empty, see k_thread_sched_abort for implementation */
}

void sys_trace_k_thread_start(struct k_thread *thread) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_START, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)thread);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_priority_set(struct k_thread *thread) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceObjectSetStateWithoutHandle((void*)thread, k_thread_priority_get(thread)) == TRC_FAIL)
	{
		return;
	}

	if (xTraceEventBegin(PSF_EVENT_THREAD_SET_PRIORITY, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)thread);
		xTraceEventAdd32(xTraceHandle, k_thread_priority_get(thread));
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_suspend(struct k_thread *thread) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_SUSPEND, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)thread);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_resume(struct k_thread *thread) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_RESUME, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)thread);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_name_set(struct k_thread *thread, int ret) {
	if (ret == 0) {
		xTraceObjectSetNameWithoutHandle((void*)thread, thread->name);
	}
}

void sys_trace_k_thread_switched_out(void) {
}

void sys_trace_k_thread_switched_in(void) {
	xTraceTaskSwitch(k_current_get(), k_thread_priority_get(k_current_get()));
}

void sys_trace_k_thread_info(struct k_thread *thread) {
}


/* Thread sceduler trace function definitions */
void sys_trace_k_thread_sched_lock() {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_SCHED_LOCK, 0, &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_sched_unlock() {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_SCHED_UNLOCK, 0, &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_sched_wakeup(struct k_thread *thread) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_SCHED_WAKEUP, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)thread);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_sched_abort(struct k_thread *thread) {
	TraceEventHandle_t xTraceHandle;
	TraceEntryHandle_t xEntryHandle;

	TRACE_ALLOC_CRITICAL_SECTION();
	TRACE_ENTER_CRITICAL_SECTION();

	/* Fetch entry handle */
	if (xTraceEntryFind((void*)thread, &xEntryHandle) == TRC_FAIL)
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return;
	}
	
	/* Delete entry */
	if (xTraceEntryDelete(xEntryHandle) == TRC_FAIL)
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return;
	}

	TRACE_EXIT_CRITICAL_SECTION();

	/* Remove thread from stack monitor */
	xTraceStackMonitorRemove((void*)thread);

	if (xTraceEventBegin(PSF_EVENT_THREAD_SCHED_ABORT, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)thread);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_sched_set_priority(struct k_thread *thread, int prio) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_SCHED_PRIORITY_SET, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)thread);
		xTraceEventAdd32(xTraceHandle, prio);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_sched_ready(struct k_thread *thread) {
	xTraceTaskReady((void*)thread);
}

void sys_trace_k_thread_sched_pend(struct k_thread *thread) {
	
}

void sys_trace_k_thread_sched_resume(struct k_thread *thread) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_SCHED_RESUME, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)thread);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_thread_sched_suspend(struct k_thread *thread) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_THREAD_SCHED_SUSPEND, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)thread);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Work trace function definitions */
void sys_trace_k_work_init(struct k_work *work, k_work_handler_t handler) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_WORK_INIT, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAddPointer(xTraceHandle, (void*)handler);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_submit_to_queue_enter(struct k_work_q *queue, struct k_work *work) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_WORK_SUBMIT_TO_QUEUE_BLOCKING, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_submit_to_queue_exit(struct k_work_q *queue, struct k_work *work, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret >= 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_WORK_SUBMIT_TO_QUEUE_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_WORK_SUBMIT_TO_QUEUE_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_submit_enter(struct k_work *work) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_WORK_SUBMIT_BLOCKING, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_submit_exit(struct k_work *work, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret >= 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_WORK_SUBMIT_SUCCESS,
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_WORK_SUBMIT_FAILURE,
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_flush_enter(struct k_work *work, struct k_work_sync *sync) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_WORK_FLUSH_BLOCKING, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAddPointer(xTraceHandle, (void*)sync);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_flush_blocking(struct k_work *work, struct k_work_sync *sync, k_timeout_t timeout) {
	
}

void sys_trace_k_work_flush_exit(struct k_work *work, struct k_work_sync *sync, bool ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_WORK_FLUSH_SUCCESS, sizeof(void*) + sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAddPointer(xTraceHandle, (void*)sync);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_cancel_enter(struct k_work *work) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_WORK_CANCEL_BLOCKING, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_cancel_exit(struct k_work *work, int ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_WORK_CANCEL_SUCCESS, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_cancel_sync_enter(struct k_work *work, struct k_work_sync *sync) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_WORK_CANCEL_SYNC_BLOCKING, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAddPointer(xTraceHandle, (void*)sync);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_cancel_sync_blocking(struct k_work *work, struct k_work_sync *sync) {
	
}

void sys_trace_k_work_cancel_sync_exit(struct k_work *work, struct k_work_sync *sync, bool ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_WORK_CANCEL_SYNC_SUCCESS,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAddPointer(xTraceHandle, (void*)sync);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Work queue trace function definitions */
void sys_trace_k_work_queue_start_enter(struct k_work_q *queue, k_thread_stack_t *stack, size_t stack_size, int prio, const struct k_work_queue_config *cfg) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_WORK_QUEUE_START_BLOCKING,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)stack);
		xTraceEventAdd32(xTraceHandle, stack_size);
		xTraceEventAdd32(xTraceHandle, prio);
		xTraceEventAddPointer(xTraceHandle, (void*)cfg);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_queue_start_exit(struct k_work_q *queue, k_thread_stack_t *stack, size_t stack_size, int prio, const struct k_work_queue_config *cfg) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_WORK_QUEUE_START_SUCCESS, 
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)stack);
		xTraceEventAdd32(xTraceHandle, stack_size);
		xTraceEventAdd32(xTraceHandle, prio);
		xTraceEventAddPointer(xTraceHandle, (void*)cfg);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_queue_drain_enter(struct k_work_q *queue, bool plug) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_WORK_QUEUE_DRAIN_BLOCKING, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAdd32(xTraceHandle, plug);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_queue_drain_exit(struct k_work_q *queue, bool plug, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret >= 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_WORK_QUEUE_DRAIN_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_WORK_QUEUE_DRAIN_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAdd32(xTraceHandle, plug);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_queue_unplug_enter(struct k_work_q *queue) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_WORK_QUEUE_UNPLUG_BLOCKING, sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_queue_unplug_exit(struct k_work_q *queue, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_WORK_QUEUE_UNPLUG_SUCCESS,
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_WORK_QUEUE_UNPLUG_FAILURE,
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Work delayable trace function definitions */
void sys_trace_k_work_delayable_init(struct k_work_delayable *dwork, k_work_handler_t handler) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_DWORK_INIT, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)dwork);
		xTraceEventAddPointer(xTraceHandle, (void*)handler);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_schedule_for_queue_enter(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay) {
}

void sys_trace_k_work_schedule_for_queue_exit(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay, int ret) {
	TraceEventHandle_t xTraceHandle;

	if (ret == 0 || ret == 1) {
		if (xTraceEventBegin(PSF_EVENT_DWORK_SCHEDULE_FOR_QUEUE_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
			xTraceEventAddPointer(xTraceHandle, (void*)queue);
			xTraceEventAddPointer(xTraceHandle, (void*)dwork);
			xTraceEventAdd32(xTraceHandle, delay.ticks);
			xTraceEventAdd32(xTraceHandle, ret);
			xTraceEventEnd(xTraceHandle);
		}
	} else {
		if (xTraceEventBegin(PSF_EVENT_DWORK_SCHEDULE_FOR_QUEUE_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
			xTraceEventAddPointer(xTraceHandle, (void*)queue);
			xTraceEventAddPointer(xTraceHandle, (void*)dwork);
			xTraceEventAdd32(xTraceHandle, delay.ticks);
			xTraceEventAdd32(xTraceHandle, ret);
			xTraceEventEnd(xTraceHandle);
		}
	}
}

void sys_trace_k_work_schedule_enter(struct k_work_delayable *dwork, k_timeout_t delay) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_DWORK_SCHEDULE_BLOCKING, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)dwork);
		xTraceEventAdd32(xTraceHandle, delay.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_schedule_exit(struct k_work_delayable *dwork, k_timeout_t delay, int ret) {
	TraceEventHandle_t xTraceHandle;
	
	if (ret == 0 || ret == 1) {
		if (xTraceEventBegin(PSF_EVENT_DWORK_SCHEDULE_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
			xTraceEventAddPointer(xTraceHandle, (void*)dwork);
			xTraceEventAdd32(xTraceHandle, delay.ticks);
			xTraceEventAdd32(xTraceHandle, ret);
			xTraceEventEnd(xTraceHandle);
		}
	} else {
		if (xTraceEventBegin(PSF_EVENT_DWORK_SCHEDULE_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
			xTraceEventAddPointer(xTraceHandle, (void*)dwork);
			xTraceEventAdd32(xTraceHandle, delay.ticks);
			xTraceEventAdd32(xTraceHandle, ret);
			xTraceEventEnd(xTraceHandle);
		}
	}
}

void sys_trace_k_work_reschedule_for_queue_enter(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay) {
}

void sys_trace_k_work_reschedule_for_queue_exit(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay, int ret) {
	TraceEventHandle_t xTraceHandle;

	if (ret == 0 || ret == 1 || ret == 2) {
		if (xTraceEventBegin(PSF_EVENT_DWORK_RESCHEDULE_FOR_QUEUE_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
			xTraceEventAddPointer(xTraceHandle, (void*)queue);
			xTraceEventAddPointer(xTraceHandle, (void*)dwork);
			xTraceEventAdd32(xTraceHandle, delay.ticks);
			xTraceEventAdd32(xTraceHandle, ret);
			xTraceEventEnd(xTraceHandle);
		}
	} else {
		if (xTraceEventBegin(PSF_EVENT_DWORK_RESCHEDULE_FOR_QUEUE_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
			xTraceEventAddPointer(xTraceHandle, (void*)queue);
			xTraceEventAddPointer(xTraceHandle, (void*)dwork);
			xTraceEventAdd32(xTraceHandle, delay.ticks);
			xTraceEventAdd32(xTraceHandle, ret);
			xTraceEventEnd(xTraceHandle);
		}
	}
}

void sys_trace_k_work_reschedule_enter(struct k_work_delayable *dwork, k_timeout_t delay) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_DWORK_RESCHEDULE_BLOCKING, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)dwork);
		xTraceEventAdd32(xTraceHandle, delay.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_reschedule_exit(struct k_work_delayable *dwork, k_timeout_t delay, int ret) {
	TraceEventHandle_t xTraceHandle;

	if (ret == 0 || ret == 1 || ret == 2) {
		if (xTraceEventBegin(PSF_EVENT_DWORK_RESCHEDULE_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
			xTraceEventAddPointer(xTraceHandle, (void*)dwork);
			xTraceEventAdd32(xTraceHandle, delay.ticks);
			xTraceEventAdd32(xTraceHandle, ret);
			xTraceEventEnd(xTraceHandle);
		}
	} else {
		if (xTraceEventBegin(PSF_EVENT_DWORK_RESCHEDULE_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
			xTraceEventAddPointer(xTraceHandle, (void*)dwork);
			xTraceEventAdd32(xTraceHandle, delay.ticks);
			xTraceEventAdd32(xTraceHandle, ret);
			xTraceEventEnd(xTraceHandle);
		}
	}
}

void sys_trace_k_work_flush_delayable_enter(struct k_work_delayable *dwork, struct k_work_sync *sync) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_DWORK_FLUSH_BLOCKING, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)dwork);
		xTraceEventAddPointer(xTraceHandle, (void*)sync);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_flush_delayable_exit(struct k_work_delayable *dwork, struct k_work_sync *sync, bool ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_DWORK_FLUSH_SUCCESS, sizeof(void*) + sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)dwork);
		xTraceEventAddPointer(xTraceHandle, (void*)sync);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_cancel_delayable_enter(struct k_work_delayable *dwork) {
}

void sys_trace_k_work_cancel_delayable_exit(struct k_work_delayable *dwork, int ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_DWORK_CANCEL_DELAYABLE_SUCCESS, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)dwork);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_cancel_delayable_sync_enter(struct k_work_delayable *dwork, struct k_work_sync *sync) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_DWORK_CANCEL_DELAYABLE_SYNC_BLOCKING, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)dwork);
		xTraceEventAddPointer(xTraceHandle, (void*)sync);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_cancel_delayable_sync_exit(struct k_work_delayable *dwork, struct k_work_sync *sync, bool ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_DWORK_CANCEL_DELAYABLE_SYNC_SUCCESS,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)dwork);
		xTraceEventAddPointer(xTraceHandle, (void*)sync);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Work poll trace function definitions */
void sys_trace_k_work_poll_init_enter(struct k_work_poll *work, k_work_handler_t handler) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_PWORK_INIT_ENTER, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAddPointer(xTraceHandle, (void*)handler);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_poll_init_exit(struct k_work_poll *work, k_work_handler_t handler) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_PWORK_INIT_EXIT, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAddPointer(xTraceHandle, (void*)handler);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_poll_submit_to_queue_enter(struct k_work_q *work_q, struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_PWORK_SUBMIT_TO_QUEUE_BLOCKING,
		sizeof(void*) + sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work_q);
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAddPointer(xTraceHandle, (void*)events);
		xTraceEventAdd32(xTraceHandle, num_events);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_poll_submit_to_queue_blocking(struct k_work_q *work_q, struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout) {

}

void sys_trace_k_work_poll_submit_to_queue_exit(struct k_work_q *work_q, struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_PWORK_SUBMIT_TO_QUEUE_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_PWORK_SUBMIT_TO_QUEUE_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work_q);
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAddPointer(xTraceHandle, (void*)events);
		xTraceEventAdd32(xTraceHandle, num_events);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_poll_submit_enter(struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_PWORK_SUBMIT_BLOCKING,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAddPointer(xTraceHandle, (void*)events);
		xTraceEventAdd32(xTraceHandle, num_events);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_poll_submit_exit(struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_PWORK_SUBMIT_SUCCESS, 
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t),
		&xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_PWORK_SUBMIT_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t),
		&xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAddPointer(xTraceHandle, (void*)events);
		xTraceEventAdd32(xTraceHandle, num_events);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_poll_cancel_enter(struct k_work_poll *work) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_PWORK_CANCEL_BLOCKING, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_work_poll_cancel_exit(struct k_work_poll *work, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_PWORK_CANCEL_SUCCESS, 
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_PWORK_CANCEL_FAILURE,
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)work);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Poll API trace function definitions */
void sys_trace_k_poll_api_event_init(struct k_poll_event *event, uint32_t type, int mode, void *obj) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_POLL_EVENT_INIT,
		sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)event);
		xTraceEventAdd32(xTraceHandle, type);
		xTraceEventAdd32(xTraceHandle, mode);
		xTraceEventAddPointer(xTraceHandle, (void*)obj);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_poll_api_event_poll_enter(struct k_poll_event *events, int num_events, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_POLL_POLL_BLOCKING,
		sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)events);
		xTraceEventAdd32(xTraceHandle, num_events);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_poll_api_event_poll_exit(struct k_poll_event *events, int num_events, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_POLL_POLL_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_POLL_POLL_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)events);
		xTraceEventAdd32(xTraceHandle, num_events);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_poll_api_signal_init(struct k_poll_signal *signal) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_POLL_SIGNAL_INIT, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)signal);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_poll_api_signal_reset(struct k_poll_signal *signal) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_POLL_SIGNAL_RESET, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)signal);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_poll_api_signal_check(struct k_poll_signal *signal, unsigned int *signaled, int *result) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_POLL_SIGNAL_CHECK,
		sizeof(void*) + sizeof(void*) + sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)signal);
		xTraceEventAddPointer(xTraceHandle, (void*)signaled);
		xTraceEventAddPointer(xTraceHandle, (void*)result);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_poll_api_signal_raise(struct k_poll_signal *signal, int result, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_POLL_SIGNAL_RAISE_SUCCESS, 
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_POLL_SIGNAL_RAISE_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)signal);
		xTraceEventAdd32(xTraceHandle, result);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Semaphore trace function definitions */
void sys_trace_k_sem_init(struct k_sem *sem, uint32_t initial_count, uint32_t limit, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_SEMAPHORE_CREATE_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_SEMAPHORE_CREATE_TIMEOUT,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)sem);
		xTraceEventAdd32(xTraceHandle, initial_count);
		xTraceEventAdd32(xTraceHandle, limit);
		xTraceEventAdd32(xTraceHandle, sem->count);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_sem_give_enter(struct k_sem *sem) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_SEMAPHORE_GIVE_SUCCESS, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)sem);
		xTraceEventAdd32(xTraceHandle, sem->count);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_sem_take_enter(struct k_sem *sem, k_timeout_t timeout) {

}

void sys_trace_k_sem_take_blocking(struct k_sem *sem, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_SEMAPHORE_TAKE_BLOCKING,
		sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)sem);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, sem->count);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_sem_take_exit(struct k_sem *sem, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_SEMAPHORE_TAKE_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_SEMAPHORE_TAKE_FAILED,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)sem);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, sem->count);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_sem_reset(struct k_sem *sem) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_SEMAPHORE_RESET, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)sem);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Mutex trace function definitions */
void sys_trace_k_mutex_init(struct k_mutex *mutex, int ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MUTEX_CREATE, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)mutex);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_mutex_lock_enter(struct k_mutex *mutex, k_timeout_t timeout) {
}

void sys_trace_k_mutex_lock_blocking(struct k_mutex *mutex, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MUTEX_TAKE_BLOCKING, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)mutex);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_mutex_lock_exit(struct k_mutex *mutex, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MUTEX_TAKE_SUCCESS, 
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MUTEX_TAKE_FAILED,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)mutex);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_mutex_unlock_enter(struct k_mutex *mutex) {
}

void sys_trace_k_mutex_unlock_exit(struct k_mutex *mutex, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MUTEX_GIVE_SUCCESS,
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MUTEX_GIVE_FAILED,
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)mutex);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Conditional variable trace function definitions */
void sys_trace_k_condvar_init(struct k_condvar *condvar, int ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_CONDVAR_INIT,
		sizeof(void*) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)condvar);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_condvar_signal_enter(struct k_condvar *condvar) {

}

void sys_trace_k_condvar_signal_blocking(struct k_condvar *condvar) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_CONDVAR_SIGNAL_BLOCKING, sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)condvar);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_condvar_signal_exit(struct k_condvar *condvar, int ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_CONDVAR_SIGNAL_SUCCESS, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)condvar);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_condvar_broadcast_enter(struct k_condvar *condvar) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_CONDVAR_BROADCAST_ENTER, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)condvar);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_condvar_broadcast_exit(struct k_condvar *condvar, int ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_CONDVAR_BROADCAST_EXIT, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)condvar);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_condvar_wait_enter(struct k_condvar *condvar, struct k_mutex *mutex, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_CONDVAR_WAIT_BLOCKING,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)condvar);
		xTraceEventAddPointer(xTraceHandle, (void*)mutex);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_condvar_wait_exit(struct k_condvar *condvar, struct k_mutex *mutex, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_CONDVAR_WAIT_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_CONDVAR_WAIT_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)condvar);
		xTraceEventAddPointer(xTraceHandle, (void*)mutex);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Queue trace function definitions */
void sys_trace_k_queue_init(struct k_queue *queue) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_QUEUE_INIT, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_cancel_wait(struct k_queue *queue) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_QUEUE_CANCEL_WAIT, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventEnd(xTraceHandle);
	}
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

	TraceEventHandle_t xTraceHandle;
	
	if (xTraceEventBegin(PSF_EVENT_QUEUE_QUEUE_INSERT_BLOCKING, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_queue_insert_exit(struct k_queue *queue, bool alloc, void *data, int ret) {
	// Ignore non alloc tracing of this event
	if (!alloc) {
		return;
	}

	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_QUEUE_INSERT_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_QUEUE_INSERT_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_append_enter(struct k_queue *queue, void *data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_QUEUE_APPEND, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_append_exit(struct k_queue *queue, void *data) {
}

void sys_trace_k_queue_alloc_append_enter(struct k_queue *queue, void *data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_QUEUE_ALLOC_APPEND_BLOCKING, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_alloc_append_exit(struct k_queue *queue, void *data, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_ALLOC_APPEND_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_ALLOC_APPEND_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_prepend_enter(struct k_queue *queue, void *data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_QUEUE_PREPEND, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_prepend_exit(struct k_queue *queue, void *data) {
}

void sys_trace_k_queue_alloc_prepend_enter(struct k_queue *queue, void *data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_QUEUE_ALLOC_PREPEND_BLOCKING, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_alloc_prepend_exit(struct k_queue *queue, void *data, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_ALLOC_PREPEND_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_ALLOC_PREPEND_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_insert_enter(struct k_queue *queue, void *prev, void *data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_QUEUE_INSERT, sizeof(void*) + sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)prev);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_insert_exit(struct k_queue *queue, void *prev, void *data) {
}

void sys_trace_k_queue_append_list_enter(struct k_queue *queue, void *head, void *tail) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_QUEUE_APPEND_LIST_BLOCKING,
		sizeof(void*) + sizeof(void*) + sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)head);
		xTraceEventAddPointer(xTraceHandle, (void*)tail);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_append_list_exit(struct k_queue *queue, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_APPEND_LIST_SUCCESS, 
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_APPEND_LIST_FAILURE,
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}
	
	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_merge_slist_enter(struct k_queue *queue, sys_slist_t *list) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_QUEUE_MERGE_SLIST_BLOCKING, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)list);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_merge_slist_exit(struct k_queue *queue, sys_slist_t *list, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_MERGE_SLIST_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_MERGE_SLIST_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)list);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_get_blocking(struct k_queue *queue, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_QUEUE_GET_BLOCKING, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_get_exit(struct k_queue *queue, k_timeout_t timeout, void *ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret != NULL) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_GET_SUCCESS,
		sizeof(void*) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_GET_TIMEOUT,
			sizeof(void*) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAddPointer(xTraceHandle, (void*)ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_remove_enter(struct k_queue *queue, void *data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_QUEUE_REMOVE_BLOCKING, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_remove_exit(struct k_queue *queue, void *data, bool ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_REMOVE_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_REMOVE_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_unique_append_enter(struct k_queue *queue, void *data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_QUEUE_UNIQUE_APPEND_BLOCKING, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_unique_append_exit(struct k_queue *queue, void *data, bool ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_UNIQUE_APPEND_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_QUEUE_UNIQUE_APPEND_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_peek_head(struct k_queue *queue, void *ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_QUEUE_PEEK_HEAD, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_queue_peek_tail(struct k_queue *queue, void *ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_QUEUE_PEEK_TAIL, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)queue);
		xTraceEventAddPointer(xTraceHandle, (void*)ret);
		xTraceEventEnd(xTraceHandle);
	}
}


/* FIFO trace function definitions */
void sys_trace_k_fifo_init_enter(struct k_fifo *fifo) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_INIT_ENTER, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_init_exit(struct k_fifo *fifo) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_INIT_EXIT, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_cancel_wait_enter(struct k_fifo *fifo) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_CANCEL_WAIT_ENTER, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_cancel_wait_exit(struct k_fifo *fifo) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_CANCEL_WAIT_EXIT, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_put_enter(struct k_fifo *fifo, void *data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_PUT_ENTER, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_put_exit(struct k_fifo *fifo, void *data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_PUT_EXIT, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_alloc_put_enter(struct k_fifo *fifo, void *data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_ALLOC_PUT_BLOCKING, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_alloc_put_exit(struct k_fifo *fifo, void *data, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_FIFO_ALLOC_PUT_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_FIFO_ALLOC_PUT_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_put_list_enter(struct k_fifo *fifo, void *head, void *tail) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_PUT_LIST_ENTER,
		sizeof(void*) + sizeof(void*) + sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventAddPointer(xTraceHandle, (void*)head);
		xTraceEventAddPointer(xTraceHandle, (void*)tail);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_put_list_exit(struct k_fifo *fifo, void *head, void *tail) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_PUT_LIST_EXIT,
		sizeof(void*) + sizeof(void*) + sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventAddPointer(xTraceHandle, (void*)head);
		xTraceEventAddPointer(xTraceHandle, (void*)tail);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_put_slist_enter(struct k_fifo *fifo, sys_slist_t *list) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_PUT_SLIST_ENTER, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventAddPointer(xTraceHandle, (void*)list);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_put_slist_exit(struct k_fifo *fifo, sys_slist_t *list) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_PUT_SLIST_EXIT, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventAddPointer(xTraceHandle, (void*)list);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_get_enter(struct k_fifo *fifo, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_GET_BLOCKING, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_get_exit(struct k_fifo *fifo, k_timeout_t timeout, void *ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret != NULL) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_FIFO_GET_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_FIFO_GET_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAddPointer(xTraceHandle, (void*)ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_peek_head_enter(struct k_fifo *fifo) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_PEEK_HEAD_ENTER, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_peek_head_exit(struct k_fifo *fifo, void *ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_PEEK_HEAD_EXIT, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventAddPointer(xTraceHandle, (void*)ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_peek_tail_enter(struct k_fifo *fifo) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_PEEK_TAIL_ENTER, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_fifo_peek_tail_exit(struct k_fifo *fifo, void *ret) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_FIFO_PEEK_TAIL_EXIT, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)fifo);
		xTraceEventAddPointer(xTraceHandle, (void*)ret);
		xTraceEventEnd(xTraceHandle);
	}
}


/* LIFO trace function definitions */
void sys_trace_k_lifo_init_enter(struct k_lifo *lifo) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_LIFO_INIT_ENTER, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)lifo);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_lifo_init_exit(struct k_lifo *lifo) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_LIFO_INIT_EXIT, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)lifo);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_lifo_put_enter(struct k_lifo *lifo, void *data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_LIFO_PUT_ENTER, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)lifo);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_lifo_put_exit(struct k_lifo *lifo, void *data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_LIFO_PUT_EXIT, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)lifo);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_lifo_alloc_put_enter(struct k_lifo *lifo, void *data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_LIFO_ALLOC_PUT_BLOCKING, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)lifo);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_lifo_alloc_put_exit(struct k_lifo *lifo, void *data, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_LIFO_ALLOC_PUT_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_LIFO_ALLOC_PUT_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)lifo);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_lifo_get_enter(struct k_lifo *lifo, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_LIFO_GET_BLOCKING, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)lifo);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_lifo_get_exit(struct k_lifo *lifo, k_timeout_t timeout, void *ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret != NULL) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_LIFO_GET_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_LIFO_GET_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	}
	
	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)lifo);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAddPointer(xTraceHandle, (void*)ret);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Stack trace function definitions */
void sys_trace_k_stack_init(struct k_stack *stack, stack_data_t *buffer, uint32_t num_entries) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_STACK_INIT, sizeof(void*) + sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)stack);
		xTraceEventAddPointer(xTraceHandle, (void*)buffer);
		xTraceEventAdd32(xTraceHandle, num_entries);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_stack_alloc_init_enter(struct k_stack *stack, uint32_t num_entries) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_STACK_ALLOC_INIT_BLOCKING, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)stack);
		xTraceEventAdd32(xTraceHandle, num_entries);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_stack_alloc_init_exit(struct k_stack *stack, uint32_t num_entries, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_STACK_ALLOC_INIT_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_STACK_ALLOC_INIT_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)stack);
		xTraceEventAdd32(xTraceHandle, num_entries);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_stack_cleanup_enter(struct k_stack *stack) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_STACK_CLEANUP_BLOCKING, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)stack);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_stack_cleanup_exit(struct k_stack *stack, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_STACK_CLEANUP_SUCCESS,
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_STACK_CLEANUP_FAILURE,
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)stack);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_stack_push_enter(struct k_stack *stack, stack_data_t data) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_STACK_PUSH_BLOCKING, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)stack);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_stack_push_exit(struct k_stack *stack, stack_data_t data, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_STACK_PUSH_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_STACK_PUSH_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)stack);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_stack_pop_blocking(struct k_stack *stack, stack_data_t *data, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_STACK_POP_BLOCKING,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)stack);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_stack_pop_exit(struct k_stack *stack, stack_data_t *data, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_STACK_POP_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_STACK_POP_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)stack);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Message queue trace function definitions */
void sys_trace_k_msgq_init(struct k_msgq *msgq) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_INIT,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)msgq);
		xTraceEventAddPointer(xTraceHandle, (void*)msgq->buffer_start);
		xTraceEventAdd32(xTraceHandle, (uint32_t)msgq->msg_size);
		xTraceEventAdd32(xTraceHandle, (uint32_t)msgq->max_msgs);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_msgq_alloc_init_enter(struct k_msgq *msgq, size_t msg_size, uint32_t max_msgs) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_ALLOC_INIT_BLOCKING,
		sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)msgq);
		xTraceEventAdd32(xTraceHandle, (uint32_t)msg_size);
		xTraceEventAdd32(xTraceHandle, (uint32_t)max_msgs);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_msgq_alloc_init_exit(struct k_msgq *msgq, size_t msg_size, uint32_t max_msgs, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_ALLOC_INIT_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_ALLOC_INIT_TIMEOUT,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)msgq);
		xTraceEventAdd32(xTraceHandle, (uint32_t)msg_size);
		xTraceEventAdd32(xTraceHandle, (uint32_t)max_msgs);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_msgq_cleanup_enter(struct k_msgq *msgq) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_CLEANUP_BLOCKING, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)msgq);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_msgq_cleanup_exit(struct k_msgq *msgq, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_CLEANUP_SUCCESS,
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_CLEANUP_TIMEOUT,
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)msgq);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_msgq_put_enter(struct k_msgq *msgq, const void *data, k_timeout_t timeout) {
}

void sys_trace_k_msgq_put_blocking(struct k_msgq *msgq, const void *data, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_PUT_BLOCKING,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)msgq);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_msgq_put_exit(struct k_msgq *msgq, const void *data, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_PUT_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_PUT_TIMEOUT,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)msgq);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_msgq_get_enter(struct k_msgq *msgq, const void *data, k_timeout_t timeout) {
}

void sys_trace_k_msgq_get_blocking(struct k_msgq *msgq, const void *data, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_GET_BLOCKING,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)msgq);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_msgq_get_exit(struct k_msgq *msgq, const void *data, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_GET_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_GET_TIMEOUT,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)msgq);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_msgq_peek(struct k_msgq *msgq, void *data, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_PEEK_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_PEEK_FAILED,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)msgq);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, (uint32_t)ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_msgq_purge(struct k_msgq *msgq) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MESSAGEQUEUE_PURGE, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)msgq);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Mailbox trace function definitions */
void sys_trace_k_mbox_init(struct k_mbox *mbox) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MAILBOX_INIT, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)mbox);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_mbox_message_put_enter(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout) {
}

void sys_trace_k_mbox_message_put_blocking(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MAILBOX_MESSAGE_PUT_BLOCKING,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)mbox);
		xTraceEventAddPointer(xTraceHandle, (void*)tx_msg);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_mbox_message_put_exit(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MAILBOX_MESSAGE_PUT_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MAILBOX_MESSAGE_PUT_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)mbox);
		xTraceEventAddPointer(xTraceHandle, (void*)tx_msg);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_mbox_put_enter(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MAILBOX_PUT_BLOCKING,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)mbox);
		xTraceEventAddPointer(xTraceHandle, (void*)tx_msg);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_mbox_put_exit(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MAILBOX_PUT_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MAILBOX_PUT_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)mbox);
		xTraceEventAddPointer(xTraceHandle, (void*)tx_msg);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_mbox_async_put_enter(struct k_mbox *mbox, struct k_sem *sem) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MAILBOX_ASYNC_PUT_ENTER, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)mbox);
		xTraceEventAddPointer(xTraceHandle, (void*)sem);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_mbox_async_put_exit(struct k_mbox *mbox, struct k_sem *sem) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MAILBOX_ASYNC_PUT_EXIT, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)mbox);
		xTraceEventAddPointer(xTraceHandle, (void*)sem);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_mbox_get_enter(struct k_mbox *mbox, struct k_mbox_msg *rx_msg, void *buffer, k_timeout_t timeout) {
}

void sys_trace_k_mbox_get_blocking(struct k_mbox *mbox, struct k_mbox_msg *rx_msg, void *buffer, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MAILBOX_GET_BLOCKING,
		sizeof(void*) + sizeof(void*) + sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)mbox);
		xTraceEventAddPointer(xTraceHandle, (void*)rx_msg);
		xTraceEventAddPointer(xTraceHandle, (void*)buffer);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_mbox_get_exit(struct k_mbox *mbox, struct k_mbox_msg *rx_msg, void *buffer, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MAILBOX_GET_SUCCESS,
		sizeof(void*) + sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t),
		&xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MAILBOX_GET_TIMEOUT,
		sizeof(void*) + sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t),
		&xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)mbox);
		xTraceEventAddPointer(xTraceHandle, (void*)rx_msg);
		xTraceEventAddPointer(xTraceHandle, (void*)buffer);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

/* @note: Hook not implemented in Zephyr kernel */
void sys_trace_k_mbox_data_get(struct k_mbox_msg *rx_msg, void *buffer) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MAILBOX_DATA_GET,
		sizeof(void*) + sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)rx_msg);
		xTraceEventAddPointer(xTraceHandle, (void*)buffer);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Pipe trace function definitions */
void sys_trace_k_pipe_init(struct k_pipe *pipe, unsigned char *buffer, size_t size) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_PIPE_INIT,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pipe);
		xTraceEventAddPointer(xTraceHandle, (void*)buffer);
		xTraceEventAdd32(xTraceHandle, (uint32_t)size);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_pipe_cleanup_enter(struct k_pipe *pipe) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_PIPE_CLEANUP_BLOCKING, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pipe);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_pipe_cleanup_exit(struct k_pipe *pipe, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_PIPE_CLEANUP_SUCCESS,
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_PIPE_CLEANUP_FAILURE,
			sizeof(void*) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pipe);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_pipe_alloc_init_enter(struct k_pipe *pipe, size_t size) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_PIPE_ALLOC_INIT_BLOCKING,
		sizeof(void*) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pipe);
		xTraceEventAdd32(xTraceHandle, (uint32_t)size);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_pipe_alloc_init_exit(struct k_pipe *pipe, size_t size, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_PIPE_ALLOC_INIT_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_PIPE_ALLOC_INIT_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pipe);
		xTraceEventAdd32(xTraceHandle, (uint32_t)size);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_pipe_put_enter(struct k_pipe *pipe, void *data, size_t bytes_to_write, size_t *bytes_written, size_t min_xfer, k_timeout_t timeout) {

}

void sys_trace_k_pipe_put_blocking(struct k_pipe *pipe, void *data, size_t bytes_to_write, size_t *bytes_written, size_t min_xfer, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_PIPE_PUT_BLOCKING,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pipe);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, (uint32_t)bytes_to_write);
		xTraceEventAddPointer(xTraceHandle, (void*)bytes_written);
		xTraceEventAdd32(xTraceHandle, (uint32_t)min_xfer);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_pipe_put_exit(struct k_pipe *pipe, void *data, size_t bytes_to_write, size_t *bytes_written, size_t min_xfer, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_PIPE_PUT_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_PIPE_PUT_TIMEOUT,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pipe);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, (uint32_t)bytes_to_write);
		xTraceEventAddPointer(xTraceHandle, (void*)bytes_written);
		xTraceEventAdd32(xTraceHandle, (uint32_t)min_xfer);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_pipe_get_enter(struct k_pipe *pipe, void *data, size_t bytes_to_read, size_t *bytes_read, size_t min_xfer, k_timeout_t timeout) {

}

void sys_trace_k_pipe_get_blocking(struct k_pipe *pipe, void *data, size_t bytes_to_read, size_t *bytes_read, size_t min_xfer, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_PIPE_GET_BLOCKING,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pipe);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, (uint32_t)bytes_to_read);
		xTraceEventAddPointer(xTraceHandle, (void*)bytes_read);
		xTraceEventAdd32(xTraceHandle, (uint32_t)min_xfer);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_pipe_get_exit(struct k_pipe *pipe, void *data, size_t bytes_to_read, size_t *bytes_read, size_t min_xfer, k_timeout_t timeout, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_PIPE_GET_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_PIPE_GET_TIMEOUT,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pipe);
		xTraceEventAddPointer(xTraceHandle, (void*)data);
		xTraceEventAdd32(xTraceHandle, (uint32_t)bytes_to_read);
		xTraceEventAddPointer(xTraceHandle, (void*)bytes_read);
		xTraceEventAdd32(xTraceHandle, (uint32_t)min_xfer);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_pipe_block_put_enter(struct k_pipe *pipe, struct k_mem_block *block, size_t size, struct k_sem *sem) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_PIPE_BLOCK_PUT_ENTER,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pipe);
		xTraceEventAddPointer(xTraceHandle, (void*)block);
		xTraceEventAdd32(xTraceHandle, (uint32_t)size);
		xTraceEventAddPointer(xTraceHandle, (void*)sem);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_pipe_block_put_exit(struct k_pipe *pipe, struct k_mem_block *block, size_t size, struct k_sem *sem) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_PIPE_BLOCK_PUT_EXIT,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pipe);
		xTraceEventAddPointer(xTraceHandle, (void*)block);
		xTraceEventAdd32(xTraceHandle, (uint32_t)size);
		xTraceEventAddPointer(xTraceHandle, (void*)sem);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Memory heap trace function definitions */
void sys_trace_k_heap_init(struct k_heap *h, void *mem, size_t bytes) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_KHEAP_INIT, sizeof(void*) + sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)h);
		xTraceEventAddPointer(xTraceHandle, (void*)mem);
		xTraceEventAdd32(xTraceHandle, (uint32_t)bytes);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_heap_alloc_enter(struct k_heap *h, size_t bytes, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_KHEAP_ALLOC_BLOCKING, sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)h);
		xTraceEventAdd32(xTraceHandle, bytes);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_heap_alloc_exit(struct k_heap *h, size_t bytes, k_timeout_t timeout, void *ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret != NULL) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_KHEAP_ALLOC_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_KHEAP_ALLOC_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)h);
		xTraceEventAdd32(xTraceHandle, bytes);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAddPointer(xTraceHandle, (void*)ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_heap_aligned_alloc_enter(struct k_heap *h, size_t bytes, k_timeout_t timeout) {

}

void sys_trace_k_heap_aligned_alloc_blocking(struct k_heap *h, size_t bytes, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_KHEAP_ALIGNED_ALLOC_BLOCKING,
		sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)h);
		xTraceEventAdd32(xTraceHandle, bytes);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAdd32(xTraceHandle, 0);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_heap_aligned_alloc_exit(struct k_heap *h, size_t bytes, k_timeout_t timeout, void *ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	/**
	 * There seems to be an issue with k_heap and forever timeouts where if there is not enough memory for direct allocation
	 * the system stops trying instantly and returns NULL.
	 */
	if (ret == NULL && K_TIMEOUT_EQ(timeout, K_FOREVER)) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_KHEAP_ALIGNED_ALLOC_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	} else if (ret == NULL) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_KHEAP_ALIGNED_ALLOC_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_KHEAP_ALIGNED_ALLOC_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)h);
		xTraceEventAdd32(xTraceHandle, (uint32_t)bytes);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventAddPointer(xTraceHandle, (void*)ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_heap_free(struct k_heap *h, void *mem) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_KHEAP_FREE, sizeof(void*) + sizeof(void*),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)h);
		xTraceEventAddPointer(xTraceHandle, (void*)mem);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_heap_sys_k_aligned_alloc_enter(struct k_heap *h, size_t align, size_t size) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_KHEAP_SYS_ALIGNED_ALLOC_BLOCKING,
		sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)h);
		xTraceEventAdd32(xTraceHandle, align);
		xTraceEventAdd32(xTraceHandle, size);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_heap_sys_k_aligned_alloc_exit(struct k_heap *h, size_t align, size_t size, void *ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret != NULL) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_KHEAP_SYS_ALIGNED_ALLOC_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_KHEAP_SYS_ALIGNED_ALLOC_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)h);
		xTraceEventAdd32(xTraceHandle, align);
		xTraceEventAdd32(xTraceHandle, size);
		xTraceEventAddPointer(xTraceHandle, (void*)ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_heap_sys_k_malloc_enter(struct k_heap *h, size_t size) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_KHEAP_SYS_MALLOC_BLOCKING, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)h);
		xTraceEventAdd32(xTraceHandle, size);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_heap_sys_k_malloc_exit(struct k_heap *h, size_t size, void *ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret != NULL) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_KHEAP_SYS_MALLOC_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_KHEAP_SYS_MALLOC_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)h);
		xTraceEventAdd32(xTraceHandle, size);
		xTraceEventAddPointer(xTraceHandle, (void*)ret);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_heap_sys_k_free_enter(struct k_heap *h) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_KHEAP_SYS_FREE_ENTER, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)h);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_heap_sys_k_free_exit(struct k_heap *h) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_KHEAP_SYS_FREE_EXIT, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)h);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_heap_sys_k_enter(struct k_heap *h, size_t nmemb, size_t size) {

}

void sys_trace_k_heap_sys_k_exit(struct k_heap *h, size_t nmemb, size_t size, void *ret) {

}

void sys_trace_k_heap_sys_k_calloc_enter(struct k_heap *h, size_t nmemb, size_t size) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_KHEAP_SYS_CALLOC_BLOCKING,
		sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)h);
		xTraceEventAdd32(xTraceHandle, nmemb);
		xTraceEventAdd32(xTraceHandle, size);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_heap_sys_k_calloc_exit(struct k_heap *h, size_t nmemb, size_t size, void *ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret != NULL) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_KHEAP_SYS_CALLOC_SUCCESS,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_KHEAP_SYS_CALLOC_FAILURE,
			sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(void*), &xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)h);
		xTraceEventAdd32(xTraceHandle, nmemb);
		xTraceEventAdd32(xTraceHandle, size);
		xTraceEventAddPointer(xTraceHandle, (void*)ret);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Memory slab trace function definitions */
void sys_trace_k_mem_slab_init(struct k_mem_slab *slab, void *buffer, size_t block_size, uint32_t num_blocks, int ret) {
	traceResult xTraceResult;
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MEMORY_SLAB_INIT_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle);
	} else {
		xTraceResult = xTraceEventBegin(PSF_EVENT_MEMORY_SLAB_INIT_FAILURE,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle);
	}

	if (xTraceResult == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)slab);
		xTraceEventAddPointer(xTraceHandle, (void*)slab->buffer);
		xTraceEventAdd32(xTraceHandle, (uint32_t)slab->block_size);
		xTraceEventAdd32(xTraceHandle, (uint32_t)slab->num_blocks);
		xTraceEventAdd32(xTraceHandle, ret);
		xTraceEventAdd32(xTraceHandle, (uint32_t)slab->num_blocks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_mem_slab_alloc_enter(struct k_mem_slab *slab, void **mem, k_timeout_t timeout) {

}

void sys_trace_k_mem_slab_alloc_blocking(struct k_mem_slab *slab, void **mem, k_timeout_t timeout) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MEMORY_SLAB_ALLOC_BLOCKING,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)slab);
		xTraceEventAddPointer(xTraceHandle, (void*)mem);
		xTraceEventAdd32(xTraceHandle, timeout.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_mem_slab_alloc_exit(struct k_mem_slab *slab, void **mem, k_timeout_t timeout, int ret) {
	TraceEventHandle_t xTraceHandle;

	if (ret == 0) {
		if (xTraceEventBegin(PSF_EVENT_MEMORY_SLAB_ALLOC_SUCCESS,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle) == TRC_SUCCESS) {
			xTraceEventAddPointer(xTraceHandle, (void*)slab);
			xTraceEventAddPointer(xTraceHandle, (void*)mem);
			xTraceEventAdd32(xTraceHandle, timeout.ticks);
			xTraceEventAdd32(xTraceHandle, ret);
			xTraceEventAdd32(xTraceHandle, slab->num_blocks);
			xTraceEventEnd(xTraceHandle);
		}
	} else if (ret == -ENOMEM || ret == -EAGAIN) {
		if (xTraceEventBegin(PSF_EVENT_MEMORY_SLAB_ALLOC_TIMEOUT,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle) == TRC_SUCCESS) {
			xTraceEventAddPointer(xTraceHandle, (void*)slab);
			xTraceEventAddPointer(xTraceHandle, (void*)mem);
			xTraceEventAdd32(xTraceHandle, timeout.ticks);
			xTraceEventAdd32(xTraceHandle, ret);
			xTraceEventEnd(xTraceHandle);
		}
	} else {
		if (xTraceEventBegin(PSF_EVENT_MEMORY_SLAB_ALLOC_ERROR,
			sizeof(void*) + sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t),
			&xTraceHandle) == TRC_SUCCESS) {
			xTraceEventAddPointer(xTraceHandle, (void*)slab);
			xTraceEventAddPointer(xTraceHandle, (void*)mem);
			xTraceEventAdd32(xTraceHandle, timeout.ticks);
			xTraceEventAdd32(xTraceHandle, ret);
			xTraceEventEnd(xTraceHandle);
		}
	}
}

void sys_trace_k_mem_slab_free_exit(struct k_mem_slab *slab, void **mem) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_MEMORY_SLAB_FREE,
		sizeof(void*) + sizeof(void*) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)slab);
		xTraceEventAddPointer(xTraceHandle, (void*)mem);
		xTraceEventAdd32(xTraceHandle, slab->num_blocks);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Timer trace function definitions */
void sys_trace_k_timer_init(struct k_timer *timer, k_timer_expiry_t expiry_fn, k_timer_expiry_t stop_fn) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_TIMER_INIT,
		sizeof(void*) + sizeof(void*) + sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)timer);
		xTraceEventAddPointer(xTraceHandle, (void*)expiry_fn);
		xTraceEventAddPointer(xTraceHandle, (void*)stop_fn);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_timer_start(struct k_timer *timer, k_timeout_t duration, k_timeout_t period) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_TIMER_START,
		sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)timer);
		xTraceEventAdd32(xTraceHandle, duration.ticks);
		xTraceEventAdd32(xTraceHandle, period.ticks);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_timer_stop(struct k_timer *timer) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_TIMER_STOP, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)timer);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_timer_status_sync_blocking(struct k_timer *timer) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_TIMER_STATUS_SYNC_AWAIT, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)timer);
		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_k_timer_status_sync_exit(struct k_timer *timer, uint32_t result) {
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_TIMER_STATUS_SYNC_EXIT, sizeof(void*) + sizeof(uint32_t),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)timer);
		xTraceEventAdd32(xTraceHandle, result);
		xTraceEventEnd(xTraceHandle);
	}
}


/* Syscall trace function definitions */
void sys_trace_syscall_enter(uint32_t id, const char *name) {
	TraceEventHandle_t xTraceHandle;
	uint32_t uiRemainingBytes = 0;
	uint32_t uiNull = 0;

	if (xTraceEventBegin(PSF_EVENT_SYSTEM_SYSCALL_ENTER, sizeof(uint32_t) + strlen(name), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAdd32(xTraceHandle, id);

		/* Add name */
		xTraceEventAddData(xTraceHandle, (void*)name, strlen(name));

		/* Events are 4-bytes aligned, pad remainder of data */
		xTraceEventPayloadRemaining(xTraceHandle, &uiRemainingBytes);
		xTraceEventAddData(xTraceHandle, (void*)&uiNull, uiRemainingBytes);

		xTraceEventEnd(xTraceHandle);
	}
}

void sys_trace_syscall_exit(uint32_t id, const char *name) {
	TraceEventHandle_t xTraceHandle;
	uint32_t uiRemainingBytes = 0;
	uint32_t uiNull = 0;
	
	if (xTraceEventBegin(PSF_EVENT_SYSTEM_SYSCALL_EXIT, sizeof(uint32_t) + strlen(name), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAdd32(xTraceHandle, id);
		
		/* Add name */
		xTraceEventAddData(xTraceHandle, (void*)name, strlen(name));

		/* Events are 4-bytes aligned, pad remainder of data */
		xTraceEventPayloadRemaining(xTraceHandle, &uiRemainingBytes);
		xTraceEventAddData(xTraceHandle, (void*)&uiNull, uiRemainingBytes);

		xTraceEventEnd(xTraceHandle);
	}
}


/* Legacy trace functions that are pending refactoring/removal by
 * the Zephyr team.
 */
void sys_trace_isr_enter(void) {
	xTraceISRBegin(xHandleISR);
}

void sys_trace_isr_exit(void) {
	xTraceISREnd(0);
}

void sys_trace_isr_exit_to_scheduler(void) {
}

void sys_trace_idle(void) {
}

void sys_trace_void(unsigned int id) {
}
