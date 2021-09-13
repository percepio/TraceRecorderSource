/*
 * Trace Recorder for Tracealyzer v4.5.1
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The SAFERTOS specific parts of the trace recorder
 */

#define KERNEL_SOURCE_FILE

#include "SafeRTOS_API.h"
#include "trcInternalBuffer.h"

#if (configUSE_TRACE_FACILITY == 1 && !defined(TRC_USE_TRACEALYZER_RECORDER))
#error Trace Recorder: You need to include trcRecorder.h at the end of your SafeRTOSConfig.h!
#endif

#if (defined(TRC_USE_TRACEALYZER_RECORDER) && TRC_USE_TRACEALYZER_RECORDER == 1)

#if (configUSE_TICKLESS_IDLE != 0 && (TRC_HWTC_TYPE == TRC_OS_TIMER_INCR || TRC_HWTC_TYPE == TRC_OS_TIMER_DECR))
	/* 	
		The below error message is to alert you on the following issue:
		
		The hardware port selected in trcConfig.h uses a periodic interrupt timer for the 
		timestamping, probably the same timer as used by SafeRTOS for the tick interrupt,
		e.g. SysTick on ARM Cortex-M.
				
		When using tickless idle, the recorder needs an independent time source in order to
		correctly record the length of the idle time, like a free-running counter.
		
		You may override this warning by defining the TRC_CFG_ACKNOWLEDGE_TICKLESS_IDLE_WARNING
		macro in your trcConfig.h file. But then the time scale may be incorrect during
		tickless idle periods.
		
		To get this correct, set up a hardware timer as a free-running counter, set the hardware
		port in trcConfig.h to TRC_HARDWARE_PORT_APPLICATION_DEFINED and define the HWTC macros
		accordingly (see trcHardwarePort.h for details).
				
		For ARM Cortex-M3, M4 and M7 MCUs this is not an issue, since the recorder uses the 
		DWT cycle counter for timestamping when available.		
	*/
	
	#ifndef TRC_CFG_ACKNOWLEDGE_TICKLESS_IDLE_WARNING
	#error Trace Recorder: This timestamping mode is not recommended with Tickless Idle.
	#endif
#endif
	
/*******************************************************************************
* prvTraceIsSchedulerSuspended
*
* Returns true if the RTOS scheduler currently is disabled, thus preventing any
* task-switches from occurring. Only called from vTraceStoreISREnd.
******************************************************************************/
unsigned char prvTraceIsSchedulerSuspended()
{
	return xTaskIsSchedulerSuspended() == pdTRUE;
}

uint8_t prvTraceGetQueueType(void* handle)
{
	// This is either declared in header file in FreeRTOS 8 and later, or as extern above
	return (uint8_t)uxQueueGetQueueType(handle);
}

void* prvGetCurrentTaskHandle()
{
	return xTaskGetCurrentTaskHandle();
}

uint32_t prvGetTaskNumber(void* pxObject)
{
	return uxTaskGetTaskNumber(pxObject);
}

uint32_t prvGetQueueNumber(void* pxObject)
{
	return uxQueueGetQueueNumber(pxObject);
}

#if ((pdKERNEL_MAJOR_VERSION == 5 && pdKERNEL_MINOR_VERSION >= 10) || pdKERNEL_MAJOR_VERSION > 5)
uint32_t prvGetTimerNumber(void* pxObject)
{
	/* There is no uxTimerGetTimerNumber function */
	return ((timerControlBlockType*)pxObject)->uxTimerNumber;
}
#endif /* ((pdKERNEL_MAJOR_VERSION == 5 && pdKERNEL_MINOR_VERSION >= 10) || pdKERNEL_MAJOR_VERSION > 5) */

#if ((pdKERNEL_MAJOR_VERSION == 5 && pdKERNEL_MINOR_VERSION >= 10) || pdKERNEL_MAJOR_VERSION > 5)
uint32_t prvGetEventGroupNumber(void* pxObject)
{
	return uxTaskGetEventGroupNumber(pxObject);
}
#endif /* ((pdKERNEL_MAJOR_VERSION == 5 && pdKERNEL_MINOR_VERSION >= 10) || pdKERNEL_MAJOR_VERSION > 5) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

static void* pCurrentTCB = NULL;
static portTaskHandleType HandleTzCtrl = 0;       /* TzCtrl task TCB */

//#pragma data_alignment=TRC_CFG_CTRL_TASK_STACK_SIZE
static portInt8Type unalignedStackTzCtrl[TRC_CFG_CTRL_TASK_STACK_SIZE + 0x10] = { 0 };	/* Allocate more than necessary */
static xTCB tcbTzCtrl = { 0 };

/* Monitored by TzCtrl task, that give warnings as User Events */
extern volatile uint32_t NoRoomForSymbol;
extern volatile uint32_t NoRoomForObjectData;
extern volatile uint32_t LongestSymbolName;
extern volatile uint32_t MaxBytesTruncated;

/* User Event Channel for giving warnings regarding NoRoomForSymbol etc. */
traceString trcWarningChannel = 0;

TRC_STREAM_PORT_ALLOCATE_FIELDS()

/* Called by TzCtrl task periodically (Normally every 100 ms) */
static void prvCheckRecorderStatus(void);

/* The TzCtrl task - receives commands from Tracealyzer (start/stop) */
static void TzCtrl( void *pvParameters );

#if ((pdKERNEL_MAJOR_VERSION == 5 && pdKERNEL_MINOR_VERSION > 10) || pdKERNEL_MAJOR_VERSION > 5)
xTaskParameters TzCtrlParameters = 
{
	TzCtrl,
	"TzCtrl",
	&tcbTzCtrl,
	0,
	TRC_CFG_CTRL_TASK_STACK_SIZE,
	NULL,
	TRC_CFG_CTRL_TASK_PRIORITY,
	0,
	{
		mpuPRIVILEGED_TASK,
		{
			{ 0, 0UL, 0UL, 0UL },
			{ 0, 0UL, 0UL, 0UL },
			{ 0, 0UL, 0UL, 0UL }
		}
	}
};
#else /* ((pdKERNEL_MAJOR_VERSION == 5 && pdKERNEL_MINOR_VERSION > 10) || pdKERNEL_MAJOR_VERSION > 5) */
xTaskParameters TzCtrlParameters =
{
	TzCtrl,
	"TzCtrl",
	&tcbTzCtrl,
	0,
	TRC_CFG_CTRL_TASK_STACK_SIZE,
	NULL,
	TRC_CFG_CTRL_TASK_PRIORITY,
	{
		mpuPRIVILEGED_TASK,
		{
			{ 0, 0UL, 0UL, 0UL },
			{ 0, 0UL, 0UL, 0UL },
			{ 0, 0UL, 0UL, 0UL }
		}
	}
};
#endif /* ((pdKERNEL_MAJOR_VERSION == 5 && pdKERNEL_MINOR_VERSION > 10) || pdKERNEL_MAJOR_VERSION > 5) */

/*******************************************************************************
 * vTraceEnable
 *
 * Function that enables the tracing and creates the control task. It will halt
 * execution until a Start command has been received if haltUntilStart is true.
 *
 ******************************************************************************/
void vTraceEnable(int startOption)
{
	int32_t bytes = 0;
	int32_t status;
	TracealyzerCommandType msg;
	extern uint32_t RecorderEnabled;
	
	/* Make sure recorder data is initialized */
	vTraceInitialize();

	if (HandleTzCtrl == 0)
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
		TzCtrlParameters.pcStackBuffer = (void*)((((uint32_t)unalignedStackTzCtrl) + 0xF) & ~0xF); /* Align the stack pointer we will use to 16-bytes. It's OK, we allocated 16 bytes more than we needed. */
		xTaskCreate( &TzCtrlParameters, &HandleTzCtrl );
		if (HandleTzCtrl == NULL)
		{
			prvTraceError(PSF_ERROR_TZCTRLTASK_NOT_CREATED);
		}
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
 * prvCheckRecorderStatus
 *
 * Called by TzCtrl task periodically (every 100 ms - seems reasonable).
 * Checks a number of diagnostic variables and give warnings as user events,
 * in most cases including a suggested solution.
 ******************************************************************************/
static void prvCheckRecorderStatus(void)
{
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
 * Task for receiving commands from Tracealyzer and for recorder diagnostics.
 *
 ******************************************************************************/
static void TzCtrl( void *pvParameters )
{
	TracealyzerCommandType msg;
	int32_t bytes = 0;
	int32_t status = 0;
	(void)pvParameters;

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

		prvCheckRecorderStatus();

		xTaskDelay(TRC_CFG_CTRL_TASK_DELAY);	/* 10ms */
	}
}

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/


#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)

/******************************************************************************
 * TraceQueueClassTable
 * Translates a SAFERTOS QueueType into trace object classes (TRACE_CLASS_).
 * Has one entry for each QueueType, gives TRACE_CLASS ID.
 ******************************************************************************/
traceObjectClass TraceQueueClassTable[3] = {
	TRACE_CLASS_QUEUE,
	TRACE_CLASS_SEMAPHORE,
	TRACE_CLASS_MUTEX
};

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

/* Initialization of the object property table */
void vTraceInitObjectPropertyTable()
{
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectClasses = TRACE_NCLASSES;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[0] = TRC_CFG_NQUEUE;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[1] = TRC_CFG_NTASK;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[2] = TRC_CFG_NISR;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[3] = TRC_CFG_NTIMER;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[4] = TRC_CFG_NEVENTGROUP;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[5] = TRC_CFG_NSEMAPHORE;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[6] = TRC_CFG_NMUTEX;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[0] = TRC_CFG_NAME_LEN_QUEUE;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[1] = TRC_CFG_NAME_LEN_TASK;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[2] = TRC_CFG_NAME_LEN_ISR;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[3] = TRC_CFG_NAME_LEN_TIMER;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[4] = TRC_CFG_NAME_LEN_EVENTGROUP;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[5] = TRC_CFG_NAME_LEN_SEMAPHORE;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[6] = TRC_CFG_NAME_LEN_MUTEX;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[0] = PropertyTableSizeQueue;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[1] = PropertyTableSizeTask;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[2] = PropertyTableSizeISR;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[3] = PropertyTableSizeTimer;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[4] = PropertyTableSizeEventGroup;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[5] = PropertyTableSizeSemaphore;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[6] = PropertyTableSizeMutex;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[0] = StartIndexQueue;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[1] = StartIndexTask;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[2] = StartIndexISR;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[3] = StartIndexTimer;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[4] = StartIndexEventGroup;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[5] = StartIndexSemaphore;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[6] = StartIndexMutex;
	RecorderDataPtr->ObjectPropertyTable.ObjectPropertyTableSizeInBytes = TRACE_OBJECT_TABLE_SIZE;
}

/* Initialization of the handle mechanism, see e.g, prvTraceGetObjectHandle */
void vTraceInitObjectHandleStack()
{
	uint32_t i = 0;

	objectHandleStacks.indexOfNextAvailableHandle[0] = objectHandleStacks.lowestIndexOfClass[0] = 0;
	objectHandleStacks.indexOfNextAvailableHandle[1] = objectHandleStacks.lowestIndexOfClass[1] = TRC_CFG_NQUEUE;
	objectHandleStacks.indexOfNextAvailableHandle[2] = objectHandleStacks.lowestIndexOfClass[2] = TRC_CFG_NQUEUE + TRC_CFG_NTASK;
	objectHandleStacks.indexOfNextAvailableHandle[3] = objectHandleStacks.lowestIndexOfClass[3] = TRC_CFG_NQUEUE + TRC_CFG_NTASK + TRC_CFG_NISR;
	objectHandleStacks.indexOfNextAvailableHandle[4] = objectHandleStacks.lowestIndexOfClass[4] = TRC_CFG_NQUEUE + TRC_CFG_NTASK + TRC_CFG_NISR + TRC_CFG_NTIMER;

	objectHandleStacks.highestIndexOfClass[0] = TRC_CFG_NQUEUE - 1;
	objectHandleStacks.highestIndexOfClass[1] = TRC_CFG_NQUEUE + TRC_CFG_NTASK - 1;
	objectHandleStacks.highestIndexOfClass[2] = TRC_CFG_NQUEUE + TRC_CFG_NTASK + TRC_CFG_NISR - 1;
	objectHandleStacks.highestIndexOfClass[3] = TRC_CFG_NQUEUE + TRC_CFG_NTASK + TRC_CFG_NISR + TRC_CFG_NTIMER - 1;
	objectHandleStacks.highestIndexOfClass[4] = TRC_CFG_NQUEUE + TRC_CFG_NTASK + TRC_CFG_NISR + TRC_CFG_NTIMER + TRC_CFG_NEVENTGROUP - 1;

	for (i = 0; i < TRACE_NCLASSES; i++)
	{
		objectHandleStacks.handleCountWaterMarksOfClass[i] = 0;
	}

	for (i = 0; i < TRACE_KERNEL_OBJECT_COUNT; i++)
	{
		objectHandleStacks.objectHandles[i] = 0;
	}
}

/* Returns the "Not enough handles" error message for this object class */
const char* pszTraceGetErrorNotEnoughHandles(traceObjectClass objectclass)
{
	switch(objectclass)
	{
	case TRACE_CLASS_TASK:
		return "Not enough TASK handles - increase TRC_CFG_NTASK in trcSnapshotConfig.h";
	case TRACE_CLASS_ISR:
		return "Not enough ISR handles - increase TRC_CFG_NISR in trcSnapshotConfig.h";
	case TRACE_CLASS_QUEUE:
		return "Not enough QUEUE handles - increase TRC_CFG_NQUEUE in trcSnapshotConfig.h";
	case TRACE_CLASS_TIMER:
		return "Not enough TIMER handles - increase TRC_CFG_NTIMER in trcSnapshotConfig.h";
	case TRACE_CLASS_EVENTGROUP:
		return "Not enough EVENTGROUP handles - increase TRC_CFG_NEVENTGROUP in trcSnapshotConfig.h";		
	default:
		return "pszTraceGetErrorHandles: Invalid objectclass!";
	}
}

#endif /* Snapshot mode */

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
