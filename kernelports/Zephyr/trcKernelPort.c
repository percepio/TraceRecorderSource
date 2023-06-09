/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
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


#if (TRC_CFG_USE_SYSCALL_EXTENSION == 1)

/* syscall extension name */
#define SYSCALLS_EXTENSION_NAME "syscalls"

/* These definitions depend on the KERNEL_VERSION parameters generated during the Zephyr build process */
#define SYSCALLS_EXTENSION_VERSION_MAJOR KERNEL_VERSION_MAJOR
#define SYSCALLS_EXTENSION_VERSION_MINOR KERNEL_VERSION_MINOR
#define SYSCALLS_EXTENSION_VERSION_PATCH KERNEL_PATCHLEVEL

/* Event codes for Enter and Exit */
#define SYSCALLS_EXTENSION_EVENT_COUNT ((K_SYSCALL_LIMIT + 1) * 2)

#define xSyscallsExtensionEnable() (xTraceExtensionCreate(SYSCALLS_EXTENSION_NAME, SYSCALLS_EXTENSION_VERSION_MAJOR, SYSCALLS_EXTENSION_VERSION_MINOR, SYSCALLS_EXTENSION_VERSION_PATCH, SYSCALLS_EXTENSION_EVENT_COUNT, &pxKernelPortData->xSyscallsExtensionHandle))
#define xSyscallsExtensionEnter(id) prvTraceStoreEvent_None(xTraceExtensionGetEventId(pxKernelPortData->xSyscallsExtensionHandle, id))
#define xSyscallsExtensionExit(id) prvTraceStoreEvent_None(xTraceExtensionGetEventId(pxKernelPortData->xSyscallsExtensionHandle, id + (K_SYSCALL_LIMIT + 1)))

#endif

/* Generic Zephyr ISR handle used for all Zephyr ISRs that the user haven't
 * manually added tracing for. */
static TraceISRHandle_t xHandleISR;

/* Trace recorder controll thread stack */
static K_THREAD_STACK_DEFINE(TzCtrl_thread_stack, (TRC_CFG_CTRL_TASK_STACK_SIZE));

/* Forward declarations */
traceResult prvTraceObjectSendNameEvent(void* pvObject, const char* szName, uint32_t uiLength);


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
		(void)xTraceTzCtrl();

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
	TraceExtensionHandle_t xSyscallsExtensionHandle;
} TraceKernelPortData_t;

static TraceKernelPortData_t* pxKernelPortData TRC_CFG_RECORDER_DATA_ATTRIBUTE;

traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* pxBuffer)
{
	TRC_ASSERT_EQUAL_SIZE(TraceKernelPortDataBuffer_t, TraceKernelPortData_t);
	
	if (pxBuffer == 0)
	{
		return TRC_FAIL;
	}
	
	pxKernelPortData = (TraceKernelPortData_t*)pxBuffer;

	pxKernelPortData->xSystemHeapHandle = 0;
	pxKernelPortData->xSyscallsExtensionHandle = 0;

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
	return k_thread_stack_space_get(thread, (size_t*)puxUnusedStack);
}
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */

unsigned char xTraceKernelPortIsSchedulerSuspended(void)
{
	return 0;
}

void vTraceSetKernelObjectName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetWorkQueueName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetHeapName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetSemaphoreName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetMutexName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetCondvarName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetQueueName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetFIFOQueueName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetLIFOQueueName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetStackName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetMessageQueueName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetMailboxName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetPipeName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetMemoryHeapName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetMemorySlabName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

void vTraceSetTimerName(void* object, const char* name)
{
	(void)xTraceObjectSetNameWithoutHandle(object, name);
}

/**
 * @brief Initialize aspects of the recorder that must preceed the
 * kernel initialization (scheduling, threads, etc.).
 * 
 * @param[in] arg
 */
static int tracelyzer_pre_kernel_init(void)
{
	xTraceInitialize();

#if (TRC_CFG_USE_SYSCALL_EXTENSION == 1)
	xSyscallsExtensionEnable();
#endif

#ifdef CONFIG_PERCEPIO_TRC_START_MODE_START
	(void)xTraceEnable(TRC_START);
#elif CONFIG_PERCEPIO_TRC_START_MODE_START_AWAIT_HOST
	(void)xTraceEnable(TRC_START_AWAIT_HOST);
#else
	(void)xTraceEnable(TRC_START_FROM_HOST);
#endif

	/* Create ISR handle */
	(void)xTraceISRRegister("Zephyr ISR", -32, &xHandleISR);

	return 0;
}

/**
 * @brief Initialize aspects of the recorder that depends on the kernel
 * being initialized.
 * 
 * @param[in] arg
 */
static int tracealyzer_post_kernel_init(void)
{
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
	(void)xTraceEventCreate2(PSF_EVENT_THREAD_FOREACH_ENTER, (TraceUnsignedBaseType_t)user_cb, (TraceUnsignedBaseType_t)user_data);
}

void sys_trace_k_thread_foreach_exit(k_thread_user_cb_t user_cb, void *user_data) {
	(void)xTraceEventCreate0(PSF_EVENT_THREAD_FOREACH_EXIT);
}

void sys_trace_k_thread_foreach_unlocked_enter(k_thread_user_cb_t user_cb, void *user_data) {
	(void)xTraceEventCreate2(PSF_EVENT_THREAD_FOREACH_UNLOCKED_ENTER, (TraceUnsignedBaseType_t)user_cb, (TraceUnsignedBaseType_t)user_data);
}

void sys_trace_k_thread_foreach_unlocked_exit(k_thread_user_cb_t user_cb, void *user_data) {
	(void)xTraceEventCreate0(PSF_EVENT_THREAD_FOREACH_UNLOCKED_EXIT);
}

void sys_trace_k_thread_create(struct k_thread *thread, size_t stack_size, int prio) {
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

	(void)xTraceEventCreate3(PSF_EVENT_THREAD_INIT, (TraceUnsignedBaseType_t)thread, (TraceUnsignedBaseType_t)stack_size, (TraceUnsignedBaseType_t)prio);

#ifdef CONFIG_THREAD_NAME
	if (strlen(thread->name) > 0) {
		xTraceObjectSetName(xEntryHandle, thread->name);
	}
#endif
}

void sys_trace_k_thread_user_mode_enter(k_thread_entry_t entry, void *p1, void *p2, void *p3) {
	(void)xTraceEventCreate5(
		PSF_EVENT_THREAD_USER_MODE_ENTER,
		(TraceUnsignedBaseType_t)k_current_get(),
		(TraceUnsignedBaseType_t)entry,
		(TraceUnsignedBaseType_t)p1,
		(TraceUnsignedBaseType_t)p2,
		(TraceUnsignedBaseType_t)p3
	);
}

void sys_trace_k_thread_heap_assign(struct k_thread *thread, struct k_heap *heap) {
	(void)xTraceEventCreate2(PSF_EVENT_THREAD_HEAP_ASSIGN, (TraceUnsignedBaseType_t)thread, (TraceUnsignedBaseType_t)heap);
}

void sys_trace_k_thread_join_blocking(struct k_thread *thread, k_timeout_t timeout) {
	(void)xTraceEventCreate2(PSF_EVENT_THREAD_JOIN_BLOCKING, (TraceUnsignedBaseType_t)thread, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_thread_join_exit(struct k_thread *thread, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_THREAD_JOIN_SUCCESS : PSF_EVENT_THREAD_JOIN_TIMEOUT, (TraceUnsignedBaseType_t)thread, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_thread_sleep_enter(k_timeout_t timeout) {
	(void)xTraceEventCreate1(PSF_EVENT_THREAD_SLEEP_ENTER, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_thread_sleep_exit(k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate1(PSF_EVENT_THREAD_SLEEP_EXIT, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_thread_msleep_enter(int32_t ms) {
	(void)xTraceEventCreate1(PSF_EVENT_THREAD_MSLEEP_ENTER, (TraceUnsignedBaseType_t)ms);
}

void sys_trace_k_thread_msleep_exit(int32_t ms, int ret) {
	(void)xTraceEventCreate0(PSF_EVENT_THREAD_MSLEEP_EXIT);
}

void sys_trace_k_thread_usleep_enter(int32_t us) {
	(void)xTraceEventCreate1(PSF_EVENT_THREAD_USLEEP_ENTER, (TraceUnsignedBaseType_t)us);
}

void sys_trace_k_thread_usleep_exit(int32_t us, int ret) {
	(void)xTraceEventCreate1(PSF_EVENT_THREAD_USLEEP_EXIT, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_thread_busy_wait_enter(uint32_t usec_to_wait) {
	(void)xTraceEventCreate1(PSF_EVENT_THREAD_BUSY_WAIT_ENTER, (TraceUnsignedBaseType_t)usec_to_wait);
}

void sys_trace_k_thread_busy_wait_exit(uint32_t usec_to_wait) {
	(void)xTraceEventCreate0(PSF_EVENT_THREAD_BUSY_WAIT_EXIT);
}

void sys_trace_k_thread_yield() {
	(void)xTraceEventCreate0(PSF_EVENT_THREAD_YIELD);
}

void sys_trace_k_thread_wakeup(struct k_thread *thread) {
	(void)xTraceEventCreate1(PSF_EVENT_THREAD_WAKEUP, (TraceUnsignedBaseType_t)thread);
}

void sys_trace_k_thread_abort(struct k_thread *thread) {
	/* Intentionally left empty, see k_thread_sched_abort for implementation */
}

void sys_trace_k_thread_start(struct k_thread *thread) {
	(void)xTraceEventCreate1(PSF_EVENT_THREAD_START, (TraceUnsignedBaseType_t)thread);
}

void sys_trace_k_thread_priority_set(struct k_thread *thread) {
	if (xTraceObjectSetStateWithoutHandle((void*)thread, k_thread_priority_get(thread)) == TRC_FAIL)
	{
		return;
	}

	(void)xTraceEventCreate2(PSF_EVENT_THREAD_SET_PRIORITY, (TraceUnsignedBaseType_t)thread, (TraceUnsignedBaseType_t)k_thread_priority_get(thread));
}

void sys_trace_k_thread_suspend(struct k_thread *thread) {
	(void)xTraceEventCreate1(PSF_EVENT_THREAD_SUSPEND, (TraceUnsignedBaseType_t)thread);
}

void sys_trace_k_thread_resume(struct k_thread *thread) {
	(void)xTraceEventCreate1(PSF_EVENT_THREAD_RESUME, (TraceUnsignedBaseType_t)thread);
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
	(void)xTraceEventCreate0(PSF_EVENT_THREAD_SCHED_LOCK);
}

void sys_trace_k_thread_sched_unlock() {
	(void)xTraceEventCreate0(PSF_EVENT_THREAD_SCHED_UNLOCK);
}

void sys_trace_k_thread_sched_wakeup(struct k_thread *thread) {
	(void)xTraceEventCreate1(PSF_EVENT_THREAD_SCHED_WAKEUP, (TraceUnsignedBaseType_t)thread);
}

void sys_trace_k_thread_sched_abort(struct k_thread *thread) {
	TraceEntryHandle_t xEntryHandle;

#if (TRC_SEND_NAME_ONLY_ON_DELETE == 1)
	uint32_t uiNameLength;
#endif

	TRACE_ALLOC_CRITICAL_SECTION();
	TRACE_ENTER_CRITICAL_SECTION();

	/* Fetch entry handle */
	if (xTraceEntryFind((void*)thread, &xEntryHandle) == TRC_FAIL)
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return;
	}

#if (TRC_SEND_NAME_ONLY_ON_DELETE == 1)
	if (strlen(thread->name) > 0) {
		/* Send name event because this is a delete */
		for (uiNameLength = 0; (thread->name[uiNameLength] != 0) && (uiNameLength < 128); uiNameLength++) {}

		prvTraceObjectSendNameEvent(thread, thread->name, uiNameLength);
	}
#endif /* (TRC_SEND_NAME_ONLY_ON_DELETE == 1) */
	
	/* Delete entry */
	if (xTraceEntryDelete(xEntryHandle) == TRC_FAIL)
	{
		TRACE_EXIT_CRITICAL_SECTION();
		return;
	}

	TRACE_EXIT_CRITICAL_SECTION();

	/* Remove thread from stack monitor */
	xTraceStackMonitorRemove((void*)thread);

	(void)xTraceEventCreate1(PSF_EVENT_THREAD_SCHED_ABORT, (TraceUnsignedBaseType_t)thread);
}

void sys_trace_k_thread_sched_set_priority(struct k_thread *thread, int prio) {
	(void)xTraceEventCreate2(PSF_EVENT_THREAD_SCHED_PRIORITY_SET, (TraceUnsignedBaseType_t)thread, (TraceUnsignedBaseType_t)prio);
}

void sys_trace_k_thread_sched_ready(struct k_thread *thread) {
	xTraceTaskReady((void*)thread);
}

void sys_trace_k_thread_sched_pend(struct k_thread *thread) {
	
}

void sys_trace_k_thread_sched_resume(struct k_thread *thread) {
	(void)xTraceEventCreate1(PSF_EVENT_THREAD_SCHED_RESUME, (TraceUnsignedBaseType_t)thread);
}

void sys_trace_k_thread_sched_suspend(struct k_thread *thread) {
	(void)xTraceEventCreate1(PSF_EVENT_THREAD_SCHED_SUSPEND, (TraceUnsignedBaseType_t)thread);
}


/* Work trace function definitions */
void sys_trace_k_work_init(struct k_work *work, k_work_handler_t handler) {
	(void)xTraceEventCreate2(PSF_EVENT_WORK_INIT, (TraceUnsignedBaseType_t)work, (TraceUnsignedBaseType_t)handler);
}

void sys_trace_k_work_submit_to_queue_enter(struct k_work_q *queue, struct k_work *work) {
	(void)xTraceEventCreate2(PSF_EVENT_WORK_SUBMIT_TO_QUEUE_BLOCKING, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)work);
}

void sys_trace_k_work_submit_to_queue_exit(struct k_work_q *queue, struct k_work *work, int ret) {
	(void)xTraceEventCreate3(
		ret >= 0 ? PSF_EVENT_WORK_SUBMIT_TO_QUEUE_SUCCESS : PSF_EVENT_WORK_SUBMIT_TO_QUEUE_FAILURE,
		(TraceUnsignedBaseType_t)queue,
		(TraceUnsignedBaseType_t)work,
		(TraceUnsignedBaseType_t)ret
	);
}

void sys_trace_k_work_submit_enter(struct k_work *work) {
	(void)xTraceEventCreate1(PSF_EVENT_WORK_SUBMIT_BLOCKING, (TraceUnsignedBaseType_t)work);
}

void sys_trace_k_work_submit_exit(struct k_work *work, int ret) {
	(void)xTraceEventCreate2(ret >= 0 ? PSF_EVENT_WORK_SUBMIT_SUCCESS : PSF_EVENT_WORK_SUBMIT_FAILURE, (TraceUnsignedBaseType_t)work, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_work_flush_enter(struct k_work *work, struct k_work_sync *sync) {
	(void)xTraceEventCreate2(PSF_EVENT_WORK_FLUSH_BLOCKING, (TraceUnsignedBaseType_t)work, (TraceUnsignedBaseType_t)sync);
}

void sys_trace_k_work_flush_blocking(struct k_work *work, struct k_work_sync *sync, k_timeout_t timeout) {
	
}

void sys_trace_k_work_flush_exit(struct k_work *work, struct k_work_sync *sync, bool ret) {
	(void)xTraceEventCreate2(PSF_EVENT_WORK_FLUSH_SUCCESS, (TraceUnsignedBaseType_t)work, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_work_cancel_enter(struct k_work *work) {
	(void)xTraceEventCreate1(PSF_EVENT_WORK_CANCEL_BLOCKING, (TraceUnsignedBaseType_t)work);
}

void sys_trace_k_work_cancel_exit(struct k_work *work, int ret) {
	(void)xTraceEventCreate2(PSF_EVENT_WORK_CANCEL_SUCCESS, (TraceUnsignedBaseType_t)work, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_work_cancel_sync_enter(struct k_work *work, struct k_work_sync *sync) {
	(void)xTraceEventCreate2(PSF_EVENT_WORK_CANCEL_SYNC_BLOCKING, (TraceUnsignedBaseType_t)work, (TraceUnsignedBaseType_t)sync);
}

void sys_trace_k_work_cancel_sync_blocking(struct k_work *work, struct k_work_sync *sync) {
	
}

void sys_trace_k_work_cancel_sync_exit(struct k_work *work, struct k_work_sync *sync, bool ret) {
	(void)xTraceEventCreate3(PSF_EVENT_WORK_CANCEL_SYNC_SUCCESS, (TraceUnsignedBaseType_t)work, (TraceUnsignedBaseType_t)sync, (TraceUnsignedBaseType_t)ret);
}


/* Work queue trace function definitions */
void sys_trace_k_work_queue_start_enter(struct k_work_q *queue, k_thread_stack_t *stack, size_t stack_size, int prio, const struct k_work_queue_config *cfg) {
	(void)xTraceEventCreate5(
		PSF_EVENT_WORK_QUEUE_START_BLOCKING,
		(TraceUnsignedBaseType_t)queue,
		(TraceUnsignedBaseType_t)stack,
		(TraceUnsignedBaseType_t)stack_size,
		(TraceUnsignedBaseType_t)prio,
		(TraceUnsignedBaseType_t)cfg
	);
}

void sys_trace_k_work_queue_start_exit(struct k_work_q *queue, k_thread_stack_t *stack, size_t stack_size, int prio, const struct k_work_queue_config *cfg) {
	(void)xTraceEventCreate1(PSF_EVENT_WORK_QUEUE_START_SUCCESS, (TraceUnsignedBaseType_t)queue);
}

void sys_trace_k_work_queue_drain_enter(struct k_work_q *queue, bool plug) {
	(void)xTraceEventCreate2(PSF_EVENT_WORK_QUEUE_DRAIN_BLOCKING, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)plug);
}

void sys_trace_k_work_queue_drain_exit(struct k_work_q *queue, bool plug, int ret) {
	(void)xTraceEventCreate2(ret >= 0 ? PSF_EVENT_WORK_QUEUE_DRAIN_SUCCESS : PSF_EVENT_WORK_QUEUE_DRAIN_FAILURE, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_work_queue_unplug_enter(struct k_work_q *queue) {
	(void)xTraceEventCreate1(PSF_EVENT_WORK_QUEUE_UNPLUG_BLOCKING, (TraceUnsignedBaseType_t)queue);
}

void sys_trace_k_work_queue_unplug_exit(struct k_work_q *queue, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_WORK_QUEUE_UNPLUG_SUCCESS : PSF_EVENT_WORK_QUEUE_UNPLUG_FAILURE, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)ret);
}


/* Work delayable trace function definitions */
void sys_trace_k_work_delayable_init(struct k_work_delayable *dwork, k_work_handler_t handler) {
	(void)xTraceEventCreate2(PSF_EVENT_DWORK_INIT, (TraceUnsignedBaseType_t)dwork, (TraceUnsignedBaseType_t)handler);
}

void sys_trace_k_work_schedule_for_queue_enter(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay) {
}

void sys_trace_k_work_schedule_for_queue_exit(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay, int ret) {
	(void)xTraceEventCreate4(((ret == 0) || (ret == 1)) ? PSF_EVENT_DWORK_SCHEDULE_FOR_QUEUE_SUCCESS : PSF_EVENT_DWORK_SCHEDULE_FOR_QUEUE_FAILURE, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)dwork, (TraceUnsignedBaseType_t)delay.ticks, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_work_schedule_enter(struct k_work_delayable *dwork, k_timeout_t delay) {
	(void)xTraceEventCreate2(PSF_EVENT_DWORK_SCHEDULE_BLOCKING, (TraceUnsignedBaseType_t)dwork, (TraceUnsignedBaseType_t)delay.ticks);
}

void sys_trace_k_work_schedule_exit(struct k_work_delayable *dwork, k_timeout_t delay, int ret) {
	(void)xTraceEventCreate2(((ret == 0) || (ret == 1)) ? PSF_EVENT_DWORK_SCHEDULE_SUCCESS : PSF_EVENT_DWORK_SCHEDULE_FAILURE, (TraceUnsignedBaseType_t)dwork, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_work_reschedule_for_queue_enter(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay) {
}

void sys_trace_k_work_reschedule_for_queue_exit(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay, int ret) {
	(void)xTraceEventCreate4(((ret == 0) || (ret == 1) || (ret == 2)) ? PSF_EVENT_DWORK_RESCHEDULE_FOR_QUEUE_SUCCESS : PSF_EVENT_DWORK_RESCHEDULE_FOR_QUEUE_FAILURE, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)dwork, (TraceUnsignedBaseType_t)delay.ticks, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_work_reschedule_enter(struct k_work_delayable *dwork, k_timeout_t delay) {
	(void)xTraceEventCreate2(PSF_EVENT_DWORK_RESCHEDULE_BLOCKING, (TraceUnsignedBaseType_t)dwork, (TraceUnsignedBaseType_t)delay.ticks);
}

void sys_trace_k_work_reschedule_exit(struct k_work_delayable *dwork, k_timeout_t delay, int ret) {
	(void)xTraceEventCreate2(((ret == 0) || (ret == 1) || (ret == 2)) ? PSF_EVENT_DWORK_RESCHEDULE_SUCCESS : PSF_EVENT_DWORK_RESCHEDULE_FAILURE, (TraceUnsignedBaseType_t)dwork, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_work_flush_delayable_enter(struct k_work_delayable *dwork, struct k_work_sync *sync) {
	(void)xTraceEventCreate2(PSF_EVENT_DWORK_FLUSH_BLOCKING, (TraceUnsignedBaseType_t)dwork, (TraceUnsignedBaseType_t)sync);
}

void sys_trace_k_work_flush_delayable_exit(struct k_work_delayable *dwork, struct k_work_sync *sync, bool ret) {
	(void)xTraceEventCreate2(PSF_EVENT_DWORK_FLUSH_SUCCESS, (TraceUnsignedBaseType_t)dwork, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_work_cancel_delayable_enter(struct k_work_delayable *dwork) {
}

void sys_trace_k_work_cancel_delayable_exit(struct k_work_delayable *dwork, int ret) {
	(void)xTraceEventCreate2(PSF_EVENT_DWORK_CANCEL_DELAYABLE_SUCCESS, (TraceUnsignedBaseType_t)dwork, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_cancel_delayable_sync_enter(struct k_work_delayable *dwork, struct k_work_sync *sync) {
	(void)xTraceEventCreate2(PSF_EVENT_DWORK_CANCEL_DELAYABLE_SYNC_BLOCKING, (TraceUnsignedBaseType_t)dwork, (TraceUnsignedBaseType_t)sync);
}

void sys_trace_cancel_delayable_sync_exit(struct k_work_delayable *dwork, struct k_work_sync *sync, bool ret) {
	(void)xTraceEventCreate2(PSF_EVENT_DWORK_CANCEL_DELAYABLE_SYNC_SUCCESS, (TraceUnsignedBaseType_t)dwork, (TraceUnsignedBaseType_t)ret);
}


/* Work poll trace function definitions */
void sys_trace_k_work_poll_init_enter(struct k_work_poll *work, k_work_handler_t handler) {
	(void)xTraceEventCreate2(PSF_EVENT_PWORK_INIT_ENTER, (TraceUnsignedBaseType_t)work, (TraceUnsignedBaseType_t)handler);
}

void sys_trace_k_work_poll_init_exit(struct k_work_poll *work, k_work_handler_t handler) {
	(void)xTraceEventCreate1(PSF_EVENT_PWORK_INIT_EXIT, (TraceUnsignedBaseType_t)work);
}

void sys_trace_k_work_poll_submit_to_queue_enter(struct k_work_q *work_q, struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout) {
	(void)xTraceEventCreate5(
		PSF_EVENT_PWORK_SUBMIT_TO_QUEUE_BLOCKING,
		(TraceUnsignedBaseType_t)work_q,
		(TraceUnsignedBaseType_t)work,
		(TraceUnsignedBaseType_t)events,
		(TraceUnsignedBaseType_t)num_events,
		(TraceUnsignedBaseType_t)timeout.ticks
	);
}

void sys_trace_k_work_poll_submit_to_queue_blocking(struct k_work_q *work_q, struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout) {

}

void sys_trace_k_work_poll_submit_to_queue_exit(struct k_work_q *work_q, struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate3(ret == 0 ? PSF_EVENT_PWORK_SUBMIT_TO_QUEUE_SUCCESS : PSF_EVENT_PWORK_SUBMIT_TO_QUEUE_FAILURE, (TraceUnsignedBaseType_t)work_q, (TraceUnsignedBaseType_t)work, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_work_poll_submit_enter(struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout) {
	(void)xTraceEventCreate4(PSF_EVENT_PWORK_SUBMIT_BLOCKING, (TraceUnsignedBaseType_t)work, (TraceUnsignedBaseType_t)events, (TraceUnsignedBaseType_t)num_events, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_work_poll_submit_exit(struct k_work_poll *work, struct k_poll_event *events, int num_events, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_PWORK_SUBMIT_SUCCESS : PSF_EVENT_PWORK_SUBMIT_FAILURE, (TraceUnsignedBaseType_t)work, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_work_poll_cancel_enter(struct k_work_poll *work) {
	(void)xTraceEventCreate1(PSF_EVENT_PWORK_CANCEL_BLOCKING, (TraceUnsignedBaseType_t)work);
}

void sys_trace_k_work_poll_cancel_exit(struct k_work_poll *work, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_PWORK_CANCEL_SUCCESS : PSF_EVENT_PWORK_CANCEL_FAILURE, (TraceUnsignedBaseType_t)work, (TraceUnsignedBaseType_t)ret);
}


/* Poll API trace function definitions */
void sys_trace_k_poll_api_event_init(struct k_poll_event *event, uint32_t type, int mode, void *obj) {
	(void)xTraceEventCreate4(PSF_EVENT_POLL_EVENT_INIT, (TraceUnsignedBaseType_t)event, (TraceUnsignedBaseType_t)type, (TraceUnsignedBaseType_t)mode, (TraceUnsignedBaseType_t)obj);
}

void sys_trace_k_poll_api_event_poll_enter(struct k_poll_event *events, int num_events, k_timeout_t timeout) {
	(void)xTraceEventCreate3(PSF_EVENT_POLL_POLL_BLOCKING, (TraceUnsignedBaseType_t)events, (TraceUnsignedBaseType_t)num_events, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_poll_api_event_poll_exit(struct k_poll_event *events, int num_events, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_POLL_POLL_SUCCESS : PSF_EVENT_POLL_POLL_FAILURE, (TraceUnsignedBaseType_t)events, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_poll_api_signal_init(struct k_poll_signal *signal) {
	(void)xTraceEventCreate1(PSF_EVENT_POLL_SIGNAL_INIT, (TraceUnsignedBaseType_t)signal);
}

void sys_trace_k_poll_api_signal_reset(struct k_poll_signal *signal) {
	(void)xTraceEventCreate1(PSF_EVENT_POLL_SIGNAL_RESET, (TraceUnsignedBaseType_t)signal);
}

void sys_trace_k_poll_api_signal_check(struct k_poll_signal *signal, unsigned int *signaled, int *result) {
	(void)xTraceEventCreate3(PSF_EVENT_POLL_SIGNAL_CHECK, (TraceUnsignedBaseType_t)signal, (TraceUnsignedBaseType_t)signaled, (TraceUnsignedBaseType_t)result);
}

void sys_trace_k_poll_api_signal_raise(struct k_poll_signal *signal, int result, int ret) {
	(void)xTraceEventCreate3(ret == 0 ? PSF_EVENT_POLL_SIGNAL_RAISE_SUCCESS : PSF_EVENT_POLL_SIGNAL_RAISE_FAILURE, (TraceUnsignedBaseType_t)signal, (TraceUnsignedBaseType_t)result, (TraceUnsignedBaseType_t)ret);
}


/* Semaphore trace function definitions */
void sys_trace_k_sem_init(struct k_sem *sem, unsigned int initial_count, unsigned int limit, int ret) {
	(void)xTraceEventCreate5(ret == 0 ? PSF_EVENT_SEMAPHORE_CREATE_SUCCESS : PSF_EVENT_SEMAPHORE_CREATE_TIMEOUT, (TraceUnsignedBaseType_t)sem, (TraceUnsignedBaseType_t)initial_count, (TraceUnsignedBaseType_t)limit, (TraceUnsignedBaseType_t)sem->count, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_sem_give_enter(struct k_sem *sem) {
	(void)xTraceEventCreate2(PSF_EVENT_SEMAPHORE_GIVE_SUCCESS, (TraceUnsignedBaseType_t)sem, (TraceUnsignedBaseType_t)sem->count);
}

void sys_trace_k_sem_take_enter(struct k_sem *sem, k_timeout_t timeout) {

}

void sys_trace_k_sem_take_blocking(struct k_sem *sem, k_timeout_t timeout) {
	(void)xTraceEventCreate3(PSF_EVENT_SEMAPHORE_TAKE_BLOCKING, (TraceUnsignedBaseType_t)sem, (TraceUnsignedBaseType_t)timeout.ticks, (TraceUnsignedBaseType_t)sem->count);
}

void sys_trace_k_sem_take_exit(struct k_sem *sem, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate3(ret == 0 ? PSF_EVENT_SEMAPHORE_TAKE_SUCCESS : PSF_EVENT_SEMAPHORE_TAKE_FAILED, (TraceUnsignedBaseType_t)sem, (TraceUnsignedBaseType_t)sem->count, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_sem_reset(struct k_sem *sem) {
	(void)xTraceEventCreate1(PSF_EVENT_SEMAPHORE_RESET, (TraceUnsignedBaseType_t)sem);
}


/* Mutex trace function definitions */
void sys_trace_k_mutex_init(struct k_mutex *mutex, int ret) {
	(void)xTraceEventCreate2(PSF_EVENT_MUTEX_CREATE, (TraceUnsignedBaseType_t)mutex, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_mutex_lock_enter(struct k_mutex *mutex, k_timeout_t timeout) {
}

void sys_trace_k_mutex_lock_blocking(struct k_mutex *mutex, k_timeout_t timeout) {
	(void)xTraceEventCreate2(PSF_EVENT_MUTEX_TAKE_BLOCKING, (TraceUnsignedBaseType_t)mutex, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_mutex_lock_exit(struct k_mutex *mutex, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_MUTEX_TAKE_SUCCESS : PSF_EVENT_MUTEX_TAKE_FAILED, (TraceUnsignedBaseType_t)mutex, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_mutex_unlock_enter(struct k_mutex *mutex) {
}

void sys_trace_k_mutex_unlock_exit(struct k_mutex *mutex, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_MUTEX_GIVE_SUCCESS : PSF_EVENT_MUTEX_GIVE_FAILED, (TraceUnsignedBaseType_t)mutex, (TraceUnsignedBaseType_t)ret);
}


/* Conditional variable trace function definitions */
void sys_trace_k_condvar_init(struct k_condvar *condvar, int ret) {
	(void)xTraceEventCreate2(PSF_EVENT_CONDVAR_INIT, (TraceUnsignedBaseType_t)condvar, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_condvar_signal_enter(struct k_condvar *condvar) {

}

void sys_trace_k_condvar_signal_blocking(struct k_condvar *condvar) {
	(void)xTraceEventCreate1(PSF_EVENT_CONDVAR_SIGNAL_BLOCKING, (TraceUnsignedBaseType_t)condvar);
}

void sys_trace_k_condvar_signal_exit(struct k_condvar *condvar, int ret) {
	(void)xTraceEventCreate2(PSF_EVENT_CONDVAR_SIGNAL_SUCCESS, (TraceUnsignedBaseType_t)condvar, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_condvar_broadcast_enter(struct k_condvar *condvar) {
	(void)xTraceEventCreate1(PSF_EVENT_CONDVAR_BROADCAST_ENTER, (TraceUnsignedBaseType_t)condvar);
}

void sys_trace_k_condvar_broadcast_exit(struct k_condvar *condvar, int ret) {
	(void)xTraceEventCreate2(PSF_EVENT_CONDVAR_BROADCAST_EXIT, (TraceUnsignedBaseType_t)condvar, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_condvar_wait_enter(struct k_condvar *condvar, struct k_mutex *mutex, k_timeout_t timeout) {
	(void)xTraceEventCreate3(PSF_EVENT_CONDVAR_WAIT_BLOCKING, (TraceUnsignedBaseType_t)condvar, (TraceUnsignedBaseType_t)mutex, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_condvar_wait_exit(struct k_condvar *condvar, struct k_mutex *mutex, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate3(ret == 0 ? PSF_EVENT_CONDVAR_WAIT_SUCCESS : PSF_EVENT_CONDVAR_WAIT_FAILURE, (TraceUnsignedBaseType_t)condvar, (TraceUnsignedBaseType_t)mutex, (TraceUnsignedBaseType_t)ret);
}


/* Queue trace function definitions */
void sys_trace_k_queue_init(struct k_queue *queue) {
	(void)xTraceEventCreate1(PSF_EVENT_QUEUE_INIT, (TraceUnsignedBaseType_t)queue);
}

void sys_trace_k_queue_cancel_wait(struct k_queue *queue) {
	(void)xTraceEventCreate1(PSF_EVENT_QUEUE_CANCEL_WAIT, (TraceUnsignedBaseType_t)queue);
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

	(void)xTraceEventCreate2(PSF_EVENT_QUEUE_QUEUE_INSERT_BLOCKING, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_queue_queue_insert_exit(struct k_queue *queue, bool alloc, void *data, int ret) {
	// Ignore non alloc tracing of this event
	if (!alloc) {
		return;
	}

	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_QUEUE_QUEUE_INSERT_SUCCESS : PSF_EVENT_QUEUE_QUEUE_INSERT_FAILURE, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_queue_append_enter(struct k_queue *queue, void *data) {
	(void)xTraceEventCreate2(PSF_EVENT_QUEUE_APPEND, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_queue_append_exit(struct k_queue *queue, void *data) {
}

void sys_trace_k_queue_alloc_append_enter(struct k_queue *queue, void *data) {
	(void)xTraceEventCreate2(PSF_EVENT_QUEUE_ALLOC_APPEND_BLOCKING, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_queue_alloc_append_exit(struct k_queue *queue, void *data, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_QUEUE_ALLOC_APPEND_SUCCESS : PSF_EVENT_QUEUE_ALLOC_APPEND_FAILURE, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_queue_prepend_enter(struct k_queue *queue, void *data) {
	(void)xTraceEventCreate2(PSF_EVENT_QUEUE_PREPEND, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_queue_prepend_exit(struct k_queue *queue, void *data) {
}

void sys_trace_k_queue_alloc_prepend_enter(struct k_queue *queue, void *data) {
	(void)xTraceEventCreate2(PSF_EVENT_QUEUE_ALLOC_PREPEND_BLOCKING, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_queue_alloc_prepend_exit(struct k_queue *queue, void *data, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_QUEUE_ALLOC_PREPEND_SUCCESS : PSF_EVENT_QUEUE_ALLOC_PREPEND_FAILURE, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_queue_insert_enter(struct k_queue *queue, void *prev, void *data) {
	(void)xTraceEventCreate3(PSF_EVENT_QUEUE_INSERT, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)prev, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_queue_insert_exit(struct k_queue *queue, void *prev, void *data) {
}

void sys_trace_k_queue_append_list_enter(struct k_queue *queue, void *head, void *tail) {
	(void)xTraceEventCreate3(PSF_EVENT_QUEUE_APPEND_LIST_BLOCKING, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)head, (TraceUnsignedBaseType_t)tail);
}

void sys_trace_k_queue_append_list_exit(struct k_queue *queue, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_QUEUE_APPEND_LIST_SUCCESS : PSF_EVENT_QUEUE_APPEND_LIST_FAILURE, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_queue_merge_slist_enter(struct k_queue *queue, sys_slist_t *list) {
	(void)xTraceEventCreate2(PSF_EVENT_QUEUE_MERGE_SLIST_BLOCKING, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)list);
}

void sys_trace_k_queue_merge_slist_exit(struct k_queue *queue, sys_slist_t *list, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_QUEUE_MERGE_SLIST_SUCCESS : PSF_EVENT_QUEUE_MERGE_SLIST_FAILURE, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_queue_get_blocking(struct k_queue *queue, k_timeout_t timeout) {
	(void)xTraceEventCreate2(PSF_EVENT_QUEUE_GET_BLOCKING, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_queue_get_exit(struct k_queue *queue, k_timeout_t timeout, void *ret) {
	(void)xTraceEventCreate2(ret != NULL ? PSF_EVENT_QUEUE_GET_SUCCESS : PSF_EVENT_QUEUE_GET_TIMEOUT, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_queue_remove_enter(struct k_queue *queue, void *data) {
	(void)xTraceEventCreate2(PSF_EVENT_QUEUE_REMOVE_BLOCKING, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_queue_remove_exit(struct k_queue *queue, void *data, bool ret) {
	(void)xTraceEventCreate2(ret ? PSF_EVENT_QUEUE_REMOVE_SUCCESS : PSF_EVENT_QUEUE_REMOVE_FAILURE, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_queue_unique_append_enter(struct k_queue *queue, void *data) {
	(void)xTraceEventCreate2(PSF_EVENT_QUEUE_UNIQUE_APPEND_BLOCKING, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_queue_unique_append_exit(struct k_queue *queue, void *data, bool ret) {
	(void)xTraceEventCreate2(ret ? PSF_EVENT_QUEUE_UNIQUE_APPEND_SUCCESS : PSF_EVENT_QUEUE_UNIQUE_APPEND_FAILURE, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_queue_peek_head(struct k_queue *queue, void *ret) {
	(void)xTraceEventCreate2(PSF_EVENT_QUEUE_PEEK_HEAD, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_queue_peek_tail(struct k_queue *queue, void *ret) {
	(void)xTraceEventCreate2(PSF_EVENT_QUEUE_PEEK_TAIL, (TraceUnsignedBaseType_t)queue, (TraceUnsignedBaseType_t)ret);
}


/* FIFO trace function definitions */
void sys_trace_k_fifo_init_enter(struct k_fifo *fifo) {
	(void)xTraceEventCreate1(PSF_EVENT_FIFO_INIT_ENTER, (TraceUnsignedBaseType_t)fifo);
}

void sys_trace_k_fifo_init_exit(struct k_fifo *fifo) {
	(void)xTraceEventCreate1(PSF_EVENT_FIFO_INIT_EXIT, (TraceUnsignedBaseType_t)fifo);
}

void sys_trace_k_fifo_cancel_wait_enter(struct k_fifo *fifo) {
	(void)xTraceEventCreate1(PSF_EVENT_FIFO_CANCEL_WAIT_ENTER, (TraceUnsignedBaseType_t)fifo);
}

void sys_trace_k_fifo_cancel_wait_exit(struct k_fifo *fifo) {
	(void)xTraceEventCreate1(PSF_EVENT_FIFO_CANCEL_WAIT_EXIT, (TraceUnsignedBaseType_t)fifo);
}

void sys_trace_k_fifo_put_enter(struct k_fifo *fifo, void *data) {
	(void)xTraceEventCreate2(PSF_EVENT_FIFO_PUT_ENTER, (TraceUnsignedBaseType_t)fifo, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_fifo_put_exit(struct k_fifo *fifo, void *data) {
	(void)xTraceEventCreate2(PSF_EVENT_FIFO_PUT_EXIT, (TraceUnsignedBaseType_t)fifo, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_fifo_alloc_put_enter(struct k_fifo *fifo, void *data) {
	(void)xTraceEventCreate2(PSF_EVENT_FIFO_ALLOC_PUT_BLOCKING, (TraceUnsignedBaseType_t)fifo, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_fifo_alloc_put_exit(struct k_fifo *fifo, void *data, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_FIFO_ALLOC_PUT_SUCCESS : PSF_EVENT_FIFO_ALLOC_PUT_FAILURE, (TraceUnsignedBaseType_t)fifo, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_fifo_put_list_enter(struct k_fifo *fifo, void *head, void *tail) {
	(void)xTraceEventCreate3(PSF_EVENT_FIFO_PUT_LIST_ENTER, (TraceUnsignedBaseType_t)fifo, (TraceUnsignedBaseType_t)head, (TraceUnsignedBaseType_t)tail);
}

void sys_trace_k_fifo_put_list_exit(struct k_fifo *fifo, void *head, void *tail) {
	(void)xTraceEventCreate3(PSF_EVENT_FIFO_PUT_LIST_EXIT, (TraceUnsignedBaseType_t)fifo, (TraceUnsignedBaseType_t)head, (TraceUnsignedBaseType_t)tail);
}

void sys_trace_k_fifo_put_slist_enter(struct k_fifo *fifo, sys_slist_t *list) {
	(void)xTraceEventCreate2(PSF_EVENT_FIFO_PUT_SLIST_ENTER, (TraceUnsignedBaseType_t)fifo, (TraceUnsignedBaseType_t)list);
}

void sys_trace_k_fifo_put_slist_exit(struct k_fifo *fifo, sys_slist_t *list) {
	(void)xTraceEventCreate2(PSF_EVENT_FIFO_PUT_SLIST_EXIT, (TraceUnsignedBaseType_t)fifo, (TraceUnsignedBaseType_t)list);
}

void sys_trace_k_fifo_get_enter(struct k_fifo *fifo, k_timeout_t timeout) {
	(void)xTraceEventCreate2(PSF_EVENT_FIFO_GET_BLOCKING, (TraceUnsignedBaseType_t)fifo, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_fifo_get_exit(struct k_fifo *fifo, k_timeout_t timeout, void *ret) {
	(void)xTraceEventCreate2(ret != NULL ? PSF_EVENT_FIFO_GET_SUCCESS : PSF_EVENT_FIFO_GET_FAILURE, (TraceUnsignedBaseType_t)fifo, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_fifo_peek_head_enter(struct k_fifo *fifo) {
	(void)xTraceEventCreate1(PSF_EVENT_FIFO_PEEK_HEAD_ENTER, (TraceUnsignedBaseType_t)fifo);
}

void sys_trace_k_fifo_peek_head_exit(struct k_fifo *fifo, void *ret) {
	(void)xTraceEventCreate2(PSF_EVENT_FIFO_PEEK_HEAD_EXIT, (TraceUnsignedBaseType_t)fifo, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_fifo_peek_tail_enter(struct k_fifo *fifo) {
	(void)xTraceEventCreate1(PSF_EVENT_FIFO_PEEK_TAIL_ENTER, (TraceUnsignedBaseType_t)fifo);
}

void sys_trace_k_fifo_peek_tail_exit(struct k_fifo *fifo, void *ret) {
	(void)xTraceEventCreate2(PSF_EVENT_FIFO_PEEK_TAIL_EXIT, (TraceUnsignedBaseType_t)fifo, (TraceUnsignedBaseType_t)ret);
}


/* LIFO trace function definitions */
void sys_trace_k_lifo_init_enter(struct k_lifo *lifo) {
	(void)xTraceEventCreate1(PSF_EVENT_LIFO_INIT_ENTER, (TraceUnsignedBaseType_t)lifo);
}

void sys_trace_k_lifo_init_exit(struct k_lifo *lifo) {
	(void)xTraceEventCreate1(PSF_EVENT_LIFO_INIT_EXIT, (TraceUnsignedBaseType_t)lifo);
}

void sys_trace_k_lifo_put_enter(struct k_lifo *lifo, void *data) {
	(void)xTraceEventCreate2(PSF_EVENT_LIFO_PUT_ENTER, (TraceUnsignedBaseType_t)lifo, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_lifo_put_exit(struct k_lifo *lifo, void *data) {
	(void)xTraceEventCreate2(PSF_EVENT_LIFO_PUT_EXIT, (TraceUnsignedBaseType_t)lifo, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_lifo_alloc_put_enter(struct k_lifo *lifo, void *data) {
	(void)xTraceEventCreate2(PSF_EVENT_LIFO_ALLOC_PUT_BLOCKING, (TraceUnsignedBaseType_t)lifo, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_lifo_alloc_put_exit(struct k_lifo *lifo, void *data, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_LIFO_ALLOC_PUT_SUCCESS : PSF_EVENT_LIFO_ALLOC_PUT_FAILURE, (TraceUnsignedBaseType_t)lifo, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_lifo_get_enter(struct k_lifo *lifo, k_timeout_t timeout) {
	(void)xTraceEventCreate2(PSF_EVENT_LIFO_GET_BLOCKING, (TraceUnsignedBaseType_t)lifo, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_lifo_get_exit(struct k_lifo *lifo, k_timeout_t timeout, void *ret) {
	(void)xTraceEventCreate2(ret != NULL ? PSF_EVENT_LIFO_GET_SUCCESS : PSF_EVENT_LIFO_GET_FAILURE, (TraceUnsignedBaseType_t)lifo, (TraceUnsignedBaseType_t)ret);
}


/* Stack trace function definitions */
void sys_trace_k_stack_init(struct k_stack *stack, stack_data_t *buffer, uint32_t num_entries) {
	(void)xTraceEventCreate3(PSF_EVENT_STACK_INIT, (TraceUnsignedBaseType_t)stack, (TraceUnsignedBaseType_t)buffer, (TraceUnsignedBaseType_t)num_entries);
}

void sys_trace_k_stack_alloc_init_enter(struct k_stack *stack, uint32_t num_entries) {
	(void)xTraceEventCreate2(PSF_EVENT_STACK_ALLOC_INIT_BLOCKING, (TraceUnsignedBaseType_t)stack, (TraceUnsignedBaseType_t)num_entries);
}

void sys_trace_k_stack_alloc_init_exit(struct k_stack *stack, uint32_t num_entries, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_STACK_ALLOC_INIT_SUCCESS : PSF_EVENT_STACK_ALLOC_INIT_FAILURE, (TraceUnsignedBaseType_t)stack, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_stack_cleanup_enter(struct k_stack *stack) {
	(void)xTraceEventCreate1(PSF_EVENT_STACK_CLEANUP_BLOCKING, (TraceUnsignedBaseType_t)stack);
}

void sys_trace_k_stack_cleanup_exit(struct k_stack *stack, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_STACK_CLEANUP_SUCCESS : PSF_EVENT_STACK_CLEANUP_FAILURE, (TraceUnsignedBaseType_t)stack, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_stack_push_enter(struct k_stack *stack, stack_data_t data) {
	(void)xTraceEventCreate2(PSF_EVENT_STACK_PUSH_BLOCKING, (TraceUnsignedBaseType_t)stack, (TraceUnsignedBaseType_t)data);
}

void sys_trace_k_stack_push_exit(struct k_stack *stack, stack_data_t data, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_STACK_PUSH_SUCCESS : PSF_EVENT_STACK_PUSH_FAILURE, (TraceUnsignedBaseType_t)stack, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_stack_pop_blocking(struct k_stack *stack, stack_data_t *data, k_timeout_t timeout) {
	(void)xTraceEventCreate3(PSF_EVENT_STACK_POP_BLOCKING, (TraceUnsignedBaseType_t)stack, (TraceUnsignedBaseType_t)data, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_stack_pop_exit(struct k_stack *stack, stack_data_t *data, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_STACK_POP_SUCCESS : PSF_EVENT_STACK_POP_FAILURE, (TraceUnsignedBaseType_t)stack, (TraceUnsignedBaseType_t)ret);
}


/* Message queue trace function definitions */
void sys_trace_k_msgq_init(struct k_msgq *msgq) {
	(void)xTraceEventCreate4(PSF_EVENT_MESSAGEQUEUE_INIT, (TraceUnsignedBaseType_t)msgq, (TraceUnsignedBaseType_t)msgq->buffer_start, (TraceUnsignedBaseType_t)msgq->msg_size, (TraceUnsignedBaseType_t)msgq->max_msgs);
}

void sys_trace_k_msgq_alloc_init_enter(struct k_msgq *msgq, size_t msg_size, uint32_t max_msgs) {
	(void)xTraceEventCreate3(PSF_EVENT_MESSAGEQUEUE_ALLOC_INIT_BLOCKING, (TraceUnsignedBaseType_t)msgq, (TraceUnsignedBaseType_t)msg_size, (TraceUnsignedBaseType_t)max_msgs);
}

void sys_trace_k_msgq_alloc_init_exit(struct k_msgq *msgq, size_t msg_size, uint32_t max_msgs, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_MESSAGEQUEUE_ALLOC_INIT_SUCCESS : PSF_EVENT_MESSAGEQUEUE_ALLOC_INIT_TIMEOUT, (TraceUnsignedBaseType_t)msgq, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_msgq_cleanup_enter(struct k_msgq *msgq) {
	(void)xTraceEventCreate1(PSF_EVENT_MESSAGEQUEUE_CLEANUP_BLOCKING, (TraceUnsignedBaseType_t)msgq);
}

void sys_trace_k_msgq_cleanup_exit(struct k_msgq *msgq, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_MESSAGEQUEUE_CLEANUP_SUCCESS : PSF_EVENT_MESSAGEQUEUE_CLEANUP_TIMEOUT, (TraceUnsignedBaseType_t)msgq, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_msgq_put_enter(struct k_msgq *msgq, const void *data, k_timeout_t timeout) {
}

void sys_trace_k_msgq_put_blocking(struct k_msgq *msgq, const void *data, k_timeout_t timeout) {
	(void)xTraceEventCreate3(PSF_EVENT_MESSAGEQUEUE_PUT_BLOCKING, (TraceUnsignedBaseType_t)msgq, (TraceUnsignedBaseType_t)data, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_msgq_put_exit(struct k_msgq *msgq, const void *data, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_MESSAGEQUEUE_PUT_SUCCESS : PSF_EVENT_MESSAGEQUEUE_PUT_TIMEOUT, (TraceUnsignedBaseType_t)msgq, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_msgq_get_enter(struct k_msgq *msgq, const void *data, k_timeout_t timeout) {
}

void sys_trace_k_msgq_get_blocking(struct k_msgq *msgq, const void *data, k_timeout_t timeout) {
	(void)xTraceEventCreate3(PSF_EVENT_MESSAGEQUEUE_GET_BLOCKING, (TraceUnsignedBaseType_t)msgq, (TraceUnsignedBaseType_t)data, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_msgq_get_exit(struct k_msgq *msgq, const void *data, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_MESSAGEQUEUE_GET_SUCCESS : PSF_EVENT_MESSAGEQUEUE_GET_TIMEOUT, (TraceUnsignedBaseType_t)msgq, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_msgq_peek(struct k_msgq *msgq, void *data, int ret) {
	(void)xTraceEventCreate3(
		ret == 0 ? PSF_EVENT_MESSAGEQUEUE_PEEK_SUCCESS : PSF_EVENT_MESSAGEQUEUE_PEEK_FAILED,
		(TraceUnsignedBaseType_t)msgq,
		(TraceUnsignedBaseType_t)data,
		(TraceUnsignedBaseType_t)ret
	);
}

void sys_trace_k_msgq_purge(struct k_msgq *msgq) {
	(void)xTraceEventCreate1(PSF_EVENT_MESSAGEQUEUE_PURGE, (TraceBaseType_t)msgq);
}


/* Mailbox trace function definitions */
void sys_trace_k_mbox_init(struct k_mbox *mbox) {
	(void)xTraceEventCreate1(PSF_EVENT_MAILBOX_INIT, (TraceUnsignedBaseType_t)mbox);
}

void sys_trace_k_mbox_message_put_enter(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout) {
}

void sys_trace_k_mbox_message_put_blocking(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout) {
	(void)xTraceEventCreate3(PSF_EVENT_MAILBOX_MESSAGE_PUT_BLOCKING, (TraceUnsignedBaseType_t)mbox, (TraceUnsignedBaseType_t)tx_msg, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_mbox_message_put_exit(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_MAILBOX_MESSAGE_PUT_SUCCESS : PSF_EVENT_MAILBOX_MESSAGE_PUT_FAILURE, (TraceUnsignedBaseType_t)mbox, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_mbox_put_enter(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout) {
	(void)xTraceEventCreate3(PSF_EVENT_MAILBOX_PUT_BLOCKING, (TraceUnsignedBaseType_t)mbox, (TraceUnsignedBaseType_t)tx_msg, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_mbox_put_exit(struct k_mbox *mbox, struct k_mbox_msg *tx_msg, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_MAILBOX_PUT_SUCCESS : PSF_EVENT_MAILBOX_PUT_FAILURE, (TraceUnsignedBaseType_t)mbox, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_mbox_async_put_enter(struct k_mbox *mbox, struct k_sem *sem) {
	(void)xTraceEventCreate2(PSF_EVENT_MAILBOX_ASYNC_PUT_ENTER, (TraceUnsignedBaseType_t)mbox, (TraceUnsignedBaseType_t)sem);
}

void sys_trace_k_mbox_async_put_exit(struct k_mbox *mbox, struct k_sem *sem) {
	(void)xTraceEventCreate2(PSF_EVENT_MAILBOX_ASYNC_PUT_EXIT, (TraceUnsignedBaseType_t)mbox, (TraceUnsignedBaseType_t)sem);
}

void sys_trace_k_mbox_get_enter(struct k_mbox *mbox, struct k_mbox_msg *rx_msg, void *buffer, k_timeout_t timeout) {
}

void sys_trace_k_mbox_get_blocking(struct k_mbox *mbox, struct k_mbox_msg *rx_msg, void *buffer, k_timeout_t timeout) {
	(void)xTraceEventCreate4(
		PSF_EVENT_MAILBOX_GET_BLOCKING,
		(TraceUnsignedBaseType_t)mbox,
		(TraceUnsignedBaseType_t)rx_msg,
		(TraceUnsignedBaseType_t)buffer,
		(TraceUnsignedBaseType_t)timeout.ticks
	);
}

void sys_trace_k_mbox_get_exit(struct k_mbox *mbox, struct k_mbox_msg *rx_msg, void *buffer, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_MAILBOX_GET_SUCCESS : PSF_EVENT_MAILBOX_GET_TIMEOUT, (TraceUnsignedBaseType_t)mbox, (TraceUnsignedBaseType_t)ret);
}

/* @note: Hook not implemented in Zephyr kernel */
void sys_trace_k_mbox_data_get(struct k_mbox_msg *rx_msg, void *buffer) {
	(void)xTraceEventCreate2(PSF_EVENT_MAILBOX_DATA_GET, (TraceUnsignedBaseType_t)rx_msg, (TraceUnsignedBaseType_t)buffer);
}


/* Pipe trace function definitions */
void sys_trace_k_pipe_init(struct k_pipe *pipe, unsigned char *buffer, size_t size) {
	(void)xTraceEventCreate3(PSF_EVENT_PIPE_INIT, (TraceUnsignedBaseType_t)pipe, (TraceUnsignedBaseType_t)buffer, (TraceUnsignedBaseType_t)size);
}

void sys_trace_k_pipe_cleanup_enter(struct k_pipe *pipe) {
	(void)xTraceEventCreate1(PSF_EVENT_PIPE_CLEANUP_BLOCKING, (TraceUnsignedBaseType_t)pipe);
}

void sys_trace_k_pipe_cleanup_exit(struct k_pipe *pipe, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_PIPE_CLEANUP_SUCCESS : PSF_EVENT_PIPE_CLEANUP_FAILURE, (TraceUnsignedBaseType_t)pipe, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_pipe_alloc_init_enter(struct k_pipe *pipe, size_t size) {
	(void)xTraceEventCreate2(PSF_EVENT_PIPE_ALLOC_INIT_BLOCKING, (TraceUnsignedBaseType_t)pipe, (TraceUnsignedBaseType_t)size);
}

void sys_trace_k_pipe_alloc_init_exit(struct k_pipe *pipe, size_t size, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_PIPE_ALLOC_INIT_SUCCESS : PSF_EVENT_PIPE_ALLOC_INIT_FAILURE, (TraceUnsignedBaseType_t)pipe, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_pipe_put_enter(struct k_pipe *pipe, void *data, size_t bytes_to_write, size_t *bytes_written, size_t min_xfer, k_timeout_t timeout) {

}

void sys_trace_k_pipe_put_blocking(struct k_pipe *pipe, void *data, size_t bytes_to_write, size_t *bytes_written, size_t min_xfer, k_timeout_t timeout) {
	(void)xTraceEventCreate6(
		PSF_EVENT_PIPE_PUT_BLOCKING,
		(TraceUnsignedBaseType_t)pipe,
		(TraceUnsignedBaseType_t)data,
		(TraceUnsignedBaseType_t)bytes_to_write,
		(TraceUnsignedBaseType_t)bytes_written,
		(TraceUnsignedBaseType_t)min_xfer,
		(TraceUnsignedBaseType_t)timeout.ticks
	);
}

void sys_trace_k_pipe_put_exit(struct k_pipe *pipe, void *data, size_t bytes_to_write, size_t *bytes_written, size_t min_xfer, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_PIPE_PUT_SUCCESS : PSF_EVENT_PIPE_PUT_TIMEOUT, (TraceUnsignedBaseType_t)pipe, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_pipe_get_enter(struct k_pipe *pipe, void *data, size_t bytes_to_read, size_t *bytes_read, size_t min_xfer, k_timeout_t timeout) {

}

void sys_trace_k_pipe_get_blocking(struct k_pipe *pipe, void *data, size_t bytes_to_read, size_t *bytes_read, size_t min_xfer, k_timeout_t timeout) {
	(void)xTraceEventCreate6(
		PSF_EVENT_PIPE_GET_BLOCKING,
		(TraceUnsignedBaseType_t)pipe,
		(TraceUnsignedBaseType_t)data,
		(TraceUnsignedBaseType_t)bytes_to_read,
		(TraceUnsignedBaseType_t)bytes_read,
		(TraceUnsignedBaseType_t)min_xfer,
		(TraceUnsignedBaseType_t)timeout.ticks
	);
}

void sys_trace_k_pipe_get_exit(struct k_pipe *pipe, void *data, size_t bytes_to_read, size_t *bytes_read, size_t min_xfer, k_timeout_t timeout, int ret) {
	(void)xTraceEventCreate2(ret == 0 ? PSF_EVENT_PIPE_GET_SUCCESS : PSF_EVENT_PIPE_GET_TIMEOUT, (TraceUnsignedBaseType_t)pipe, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_pipe_block_put_enter(struct k_pipe *pipe, struct k_mem_block *block, size_t size, struct k_sem *sem) {
	(void)xTraceEventCreate4(
		PSF_EVENT_PIPE_BLOCK_PUT_ENTER,
		(TraceUnsignedBaseType_t)pipe,
		(TraceUnsignedBaseType_t)block,
		(TraceUnsignedBaseType_t)size,
		(TraceUnsignedBaseType_t)sem
	);
}

void sys_trace_k_pipe_block_put_exit(struct k_pipe *pipe, struct k_mem_block *block, size_t size, struct k_sem *sem) {
	(void)xTraceEventCreate1(PSF_EVENT_PIPE_BLOCK_PUT_EXIT, (TraceUnsignedBaseType_t)pipe);
}


/* Memory heap trace function definitions */
void sys_trace_k_heap_init(struct k_heap *h, void *mem, size_t bytes) {
	(void)xTraceEventCreate3(PSF_EVENT_KHEAP_INIT, (TraceUnsignedBaseType_t)h, (TraceUnsignedBaseType_t)mem, (TraceUnsignedBaseType_t)bytes);
}

void sys_trace_k_heap_alloc_enter(struct k_heap *h, size_t bytes, k_timeout_t timeout) {
	(void)xTraceEventCreate3(PSF_EVENT_KHEAP_ALLOC_BLOCKING, (TraceUnsignedBaseType_t)h, (TraceUnsignedBaseType_t)bytes, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_heap_alloc_exit(struct k_heap *h, size_t bytes, k_timeout_t timeout, void *ret) {
	(void)xTraceEventCreate2(ret != NULL ? PSF_EVENT_KHEAP_ALLOC_SUCCESS : PSF_EVENT_KHEAP_ALLOC_FAILURE, (TraceUnsignedBaseType_t)h, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_heap_aligned_alloc_enter(struct k_heap *h, size_t bytes, k_timeout_t timeout) {

}

void sys_trace_k_heap_aligned_alloc_blocking(struct k_heap *h, size_t bytes, k_timeout_t timeout) {
	(void)xTraceEventCreate4(PSF_EVENT_KHEAP_ALIGNED_ALLOC_BLOCKING, (TraceUnsignedBaseType_t)h, (TraceUnsignedBaseType_t)bytes, (TraceUnsignedBaseType_t)timeout.ticks, (TraceUnsignedBaseType_t)0);
}

void sys_trace_k_heap_aligned_alloc_exit(struct k_heap *h, size_t bytes, k_timeout_t timeout, void *ret) {
	(void)xTraceEventCreate2((ret != NULL) ? PSF_EVENT_KHEAP_ALIGNED_ALLOC_SUCCESS : PSF_EVENT_KHEAP_ALIGNED_ALLOC_FAILURE, (TraceUnsignedBaseType_t)h, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_heap_free(struct k_heap *h, void *mem) {
	(void)xTraceEventCreate2(PSF_EVENT_KHEAP_FREE, (TraceUnsignedBaseType_t)h, (TraceUnsignedBaseType_t)mem);
}

void sys_trace_k_heap_sys_k_aligned_alloc_enter(struct k_heap *h, size_t align, size_t size) {
	(void)xTraceEventCreate3(PSF_EVENT_KHEAP_SYS_ALIGNED_ALLOC_BLOCKING, (TraceUnsignedBaseType_t)h, (TraceUnsignedBaseType_t)align, (TraceUnsignedBaseType_t)size);
}

void sys_trace_k_heap_sys_k_aligned_alloc_exit(struct k_heap *h, size_t align, size_t size, void *ret) {
	(void)xTraceEventCreate2(ret != NULL ? PSF_EVENT_KHEAP_SYS_ALIGNED_ALLOC_SUCCESS : PSF_EVENT_KHEAP_SYS_ALIGNED_ALLOC_FAILURE, (TraceUnsignedBaseType_t)h, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_heap_sys_k_malloc_enter(struct k_heap *h, size_t size) {
	(void)xTraceEventCreate2(PSF_EVENT_KHEAP_SYS_MALLOC_BLOCKING, (TraceUnsignedBaseType_t)h, (TraceUnsignedBaseType_t)size);
}

void sys_trace_k_heap_sys_k_malloc_exit(struct k_heap *h, size_t size, void *ret) {
	(void)xTraceEventCreate2(ret != NULL ? PSF_EVENT_KHEAP_SYS_MALLOC_SUCCESS : PSF_EVENT_KHEAP_SYS_MALLOC_FAILURE, (TraceUnsignedBaseType_t)h, (TraceUnsignedBaseType_t)ret);
}

void sys_trace_k_heap_sys_k_free_enter(struct k_heap *h) {
	(void)xTraceEventCreate1(PSF_EVENT_KHEAP_SYS_FREE_ENTER, (TraceUnsignedBaseType_t)h);
}

void sys_trace_k_heap_sys_k_free_exit(struct k_heap *h) {
	(void)xTraceEventCreate1(PSF_EVENT_KHEAP_SYS_FREE_EXIT, (TraceUnsignedBaseType_t)h);
}

void sys_trace_k_heap_sys_k_enter(struct k_heap *h, size_t nmemb, size_t size) {

}

void sys_trace_k_heap_sys_k_exit(struct k_heap *h, size_t nmemb, size_t size, void *ret) {

}

void sys_trace_k_heap_sys_k_calloc_enter(struct k_heap *h, size_t nmemb, size_t size) {
	(void)xTraceEventCreate3(PSF_EVENT_KHEAP_SYS_CALLOC_BLOCKING, (TraceUnsignedBaseType_t)h, (TraceUnsignedBaseType_t)nmemb, (TraceUnsignedBaseType_t)size);
}

void sys_trace_k_heap_sys_k_calloc_exit(struct k_heap *h, size_t nmemb, size_t size, void *ret) {
	(void)xTraceEventCreate2(ret != NULL ? PSF_EVENT_KHEAP_SYS_CALLOC_SUCCESS : PSF_EVENT_KHEAP_SYS_CALLOC_FAILURE, (TraceUnsignedBaseType_t)h, (TraceUnsignedBaseType_t)ret);
}


/* Memory slab trace function definitions */
void sys_trace_k_mem_slab_init(struct k_mem_slab *slab, void *buffer, size_t block_size, uint32_t num_blocks, int ret) {
	(void)xTraceEventCreate5(
		ret == 0 ? PSF_EVENT_MEMORY_SLAB_INIT_SUCCESS : PSF_EVENT_MEMORY_SLAB_INIT_FAILURE,
		(TraceUnsignedBaseType_t)slab,
		(TraceUnsignedBaseType_t)slab->buffer,
		(TraceUnsignedBaseType_t)slab->block_size,
		(TraceUnsignedBaseType_t)slab->num_blocks,
		(TraceUnsignedBaseType_t)ret
	);
}

void sys_trace_k_mem_slab_alloc_enter(struct k_mem_slab *slab, void **mem, k_timeout_t timeout) {

}

void sys_trace_k_mem_slab_alloc_blocking(struct k_mem_slab *slab, void **mem, k_timeout_t timeout) {
	(void)xTraceEventCreate3(PSF_EVENT_MEMORY_SLAB_ALLOC_BLOCKING, (TraceUnsignedBaseType_t)slab, (TraceUnsignedBaseType_t)mem, (TraceUnsignedBaseType_t)timeout.ticks);
}

void sys_trace_k_mem_slab_alloc_exit(struct k_mem_slab *slab, void **mem, k_timeout_t timeout, int ret) {
	if (ret == 0) {
		(void)xTraceEventCreate5(
			PSF_EVENT_MEMORY_SLAB_ALLOC_SUCCESS,
			(TraceUnsignedBaseType_t)slab,
			(TraceUnsignedBaseType_t)mem,
			(TraceUnsignedBaseType_t)timeout.ticks,
			(TraceUnsignedBaseType_t)ret,
			(TraceUnsignedBaseType_t)slab->num_blocks
		);
	} else if (ret == -ENOMEM || ret == -EAGAIN) {
		(void)xTraceEventCreate4(
			PSF_EVENT_MEMORY_SLAB_ALLOC_TIMEOUT,
			(TraceUnsignedBaseType_t)slab,
			(TraceUnsignedBaseType_t)mem,
			(TraceUnsignedBaseType_t)timeout.ticks,
			(TraceUnsignedBaseType_t)ret
		);
	} else {
		(void)xTraceEventCreate4(
			PSF_EVENT_MEMORY_SLAB_ALLOC_ERROR,
			(TraceUnsignedBaseType_t)slab,
			(TraceUnsignedBaseType_t)mem,
			(TraceUnsignedBaseType_t)timeout.ticks,
			(TraceUnsignedBaseType_t)ret
		);
	}
}

void sys_trace_k_mem_slab_free_exit(struct k_mem_slab *slab, void **mem) {
	(void)xTraceEventCreate3(PSF_EVENT_MEMORY_SLAB_FREE, (TraceUnsignedBaseType_t)slab, (TraceUnsignedBaseType_t)mem, (TraceUnsignedBaseType_t)slab->num_blocks);
}


/* Timer trace function definitions */
void sys_trace_k_timer_init(struct k_timer *timer, k_timer_expiry_t expiry_fn, k_timer_expiry_t stop_fn) {
	(void)xTraceEventCreate3(PSF_EVENT_TIMER_INIT, (TraceUnsignedBaseType_t)timer, (TraceUnsignedBaseType_t)expiry_fn, (TraceUnsignedBaseType_t)stop_fn);
}

void sys_trace_k_timer_start(struct k_timer *timer, k_timeout_t duration, k_timeout_t period) {
	(void)xTraceEventCreate3(PSF_EVENT_TIMER_START, (TraceUnsignedBaseType_t)timer, (TraceUnsignedBaseType_t)duration.ticks, (TraceUnsignedBaseType_t)period.ticks);
}

void sys_trace_k_timer_stop(struct k_timer *timer) {
	(void)xTraceEventCreate1(PSF_EVENT_TIMER_STOP, (TraceUnsignedBaseType_t)timer);
}

void sys_trace_k_timer_status_sync_blocking(struct k_timer *timer) {
	(void)xTraceEventCreate1(PSF_EVENT_TIMER_STATUS_SYNC_AWAIT, (TraceUnsignedBaseType_t)timer);
}

void sys_trace_k_timer_status_sync_exit(struct k_timer *timer, uint32_t result) {
	(void)xTraceEventCreate2(PSF_EVENT_TIMER_STATUS_SYNC_EXIT, (TraceUnsignedBaseType_t)timer, (TraceUnsignedBaseType_t)result);
}


/* Syscall trace function definitions */
void sys_trace_syscall_enter(uint32_t id, const char *name) {
#if (TRC_CFG_USE_SYSCALL_EXTENSION == 1)
	if (xTraceIsRecorderEnabled())
		xSyscallsExtensionEnter(id);
#else
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_SYSTEM_SYSCALL_ENTER, sizeof(uint32_t) + strlen(name), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddUnsignedBaseType(xTraceHandle, (TraceUnsignedBaseType_t)id);

		/* Add name */
		xTraceEventAddData(xTraceHandle, (void*)name, strlen(name));

		xTraceEventEnd(xTraceHandle);
	}
#endif
}

void sys_trace_syscall_exit(uint32_t id, const char *name) {
#if (TRC_CFG_USE_SYSCALL_EXTENSION == 1)
	if (xTraceIsRecorderEnabled())
		xSyscallsExtensionExit(id);
#else
	TraceEventHandle_t xTraceHandle;
	
	if (xTraceEventBegin(PSF_EVENT_SYSTEM_SYSCALL_EXIT, 0, &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventEnd(xTraceHandle);
	}
#endif
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
