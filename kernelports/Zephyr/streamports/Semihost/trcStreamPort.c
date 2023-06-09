/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
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

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

TraceStreamPortFile_t* pxStreamPortFile TRC_CFG_RECORDER_DATA_ATTRIBUTE;

traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer)
{
	TRC_ASSERT_EQUAL_SIZE(TraceStreamPortBuffer_t, TraceStreamPortFile_t);

	TRC_ASSERT(pxBuffer != 0);

	pxStreamPortFile = (TraceStreamPortFile_t*)pxBuffer;
	pxStreamPortFile->pxFileDescriptor = -1;

#if (TRC_USE_INTERNAL_BUFFER == 1)
	return xTraceInternalEventBufferInitialize(pxStreamPortFile->buffer, sizeof(pxStreamPortFile->buffer));
#else
	return TRC_SUCCESS;
#endif
}

traceResult xTraceStreamPortOnTraceBegin(void)
{
	if (pxStreamPortFile == 0)
	{
		return TRC_FAIL;
	}
	
	if (pxStreamPortFile->pxFileDescriptor == -1)
	{
		pxStreamPortFile->pxFileDescriptor = semihost_open(TRC_CFG_STREAM_PORT_TRACE_FILE, SEMIHOST_OPEN_WB);
		if (pxStreamPortFile->pxFileDescriptor < 0)
		{
			printf("Could not open trace file, error code %ld.\n", pxStreamPortFile->pxFileDescriptor);
			return TRC_FAIL;
		}
		else
		{
			printf("Trace file created.\n");
		}
	}
	
	return TRC_SUCCESS;
}

traceResult xTraceStreamPortOnTraceEnd(void)
{
	if (pxStreamPortFile == 0)
	{
		return TRC_FAIL;
	}
	
	if (pxStreamPortFile->pxFileDescriptor > -1)
	{
		semihost_close(pxStreamPortFile->pxFileDescriptor);
		pxStreamPortFile->pxFileDescriptor = -1;
		printf("Trace file closed.\n");
	}
	
	return TRC_SUCCESS;
}

traceResult xTraceStreamPortWriteData(void* pvData, uint32_t uiSize, int32_t* piBytesWritten)
{
	if (pxStreamPortFile != 0 && pxStreamPortFile->pxFileDescriptor > -1)
	{
		long result = semihost_write(pxStreamPortFile->pxFileDescriptor, pvData, uiSize);
		if (result == 0)
		{
			*piBytesWritten = uiSize;
			return TRC_SUCCESS;
		}
		else {
			*piBytesWritten = 0;
			return TRC_FAIL;
		}
	}

	return TRC_FAIL;
}

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
