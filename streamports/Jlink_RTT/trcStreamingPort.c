/*
 * Trace Recorder for Tracealyzer v4.5.0
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Supporting functions for trace streaming, used by the "stream ports" 
 * for reading and writing data to the interface.
 *
 * Note that this stream port is more complex than the typical case, since
 * the J-Link interface uses a separate RAM buffer in SEGGER_RTT.c, instead
 * of the default buffer included in the recorder core. The other stream ports 
 * offer more typical examples of how to define a custom streaming interface.
 */
 
#include "trcRecorder.h"

#if (TRC_USE_TRACEALYZER_RECORDER == 1)
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

int32_t readFromRTT(void* ptrData, uint32_t size, int32_t* ptrBytesRead)
{
	uint32_t bytesRead = 0; 
	
	if (SEGGER_RTT_HASDATA(TRC_CFG_RTT_DOWN_BUFFER_INDEX))
	{
		bytesRead = SEGGER_RTT_Read((TRC_CFG_RTT_DOWN_BUFFER_INDEX), (char*)ptrData, size);
	
		if (ptrBytesRead != NULL)
			*ptrBytesRead = (int32_t)bytesRead;

	}

	return 0;
}

int32_t writeToRTT(void* ptrData, uint32_t size, int32_t* ptrBytesWritten)
{
	uint32_t bytesWritten = SEGGER_RTT_Write((TRC_CFG_RTT_UP_BUFFER_INDEX), (const char*)ptrData, size);
	
	if (ptrBytesWritten != NULL)
		*ptrBytesWritten = (int32_t)bytesWritten;

	return 0;
}

#endif
#endif
