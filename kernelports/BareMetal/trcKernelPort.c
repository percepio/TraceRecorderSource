/*
 * Trace Recorder for Tracealyzer v4.5.1
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * For bare-metal use of Tracealyzer (no RTOS)
 */

#include <stdio.h>
#include <stdint.h>
#include "trcRecorder.h"
#include "trcInternalBuffer.h"

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
	
/* Monitored by vTracePeriodicControl, producing warnings if needed. */
extern volatile uint32_t NoRoomForSymbol;
extern volatile uint32_t NoRoomForObjectData;
extern volatile uint32_t LongestSymbolName;
extern volatile uint32_t MaxBytesTruncated;

/* Keeps track of previous values, to only react on changes. */
static uint32_t NoRoomForSymbol_last = 0;
static uint32_t NoRoomForObjectData_last = 0;
static uint32_t LongestSymbolName_last = 0;
static uint32_t MaxBytesTruncated_last = 0;

/* User Event Channel for giving warnings regarding NoRoomForSymbol etc. */
traceString trcWarningChannel;

#define TRC_PORT_MALLOC(size) malloc(size)

TRC_STREAM_PORT_ALLOCATE_FIELDS()

/* Called by TzCtrl task periodically (Normally every 100 ms) */
static void prvCheckRecorderStatus(void);

extern void prvTraceWarning(int errCode);

int recorderInitialized = 0;

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
	extern uint32_t RecorderEnabled;
	TracealyzerCommandType msg;

	/* Only do this first time...*/
	if (recorderInitialized == 0)
	{
		TRC_STREAM_PORT_INIT();
		
		/* Note: Requires that TRC_CFG_INCLUDE_USER_EVENTS is 1. */
		trcWarningChannel = xTraceRegisterString("Warnings from Recorder");
		
		recorderInitialized = 1;
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
	else
	{
		/* On TRC_INIT */
		TRC_PORT_SPECIFIC_INIT();
	}
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
	if (NoRoomForSymbol > NoRoomForSymbol_last)
	{
		if (NoRoomForSymbol > 0)
		{
			prvTraceWarning(PSF_WARNING_SYMBOL_TABLE_SLOTS);
		}
		NoRoomForSymbol_last = NoRoomForSymbol;
	}

	if (NoRoomForObjectData > NoRoomForObjectData_last)
	{
		if (NoRoomForObjectData > 0)
		{
			prvTraceWarning(PSF_WARNING_OBJECT_DATA_SLOTS);
		}
		NoRoomForObjectData_last = NoRoomForObjectData;
	}

	if (LongestSymbolName > LongestSymbolName_last)
	{
		if (LongestSymbolName > TRC_CFG_SYMBOL_MAX_LENGTH)
		{
			prvTraceWarning(PSF_WARNING_SYMBOL_MAX_LENGTH);
		}
		LongestSymbolName_last = LongestSymbolName;
	}

	if (MaxBytesTruncated > MaxBytesTruncated_last)
	{
		if (MaxBytesTruncated > 0)
		{
			prvTraceWarning(PSF_WARNING_STRING_TOO_LONG);
		}
		MaxBytesTruncated_last = MaxBytesTruncated;
	}
}

/************************************************************************
* vTracePeriodicControl
* 
* This function should be called periodically. It has two jobs:
*
* - Read and perform start/stop commands from host (Tracealyzer).
*
* - Flush the trace buffer to the streaming interface (in case of 
*   streaming via the internal buffer, i.e. if 
*   TRC_STREAM_PORT_USE_INTERNAL_BUFFER is 1.
************************************************************************/
void vTracePeriodicControl( void )
{
	TracealyzerCommandType msg;
	int32_t bytes = 0;
	int32_t status = 0;

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
	Otherwise, step out of this loop and return. */		
		
	} while (bytes != 0);

	prvCheckRecorderStatus();

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
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[3] = TRC_CFG_NTASK;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[4] = TRC_CFG_NISR;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[5] = TRC_CFG_NTIMER;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[6] = TRC_CFG_NEVENTGROUP;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[7] = TRC_CFG_NSTREAMBUFFER;
	RecorderDataPtr->ObjectPropertyTable.NumberOfObjectsPerClass[8] = TRC_CFG_NMESSAGEBUFFER;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[0] = TRC_CFG_NAME_LEN_QUEUE;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[1] = TRC_CFG_NAME_LEN_SEMAPHORE;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[2] = TRC_CFG_NAME_LEN_MUTEX;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[3] = TRC_CFG_NAME_LEN_TASK;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[4] = TRC_CFG_NAME_LEN_ISR;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[5] = TRC_CFG_NAME_LEN_TIMER;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[6] = TRC_CFG_NAME_LEN_EVENTGROUP;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[7] = TRC_CFG_NAME_LEN_STREAMBUFFER;
	RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[8] = TRC_CFG_NAME_LEN_MESSAGEBUFFER;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[0] = PropertyTableSizeQueue;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[1] = PropertyTableSizeSemaphore;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[2] = PropertyTableSizeMutex;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[3] = PropertyTableSizeTask;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[4] = PropertyTableSizeISR;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[5] = PropertyTableSizeTimer;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[6] = PropertyTableSizeEventGroup;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[7] = PropertyTableSizeStreamBuffer;
	RecorderDataPtr->ObjectPropertyTable.TotalPropertyBytesPerClass[8] = PropertyTableSizeMessageBuffer;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[0] = StartIndexQueue;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[1] = StartIndexSemaphore;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[2] = StartIndexMutex;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[3] = StartIndexTask;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[4] = StartIndexISR;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[5] = StartIndexTimer;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[6] = StartIndexEventGroup;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[7] = StartIndexStreamBuffer;
	RecorderDataPtr->ObjectPropertyTable.StartIndexOfClass[8] = StartIndexMessageBuffer;
	RecorderDataPtr->ObjectPropertyTable.ObjectPropertyTableSizeInBytes = TRACE_OBJECT_TABLE_SIZE;
}

/* Initialization of the handle mechanism, see e.g, prvTraceGetObjectHandle */
void vTraceInitObjectHandleStack()
{
	objectHandleStacks.indexOfNextAvailableHandle[0] = objectHandleStacks.lowestIndexOfClass[0] = 0;
	objectHandleStacks.indexOfNextAvailableHandle[1] = objectHandleStacks.lowestIndexOfClass[1] = TRC_CFG_NQUEUE;
	objectHandleStacks.indexOfNextAvailableHandle[2] = objectHandleStacks.lowestIndexOfClass[2] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE;
	objectHandleStacks.indexOfNextAvailableHandle[3] = objectHandleStacks.lowestIndexOfClass[3] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX;
	objectHandleStacks.indexOfNextAvailableHandle[4] = objectHandleStacks.lowestIndexOfClass[4] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX + TRC_CFG_NTASK;
	objectHandleStacks.indexOfNextAvailableHandle[5] = objectHandleStacks.lowestIndexOfClass[5] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX + TRC_CFG_NTASK + TRC_CFG_NISR;
	objectHandleStacks.indexOfNextAvailableHandle[6] = objectHandleStacks.lowestIndexOfClass[6] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX + TRC_CFG_NTASK + TRC_CFG_NISR + TRC_CFG_NTIMER;
	objectHandleStacks.indexOfNextAvailableHandle[7] = objectHandleStacks.lowestIndexOfClass[7] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX + TRC_CFG_NTASK + TRC_CFG_NISR + TRC_CFG_NTIMER + TRC_CFG_NEVENTGROUP;
	objectHandleStacks.indexOfNextAvailableHandle[8] = objectHandleStacks.lowestIndexOfClass[8] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX + TRC_CFG_NTASK + TRC_CFG_NISR + TRC_CFG_NTIMER + TRC_CFG_NEVENTGROUP + TRC_CFG_NSTREAMBUFFER;

	objectHandleStacks.highestIndexOfClass[0] = TRC_CFG_NQUEUE - 1;
	objectHandleStacks.highestIndexOfClass[1] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE - 1;
	objectHandleStacks.highestIndexOfClass[2] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX - 1;
	objectHandleStacks.highestIndexOfClass[3] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX + TRC_CFG_NTASK - 1;
	objectHandleStacks.highestIndexOfClass[4] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX + TRC_CFG_NTASK + TRC_CFG_NISR - 1;
	objectHandleStacks.highestIndexOfClass[5] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX + TRC_CFG_NTASK + TRC_CFG_NISR + TRC_CFG_NTIMER - 1;
	objectHandleStacks.highestIndexOfClass[6] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX + TRC_CFG_NTASK + TRC_CFG_NISR + TRC_CFG_NTIMER + TRC_CFG_NEVENTGROUP - 1;
	objectHandleStacks.highestIndexOfClass[7] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX + TRC_CFG_NTASK + TRC_CFG_NISR + TRC_CFG_NTIMER + TRC_CFG_NEVENTGROUP + TRC_CFG_NSTREAMBUFFER - 1;
	objectHandleStacks.highestIndexOfClass[8] = TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX + TRC_CFG_NTASK + TRC_CFG_NISR + TRC_CFG_NTIMER + TRC_CFG_NEVENTGROUP + TRC_CFG_NSTREAMBUFFER + TRC_CFG_NMESSAGEBUFFER - 1;
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
	case TRACE_CLASS_SEMAPHORE:
		return "Not enough SEMAPHORE handles - increase TRC_CFG_NSEMAPHORE in trcSnapshotConfig.h";
	case TRACE_CLASS_MUTEX:
		return "Not enough MUTEX handles - increase TRC_CFG_NMUTEX in trcSnapshotConfig.h";
	case TRACE_CLASS_QUEUE:
		return "Not enough QUEUE handles - increase TRC_CFG_NQUEUE in trcSnapshotConfig.h";
	case TRACE_CLASS_TIMER:
		return "Not enough TIMER handles - increase TRC_CFG_NTIMER in trcSnapshotConfig.h";
	case TRACE_CLASS_EVENTGROUP:
		return "Not enough EVENTGROUP handles - increase TRC_CFG_NEVENTGROUP in trcSnapshotConfig.h";
	case TRACE_CLASS_STREAMBUFFER:
		return "Not enough STREAMBUFFER handles - increase TRC_CFG_NSTREAMBUFFER in trcSnapshotConfig.h";
	case TRACE_CLASS_MESSAGEBUFFER:
		return "Not enough MESSAGEBUFFER handles - increase TRC_CFG_NMESSAGEBUFFER in trcSnapshotConfig.h";
	default:
		return "pszTraceGetErrorHandles: Invalid objectclass!";
	}
}

void vTraceEnable(int startOption)
{
	prvTraceInitTraceData();

	if (startOption == TRC_START)
	{
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

#endif /* Snapshot mode */