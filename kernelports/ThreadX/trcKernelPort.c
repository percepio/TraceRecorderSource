/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The ThreadX specific parts of the trace recorder
 */

#include <trcRecorder.h>

extern TX_THREAD* _tx_thread_created_ptr;
TX_THREAD pxIdleTxThreadDummy;

#if ((TRC_CFG_RECORDER_MODE) == TRC_RECORDER_MODE_STREAMING || ((TRC_CFG_ENABLE_STACK_MONITOR) == 1) && (TRC_CFG_SCHEDULING_ONLY == 0))

/* Trace recorder control thread stack */
static UCHAR uiTzCtrlTaskMemoryArea[TRC_CFG_CTRL_TASK_STACK_SIZE];
static TX_BYTE_POOL xTzCtrlThreadBytePool;

/*  */
TraceHeapHandle_t xSystemHeapHandle;

#endif /* ((TRC_CFG_RECORDER_MODE) == TRC_RECORDER_MODE_STREAMING || ((TRC_CFG_ENABLE_STACK_MONITOR) == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)) */


/* Streaming kernel port */
#if ((TRC_CFG_RECORDER_MODE) == TRC_RECORDER_MODE_STREAMING)


#endif /*TRC_RECORDER_MODE_STREAMING == 1*/


/**
 * @brief TzCtrl_thread_entry
 *
 * Task for sending the trace data from the internal buffer to the stream
 * interface (assuming TRC_STREAM_PORT_USE_INTERNAL_BUFFER == 1) and for
 * receiving commands from Tracealyzer. Also does some diagnostics.
 *
 * @param[in] _arg
 */
void TzCtrlThreadEntry(ULONG _arg)
{
	(void)_arg;

	while (1)
	{
		xTraceTzCtrl();

		tx_thread_sleep((TRC_CFG_CTRL_TASK_DELAY));
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

	return TRC_SUCCESS;
}

traceResult xTraceKernelPortEnable(void)
{
	TX_THREAD *pxTxThreadPtr = _tx_thread_created_ptr;

	if (pxTxThreadPtr != 0) {
		TRACE_ALLOC_CRITICAL_SECTION();
		TRACE_ENTER_CRITICAL_SECTION();

		for (uint32_t i = 0; i < _tx_thread_created_count; i++) {
			TraceEntryHandle_t xEntryHandle;

			if (pxTxThreadPtr->tx_thread_name == 0)
			{
				continue;
			}

			if (xTraceEntryFind((void*)pxTxThreadPtr, &xEntryHandle) == TRC_FAIL)
			{
				/* No previous entry found. Create one. */
				if (xTraceEntryCreateWithAddress((void*)pxTxThreadPtr, &xEntryHandle) == TRC_FAIL)
				{
					continue;
				}
			}

			xTraceEntrySetSymbol(xEntryHandle, pxTxThreadPtr->tx_thread_name, strlen(pxTxThreadPtr->tx_thread_name));

			pxTxThreadPtr = pxTxThreadPtr->tx_thread_created_next;
		}

		TRACE_EXIT_CRITICAL_SECTION();
	}


	CHAR *puiMemoryPointer = TX_NULL;

	tx_byte_pool_create(&xTzCtrlThreadBytePool, "TZ_byte_pool", (VOID*)uiTzCtrlTaskMemoryArea, TRC_CFG_CTRL_TASK_STACK_SIZE);
	tx_byte_allocate(&xTzCtrlThreadBytePool, (VOID **)&puiMemoryPointer, TRC_CFG_CTRL_TASK_STACK_SIZE - 32, TX_NO_WAIT);
	tx_thread_create(&pxKernelPortData->xTzCtrlHandle, "TzCtrl", TzCtrlThreadEntry, 0, puiMemoryPointer,
							TRC_CFG_CTRL_TASK_STACK_SIZE - 32, TRC_CFG_CTRL_TASK_PRIORITY,
							TRC_CFG_CTRL_TASK_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

	TraceTaskHandle_t xTaskHandle;

	/* While ThreadX does not have an IDLE thread we spawn a symbol and symbol data so
	 * that Tz can track when the system is not actively running another thread. We map
	 * idle to a generic Tz variable to avoid clashing with other potential objects
	 * in the system. */
	xTraceTaskRegister((void*)&pxIdleTxThreadDummy, "IDLE", TX_MAX_PRIORITIES, &xTaskHandle);

	return TRC_SUCCESS;
}

TraceHeapHandle_t xTraceKernelPortGetSystemHeapHandle(void)
{
	return 0;
}

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)
traceResult xTraceKernelPortGetUnusedStack(void* pvThread, TraceUnsignedBaseType_t* puxUnusedStack)
{
#ifdef TX_ENABLE_STACK_CHECKING
	*puxUnusedStack = (char*)((TX_THREAD*)pvThread)->tx_thread_stack_highest_ptr - (char*)((TX_THREAD*)pvThread)->tx_thread_stack_start;
#else
	(void)pvThread;
	*puxUnusedStack = 0;
#endif

	return TRC_SUCCESS;
}
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */

unsigned char xTraceKernelPortIsSchedulerSuspended(void)
{
	return 0;
}
