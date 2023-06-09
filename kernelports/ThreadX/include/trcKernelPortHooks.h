/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The ThreadX specific parts needed by the trace recorder
 */

#ifndef TRC_KERNEL_PORT_HOOKS_H
#define TRC_KERNEL_PORT_HOOKS_H

#include <trcRecorder.h>

#ifdef __cplusplus
extern "C" {
#endif

void xTraceResetCurrentThread();
void xTraceCheckThreadSwitch();

void xTraceISREnterHook(UINT uiTxEventCode, UINT uiISRNr);
void xTraceISRExitHook(UINT uiTxEventCode);
void xTraceTimeSliceHook(UINT uiTxEventCode, TX_THREAD *pxNextThreadPtr);

void xTraceBlockAllocateHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool, ULONG uiWaitOption, UINT uiRemainingBlocks);
void xTraceBlockPoolCreateHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool, ULONG uiBlockSize, VOID *pvPoolStart, ULONG uiTotalBlocks);
void xTraceBlockPoolDeleteHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool);
void xTraceBlockPoolInfoGetHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool);
void xTraceBlockPoolPerformanceInfoGetHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool);
void xTraceBlockPoolPerformanceSystemInfoGetHook(UINT uiTxEventCode);
void xTraceBlockPoolPrioritizeHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool);
void xTraceBlockReleaseHook(UINT uiTxEventCode, VOID *pvBlockPool);

void xTraceByteAllocateHook(UINT uiTxEventCode);
void xTraceBytePoolCreateHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool, VOID *pvStartPtr, ULONG uiPoolSize);
void xTraceBytePoolDeleteHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool);
void xTraceBytePoolInfoGetHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool);
void xTraceBytePoolPerformanceInfoGetHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool);
void xTraceBytePoolPerformanceSystemInfoGetHook(UINT uiTxEventCode);
void xTraceBytePoolPrioritizeHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool);
void xTraceByteReleaseHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool, VOID *pvMemory, UINT uiAvailableBytes);

void xTraceEventFlagsCreateHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup);
void xTraceEventFlagsDeleteHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup);
void xTraceEventFlagsGetHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup, ULONG ulRequestedFlags, UINT uiGetOption);
void xTraceEventFlagsInfoGetHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup);
void xTraceEventFlagsPerformanceInfoGetHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup);
void xTraceEventFlagsPerformanceSystemInfoGetHook(UINT uiTxEventCode);
void xTraceEventFlagsSetHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup, ULONG ulFlagsToSet, UINT uiSetOption);
void xTraceEventFlagsSetNotifyHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup);

void xTraceMutexCreateHook(UINT uiTxEventCode, TX_MUTEX *pxMutex, UINT uiPrioInherit);
void xTraceMutexDeleteHook(UINT uiTxEventCode, TX_MUTEX *pxMutex);
void xTraceMutexGetHook(UINT uiTxEventCode, TX_MUTEX *pxMutex);
void xTraceMutexInfoGetHook(UINT uiTxEventCode, TX_MUTEX *pxMutex);
void xTraceMutexPerformanceInfoGetHook(UINT uiTxEventCode, TX_MUTEX *pxMutex);
void xTraceMutexPerformanceSystemInfoGetHook(UINT uiTxEventCode);
void xTraceMutexPrioritizeHook(UINT uiTxEventCode, TX_MUTEX *pxMutex);
void xTraceMutexPutHook(UINT uiTxEventCode, TX_MUTEX *pxMutex, UINT uiStateAfter);

void xTraceQueueCreateHook(UINT uiTxEventCode, TX_QUEUE *pxQueue, UINT uiMessageSize, VOID *pvQueueStart, UINT uiQueueSize);
void xTraceQueueDeleteHook(UINT uiTxEventCode, TX_QUEUE *pxQueue);
void xTraceQueueFlushHook(UINT uiTxEventCode, TX_QUEUE *pxQueue);
void xTraceQueueFrontSendHook(UINT uiTxEventCode, TX_QUEUE *pxQueue);
void xTraceQueueInfoGetHook(UINT uiTxEventCode, TX_QUEUE *pxQueue);
void xTraceQueuePerformanceInfoGetHook(UINT uiTxEventCode, TX_QUEUE *pxQueue);
void xTraceQueuePerformanceSystemInfoGetHook(UINT uiTxEventCode);
void xTraceQueuePrioritizeHook(UINT uiTxEventCode, TX_QUEUE *pxQueue);
void xTraceQueueReceiveHook(UINT uiTxEventCode);
void xTraceQueueSendHook(UINT uiTxEventCode);
void xTraceQueueSendNotifyHook(UINT uiTxEventCode, TX_QUEUE *pxQueue);

void xTraceSemaphoreCeilingPutHook(UINT uiTxEventCode);
void xTraceSemaphoreCreateHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore, ULONG uiInitialCount);
void xTraceSemaphoreDeleteHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore);
void xTraceSemaphoreGetHook(UINT uiTxEventCode);
void xTraceSemaphoreInfoGetHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore);
void xTraceSemaphorePerformanceInfoGetHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore);
void xTraceSemaphorePerformanceSystemInfoGetHook(UINT uiTxEventCode);
void xTraceSemaphorePrioritizeHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore);
void xTraceSemaphorePutHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore);
void xTraceSemaphorePutNotifyHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore);

void xTraceThreadResumeHook(UINT uiTxEventCode, TX_THREAD *pxThread);
void xTraceThreadSuspendHook(UINT uiTxEventCode, TX_THREAD *pxThread);
void xTraceThreadCreateHook(UINT uiTxEventCode, TX_THREAD *pxThread, UINT uiPriority);
void xTraceThreadDeleteHook(UINT uiTxEventCode, TX_THREAD *pxThread);
void xTraceThreadEntryExitNotifyHook(UINT uiTxEventCode, TX_THREAD *pxThread);
void xTraceThreadIdentifyHook(UINT uiTxEventCode);
void xTraceThreadInfoGetHook(UINT uiTxEventCode, TX_THREAD *pxThread);
void xTraceThreadPerformanceInfoGetHook(UINT uiTxEventCode, TX_THREAD *pxThread);
void xTraceThreadPerformanceSystemInfoGetHook(UINT uiTxEventCode);
void xTraceThreadPreemptionChangeHook(UINT uiTxEventCode, TX_THREAD *pxThread, UINT uiNewThreshold);
void xTraceThreadPriorityChangeHook(UINT uiTxEventCode, TX_THREAD *pxThread, UINT uiNewPriority);
void xTraceThreadRelinquishHook(UINT uiTxEventCode);
void xTraceThreadResetHook(UINT uiTxEventCode, TX_THREAD *pxThread);
void xTraceThreadResumeAPIHook(UINT uiTxEventCode, TX_THREAD *pxThread);
void xTraceThreadSleepHook(UINT uiTxEventCode, TX_THREAD *pxThread);
void xTraceThreadStackErrorNotifyHook(UINT uiTxEventCode);
void xTraceThreadSuspendAPIHook(UINT uiTxEventCode, TX_THREAD *pxThread);
void xTraceThreadTerminateHook(UINT uiTxEventCode, TX_THREAD *pxThread);
void xTraceThreadTimeSliceChangeHook(UINT uiTxEventCode, TX_THREAD *pxThread, UINT uiNewTimeSlice);
void xTraceThreadWaitAbortHook(UINT uiTxEventCode, TX_THREAD *pxThread);

void xTraceTimeGetHook(UINT uiTxEventCode);
void xTraceTimeSetHook(UINT uiTxEventCode, TX_TIMER *pxTimer);
void xTraceTimerActivateHook(UINT uiTxEventCode,  TX_TIMER *pxTimer);
void xTraceTimerChangeHook(UINT uiTxEventCode, TX_TIMER *pxTimer, ULONG uiInitialTicks, ULONG uiRescheduleTicks);
void xTraceTimerCreateHook(UINT uiTxEventCode, TX_TIMER *pxTimer, ULONG uiInitialTicks, ULONG uiRescheduleTicks, UINT uiAutoActivate);
void xTraceTimerDeactivateHook(UINT uiTxEventCode, TX_TIMER *pxTimer);
void xTraceTimerDeleteHook(UINT uiTxEventCode, TX_TIMER *pxTimer);
void xTraceTimerInfoGetHook(UINT uiTxEventCode, TX_TIMER *pxTimer);
void xTraceTimerPerformanceInfoGet(UINT uiTxEventCode, TX_TIMER *pxTimer);
void xTraceTimerPerformanceSystemInfoGet(UINT uiTxEventCode);

#undef TX_TRACE_IN_LINE_INSERT
#define TX_TRACE_IN_LINE_INSERT(i, a, b, c, d, e) \
	{ \
		TRC_TG_##e(_##i)(i, a, b, c, d, e); \
	}

/* Trace macro group to hook mapping helpers */
#if (!TRC_CFG_SCHEDULING_ONLY)
#define TRC_TG_TX_TRACE_BLOCK_POOL_EVENTS(i) TRC_TP##i
#define TRC_TG_TX_TRACE_BYTE_POOL_EVENTS(i) TRC_TP##i
#define TRC_TG_TX_TRACE_EVENT_FLAGS_EVENTS(i) TRC_TP##i
#define TRC_TG_TX_TRACE_MUTEX_EVENTS(i) TRC_TP##i
#define TRC_TG_TX_TRACE_QUEUE_EVENTS(i) TRC_TP##i
#define TRC_TG_TX_TRACE_SEMAPHORE_EVENTS(i) TRC_TP##i
#define TRC_TG_TX_TRACE_TIME_EVENTS(i) TRC_TP##i
#define TRC_TG_TX_TRACE_TIMER_EVENTS(i) TRC_TP##i
#else
#define TRC_TG_TX_TRACE_BLOCK_POOL_EVENTS(i)
#define TRC_TG_TX_TRACE_BYTE_POOL_EVENTS(i)
#define TRC_TG_TX_TRACE_EVENT_FLAGS_EVENTS(i)
#define TRC_TG_TX_TRACE_MUTEX_EVENTS(i)
#define TRC_TG_TX_TRACE_QUEUE_EVENTS(i)
#define TRC_TG_TX_TRACE_SEMAPHORE_EVENTS(i)
#define TRC_TG_TX_TRACE_TIME_EVENTS(i)
#define TRC_TG_TX_TRACE_TIMER_EVENTS(i)
#endif
#define TRC_TG_TX_TRACE_THREAD_EVENTS(i) TRC_TP##i
#define TRC_TG_TX_TRACE_INTERNAL_EVENTS(i) TRC_TP##i
#define TRC_TG_TX_TRACE_INTERRUPT_CONTROL_EVENT(i) TRC_TP##i
#define TRC_TG_TX_TRACE_USER_EVENTS(i) TRC_TP_TX_TRACE_USER_EVENT
#define TRC_TG_filter(i)


#define TRC_TP_TX_TRACE_ISR_ENTER(i, a, b, c, d, e)												xTraceISREnterHook(i, b)
#define TRC_TP_TX_TRACE_ISR_EXIT(i, a, b, c, d, e)												xTraceISRExitHook(i)
#define TRC_TP_TX_TRACE_TIME_SLICE(i, a, b, c, d, e)											xTraceTimeSliceHook(i, a)
#define TRC_TP_TX_TRACE_RUNNING(i, a, b, c, d, e)


#define TRC_TP_TX_TRACE_BLOCK_ALLOCATE(i, a, b, c, d, e)										xTraceBlockAllocateHook(i, a, c, d)
#define TRC_TP_TX_TRACE_BLOCK_POOL_CREATE(i, a, b, c, d, e)										xTraceBlockPoolCreateHook(i, a, d, TX_ULONG_TO_POINTER_CONVERT(b), c)
#define TRC_TP_TX_TRACE_BLOCK_POOL_DELETE(i, a, b, c, d, e)										xTraceBlockPoolDeleteHook(i, a)
#define TRC_TP_TX_TRACE_BLOCK_POOL_INFO_GET(i, a, b, c, d, e)									xTraceBlockPoolInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_BLOCK_POOL_PERFORMANCE_INFO_GET(i, a, b, c, d, e)						xTraceBlockPoolPerformanceInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_BLOCK_POOL__PERFORMANCE_SYSTEM_INFO_GET(i, a, b, c, d, e)				xTraceBlockPoolPerformanceSystemInfoGetHook(i)
#define TRC_TP_TX_TRACE_BLOCK_POOL_PRIORITIZE(i, a, b, c, d, e)									xTraceBlockPoolPrioritizeHook(i, a)
#define TRC_TP_TX_TRACE_BLOCK_RELEASE(i, a, b, c, d, e)											xTraceBlockReleaseHook(i, a)


#define TRC_TP_TX_TRACE_BYTE_ALLOCATE(i, a, b, c, d, e)											xTraceByteAllocateHook(i)
#define TRC_TP_TX_TRACE_BYTE_POOL_CREATE(i, a, b, c, d, e)										xTraceBytePoolCreateHook(i, a, TX_ULONG_TO_POINTER_CONVERT(b), c)
#define TRC_TP_TX_TRACE_BYTE_POOL_DELETE(i, a, b, c, d, e)										xTraceBytePoolDeleteHook(i, a)
#define TRC_TP_TX_TRACE_BYTE_POOL_INFO_GET(i, a, b, c, d, e)									xTraceBytePoolInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_BYTE_POOL_PERFORMANCE_INFO_GET(i, a, b, c, d, e)						xTraceBytePoolPerformanceInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_BYTE_POOL__PERFORMANCE_SYSTEM_INFO_GET(i, a, b, c, d, e)				xTraceBytePoolPerformanceSystemInfoGetHook(i)
#define TRC_TP_TX_TRACE_BYTE_POOL_PRIORITIZE(i, a, b, c, d, e)									xTraceBytePoolPrioritizeHook(i, a)
#define TRC_TP_TX_TRACE_BYTE_RELEASE(i, a, b, c, d, e)											xTraceByteReleaseHook(i, a, TX_ULONG_TO_POINTER_CONVERT(b), d)


#define TRC_TP_TX_TRACE_EVENT_FLAGS_CREATE(i, a, b, c, d, e)									xTraceEventFlagsCreateHook(i, a)
#define TRC_TP_TX_TRACE_EVENT_FLAGS_DELETE(i, a, b, c, d, e)									xTraceEventFlagsDeleteHook(i, a)
#define TRC_TP_TX_TRACE_EVENT_FLAGS_GET(i, a, b, c, d, e)										xTraceEventFlagsGetHook(i, a, b, d)
#define TRC_TP_TX_TRACE_EVENT_FLAGS_INFO_GET(i, a, b, c, d, e)									xTraceEventFlagsInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_EVENT_FLAGS_PERFORMANCE_INFO_GET(i, a, b, c, d, e)						xTraceEventFlagsPerformanceInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_EVENT_FLAGS__PERFORMANCE_SYSTEM_INFO_GET(i, a, b, c, d, e)				xTraceEventFlagsPerformanceSystemInfoGetHook(i)
#define TRC_TP_TX_TRACE_EVENT_FLAGS_SET(i, a, b, c, d, e)										xTraceEventFlagsSetHook(i, a, b, c)
#define TRC_TP_TX_TRACE_EVENT_FLAGS_SET_NOTIFY(i, a, b, c, d, e)								xTraceEventFlagsSetNotifyHook(i, a)

#define TRC_TP_TX_TRACE_INTERRUPT_CONTROL(i, a, b, c, d, e)

#define TRC_TP_TX_TRACE_MUTEX_CREATE(i, a, b, c, d, e)											xTraceMutexCreateHook(i, a, b)
#define TRC_TP_TX_TRACE_MUTEX_DELETE(i, a, b, c, d, e)											xTraceMutexDeleteHook(i, a)
#define TRC_TP_TX_TRACE_MUTEX_GET(i, a, b, c, d, e)												xTraceMutexGetHook(i, a)
#define TRC_TP_TX_TRACE_MUTEX_INFO_GET(i, a, b, c, d, e)										xTraceMutexInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_MUTEX_PERFORMANCE_INFO_GET(i, a, b, c, d, e)							xTraceMutexPerformanceInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_MUTEX_PERFORMANCE_SYSTEM_INFO_GET(i, a, b, c, d, e)						xTraceMutexPerformanceSystemInfoGetHook(i)
#define TRC_TP_TX_TRACE_MUTEX_PRIORITIZE(i, a, b, c, d, e)										xTraceMutexPrioritizeHook(i, a)
#define TRC_TP_TX_TRACE_MUTEX_PUT(i, a, b, c, d, e)												xTraceMutexPutHook(i, a, c)

#define TRC_TP_TX_TRACE_QUEUE_CREATE(i, a, b, c, d, e)											xTraceQueueCreateHook(i, a, b, TX_ULONG_TO_POINTER_CONVERT(c), d)
#define TRC_TP_TX_TRACE_QUEUE_DELETE(i, a, b, c, d, e)											xTraceQueueDeleteHook(i, a)
#define TRC_TP_TX_TRACE_QUEUE_FLUSH(i, a, b, c, d, e)											xTraceQueueFlushHook(i, a)
#define TRC_TP_TX_TRACE_QUEUE_FRONT_SEND(i, a, b, c, d, e)										xTraceQueueFrontSendHook(i, a)
#define TRC_TP_TX_TRACE_QUEUE_INFO_GET(i, a, b, c, d, e)										xTraceQueueInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_QUEUE_PERFORMANCE_INFO_GET(i, a, b, c, d, e)							xTraceQueuePerformanceInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_QUEUE_PERFORMANCE_SYSTEM_INFO_GET(i, a, b, c, d, e)						xTraceQueuePerformanceSystemInfoGetHook(i)
#define TRC_TP_TX_TRACE_QUEUE_PRIORITIZE(i, a, b, c, d, e)										xTraceQueuePrioritizeHook(i, a)
#define TRC_TP_TX_TRACE_QUEUE_RECEIVE(i, a, b, c, d, e)											xTraceQueueReceiveHook(i)
#define TRC_TP_TX_TRACE_QUEUE_SEND(i, a, b, c, d, e)											xTraceQueueSendHook(i)
#define TRC_TP_TX_TRACE_QUEUE_SEND_NOTIFY(i, a, b, c, d, e)										xTraceQueueSendNotifyHook(i, a)

#define TRC_TP_TX_TRACE_SEMAPHORE_CEILING_PUT(i, a, b, c, d, e)									xTraceSemaphoreCeilingPutHook(i)
#define TRC_TP_TX_TRACE_SEMAPHORE_CREATE(i, a, b, c, d, e)										xTraceSemaphoreCreateHook(i, a, b)
#define TRC_TP_TX_TRACE_SEMAPHORE_DELETE(i, a, b, c, d, e)										xTraceSemaphoreDeleteHook(i, a)
#define TRC_TP_TX_TRACE_SEMAPHORE_GET(i, a, b, c, d, e)											xTraceSemaphoreGetHook(i)
#define TRC_TP_TX_TRACE_SEMAPHORE_INFO_GET(i, a, b, c, d, e)									xTraceSemaphoreInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_SEMAPHORE_PERFORMANCE_INFO_GET(i, a, b, c, d, e)						xTraceSemaphorePerformanceInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_SEMAPHORE__PERFORMANCE_SYSTEM_INFO_GET(i, a, b, c, d, e)				xTraceSemaphorePerformanceSystemInfoGetHook(i)
#define TRC_TP_TX_TRACE_SEMAPHORE_PRIORITIZE(i, a, b, c, d, e)									xTraceSemaphorePrioritizeHook(i, a)
#define TRC_TP_TX_TRACE_SEMAPHORE_PUT(i, a, b, c, d, e)											xTraceSemaphorePutHook(i, a)
#define TRC_TP_TX_TRACE_SEMAPHORE_PUT_NOTIFY(i, a, b, c, d, e)									xTraceSemaphorePutNotifyHook(i, a)

#define TRC_TP_TX_TRACE_THREAD_RESUME(i, a, b, c, d, e) 										xTraceThreadResumeHook(i, a)
#define TRC_TP_TX_TRACE_THREAD_SUSPEND(i, a, b, c, d, e) 										xTraceThreadSuspendHook(i, a)
#define TRC_TP_TX_TRACE_THREAD_CREATE(i, a, b, c, d, e) 										xTraceThreadCreateHook(i, a, b)
#define TRC_TP_TX_TRACE_THREAD_DELETE(i, a, b, c, d, e) 										xTraceThreadDeleteHook(i, a)
#define TRC_TP_TX_TRACE_THREAD_ENTRY_EXIT_NOTIFY(i, a, b, c, d, e)								xTraceThreadEntryExitNotifyHook(i, a)
#define TRC_TP_TX_TRACE_THREAD_IDENTIFY(i, a, b, c, d, e)										xTraceThreadIdentifyHook(i)
#define TRC_TP_TX_TRACE_THREAD_INFO_GET(i, a, b, c, d, e)										xTraceThreadInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_THREAD_PERFORMANCE_INFO_GET(i, a, b, c, d, e)							xTraceThreadPerformanceInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_THREAD_PERFORMANCE_SYSTEM_INFO_GET(i, a, b, c, d, e)					xTraceThreadPerformanceSystemInfoGetHook(i)
#define TRC_TP_TX_TRACE_THREAD_PREEMPTION_CHANGE(i, a, b, c, d, e)								xTraceThreadPreemptionChangeHook(i, a, b)
#define TRC_TP_TX_TRACE_THREAD_PRIORITY_CHANGE(i, a, b, c, d, e)								xTraceThreadPriorityChangeHook(i, a, b)
#define TRC_TP_TX_TRACE_THREAD_RELINQUISH(i, a, b, c, d, e)										xTraceThreadRelinquishHook(i)
#define TRC_TP_TX_TRACE_THREAD_RESET(i, a, b, c, d, e)											xTraceThreadResetHook(i, a)
#define TRC_TP_TX_TRACE_THREAD_RESUME_API(i, a, b, c, d, e)										xTraceThreadResumeAPIHook(i, a)
#define TRC_TP_TX_TRACE_THREAD_SLEEP(i, a, b, c, d, e)											xTraceThreadSleepHook(i, a)
#define TRC_TP_TX_TRACE_THREAD_STACK_ERROR_NOTIFY(i, a, b, c, d, e)								xTraceThreadStackErrorNotifyHook(i)
#define TRC_TP_TX_TRACE_THREAD_SUSPEND_API(i, a, b, c, d, e)									xTraceThreadSuspendAPIHook(i, a)
#define TRC_TP_TX_TRACE_THREAD_TERMINATE(i, a, b, c, d, e)										xTraceThreadTerminateHook(i, a)
#define TRC_TP_TX_TRACE_THREAD_TIME_SLICE_CHANGE(i, a, b, c, d, e)								xTraceThreadTimeSliceChangeHook(i, a, b)
#define TRC_TP_TX_TRACE_THREAD_WAIT_ABORT(i, a, b, c, d, e)										xTraceThreadWaitAbortHook(i, a)

#define TRC_TP_TX_TRACE_TIME_GET(i, a, b, c, d, e)												xTraceTimeGetHook(i)
#define TRC_TP_TX_TRACE_TIME_SET(i, a, b, c, d, e)												xTraceTimeSetHook(i, a)
#define TRC_TP_TX_TRACE_TIMER_ACTIVATE(i, a, b, c, d, e)										xTraceTimerActivateHook(i, a)
#define TRC_TP_TX_TRACE_TIMER_CHANGE(i, a, b, c, d, e)											xTraceTimerChangeHook(i, a, b, c)
#define TRC_TP_TX_TRACE_TIMER_CREATE(i, a, b, c, d, e)											xTraceTimerCreateHook(i, a, b, c, d)
#define TRC_TP_TX_TRACE_TIMER_DEACTIVATE(i, a, b, c, d, e)										xTraceTimerDeactivateHook(i, a)
#define TRC_TP_TX_TRACE_TIMER_DELETE(i, a, b, c, d, e)											xTraceTimerDeleteHook(i, a)
#define TRC_TP_TX_TRACE_TIMER_INFO_GET(i, a, b, c, d, e)										xTraceTimerInfoGetHook(i, a)
#define TRC_TP_TX_TRACE_TIMER_PERFORMANCE_INFO_GET(i, a, b, c, d, e)							xTraceTimerPerformanceInfoGet(i, a)
#define TRC_TP_TX_TRACE_TIMER_PERFORMANCE_SYSTEM_INFO_GET(i, a, b, c, d, e)						xTraceTimerPerformanceSystemInfoGet(i)

#define TRC_TP_TX_TRACE_USER_EVENT(i, a, b, c, d, e)


#define TRC_TP_FX_TRACE_INTERNAL_LOG_SECTOR_CACHE_MISS(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_INTERNAL_DIR_CACHE_MISS(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_INTERNAL_MEDIA_FLUSH(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_INTERNAL_DIR_ENTRY_READ(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_INTERNAL_DIR_ENTRY_WRITE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_INTERNAL_IO_DRIVER_READ(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_INTERNAL_IO_DRIVER_WRITE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_INTERNAL_IO_DRIVER_FLUSH(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_INTERNAL_IO_DRIVER_ABORT(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_INTERNAL_IO_DRIVER_INIT(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_INTERNAL_IO_DRIVER_BOOT_READ(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_INTERNAL_IO_DRIVER_RELEASE_SECTORS(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_INTERNAL_IO_DRIVER_BOOT_WRITE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_INTERNAL_IO_DRIVER_UNINIT(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_ATTRIBUTES_READ(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_ATTRIBUTES_SET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_CREATE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_DEFAULT_GET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_DEFAULT_SET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_DELETE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_FIRST_ENTRY_FIND(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_FIRST_FULL_ENTRY_FIND(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_INFORMATION_GET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_LOCAL_PATH_CLEAR(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_LOCAL_PATH_GET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_LOCAL_PATH_RESTORE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_LOCAL_PATH_SET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_LONG_NAME_GET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_NAME_TEST(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_NEXT_ENTRY_FIND(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_NEXT_FULL_ENTRY_FIND(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_RENAME(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_DIRECTORY_SHORT_NAME_GET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_ALLOCATE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_ATTRIBUTES_READ(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_ATTRIBUTES_SET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_BEST_EFFORT_ALLOCATE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_CLOSE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_CREATE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_DATE_TIME_SET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_DELETE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_OPEN(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_READ(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_RELATIVE_SEEK(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_RENAME(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_SEEK(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_TRUNCATE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_TRUNCATE_RELEASE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_FILE_WRITE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_MEDIA_ABORT(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_MEDIA_CACHE_INVALIDATE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_MEDIA_CHECK(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_MEDIA_CLOSE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_MEDIA_FLUSH(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_MEDIA_FORMAT(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_MEDIA_OPEN(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_MEDIA_READ(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_MEDIA_SPACE_AVAILABLE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_MEDIA_VOLUME_GET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_MEDIA_VOLUME_SET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_MEDIA_WRITE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_SYSTEM_DATE_GET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_SYSTEM_DATE_SET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_SYSTEM_INITIALIZE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_SYSTEM_TIME_GET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_SYSTEM_TIME_SET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_UNICODE_DIRECTORY_CREATE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_UNICODE_DIRECTORY_RENAME(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_UNICODE_FILE_CREATE(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_UNICODE_FILE_RENAME(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_UNICODE_LENGTH_GET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_UNICODE_NAME_GET(i, a, b, c, d, e)
#define TRC_TP_FX_TRACE_UNICODE_SHORT_NAME_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_ARP_REQUEST_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_ARP_REQUEST_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_ARP_RESPONSE_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_ARP_RESPONSE_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_ICMP_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_ICMP_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IGMP_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IP_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IP_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_TCP_DATA_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_TCP_DATA_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_TCP_FIN_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_TCP_FIN_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_TCP_RESET_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_TCP_RESET_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_TCP_SYN_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_TCP_SYN_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_UDP_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_UDP_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_RARP_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_RARP_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_TCP_RETRY(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_TCP_STATE_CHANGE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_PACKET_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_INITIALIZE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_LINK_ENABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_LINK_DISABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_PACKET_BROADCAST(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_ARP_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_ARP_RESPONSE_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_RARP_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_MULTICAST_JOIN(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_MULTICAST_LEAVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_GET_STATUS(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_GET_SPEED(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_GET_DUPLEX_TYPE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_GET_ERROR_COUNT(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_GET_RX_COUNT(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_GET_TX_COUNT(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_GET_ALLOC_ERRORS(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_UNINITIALIZE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_INTERNAL_IO_DRIVER_DEFERRED_PROCESSING(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_ARP_DYNAMIC_ENTRIES_INVALIDATE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_ARP_DYNAMIC_ENTRY_SET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_ARP_ENABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_ARP_GRATUITOUS_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_ARP_HARDWARE_ADDRESS_FIND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_ARP_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_ARP_IP_ADDRESS_FIND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_ARP_STATIC_ENTRIES_DELETE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_ARP_STATIC_ENTRY_CREATE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_ARP_STATIC_ENTRY_DELETE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_ICMP_ENABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_ICMP_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_ICMP_PING(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IGMP_ENABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IGMP_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IGMP_LOOPBACK_DISABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IGMP_LOOPBACK_ENABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IGMP_MULTICAST_JOIN(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IGMP_MULTICAST_LEAVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_ADDRESS_CHANGE_NOTIFY(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_ADDRESS_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_ADDRESS_SET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_CREATE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_DELETE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_DRIVER_DIRECT_COMMAND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_FORWARDING_DISABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_FORWARDING_ENABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_FRAGMENT_DISABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_FRAGMENT_ENABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_GATEWAY_ADDRESS_SET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_RAW_PACKET_DISABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_RAW_PACKET_ENABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_RAW_PACKET_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_RAW_PACKET_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_STATUS_CHECK(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_PACKET_ALLOCATE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_PACKET_COPY(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_PACKET_DATA_APPEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_PACKET_DATA_RETRIEVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_PACKET_LENGTH_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_PACKET_POOL_CREATE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_PACKET_POOL_DELETE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_PACKET_POOL_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_PACKET_RELEASE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_PACKET_TRANSMIT_RELEASE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_RARP_DISABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_RARP_ENABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_RARP_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_SYSTEM_INITIALIZE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_CLIENT_SOCKET_BIND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_CLIENT_SOCKET_CONNECT(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_CLIENT_SOCKET_PORT_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_CLIENT_SOCKET_UNBIND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_ENABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_FREE_PORT_FIND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SERVER_SOCKET_ACCEPT(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SERVER_SOCKET_LISTEN(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SERVER_SOCKET_RELISTEN(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SERVER_SOCKET_UNACCEPT(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SERVER_SOCKET_UNLISTEN(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_CREATE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_DELETE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_DISCONNECT(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_MSS_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_MSS_PEER_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_MSS_SET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_RECEIVE_NOTIFY(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_STATE_WAIT(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_TRANSMIT_CONFIGURE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_ENABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_FREE_PORT_FIND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_SOCKET_BIND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_SOCKET_CHECKSUM_DISABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_SOCKET_CHECKSUM_ENABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_SOCKET_CREATE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_SOCKET_DELETE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_SOCKET_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_SOCKET_PORT_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_SOCKET_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_SOCKET_RECEIVE_NOTIFY(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_SOCKET_SEND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_SOCKET_UNBIND(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_SOURCE_EXTRACT(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_INTERFACE_ATTACH(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_SOCKET_BYTES_AVAILABLE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_STATIC_ROUTE_ADD(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_STATIC_ROUTE_DELETE(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_PEER_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_WINDOW_UPDATE_NOTIFY_SET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_UDP_SOCKET_INTERFACE_SET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_IP_INTERFACE_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_PACKET_DATA_EXTRACT_OFFSET(i, a, b, c, d, e)
#define TRC_TP_NX_TRACE_TCP_SOCKET_BYTES_AVAILABLE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_EVENTS_BASE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_CLASS_INSTANCE_CREATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_CLASS_INSTANCE_DESTROY(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_CONFIGURATION_DELETE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_CONFIGURATION_ENUMERATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_CONFIGURATION_INSTANCE_CREATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_CONFIGURATION_INSTANCE_DELETE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_CONFIGURATION_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_DEVICE_ADDRESS_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_DEVICE_CONFIGURATION_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_DEVICE_CONFIGURATION_SELECT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_DEVICE_DESCRIPTOR_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_DEVICE_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_DEVICE_REMOVE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_DEVICE_RESOURCE_FREE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_ENDPOINT_INSTANCE_CREATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_ENDPOINT_INSTANCE_DELETE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_ENDPOINT_RESET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_ENDPOINT_TRANSFER_ABORT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_HCD_REGISTER(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_INITIALIZE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_INTERFACE_ENDPOINT_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_INTERFACE_INSTANCE_CREATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_INTERFACE_INSTANCE_DELETE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_INTERFACE_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_INTERFACE_SETTING_SELECT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_NEW_CONFIGURATION_CREATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_NEW_DEVICE_CREATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_NEW_ENDPOINT_CREATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_RH_CHANGE_PROCESS(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_RH_DEVICE_EXTRACTION(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_RH_DEVICE_INSERTION(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_TRANSFER_REQUEST(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_STACK_TRANSFER_REQUEST_ABORT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_EVENTS_BASE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_ASIX_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_ASIX_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_ASIX_INTERRUPT_NOTIFICATION(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_ASIX_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_ASIX_WRITE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_AUDIO_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_AUDIO_CONTROL_VALUE_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_AUDIO_CONTROL_VALUE_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_AUDIO_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_AUDIO_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_AUDIO_STREAMING_SAMPLING_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_AUDIO_STREAMING_SAMPLING_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_AUDIO_WRITE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_CDC_ACM_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_CDC_ACM_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_CDC_ACM_IOCTL_SET_LINE_CODING(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_CDC_ACM_IOCTL_GET_LINE_CODING(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_CDC_ACM_IOCTL_SET_LINE_STATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_CDC_ACM_IOCTL_SEND_BREAK(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_CDC_ACM_IOCTL_ABORT_IN_PIPE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_CDC_ACM_IOCTL_ABORT_OUT_PIPE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_CDC_ACM_IOCTL_NOTIFICATION_CALLBACK(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_CDC_ACM_IOCTL_GET_DEVICE_STATUS(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_CDC_ACM_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_CDC_ACM_RECEPTION_START(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_CDC_ACM_RECEPTION_STOP(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_CDC_ACM_WRITE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HID_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HID_CLIENT_REGISTER(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HID_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HID_IDLE_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HID_IDLE_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HID_KEYBOARD_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HID_KEYBOARD_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HID_MOUSE_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HID_MOUSE_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HID_REMOTE_CONTROL_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HID_REMOTE_CONTROL_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HID_REPORT_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HID_REPORT_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HUB_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HUB_CHANGE_DETECT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HUB_PORT_CHANGE_CONNECTION_PROCESS(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HUB_PORT_CHANGE_ENABLE_PROCESS(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HUB_PORT_CHANGE_OVER_CURRENT_PROCESS(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HUB_PORT_CHANGE_RESET_PROCESS(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HUB_PORT_CHANGE_SUSPEND_PROCESS(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_HUB_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_DEVICE_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_DEVICE_RESET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_NOTIFICATION(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_NUM_OBJECTS_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_OBJECT_CLOSE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_OBJECT_COPY(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_OBJECT_DELETE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_OBJECT_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_OBJECT_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_OBJECT_INFO_SEND(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_OBJECT_MOVE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_OBJECT_SEND(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_OBJECT_TRANSFER_ABORT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_REQUEST_CANCEL(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_SESSION_CLOSE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_SESSION_OPEN(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_STORAGE_IDS_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_STORAGE_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_THUMB_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PIMA_WRITE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PRINTER_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PRINTER_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PRINTER_NAME_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PRINTER_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PRINTER_WRITE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PRINTER_SOFT_RESET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PRINTER_STATUS_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_IOCTL_SET_LINE_CODING(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_IOCTL_GET_LINE_CODING(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_IOCTL_SET_LINE_STATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_IOCTL_PURGE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_IOCTL_SEND_BREAK(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_IOCTL_ABORT_IN_PIPE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_IOCTL_ABORT_OUT_PIPE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_IOCTL_REPORT_DEVICE_STATUS_CHANGE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_IOCTL_GET_DEVICE_STATUS(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_RECEPTION_START(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_RECEPTION_STOP(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_PROLIFIC_WRITE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_STORAGE_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_STORAGE_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_STORAGE_MEDIA_CAPACITY_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_STORAGE_MEDIA_FORMAT_CAPACITY_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_STORAGE_MEDIA_MOUNT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_STORAGE_MEDIA_OPEN(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_STORAGE_MEDIA_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_STORAGE_MEDIA_WRITE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_STORAGE_REQUEST_SENSE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_STORAGE_START_STOP(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_STORAGE_UNIT_READY_TEST(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_DPUMP_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_DPUMP_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_DPUMP_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_DPUMP_WRITE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_SWAR_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_SWAR_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_SWAR_IOCTL_ABORT_IN_PIPE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_SWAR_IOCTL_ABORT_OUT_PIPE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_SWAR_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_SWAR_RECEPTION_START(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_SWAR_RECEPTION_STOP(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_SWAR_WRITE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_IOCTL_SET_LINE_CODING(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_IOCTL_GET_LINE_CODING(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_IOCTL_SET_LINE_STATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_IOCTL_PURGE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_IOCTL_SEND_BREAK(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_IOCTL_ABORT_IN_PIPE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_IOCTL_ABORT_OUT_PIPE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_IOCTL_REPORT_DEVICE_STATUS_CHANGE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_IOCTL_GET_DEVICE_STATUS(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_IOCTL_NOTIFICATION_CALLBACK(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_RECEPTION_START(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_RECEPTION_STOP(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_HOST_CLASS_GSER_WRITE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_EVENTS_BASE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_ALTERNATE_SETTING_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_ALTERNATE_SETTING_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_CLASS_REGISTER(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_CLEAR_FEATURE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_CONFIGURATION_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_CONFIGURATION_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_CONNECT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_DESCRIPTOR_SEND(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_DISCONNECT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_ENDPOINT_STALL(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_GET_STATUS(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_HOST_WAKEUP(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_INITIALIZE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_INTERFACE_DELETE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_INTERFACE_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_INTERFACE_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_SET_FEATURE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_TRANSFER_ABORT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_TRANSFER_ALL_REQUEST_ABORT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_TRANSFER_REQUEST(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_STACK_MICROSOFT_EXTENSION_REGISTER(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_EVENTS_BASE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_DPUMP_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_DPUMP_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_DPUMP_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_DPUMP_WRITE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_DPUMP_CHANGE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_CDC_ACM_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_CDC_ACM_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_CDC_ACM_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_CDC_ACM_WRITE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_HID_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_HID_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_HID_EVENT_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_HID_EVENT_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_HID_REPORT_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_HID_REPORT_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_HID_DESCRIPTOR_SEND(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_DEVICE_INFO_SEND(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_EVENT_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_EVENT_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_OBJECT_ADD(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_OBJECT_DATA_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_OBJECT_DATA_SEND(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_OBJECT_DELETE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_OBJECT_HANDLES_SEND(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_OBJECT_INFO_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_OBJECT_INFO_SEND(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_OBJECTS_NUMBER_SEND(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_PARTIAL_OBJECT_DATA_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_RESPONSE_SEND(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_STORAGE_ID_SEND(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_STORAGE_INFO_SEND(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_GET_DEVICE_PROP_DESC_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_GET_DEVICE_PROP_VALUE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_GET_DEVICE_PROP_VALUE_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_GET_OBJECT_PROP_DESC(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_GET_OBJECT_PROP_VALUE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_OBJECTS_PROPS_SUPPORTED_GET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_GET_OBJECT_REFERENCES(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_OBJECTS_PROPS_SUPPORTED_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_SET_OBJECT_REFERENCES(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_STORAGE_FORMAT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_DEVICE_RESET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_PIMA_SET_OBJECT_PROP_VALUE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_RNDIS_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_RNDIS_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_RNDIS_PACKET_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_RNDIS_PACKET_TRANSMIT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_RNDIS_MSG_QUERY(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_RNDIS_MSG_KEEP_ALIVE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_RNDIS_MSG_RESET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_RNDIS_MSG_SET(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_FORMAT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_INQUIRY(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_MODE_SELECT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_MODE_SENSE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_PREVENT_ALLOW_MEDIA_REMOVAL(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_READ_CAPACITY(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_READ_FORMAT_CAPACITY(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_READ_TOC(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_REQUEST_SENSE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_TEST_READY(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_START_STOP(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_VERIFY(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_STORAGE_WRITE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_DFU_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_DFU_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_CDC_ECM_ACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_CDC_ECM_DEACTIVATE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_CDC_ECM_CHANGE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_CDC_ECM_READ(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_CDC_ECM_WRITE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_CDC_ECM_PACKET_TRANSMIT(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_DEVICE_CLASS_CDC_ECM_PACKET_RECEIVE(i, a, b, c, d, e)
#define TRC_TP_UX_TRACE_ERROR(i, a, b, c, d, e)


#ifdef __cplusplus
}
#endif /* C++ */

#endif /* TRC_KERNEL_PORT_HOOKS_H */
