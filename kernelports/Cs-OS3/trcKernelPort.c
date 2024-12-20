/*
 * Trace Recorder for Tracealyzer v4.10.2
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Tracealyzer support for Cs/OS3
 */

#include <os.h>
#include <trcRecorder.h>

#if (defined(TRC_USE_TRACEALYZER_RECORDER) && TRC_USE_TRACEALYZER_RECORDER == 1)

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) || (defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0))
uint32_t uiTraceTickCount;

/* TzCtrl task Stack and TCB */
static  CPU_STK  TzCtrlStk[TRC_CFG_CTRL_TASK_STACK_SIZE];
static  OS_TCB   TzCtrlTCB;
static  OS_TCB*  ptrTzCtrlTCB = NULL;

/* The TzCtrl task - receives commands from Tracealyzer (start/stop) */
static  void  TzCtrl (void  *p_arg);

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) || (defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)) */

#define TRC_MAX_STRING_APPEND_SIZE 64

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
traceResult xTraceKernelPortInitialize(TraceKernelPortDataBuffer_t* const pxBuffer)
{
    return TRC_SUCCESS;
}

traceResult xTraceKernelPortEnable(void)
{
    OS_ERR  err;


	if (ptrTzCtrlTCB == 0)
	{
        /* Creates the TzCtrl task - receives trace commands (start, stop, ...) */
		OSTaskCreate(&TzCtrlTCB,                                /* Create the control task                              */
					 "Tracealyzer Control Task",
					  TzCtrl,
					  0,
					  (TRC_CFG_CTRL_TASK_PRIORITY),
					 &TzCtrlStk[0],
					  (TRC_CFG_CTRL_TASK_STACK_SIZE) / 10u,
					  (TRC_CFG_CTRL_TASK_STACK_SIZE),
					  0u,
					  0u,
					  0,
					 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
					 &err);

		ptrTzCtrlTCB = &TzCtrlTCB;
	}


	return TRC_SUCCESS;
}

/*******************************************************************************
 * TzCtrl
 *
 * Task used for reporting the stack usage for each task.
 ******************************************************************************/
static void TzCtrl (void  *p_arg)
{
	OS_ERR err;

	(void)p_arg;

	while (1)
	{
		xTraceTzCtrl();

		OSTimeDly(TRC_CFG_CTRL_TASK_DELAY, OS_OPT_TIME_DLY, &err);
	}
}

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
