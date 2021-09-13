/*
 * Trace Recorder for Tracealyzer v4.5.1
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * uC/OS3 specific parts of the trace recorder
 */

#include <os.h>
#include <lib_str.h>
#include "trcInternalBuffer.h"

#if (defined(TRC_USE_TRACEALYZER_RECORDER) && TRC_USE_TRACEALYZER_RECORDER == 1)

#if (OS_VERSION < 30700u)
/* This is used to keep track of the tick task so we are able to filter it */
void* prvTrcTickTask = 0;
#endif /* (OS_VERSION < 30700u) */

#include <stdint.h>

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) || (defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0))

/* TzCtrl task Stack and TCB */
static  CPU_STK  TzCtrlStk[TRC_CFG_CTRL_TASK_STACK_SIZE];
static  OS_TCB   TzCtrlTCB;
static  OS_TCB*  ptrTzCtrlTCB = NULL;

/* The TzCtrl task - receives commands from Tracealyzer (start/stop) */
static  void     TzCtrl (void  *p_arg);

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)
void prvReportStackUsage(void);
#else /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */
#define prvReportStackUsage()
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) || (defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)) */

#define TRC_MAX_STRING_APPEND_SIZE 64

/* Variables to hold the latest suspended and resumed task */
static void* lastResumedTask = 0;
static void* lastSuspendedTask = 0;

void* prvTraceGetCurrentTaskHandle(void)
{
	return (void*)OSTCBCurPtr;                                         /* OSTCBCurPtr is a pointer to currently running TCB    */
}

char* prvTraceAppend(const char *name, const char *suffix)
{
	static char buf[TRC_MAX_STRING_APPEND_SIZE];
	uint32_t i = 0, j = 0;
	
	/* (i < ((TRC_MAX_STRING_APPEND_SIZE) - 2)) leaves room for ' ' and null termination */
	while ((i < ((TRC_MAX_STRING_APPEND_SIZE) - 2)) && (name[i] != 0))
	{
		buf[i] = name[i];
		i++;
	}
	
	buf[i] = ' ';
	i++;
	
	/* (i < ((TRC_MAX_STRING_APPEND_SIZE) - 1)) leaves room for null termination */
	while ((i < ((TRC_MAX_STRING_APPEND_SIZE) - 1)) && (suffix[j] != 0))
	{
		buf[i] = suffix[j];
		i++;
		j++;
	}
	
	/* Always null termination */
	buf[i] = 0;

	return (&buf[0]);
}

/* Tasks */
uint16_t prvTraceGetTaskNumberLow16(void* handle)
{
	if (handle == 0)
		return 0;
	
	return TRACE_GET_LOW16(((OS_TCB*)handle)->TaskID);
}

uint16_t prvTraceGetTaskNumberHigh16(void* handle)
{
	return TRACE_GET_HIGH16(((OS_TCB*)handle)->TaskID);
}

void prvTraceSetTaskNumberLow16(void* handle, uint16_t value)
{
	((OS_TCB*)handle)->TaskID = TRACE_SET_LOW16(((OS_TCB*)handle)->TaskID, value);
}

void prvTraceSetTaskNumberHigh16(void* handle, uint16_t value)
{
	((OS_TCB*)handle)->TaskID = TRACE_SET_HIGH16(((OS_TCB*)handle)->TaskID, value);
}

/* Task Queues */
#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
uint16_t prvTraceGetTaskQueueNumberLow16(void* handle)
{
	return TRACE_GET_LOW16(((OS_MSG_Q *)handle)->MsgQID);
}

uint16_t prvTraceGetTaskQueueNumberHigh16(void* handle)
{
	return TRACE_GET_HIGH16(((OS_MSG_Q *)handle)->MsgQID);
}

void prvTraceSetTaskQueueNumberLow16(void* handle, uint16_t value)
{
	((OS_MSG_Q *)handle)->MsgQID = TRACE_SET_LOW16(((OS_MSG_Q *)handle)->MsgQID, value);
}

void prvTraceSetTaskQueueNumberHigh16(void* handle, uint16_t value)
{
	((OS_MSG_Q *)handle)->MsgQID = TRACE_SET_HIGH16(((OS_MSG_Q *)handle)->MsgQID, value);
}
#endif /* (OS_CFG_TASK_Q_EN == DEF_ENABLED) */

/* Task Semaphores */
uint16_t prvTraceGetTaskSemNumberLow16(void* handle)
{
	return TRACE_GET_LOW16(((OS_TCB *)handle)->SemID);
}

uint16_t prvTraceGetTaskSemNumberHigh16(void* handle)
{
	return TRACE_GET_HIGH16(((OS_TCB *)handle)->SemID);
}

void prvTraceSetTaskSemNumberLow16(void* handle, uint16_t value)
{
	((OS_TCB *)handle)->SemID = TRACE_SET_LOW16(((OS_TCB *)handle)->SemID, value);
}

void prvTraceSetTaskSemNumberHigh16(void* handle, uint16_t value)
{
	((OS_TCB *)handle)->SemID = TRACE_SET_HIGH16(((OS_TCB *)handle)->SemID, value);
}

/* Queues */
uint16_t prvTraceGetQueueNumberLow16(void* handle)
{
	return TRACE_GET_LOW16(((OS_Q*)handle)->MsgQ.MsgQID);
}

uint16_t prvTraceGetQueueNumberHigh16(void* handle)
{
	return TRACE_GET_HIGH16(((OS_Q*)handle)->MsgQ.MsgQID);
}

void prvTraceSetQueueNumberLow16(void* handle, uint16_t value)
{
	((OS_Q*)handle)->MsgQ.MsgQID = TRACE_SET_LOW16(((OS_Q*)handle)->MsgQ.MsgQID, value);
}

void prvTraceSetQueueNumberHigh16(void* handle, uint16_t value)
{
	((OS_Q*)handle)->MsgQ.MsgQID = TRACE_SET_HIGH16(((OS_Q*)handle)->MsgQ.MsgQID, value);
}

/* Semaphores */
uint16_t prvTraceGetSemaphoreNumberLow16(void* handle)
{
	return TRACE_GET_LOW16(((OS_SEM*)handle)->SemID);
}

uint16_t prvTraceGetSemaphoreNumberHigh16(void* handle)
{
	return TRACE_GET_HIGH16(((OS_SEM*)handle)->SemID);
}

void prvTraceSetSemaphoreNumberLow16(void* handle, uint16_t value)
{
	((OS_SEM*)handle)->SemID = TRACE_SET_LOW16(((OS_SEM*)handle)->SemID, value);
}

void prvTraceSetSemaphoreNumberHigh16(void* handle, uint16_t value)
{
	((OS_SEM*)handle)->SemID = TRACE_SET_HIGH16(((OS_SEM*)handle)->SemID, value);
}

/* Mutexes */
uint16_t prvTraceGetMutexNumberLow16(void* handle)
{
	return TRACE_GET_LOW16(((OS_MUTEX*)handle)->MutexID);
}

uint16_t prvTraceGetMutexNumberHigh16(void* handle)
{
	return TRACE_GET_HIGH16(((OS_MUTEX*)handle)->MutexID);
}

void prvTraceSetMutexNumberLow16(void* handle, uint16_t value)
{
	((OS_MUTEX*)handle)->MutexID = TRACE_SET_LOW16(((OS_MUTEX*)handle)->MutexID, value);
}

void prvTraceSetMutexNumberHigh16(void* handle, uint16_t value)
{
	((OS_MUTEX*)handle)->MutexID = TRACE_SET_HIGH16(((OS_MUTEX*)handle)->MutexID, value);
}

/* Flags */
uint16_t prvTraceGetFlagNumberLow16(void* handle)
{
	return TRACE_GET_LOW16(((OS_FLAG_GRP*)handle)->FlagID);
}

uint16_t prvTraceGetFlagNumberHigh16(void* handle)
{
	return TRACE_GET_HIGH16(((OS_FLAG_GRP*)handle)->FlagID);
}

void prvTraceSetFlagNumberLow16(void* handle, uint16_t value)
{
	((OS_FLAG_GRP*)handle)->FlagID = TRACE_SET_LOW16(((OS_FLAG_GRP*)handle)->FlagID, value);
}

void prvTraceSetFlagNumberHigh16(void* handle, uint16_t value)
{
	((OS_FLAG_GRP*)handle)->FlagID = TRACE_SET_HIGH16(((OS_FLAG_GRP*)handle)->FlagID, value);
}

/* Mems */
uint16_t prvTraceGetMemNumberLow16(void* handle)
{
	return TRACE_GET_LOW16(((OS_MEM*)handle)->MemID);
}

uint16_t prvTraceGetMemNumberHigh16(void* handle)
{
	return TRACE_GET_HIGH16(((OS_MEM*)handle)->MemID);
}

void prvTraceSetMemNumberLow16(void* handle, uint16_t value)
{
	((OS_MEM*)handle)->MemID = TRACE_SET_LOW16(((OS_MEM*)handle)->MemID, value);
}

void prvTraceSetMemNumberHigh16(void* handle, uint16_t value)
{
	((OS_MEM*)handle)->MemID = TRACE_SET_HIGH16(((OS_MEM*)handle)->MemID, value);
}

/* Suspend and resume */
void prvTraceTaskSuspendEnter(void* p_tcb)
{
	TRACE_ALLOC_CRITICAL_SECTION();
	TRACE_ENTER_CRITICAL_SECTION();
	lastSuspendedTask = p_tcb;
}

void prvTraceTaskSuspendExit(uint32_t p_err)
{
	if(p_err == OS_ERR_NONE){
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
		if (lastSuspendedTask == 0)
		{
			trcKERNEL_HOOKS_TASK_SUSPEND(TASK_SUSPEND, OSTCBCurPtr);
		}
		else
		{
			trcKERNEL_HOOKS_TASK_SUSPEND(TASK_SUSPEND, lastSuspendedTask);
		}
#else
		if (lastSuspendedTask == 0)
		{
			prvTraceStoreEvent1(PSF_EVENT_TASK_SUSPEND, (uint32_t)OSTCBCurPtr);
		}
		else
		{
			prvTraceStoreEvent1(PSF_EVENT_TASK_SUSPEND, lastSuspendedTask);
		}
#endif
	}
	else{
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
		if (lastSuspendedTask == 0)
		{
			trcKERNEL_HOOKS_TASK_SUSPEND(TASK_SUSPEND_FAIL, OSTCBCurPtr);
		}
		else
		{
			trcKERNEL_HOOKS_TASK_SUSPEND(TASK_SUSPEND_FAIL, lastSuspendedTask);
		}
#else
		if (lastSuspendedTask == 0)
		{
			prvTraceStoreEvent1(PSF_EVENT_TASK_SUSPEND_FAILED, (uint32_t)OSTCBCurPtr);
		}
		else
		{
			prvTraceStoreEvent1(PSF_EVENT_TASK_SUSPEND_FAILED, lastSuspendedTask);
		}
#endif
	}
	TRACE_ALLOC_CRITICAL_SECTION();
	TRACE_EXIT_CRITICAL_SECTION();
}
void prvTraceTaskResumeEnter(void* p_tcb)
{
	TRACE_ALLOC_CRITICAL_SECTION();
	TRACE_ENTER_CRITICAL_SECTION();
	lastResumedTask = p_tcb;
}

void prvTraceTaskResumeExit(uint32_t p_err)
{
	if(p_err == OS_ERR_NONE){
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
		trcKERNEL_HOOKS_TASK_RESUME(TASK_RESUME, lastResumedTask);
#else
		prvTraceStoreEvent1(PSF_EVENT_TASK_RESUME, lastResumedTask);
#endif
	}
	else{
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
		trcKERNEL_HOOKS_TASK_RESUME(TASK_RESUME_FAIL, lastResumedTask);
#else
		prvTraceStoreEvent1(PSF_EVENT_TASK_RESUME_FAILED, lastResumedTask);
#endif
	}
	TRACE_ALLOC_CRITICAL_SECTION();
	TRACE_EXIT_CRITICAL_SECTION();
}


#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)
uint32_t prvTraceGetStackHighWaterMark(void* task)
{
	CPU_STK_SIZE free = 0, used = 0;
	OS_ERR err;
	
	OSTaskStkChk(task, &free, &used, &err);
	
	return free;
}
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

static void* pCurrentTCB = NULL;

/* Monitored by TzCtrl task, that give warnings as User Events */
extern volatile uint32_t NoRoomForSymbol;
extern volatile uint32_t NoRoomForObjectData;
extern volatile uint32_t LongestSymbolName;
extern volatile uint32_t MaxBytesTruncated;
#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)
extern volatile uint32_t TaskStacksNotIncluded;
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */

/* User Event Channel for giving warnings regarding NoRoomForSymbol etc. */
traceString trcWarningChannel = 0;

TRC_STREAM_PORT_ALLOCATE_FIELDS()

/* Called by TzCtrl task periodically (Normally every 100 ms) */
static void prvCheckRecorderStatus(void);

extern void prvTraceWarning(int errCode);

/*******************************************************************************
 * vTraceEnable
 *
 * Function that enables the tracing and creates the control task. It will halt
 * execution until a Start command has been received if haltUntilStart is true.
 *
 ******************************************************************************/
void vTraceEnable(int startOption)
{
    OS_ERR err;
	int32_t bytes = 0;
	int32_t status;
	TracealyzerCommandType msg;
    extern uint32_t RecorderEnabled;
	
	/* Make sure recorder data is initialized */
	vTraceInitialize();

	if (ptrTzCtrlTCB == 0)
	{
		TRC_STREAM_PORT_INIT();
		
		/* The #WFR channel means "Warnings from Recorder" and
		* is used to store warnings and errors from the recorder.
		* The abbreviation #WFR is used instead of the longer full name,
		* to avoid truncation by small slots in the symbol table.
		* This is translated in Tracealyzer and shown as the full name,
		* "Warnings from Recorder".
		*
		* Note: Requires that TRC_CFG_INCLUDE_USER_EVENTS is 1. */
		trcWarningChannel = xTraceRegisterString("#WFR");

        /* Creates the TzCtrl task - receives trace commands (start, stop, ...) */
		OSTaskCreate((OS_TCB     *)&TzCtrlTCB,             /* Create the control task                              */
					(CPU_CHAR   *)"Tracealyzer Control Task",
					(OS_TASK_PTR ) TzCtrl,
					(void       *) 0,
					(OS_PRIO     ) (TRC_CFG_CTRL_TASK_PRIORITY),
					(CPU_STK    *)&TzCtrlStk[0],
					(CPU_STK_SIZE) (TRC_CFG_CTRL_TASK_STACK_SIZE) / 10u,
					(CPU_STK_SIZE) (TRC_CFG_CTRL_TASK_STACK_SIZE),
					(OS_MSG_QTY  ) 0u,
					(OS_TICK     ) 0u,
					(void       *) 0,
					(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
					(OS_ERR     *)&err);
	
		ptrTzCtrlTCB = &TzCtrlTCB;
	}
	
	if (startOption == TRC_START_AWAIT_HOST)
	{
		/* We keep trying to read commands until the recorder has been started */
		do
		{
			bytes = 0;
			
			status = TRC_STREAM_PORT_READ_DATA(&msg, sizeof(TracealyzerCommandType), (int32_t*)&bytes);
			
			if (status != 0)
			{
				prvTraceWarning(PSF_WARNING_STREAM_PORT_READ);
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
	else if (startOption == TRC_INIT)
	{
		/* On TRC_INIT */
		TRC_PORT_SPECIFIC_INIT();
	}
}

/*******************************************************************************
 * prvTraceOnBegin
 *
 * Called on trace begin.
 ******************************************************************************/
void prvTraceOnBegin()
{
	TRC_STREAM_PORT_ON_TRACE_BEGIN();
}

/*******************************************************************************
 * prvTraceOnEnd
 *
 * Called on trace end.
 ******************************************************************************/
void prvTraceOnEnd()
{
	TRC_STREAM_PORT_ON_TRACE_END();
}

/*******************************************************************************
 * prvIsNewTCB
 *
 * Tells if this task is already executing, or if there has been a task-switch.
 * Assumed to be called within a trace hook in kernel context.
 ******************************************************************************/
uint32_t prvIsNewTCB(void* pNewTCB)
{
	if (pCurrentTCB != pNewTCB)
	{
		pCurrentTCB = pNewTCB;
		return 1;
	}
	return 0;
}

/*******************************************************************************
 * prvTraceIsSchedulerSuspended
 *
 * Returns true if the RTOS scheduler currently is disabled, thus preventing any
 * task-switches from occurring. Only called from vTraceStoreISREnd.
 ******************************************************************************/
unsigned char prvTraceIsSchedulerSuspended(void)
{
    return OSRunning != OS_STATE_OS_STOPPED;
}


/*******************************************************************************
 * prvCheckRecorderStatus
 *
 * Called by TzCtrl task periodically (every 100 ms - seems reasonable).
 * Checks a number of diagnostic variables and give warnings as user events,
 * in most cases including a suggested solution.
 ******************************************************************************/
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

/*******************************************************************************
 * TzCtrl
 *
 * Task for sending the trace data from the internal buffer to the stream 
 * interface (assuming TRC_STREAM_PORT_USE_INTERNAL_BUFFER == 1) and for
 * receiving commands from Tracealyzer. Also does some diagnostics.
 ******************************************************************************/
static void TzCtrl(void  *p_arg)
{
    OS_ERR  err;

	TracealyzerCommandType msg;
	int32_t bytes = 0;
	int32_t status = 0;
	(void)p_arg;

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

		OSTimeDly(TRC_CFG_CTRL_TASK_DELAY, OS_OPT_TIME_DLY, &err);	/* 10ms */
	}
}

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/


#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)

/* Initialization of the object property table */
void vTraceInitObjectPropertyTable()
{
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectClasses = TRACE_NCLASSES;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[0] = TRC_CFG_NQUEUE;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[1] = TRC_CFG_NSEMAPHORE;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[2] = TRC_CFG_NMUTEX;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[3] = TRC_CFG_NFLAG;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[4] = TRC_CFG_NMEM;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[5] = TRC_CFG_NTASK_SEM;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[6] = TRC_CFG_NTASK_Q;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[7] = TRC_CFG_NTASK;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[8] = TRC_CFG_NISR;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[0] = TRC_CFG_NAME_LEN_QUEUE;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[1] = TRC_CFG_NAME_LEN_SEMAPHORE;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[2] = TRC_CFG_NAME_LEN_MUTEX;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[3] = TRC_CFG_NAME_LEN_FLAG;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[4] = TRC_CFG_NAME_LEN_MEM;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[5] = TRC_CFG_NAME_LEN_TASK_SEM;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[6] = TRC_CFG_NAME_LEN_TASK_Q;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[7] = TRC_CFG_NAME_LEN_TASK;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[8] = TRC_CFG_NAME_LEN_ISR;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[0] = PropertyTableSizeQueue;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[1] = PropertyTableSizeSemaphore;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[2] = PropertyTableSizeMutex;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[3] = PropertyTableSizeFlag;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[4] = PropertyTableSizeMem;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[5] = PropertyTableSizeTaskSem;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[6] = PropertyTableSizeTaskQ;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[7] = PropertyTableSizeTask;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[8] = PropertyTableSizeISR;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[0] = StartIndexQueue;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[1] = StartIndexSemaphore;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[2] = StartIndexMutex;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[3] = StartIndexFlag;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[4] = StartIndexMem;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[5] = StartIndexTaskSem;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[6] = StartIndexTaskQ;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[7] = StartIndexTask;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[8] = StartIndexISR;
	RecorderDataPtr->ObjectPropertyTable.ObjectPropertyTableSizeInBytes = TRACE_OBJECT_TABLE_SIZE;
}

/* Initialization of the handle mechanism, see e.g, prvTraceGetObjectHandle */
void vTraceInitObjectHandleStack()
{
    uint32_t i = 0;

	objectHandleStacks.indexOfNextAvailableHandle[0] = objectHandleStacks.lowestIndexOfClass[0] = 0;
	objectHandleStacks.indexOfNextAvailableHandle[1] = objectHandleStacks.lowestIndexOfClass[1] = (TRC_CFG_NQUEUE);
	objectHandleStacks.indexOfNextAvailableHandle[2] = objectHandleStacks.lowestIndexOfClass[2] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE);
	objectHandleStacks.indexOfNextAvailableHandle[3] = objectHandleStacks.lowestIndexOfClass[3] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX);
	objectHandleStacks.indexOfNextAvailableHandle[4] = objectHandleStacks.lowestIndexOfClass[4] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) + (TRC_CFG_NFLAG);
	objectHandleStacks.indexOfNextAvailableHandle[5] = objectHandleStacks.lowestIndexOfClass[5] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) + (TRC_CFG_NFLAG) + (TRC_CFG_NMEM);
	objectHandleStacks.indexOfNextAvailableHandle[6] = objectHandleStacks.lowestIndexOfClass[6] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) + (TRC_CFG_NFLAG) + (TRC_CFG_NMEM) + (TRC_CFG_NTASK_SEM);
	objectHandleStacks.indexOfNextAvailableHandle[7] = objectHandleStacks.lowestIndexOfClass[7] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) + (TRC_CFG_NFLAG) + (TRC_CFG_NMEM) + (TRC_CFG_NTASK_SEM) + (TRC_CFG_NTASK_Q);
	objectHandleStacks.indexOfNextAvailableHandle[8] = objectHandleStacks.lowestIndexOfClass[8] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) + (TRC_CFG_NFLAG) + (TRC_CFG_NMEM) + (TRC_CFG_NTASK_SEM) + (TRC_CFG_NTASK_Q) + (TRC_CFG_NTASK);

	objectHandleStacks.highestIndexOfClass[0] = (TRC_CFG_NQUEUE) - 1;
	objectHandleStacks.highestIndexOfClass[1] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) - 1;
	objectHandleStacks.highestIndexOfClass[2] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) - 1;
	objectHandleStacks.highestIndexOfClass[3] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) + (TRC_CFG_NFLAG) - 1;
	objectHandleStacks.highestIndexOfClass[4] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) + (TRC_CFG_NFLAG) + (TRC_CFG_NMEM) - 1;
	objectHandleStacks.highestIndexOfClass[5] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) + (TRC_CFG_NFLAG) + (TRC_CFG_NMEM) + (TRC_CFG_NTASK_SEM) - 1;
	objectHandleStacks.highestIndexOfClass[6] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) + (TRC_CFG_NFLAG) + (TRC_CFG_NMEM) + (TRC_CFG_NTASK_SEM) + (TRC_CFG_NTASK_Q) - 1;
	objectHandleStacks.highestIndexOfClass[7] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) + (TRC_CFG_NFLAG) + (TRC_CFG_NMEM) + (TRC_CFG_NTASK_SEM) + (TRC_CFG_NTASK_Q) + (TRC_CFG_NTASK) - 1;
	objectHandleStacks.highestIndexOfClass[8] = (TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) + (TRC_CFG_NFLAG) + (TRC_CFG_NMEM) + (TRC_CFG_NTASK_SEM) + (TRC_CFG_NTASK_Q) + (TRC_CFG_NTASK) + (TRC_CFG_NISR) - 1;

	for (i = 0; i < TRACE_NCLASSES; i++)
	{
		objectHandleStacks.handleCountWaterMarksOfClass[i] = 0;
	}

	for (i = 0; i < TRACE_KERNEL_OBJECT_COUNT; i++)
	{
		objectHandleStacks.objectHandles[i] = 0;
	}
}

/******************************************************************************
* vTraceEnable(int startOption) - snapshot mode
*
* Initializes and optionally starts the trace, depending on the start option.
* To use the trace recorder, the startup must call vTraceEnable before any RTOS
* calls are made (including "create" calls). Three start options are provided:
*
* TRC_START: Starts the tracing directly. In snapshot mode this allows for
* starting the trace at any point in your code, assuming vTraceEnable(TRC_INIT)
* has been called in the startup.
* Can also be used for streaming without Tracealyzer control, e.g. to a local
* flash file system (assuming such a "stream port", see trcStreamingPort.h).
*
* TRC_INIT: Initializes the trace recorder, but does not start the tracing.
* In snapshot mode, this must be followed by a vTraceEnable(TRC_START) sometime
* later.
*
* Usage examples, in snapshot mode:
*
* Snapshot trace, from startup:
* 	<board init>
* 	vTraceEnable(TRC_START);
* 	<RTOS init>
*
* Snapshot trace, from a later point:
* 	<board init>
* 	vTraceEnable(TRC_INIT);
* 	<RTOS init>
* 	...
* 	vTraceEnable(TRC_START); // e.g., in task context, at some relevant event
*
*
* Note: See other implementation of vTraceEnable in trcStreamingRecorder.c
******************************************************************************/
void vTraceEnable(int startOption)
{	
	/* Make sure recorder data is initialized */
	vTraceInitialize();

	if (startOption == TRC_START)
	{
		prvTraceInitTimestamps();
		
		vTraceStart();
	}
	else if (startOption == TRC_START_AWAIT_HOST)
	{
		prvTraceError("vTraceEnable(TRC_START_AWAIT_HOST) not allowed in Snapshot mode");
	}
	else if (startOption != TRC_INIT)
	{
		prvTraceError("Unexpected argument to vTraceEnable (snapshot mode)");
	}
}

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)
/******************************************************************************
* vTraceStartStackMonitor()
*
* Starts the TzCtrl task which reports stack usage.
* This must be called after OSInit().
******************************************************************************/
void vTraceStartStackMonitor()
{
	OS_ERR err;
	
	if (ptrTzCtrlTCB == NULL)
	{
		/* Creates the TzCtrl task - reports unsed stack */
		OSTaskCreate((OS_TCB     *) &TzCtrlTCB,             /* Create the control task                              */
					 (CPU_CHAR   *) "TzCtrl",
					 (OS_TASK_PTR ) TzCtrl,
					 (void       *) 0,
					 (OS_PRIO     ) (TRC_CFG_CTRL_TASK_PRIORITY),
					 (CPU_STK    *) &TzCtrlStk[0],
					 (CPU_STK_SIZE) (TRC_CFG_CTRL_TASK_STACK_SIZE) / 10u,
					 (CPU_STK_SIZE) (TRC_CFG_CTRL_TASK_STACK_SIZE),
					 (OS_MSG_QTY  ) 0u,
					 (OS_TICK     ) 0u,
					 (void       *) 0,
					 (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
					 (OS_ERR     *) &err);
		
		ptrTzCtrlTCB = &TzCtrlTCB;
	}
}
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)
/*******************************************************************************
 * TzCtrl
 *
 * Task used for reporting the stack usage for each task.
 ******************************************************************************/
static void TzCtrl(void  *p_arg)
{
	OS_ERR err;
	
	(void)p_arg;

	while (1)
	{
		if (xTraceIsRecordingEnabled())
		{
			prvReportStackUsage();
		}

		OSTimeDly(TRC_CFG_CTRL_TASK_DELAY, OS_OPT_TIME_DLY, &err);
	}
}
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */

/* Returns the "Not enough handles" error message for this object class */
const char* pszTraceGetErrorNotEnoughHandles(traceObjectClass objectclass)
{
	switch(objectclass)
	{
	case TRACE_CLASS_QUEUE:
		return "Not enough QUEUE handles - increase TRC_CFG_NQUEUE in trcSnapshotConfig.h";
	case TRACE_CLASS_SEMAPHORE:
		return "Not enough SEMAPHORE handles - increase TRC_CFG_NSEMAPHORE in trcSnapshotConfig.h";
	case TRACE_CLASS_MUTEX:
		return "Not enough MUTEX handles - increase TRC_CFG_NMUTEX in trcSnapshotConfig.h";
	case TRACE_CLASS_FLAG:
		return "Not enough FLAG handles - increase TRC_CFG_NFLAG in trcSnapshotConfig.h";
	case TRACE_CLASS_MEM:
		return "Not enough MEM handles - increase TRC_CFG_NMEM in trcSnapshotConfig.h";
	case TRACE_CLASS_TASK_SEM:
		return "Not enough TASK_SEM handles - increase TRC_CFG_NTASK_SEM in trcSnapshotConfig.h";
	case TRACE_CLASS_TASK_Q:
		return "Not enough TASK_Q handles - increase TRC_CFG_NTASK_Q in trcSnapshotConfig.h";
	case TRACE_CLASS_TASK:
		return "Not enough TASK handles - increase TRC_CFG_NTASK in trcSnapshotConfig.h";
	case TRACE_CLASS_ISR:
		return "Not enough ISR handles - increase TRC_CFG_NISR in trcSnapshotConfig.h";
	default:
		return "pszTraceGetErrorHandles: Invalid objectclass!";
	}
}

/*******************************************************************************
 * prvTraceIsSchedulerSuspended
 *
 * Returns true if the RTOS scheduler currently is disabled, thus preventing any
 * task-switches from occurring. Only called from vTraceStoreISREnd.
 ******************************************************************************/
#if (TRC_CFG_INCLUDE_ISR_TRACING == 1)
unsigned char prvTraceIsSchedulerSuspended(void)
{
    return OSRunning != OS_STATE_OS_STOPPED;
}
#endif

#endif /* Snapshot mode */

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
