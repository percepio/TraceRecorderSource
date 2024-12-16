/*
 * Trace Recorder for Tracealyzer v4.10.2
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * ThreadX specific hooks
 */

#include <trcRecorder.h>

extern volatile ULONG _tx_thread_system_state;
extern TX_THREAD *_tx_thread_execute_ptr;
extern TX_THREAD *_tx_thread_current_ptr;
extern TX_THREAD pxIdleTxThreadDummy;
static TX_THREAD *pxLastRunningTxThread = TX_NULL;

traceResult xTraceEventCreate1_orig(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1);

void xTraceResetCurrentThread()
{
	pxLastRunningTxThread = TX_NULL;
}

void xTraceCheckThreadSwitch()
{
	static uint8_t uiFirstThreadSwitch = 0;

	if (uiFirstThreadSwitch == 0 && _tx_thread_execute_ptr != 0 && _tx_thread_system_state == 0)
	{
		uiFirstThreadSwitch = 1;

		/* Update */
		pxLastRunningTxThread = _tx_thread_execute_ptr;

		(void)xTraceEventCreate1_orig(PSF_EVENT_THREAD_SYSTEM_SUSPEND_SUCCESS, (TraceUnsignedBaseType_t)_tx_thread_execute_ptr);
	}

	if (uiFirstThreadSwitch)
	{
		/* Handle thread swapping while considering our dummy idle thread mapping. */
		TX_THREAD *pxCurrentTxThread = _tx_thread_current_ptr == TX_NULL ? &pxIdleTxThreadDummy : _tx_thread_current_ptr;

		if (pxCurrentTxThread != pxLastRunningTxThread)
		{
			/* Update */
			pxLastRunningTxThread = pxCurrentTxThread;

			(void)xTraceEventCreate1_orig(PSF_EVENT_TASK_POTENTIAL_SWITCH_RESULT, (TraceUnsignedBaseType_t)pxCurrentTxThread);
		}
	}
}


void xTraceISREnterHook(UINT uiTxEventCode, UINT uiISRNr)
{
	(void)uiTxEventCode;
	(void)uiISRNr;
}

void xTraceISRExitHook(UINT uiTxEventCode)
{
	(void)uiTxEventCode;
}

void xTraceTimeSliceHook(UINT uiTxEventCode, TX_THREAD *pxNextThreadPtr)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxNextThreadPtr);

	/* Due to the design of ThreadX tracing we have to insert a potential switch
	 * check here. */
	(void)xTraceEventCreate0(PSF_EVENT_TASK_POTENTIAL_SWITCH);
}


void xTraceBlockAllocateHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool, ULONG uiWaitOption, UINT uiRemainingBlocks)
{
	(void)xTraceEventCreate3(uiTxEventCode, (TraceUnsignedBaseType_t)pxBlockPool, (TraceUnsignedBaseType_t)uiWaitOption, (TraceUnsignedBaseType_t)uiRemainingBlocks);
}

void xTraceBlockPoolCreateHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool, ULONG uiBlockSize, VOID *pvPoolStart, ULONG uiTotalBlocks)
{
	if (pxBlockPool->tx_block_pool_name != 0) {
		(void)xTraceObjectSetNameWithoutHandle((void*)pxBlockPool, pxBlockPool->tx_block_pool_name);
	}

	(void)xTraceEventCreate4(uiTxEventCode, (TraceUnsignedBaseType_t)pxBlockPool, (TraceUnsignedBaseType_t)uiBlockSize, (TraceUnsignedBaseType_t)pvPoolStart, (TraceUnsignedBaseType_t)uiTotalBlocks);
}

void xTraceBlockPoolDeleteHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool)
{
	(void)xTraceObjectUnregisterWithoutHandle(uiTxEventCode, (void*)pxBlockPool, 0);
}

void xTraceBlockPoolInfoGetHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxBlockPool);
}

void xTraceBlockPoolPerformanceInfoGetHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxBlockPool);
}

void xTraceBlockPoolPerformanceSystemInfoGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceBlockPoolPrioritizeHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxBlockPool);
}

void xTraceBlockReleaseHook(UINT uiTxEventCode, VOID *pvBlockPool)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pvBlockPool);
}


void xTraceByteAllocateHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceBytePoolCreateHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool, VOID *pvStartPtr, ULONG uiPoolSize)
{
	if (pxBytePool->tx_byte_pool_name != 0) {
		(void)xTraceObjectSetNameWithoutHandle((void*)pxBytePool, pxBytePool->tx_byte_pool_name);
	}

	(void)xTraceEventCreate3(uiTxEventCode, (TraceUnsignedBaseType_t)pxBytePool, (TraceUnsignedBaseType_t)pvStartPtr, (TraceUnsignedBaseType_t)uiPoolSize);
}

void xTraceBytePoolDeleteHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool)
{
	(void)xTraceObjectUnregisterWithoutHandle(uiTxEventCode, (void*)pxBytePool, 0);
}

void xTraceBytePoolInfoGetHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxBytePool);
}

void xTraceBytePoolPerformanceInfoGetHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxBytePool);
}

void xTraceBytePoolPerformanceSystemInfoGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceBytePoolPrioritizeHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxBytePool);
}

void xTraceByteReleaseHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool, VOID *pvMemory, UINT uiAvailableBytes)
{
	(void)xTraceEventCreate3(uiTxEventCode, (TraceUnsignedBaseType_t)pxBytePool, (TraceUnsignedBaseType_t)pvMemory, (TraceUnsignedBaseType_t)uiAvailableBytes);
}


void xTraceEventFlagsCreateHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup)
{
	if (pxFlagsGroup->tx_event_flags_group_name != 0) {
		(void)xTraceObjectSetNameWithoutHandle((void*)pxFlagsGroup, pxFlagsGroup->tx_event_flags_group_name);
	}

	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxFlagsGroup);
}

void xTraceEventFlagsDeleteHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup)
{
	(void)xTraceObjectUnregisterWithoutHandle(uiTxEventCode, (void*)pxFlagsGroup, 0);
}

void xTraceEventFlagsGetHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup, ULONG ulRequestedFlags, UINT uiGetOption)
{
	(void)xTraceEventCreate3(uiTxEventCode, (TraceUnsignedBaseType_t)pxFlagsGroup, (TraceUnsignedBaseType_t)ulRequestedFlags, (TraceUnsignedBaseType_t)uiGetOption);
}

void xTraceEventFlagsInfoGetHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxFlagsGroup);
}

void xTraceEventFlagsPerformanceInfoGetHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxFlagsGroup);
}

void xTraceEventFlagsPerformanceSystemInfoGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceEventFlagsSetHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup, ULONG ulFlagsToSet, UINT uiSetOption)
{
	(void)xTraceEventCreate3(uiTxEventCode, (TraceUnsignedBaseType_t)pxFlagsGroup, (TraceUnsignedBaseType_t)ulFlagsToSet, (TraceUnsignedBaseType_t)uiSetOption);
}

void xTraceEventFlagsSetNotifyHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxFlagsGroup);
}


void xTraceMutexCreateHook(UINT uiTxEventCode, TX_MUTEX *pxMutex, UINT uiPrioInherit)
{
	if (pxMutex->tx_mutex_name != 0) {
		(void)xTraceObjectSetNameWithoutHandle((void*)pxMutex, pxMutex->tx_mutex_name);
	}

	(void)xTraceEventCreate2(uiTxEventCode, (TraceUnsignedBaseType_t)pxMutex, (TraceUnsignedBaseType_t)uiPrioInherit);
}

void xTraceMutexDeleteHook(UINT uiTxEventCode, TX_MUTEX *pxMutex)
{
	(void)xTraceObjectUnregisterWithoutHandle(uiTxEventCode, (void*)pxMutex, 0);
}

void xTraceMutexGetHook(UINT uiTxEventCode, TX_MUTEX *pxMutex)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
	(void)pxMutex;
}

void xTraceMutexInfoGetHook(UINT uiTxEventCode, TX_MUTEX *pxMutex)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxMutex);
}

void xTraceMutexPerformanceInfoGetHook(UINT uiTxEventCode, TX_MUTEX *pxMutex)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxMutex);
}

void xTraceMutexPerformanceSystemInfoGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceMutexPrioritizeHook(UINT uiTxEventCode, TX_MUTEX *pxMutex)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxMutex);
}

void xTraceMutexPutHook(UINT uiTxEventCode, TX_MUTEX *pxMutex, UINT uiStateAfter)
{
	(void)xTraceEventCreate2(uiTxEventCode, (TraceUnsignedBaseType_t)pxMutex, (TraceUnsignedBaseType_t)uiStateAfter);
}


void xTraceQueueCreateHook(UINT uiTxEventCode, TX_QUEUE *pxQueue, UINT uiMessageSize, VOID *pvQueueStart, UINT uiQueueSize)
{
	if (pxQueue->tx_queue_name != 0) {
		(void)xTraceObjectSetNameWithoutHandle((void*)pxQueue, pxQueue->tx_queue_name);
	}

	(void)xTraceEventCreate4(uiTxEventCode, (TraceUnsignedBaseType_t)pxQueue, (TraceUnsignedBaseType_t)uiMessageSize, (TraceUnsignedBaseType_t)pvQueueStart, (TraceUnsignedBaseType_t)uiQueueSize);
}

void xTraceQueueDeleteHook(UINT uiTxEventCode, TX_QUEUE *pxQueue)
{
	(void)xTraceObjectUnregisterWithoutHandle(uiTxEventCode, (void*)pxQueue, 0);
}

void xTraceQueueFlushHook(UINT uiTxEventCode, TX_QUEUE *pxQueue)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxQueue);
}

void xTraceQueueFrontSendHook(UINT uiTxEventCode, TX_QUEUE *pxQueue)
{
	xTraceCheckThreadSwitch();

	/* Intentionally left empty */
	(void)uiTxEventCode;
	(void)pxQueue;
}

void xTraceQueueInfoGetHook(UINT uiTxEventCode, TX_QUEUE *pxQueue)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxQueue);
}

void xTraceQueuePerformanceInfoGetHook(UINT uiTxEventCode, TX_QUEUE *pxQueue)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxQueue);
}

void xTraceQueuePerformanceSystemInfoGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceQueuePrioritizeHook(UINT uiTxEventCode, TX_QUEUE *pxQueue)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxQueue);
}

void xTraceQueueReceiveHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceQueueSendHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceQueueSendNotifyHook(UINT uiTxEventCode, TX_QUEUE *pxQueue)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxQueue);
}


void xTraceSemaphoreCeilingPutHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceSemaphoreCreateHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore, ULONG uiInitialCount)
{
	if (pxSemaphore->tx_semaphore_name != 0) {
		(void)xTraceObjectSetNameWithoutHandle((void*)pxSemaphore, pxSemaphore->tx_semaphore_name);
	}

	(void)xTraceEventCreate2(uiTxEventCode, (TraceUnsignedBaseType_t)pxSemaphore, (TraceUnsignedBaseType_t)uiInitialCount);
}

void xTraceSemaphoreDeleteHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore)
{
	(void)xTraceObjectUnregisterWithoutHandle(uiTxEventCode, (void*)pxSemaphore, pxSemaphore->tx_semaphore_count);
}

void xTraceSemaphoreGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceSemaphoreInfoGetHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxSemaphore);
}

void xTraceSemaphorePerformanceInfoGetHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxSemaphore);
}

void xTraceSemaphorePerformanceSystemInfoGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceSemaphorePrioritizeHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxSemaphore);
}

void xTraceSemaphorePutHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxSemaphore);
}

void xTraceSemaphorePutNotifyHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxSemaphore);
}

void xTraceThreadResumeHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	(void)uiTxEventCode;

	(void)xTraceTaskReady(pxThread);
}

void xTraceThreadSuspendHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	(void)pxThread;
	(void)uiTxEventCode;

	(void)xTraceEventCreate0(PSF_EVENT_TASK_POTENTIAL_SWITCH);
}

void xTraceThreadCreateHook(UINT uiTxEventCode, TX_THREAD *pxThread, UINT uiPriority)
{
	(void)uiTxEventCode;

	(void)xTraceTaskRegisterWithoutHandle((void*)pxThread, pxThread->tx_thread_name, uiPriority);
}

void xTraceThreadDeleteHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	(void)uiTxEventCode;

	(void)xTraceTaskUnregisterWithoutHandle((void*)pxThread, pxThread->tx_thread_priority);
}

void xTraceThreadEntryExitNotifyHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxThread);
}

void xTraceThreadIdentifyHook(UINT uiTxEventCode)
{
	(void)xTraceEventCreate0(uiTxEventCode);
}

void xTraceThreadInfoGetHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxThread);
}

void xTraceThreadPerformanceInfoGetHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxThread);
}

void xTraceThreadPerformanceSystemInfoGetHook(UINT uiTxEventCode)
{
	(void)xTraceEventCreate0(uiTxEventCode);
}

void xTraceThreadPreemptionChangeHook(UINT uiTxEventCode, TX_THREAD *pxThread, UINT uiNewThreshold)
{
	(void)xTraceEventCreate2(uiTxEventCode, (TraceUnsignedBaseType_t)pxThread, (TraceUnsignedBaseType_t)uiNewThreshold);
}

void xTraceThreadPriorityChangeHook(UINT uiTxEventCode, TX_THREAD *pxThread, UINT uiNewPriority)
{
	(void)xTraceEventCreate2(uiTxEventCode, (TraceUnsignedBaseType_t)pxThread, (TraceUnsignedBaseType_t)uiNewPriority);
}

void xTraceThreadRelinquishHook(UINT uiTxEventCode)
{
	(void)xTraceEventCreate0(uiTxEventCode);

	/* Due to the design of ThreadX tracing we have to insert a potential switch
	 * check here. */
	(void)xTraceEventCreate0(PSF_EVENT_TASK_POTENTIAL_SWITCH);
}

void xTraceThreadResetHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxThread);
}

void xTraceThreadResumeAPIHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxThread);
}

void xTraceThreadSleepHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
	(void)pxThread;
}

void xTraceThreadStackErrorNotifyHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceThreadSuspendAPIHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxThread);
}

void xTraceThreadTerminateHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxThread);
}

void xTraceThreadTimeSliceChangeHook(UINT uiTxEventCode, TX_THREAD *pxThread, UINT uiNewTimeSlice)
{
	(void)xTraceEventCreate2(uiTxEventCode, (TraceUnsignedBaseType_t)pxThread, (TraceUnsignedBaseType_t)uiNewTimeSlice);
}

void xTraceThreadWaitAbortHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxThread);
}


void xTraceTimeGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceTimeSetHook(UINT uiTxEventCode, TX_TIMER *pxTimer)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxTimer);
}

void xTraceTimerActivateHook(UINT uiTxEventCode,  TX_TIMER *pxTimer)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxTimer);
}

void xTraceTimerChangeHook(UINT uiTxEventCode, TX_TIMER *pxTimer, ULONG uiInitialTicks, ULONG uiRescheduleTicks)
{
	(void)xTraceEventCreate3(uiTxEventCode, (TraceUnsignedBaseType_t)pxTimer, (TraceUnsignedBaseType_t)uiInitialTicks, (TraceUnsignedBaseType_t)uiRescheduleTicks);
}

void xTraceTimerCreateHook(UINT uiTxEventCode, TX_TIMER *pxTimer, ULONG uiInitialTicks, ULONG uiRescheduleTicks, UINT uiAutoActivate)
{
	if (pxTimer->tx_timer_name != 0) {
		(void)xTraceObjectSetNameWithoutHandle((void*)pxTimer, pxTimer->tx_timer_name);
	}

	(void)xTraceEventCreate4(uiTxEventCode, (TraceUnsignedBaseType_t)pxTimer, (TraceUnsignedBaseType_t)uiInitialTicks, (TraceUnsignedBaseType_t)uiRescheduleTicks, (TraceUnsignedBaseType_t)uiAutoActivate);
}

void xTraceTimerDeactivateHook(UINT uiTxEventCode, TX_TIMER *pxTimer)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxTimer);
}

void xTraceTimerDeleteHook(UINT uiTxEventCode, TX_TIMER *pxTimer)
{
	(void)xTraceObjectUnregisterWithoutHandle(uiTxEventCode, (void*)pxTimer, 0);
}

void xTraceTimerInfoGetHook(UINT uiTxEventCode, TX_TIMER *pxTimer)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxTimer);
}

void xTraceTimerPerformanceInfoGet(UINT uiTxEventCode, TX_TIMER *pxTimer)
{
	(void)xTraceEventCreate1(uiTxEventCode, (TraceUnsignedBaseType_t)pxTimer);
}

void xTraceTimerPerformanceSystemInfoGet(UINT uiTxEventCode)
{
	(void)uiTxEventCode;
}
