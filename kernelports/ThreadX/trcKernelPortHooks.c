#include <trcRecorder.h>

extern volatile ULONG _tx_thread_system_state;
extern TX_THREAD *_tx_thread_execute_ptr;
extern TX_THREAD *_tx_thread_current_ptr;
extern TX_THREAD pxIdleTxThreadDummy;
static TX_THREAD *pxLastRunningTxThread = TX_NULL;

traceResult xTraceEventBeginRawOffline_orig(uint32_t uiSize, TraceEventHandle_t* pxEventHandle);
traceResult xTraceEventEndOffline_orig(TraceEventHandle_t pxEventHandle);

/* Original version of TRC_EVENT_BEGIN which uses xTraceEventBeginRawOffline_orig for the purpose
 * of allowing interrupt locking for xTraceCheckThreadSwitch together with event posting. */
#define TRC_EVENT_BEGIN_OFFLINE_ORIG(uiEventCode, uiPayloadSize, pxEventHandle) \
	(xTraceIsRecorderEnabled() ? \
		( \
			(xTraceEventBeginRawOffline_orig(sizeof(TraceEvent0_t) + (uiPayloadSize), pxEventHandle)) == TRC_SUCCESS ? \
			( \
				pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter++, \
				SET_BASE_EVENT_DATA((TraceEvent0_t*)(((TraceEventData_t*)*(pxEventHandle))->pvBlob), \
					uiEventCode, \
					(((TraceEventData_t*)*(pxEventHandle))->size - sizeof(TraceEvent0_t)) / sizeof(uint32_t), \
					pxTraceEventDataTable->coreEventData[TRC_CFG_GET_CURRENT_CORE()].eventCounter), \
				((TraceEventData_t*)*(pxEventHandle))->offset += sizeof(TraceEvent0_t), \
				TRC_SUCCESS \
			) : TRC_FAIL \
		) : TRC_FAIL \
	)

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

		TraceEventHandle_t xTraceHandle;

		/* Since the regular xTraceEventBegin and xTraceEventEnd are wrapped via CTI, which calls this
		 * function, we have to use the original (non CTI wrapped) begin/end functions.
		 */
		if (TRC_EVENT_BEGIN_OFFLINE_ORIG(PSF_EVENT_THREAD_SYSTEM_SUSPEND_SUCCESS, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
			xTraceEventAddPointer(xTraceHandle, (void*)_tx_thread_execute_ptr);
			xTraceEventEndOffline_orig(xTraceHandle);
		}
	}

	if (uiFirstThreadSwitch)
	{
		/* Handle thread swapping while considering our dummy idle thread mapping. */
		TX_THREAD *pxCurrentTxThread = _tx_thread_current_ptr == TX_NULL ? &pxIdleTxThreadDummy : _tx_thread_current_ptr;

		if (pxCurrentTxThread != pxLastRunningTxThread) {
			/* Update */
			pxLastRunningTxThread = pxCurrentTxThread;

			/* There has been a thread switch */
			TraceEventHandle_t xTraceHandle;

			/* Since the regular xTraceEventBegin and xTraceEventEnd are wrapped via CTI, which calls this
			 * function, we have to use the original (non CTI wrapped) begin/end functions.
			 */
			if (TRC_EVENT_BEGIN_OFFLINE_ORIG(PSF_EVENT_TASK_POTENTIAL_SWITCH_RESULT, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
				xTraceEventAddPointer(xTraceHandle, (void*)pxCurrentTxThread);
				xTraceEventEndOffline_orig(xTraceHandle);
			}
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
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxNextThreadPtr);
		xTraceEventEnd(xTraceHandle);
	}

	/* Due to the design of ThreadX tracing we have to insert a potential switch
	 * check here. */
	if (xTraceEventBegin(PSF_EVENT_TASK_POTENTIAL_SWITCH, 0, &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventEnd(xTraceHandle);
	}
}


void xTraceBlockAllocateHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool, ULONG uiWaitOption, UINT uiRemainingBlocks)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(ULONG) + sizeof(UINT), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxBlockPool);
		xTraceEventAddUnsignedBaseType(xTraceHandle, uiWaitOption);
		xTraceEventAdd32(xTraceHandle, uiRemainingBlocks);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceBlockPoolCreateHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool, ULONG uiBlockSize, VOID *pvPoolStart, ULONG uiTotalBlocks)
{
	TraceEventHandle_t xTraceHandle;

	if (pxBlockPool->tx_block_pool_name != 0) {
		xTraceObjectSetNameWithoutHandle((void*)pxBlockPool, pxBlockPool->tx_block_pool_name);
	}

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(ULONG) + sizeof(void*) + sizeof(ULONG), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxBlockPool);
		xTraceEventAddUnsignedBaseType(xTraceHandle, uiBlockSize);
		xTraceEventAddPointer(xTraceHandle, (void*)pvPoolStart);
		xTraceEventAddUnsignedBaseType(xTraceHandle, uiTotalBlocks);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceBlockPoolDeleteHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool)
{
	xTraceObjectUnregisterWithoutHandle(uiTxEventCode, (void*)pxBlockPool, 0);
}

void xTraceBlockPoolInfoGetHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxBlockPool);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceBlockPoolPerformanceInfoGetHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxBlockPool);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceBlockPoolPerformanceSystemInfoGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceBlockPoolPrioritizeHook(UINT uiTxEventCode, TX_BLOCK_POOL *pxBlockPool)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxBlockPool);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceBlockReleaseHook(UINT uiTxEventCode, VOID *pvBlockPool)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pvBlockPool);
		xTraceEventEnd(xTraceHandle);
	}
}


void xTraceByteAllocateHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceBytePoolCreateHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool, VOID *pvStartPtr, ULONG uiPoolSize)
{
	TraceEventHandle_t xTraceHandle;

	if (pxBytePool->tx_byte_pool_name != 0) {
		xTraceObjectSetNameWithoutHandle((void*)pxBytePool, pxBytePool->tx_byte_pool_name);
	}

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(void*) + sizeof(ULONG),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxBytePool);
		xTraceEventAddPointer(xTraceHandle, (void*)pvStartPtr);
		xTraceEventAddUnsignedBaseType(xTraceHandle, uiPoolSize);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceBytePoolDeleteHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool)
{
	xTraceObjectUnregisterWithoutHandle(uiTxEventCode, (void*)pxBytePool, 0);
}

void xTraceBytePoolInfoGetHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxBytePool);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceBytePoolPerformanceInfoGetHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxBytePool);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceBytePoolPerformanceSystemInfoGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceBytePoolPrioritizeHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxBytePool);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceByteReleaseHook(UINT uiTxEventCode, TX_BYTE_POOL *pxBytePool, VOID *pvMemory, UINT uiAvailableBytes)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(void*) + sizeof(UINT),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxBytePool);
		xTraceEventAddPointer(xTraceHandle, (void*)pvMemory);
		xTraceEventAdd32(xTraceHandle, uiAvailableBytes);
		xTraceEventEnd(xTraceHandle);
	}
}


void xTraceEventFlagsCreateHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup)
{
	TraceEventHandle_t xTraceHandle;

	if (pxFlagsGroup->tx_event_flags_group_name != 0) {
		xTraceObjectSetNameWithoutHandle((void*)pxFlagsGroup, pxFlagsGroup->tx_event_flags_group_name);
	}

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxFlagsGroup);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceEventFlagsDeleteHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup)
{
	xTraceObjectUnregisterWithoutHandle(uiTxEventCode, (void*)pxFlagsGroup, 0);
}

void xTraceEventFlagsGetHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup, ULONG ulRequestedFlags, UINT uiGetOption)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(ULONG) + sizeof(UINT), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxFlagsGroup);
		xTraceEventAddUnsignedBaseType(xTraceHandle, ulRequestedFlags);
		xTraceEventAdd32(xTraceHandle, uiGetOption);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceEventFlagsInfoGetHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxFlagsGroup);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceEventFlagsPerformanceInfoGetHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxFlagsGroup);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceEventFlagsPerformanceSystemInfoGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceEventFlagsSetHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup, ULONG ulFlagsToSet, UINT uiSetOption)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(ULONG) + sizeof(UINT), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxFlagsGroup);
		xTraceEventAddUnsignedBaseType(xTraceHandle, ulFlagsToSet);
		xTraceEventAdd32(xTraceHandle, uiSetOption);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceEventFlagsSetNotifyHook(UINT uiTxEventCode, TX_EVENT_FLAGS_GROUP *pxFlagsGroup)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxFlagsGroup);
		xTraceEventEnd(xTraceHandle);
	}
}


void xTraceMutexCreateHook(UINT uiTxEventCode, TX_MUTEX *pxMutex, UINT uiPrioInherit)
{
	TraceEventHandle_t xTraceHandle;

	if (pxMutex->tx_mutex_name != 0) {
		xTraceObjectSetNameWithoutHandle((void*)pxMutex, pxMutex->tx_mutex_name);
	}

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(UINT), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxMutex);
		xTraceEventAdd32(xTraceHandle, uiPrioInherit);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceMutexDeleteHook(UINT uiTxEventCode, TX_MUTEX *pxMutex)
{
	xTraceObjectUnregisterWithoutHandle(uiTxEventCode, (void*)pxMutex, 0);
}

void xTraceMutexGetHook(UINT uiTxEventCode, TX_MUTEX *pxMutex)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
	(void)pxMutex;
}

void xTraceMutexInfoGetHook(UINT uiTxEventCode, TX_MUTEX *pxMutex)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxMutex);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceMutexPerformanceInfoGetHook(UINT uiTxEventCode, TX_MUTEX *pxMutex)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxMutex);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceMutexPerformanceSystemInfoGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceMutexPrioritizeHook(UINT uiTxEventCode, TX_MUTEX *pxMutex)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxMutex);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceMutexPutHook(UINT uiTxEventCode, TX_MUTEX *pxMutex, UINT uiStateAfter)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(UINT), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxMutex);
		xTraceEventAdd32(xTraceHandle, uiStateAfter);
		xTraceEventEnd(xTraceHandle);
	}
}


void xTraceQueueCreateHook(UINT uiTxEventCode, TX_QUEUE *pxQueue, UINT uiMessageSize, VOID *pvQueueStart, UINT uiQueueSize)
{
	TraceEventHandle_t xTraceHandle;

	if (pxQueue->tx_queue_name != 0) {
		xTraceObjectSetNameWithoutHandle((void*)pxQueue, pxQueue->tx_queue_name);
	}

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(UINT) + sizeof(void*) + sizeof(UINT),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxQueue);
		xTraceEventAdd32(xTraceHandle, uiMessageSize);
		xTraceEventAddPointer(xTraceHandle, (void*)pvQueueStart);
		xTraceEventAdd32(xTraceHandle, uiQueueSize);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceQueueDeleteHook(UINT uiTxEventCode, TX_QUEUE *pxQueue)
{
	xTraceObjectUnregisterWithoutHandle(uiTxEventCode, (void*)pxQueue, 0);
}

void xTraceQueueFlushHook(UINT uiTxEventCode, TX_QUEUE *pxQueue)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxQueue);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceQueueFrontSendHook(UINT uiTxEventCode, TX_QUEUE *pxQueue)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
	(void)pxQueue;
}

void xTraceQueueInfoGetHook(UINT uiTxEventCode, TX_QUEUE *pxQueue)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxQueue);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceQueuePerformanceInfoGetHook(UINT uiTxEventCode, TX_QUEUE *pxQueue)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxQueue);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceQueuePerformanceSystemInfoGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceQueuePrioritizeHook(UINT uiTxEventCode, TX_QUEUE *pxQueue)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxQueue);
		xTraceEventEnd(xTraceHandle);
	}
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
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxQueue);
		xTraceEventEnd(xTraceHandle);
	}
}


void xTraceSemaphoreCeilingPutHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceSemaphoreCreateHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore, ULONG uiInitialCount)
{
	TraceEventHandle_t xTraceHandle;

	if (pxSemaphore->tx_semaphore_name != 0) {
		xTraceObjectSetNameWithoutHandle((void*)pxSemaphore, pxSemaphore->tx_semaphore_name);
	}

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(ULONG), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxSemaphore);
		xTraceEventAddUnsignedBaseType(xTraceHandle, uiInitialCount);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceSemaphoreDeleteHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore)
{
	xTraceObjectUnregisterWithoutHandle(uiTxEventCode, (void*)pxSemaphore, pxSemaphore->tx_semaphore_count);
}

void xTraceSemaphoreGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceSemaphoreInfoGetHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxSemaphore);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceSemaphorePerformanceInfoGetHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxSemaphore);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceSemaphorePerformanceSystemInfoGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceSemaphorePrioritizeHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxSemaphore);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceSemaphorePutHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxSemaphore);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceSemaphorePutNotifyHook(UINT uiTxEventCode, TX_SEMAPHORE *pxSemaphore)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxSemaphore);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceThreadResumeHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	(void)uiTxEventCode;

	xTraceTaskReady(pxThread);
}

void xTraceThreadSuspendHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	(void)pxThread;
	(void)uiTxEventCode;

	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(PSF_EVENT_TASK_POTENTIAL_SWITCH, 0, &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceThreadCreateHook(UINT uiTxEventCode, TX_THREAD *pxThread, UINT uiPriority)
{
	(void)uiTxEventCode;

	TraceTaskHandle_t xTaskHandle;

	xTraceTaskRegister((void*)pxThread, pxThread->tx_thread_name, uiPriority, &xTaskHandle);
}

void xTraceThreadDeleteHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	(void)uiTxEventCode;

	xTraceTaskUnregisterWithoutHandle((void*)pxThread, pxThread->tx_thread_priority);
}

void xTraceThreadEntryExitNotifyHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxThread);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceThreadIdentifyHook(UINT uiTxEventCode)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, 0, &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceThreadInfoGetHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxThread);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceThreadPerformanceInfoGetHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxThread);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceThreadPerformanceSystemInfoGetHook(UINT uiTxEventCode)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, 0, &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceThreadPreemptionChangeHook(UINT uiTxEventCode, TX_THREAD *pxThread, UINT uiNewThreshold)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(UINT), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxThread);
		xTraceEventAdd32(xTraceHandle, uiNewThreshold);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceThreadPriorityChangeHook(UINT uiTxEventCode, TX_THREAD *pxThread, UINT uiNewPriority)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(UINT), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxThread);
		xTraceEventAdd32(xTraceHandle, uiNewPriority);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceThreadRelinquishHook(UINT uiTxEventCode)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, 0, &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventEnd(xTraceHandle);
	}

	/* Due to the design of ThreadX tracing we have to insert a potential switch
	 * check here. */
	if (xTraceEventBegin(PSF_EVENT_TASK_POTENTIAL_SWITCH, 0, &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceThreadResetHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxThread);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceThreadResumeAPIHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxThread);
		xTraceEventEnd(xTraceHandle);
	}
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
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxThread);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceThreadTerminateHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxThread);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceThreadTimeSliceChangeHook(UINT uiTxEventCode, TX_THREAD *pxThread, UINT uiNewTimeSlice)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(UINT), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxThread);
		xTraceEventAdd32(xTraceHandle, uiNewTimeSlice);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceThreadWaitAbortHook(UINT uiTxEventCode, TX_THREAD *pxThread)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxThread);
		xTraceEventEnd(xTraceHandle);
	}
}


void xTraceTimeGetHook(UINT uiTxEventCode)
{
	/* Intentionally left empty */
	(void)uiTxEventCode;
}

void xTraceTimeSetHook(UINT uiTxEventCode, TX_TIMER *pxTimer)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxTimer);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceTimerActivateHook(UINT uiTxEventCode,  TX_TIMER *pxTimer)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxTimer);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceTimerChangeHook(UINT uiTxEventCode, TX_TIMER *pxTimer, ULONG uiInitialTicks, ULONG uiRescheduleTicks)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(ULONG) + sizeof(ULONG), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxTimer);
		xTraceEventAddUnsignedBaseType(xTraceHandle, uiInitialTicks);
		xTraceEventAddUnsignedBaseType(xTraceHandle, uiRescheduleTicks);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceTimerCreateHook(UINT uiTxEventCode, TX_TIMER *pxTimer, ULONG uiInitialTicks, ULONG uiRescheduleTicks, UINT uiAutoActivate)
{
	TraceEventHandle_t xTraceHandle;

	if (pxTimer->tx_timer_name != 0) {
		xTraceObjectSetNameWithoutHandle((void*)pxTimer, pxTimer->tx_timer_name);
	}

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*) + sizeof(ULONG) + sizeof(ULONG) + sizeof(UINT),
		&xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxTimer);
		xTraceEventAddUnsignedBaseType(xTraceHandle, uiInitialTicks);
		xTraceEventAddUnsignedBaseType(xTraceHandle, uiRescheduleTicks);
		xTraceEventAdd32(xTraceHandle, uiAutoActivate);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceTimerDeactivateHook(UINT uiTxEventCode, TX_TIMER *pxTimer)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxTimer);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceTimerDeleteHook(UINT uiTxEventCode, TX_TIMER *pxTimer)
{
	xTraceObjectUnregisterWithoutHandle(uiTxEventCode, (void*)pxTimer, 0);
}

void xTraceTimerInfoGetHook(UINT uiTxEventCode, TX_TIMER *pxTimer)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxTimer);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceTimerPerformanceInfoGet(UINT uiTxEventCode, TX_TIMER *pxTimer)
{
	TraceEventHandle_t xTraceHandle;

	if (xTraceEventBegin(uiTxEventCode, sizeof(void*), &xTraceHandle) == TRC_SUCCESS) {
		xTraceEventAddPointer(xTraceHandle, (void*)pxTimer);
		xTraceEventEnd(xTraceHandle);
	}
}

void xTraceTimerPerformanceSystemInfoGet(UINT uiTxEventCode)
{
	(void)uiTxEventCode;
}
