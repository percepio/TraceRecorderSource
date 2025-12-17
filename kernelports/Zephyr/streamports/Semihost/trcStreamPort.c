/*
 * Trace Recorder for Tracealyzer v4.11.0
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Supporting functions for trace streaming, used by the "stream ports" 
 * for reading and writing data to the interface.
 * Existing ports can easily be modified to fit another setup, e.g., a 
 * different TCP/IP stack, or to define your own stream port.
 */

#include <trcRecorder.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

TraceStreamPortBuffer_t* pxStreamPortFile TRC_CFG_RECORDER_DATA_ATTRIBUTE;

#define TRC_STREAM_PORT_FILE_NAME_MAX_LENGTH 18

static traceResult prvTraceStreamPortGenerateFileName(char acTraceFileName[], int uiBufferLength, int uiChannel);
static void prvTraceStreamPortFileClose(int uiChannel);
static traceResult prvTraceStreamPortFileOpen(char acTraceFileName[], int uiChannel);

static traceResult prvTraceStreamPortGenerateFileName(char acTraceFileName[], int uiBufferLength, int uiChannel)
{
	int i;
	int foundDot = -1;

#if (TRC_CFG_CORE_COUNT > 1)
#if (TRC_CFG_CORE_COUNT >= 10)
#error "Core count must be less than 10 for single digit core numbers in trace file names. Modify this to support more cores."
#endif
	/* Find the last instance of '.' */
	for (i = 0; i < uiBufferLength; i++)
	{
		if (TRC_CFG_STREAM_PORT_TRACE_FILE[i] == '.')
		{
			foundDot = i;
		}
		if (TRC_CFG_STREAM_PORT_TRACE_FILE[i] == '\0')
		{
			break; // Continue until we hit the null terminator
		}
	}

	if (foundDot < 0)
	{
		/* No dot found, set it to end */
		foundDot = i;
	}
	
	if (foundDot > uiBufferLength - 6)
	{
		/* Not enough space to insert core number, truncate */
		foundDot = uiBufferLength - 6;
	}

	for (i = 0; i < foundDot; i++)
	{
		acTraceFileName[i] = TRC_CFG_STREAM_PORT_TRACE_FILE[i];
	}

	acTraceFileName[foundDot] = (char)((int)'0' + uiChannel); // Assuming core number is 0, 1, 2, etc.
	acTraceFileName[foundDot+1] = '.';
	acTraceFileName[foundDot+2] = 'p';
	acTraceFileName[foundDot+3] = 's';
	acTraceFileName[foundDot+4] = 'f';
	acTraceFileName[foundDot+5] = '\0';
#else
	for (i = 0; i < uiBufferLength; i++)
	{
		acTraceFileName[i] = TRC_CFG_STREAM_PORT_TRACE_FILE[i];

		if (acTraceFileName[i] == '\0')
		{
			break; // Continue until we hit the null terminator
		}
	}
#endif

	return TRC_SUCCESS;
}

static void prvTraceStreamPortFileClose(int uiChannel)
{
	/* Make sure previous trace file handles are closed */
	if (pxStreamPortFile->pxFileDescriptor[uiChannel] > -1)
	{
		semihost_close(pxStreamPortFile->pxFileDescriptor[uiChannel]);
		pxStreamPortFile->pxFileDescriptor[uiChannel] = -1;
		printf("Trace file closed.\n");
	}
}

static traceResult prvTraceStreamPortFileOpen(char acTraceFileName[], int uiChannel)
{
	if (pxStreamPortFile->pxFileDescriptor[uiChannel] != -1)
		return TRC_SUCCESS; // File already open

	pxStreamPortFile->pxFileDescriptor[uiChannel] = semihost_open(acTraceFileName, SEMIHOST_OPEN_WB);
	if (pxStreamPortFile->pxFileDescriptor[uiChannel] < 0)
	{
		printf("Could not open trace file, error code %ld.\n", pxStreamPortFile->pxFileDescriptor[uiChannel]);
		return TRC_FAIL;
	}

	printf("Trace file created.\n");

	return TRC_SUCCESS;
}

traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer)
{
	int i;

	TRC_ASSERT(pxBuffer != 0);

	pxStreamPortFile = pxBuffer;
	for (i = 0; i < TRC_CFG_CORE_COUNT; i++)
	{
		pxStreamPortFile->pxFileDescriptor[i] = -1; // Initialize all file descriptors to -1
	}

	return TRC_SUCCESS;
}

traceResult xTraceStreamPortOnTraceBegin(void)
{

	int i;
	char acTraceFileName[TRC_STREAM_PORT_FILE_NAME_MAX_LENGTH] = { 0 };

	if (pxStreamPortFile == 0)
	{
		return TRC_FAIL;
	}

	for (i = 0; i < TRC_CFG_CORE_COUNT; i++)
	{
		prvTraceStreamPortFileClose(i);

		prvTraceStreamPortGenerateFileName(acTraceFileName, sizeof(acTraceFileName), i);

		if (prvTraceStreamPortFileOpen(acTraceFileName, i) == TRC_FAIL)
		{
			return TRC_FAIL;
		}
	}
	
	return TRC_SUCCESS;
}

traceResult xTraceStreamPortOnTraceEnd(void)
{
	int i;

	if (pxStreamPortFile == 0)
	{
		return TRC_FAIL;
	}

	for (i = 0; i < TRC_CFG_CORE_COUNT; i++)
	{
		if (pxStreamPortFile->pxFileDescriptor[i] > -1)
		{
			semihost_close(pxStreamPortFile->pxFileDescriptor[i]);
			pxStreamPortFile->pxFileDescriptor[i] = -1;
			printf("Trace file closed.\n");
		}
	}

	return TRC_SUCCESS;
}

traceResult xTraceStreamPortWriteData(void* pvData, uint32_t uiSize, uint32_t uiChannel, int32_t* piBytesWritten)
{
	long result;

	if (pxStreamPortFile == 0)
	{
		return TRC_FAIL;
	}
	
	if (pxStreamPortFile->pxFileDescriptor[uiChannel] < )
	{
		return TRC_FAIL;
	}

	result = semihost_write(pxStreamPortFile->pxFileDescriptor[uiChannel], pvData, uiSize);
	if (result == 0)
	{
		*piBytesWritten = uiSize;
		return TRC_SUCCESS;
	}

	*piBytesWritten = 0;
	return TRC_FAIL;
}

#endif
