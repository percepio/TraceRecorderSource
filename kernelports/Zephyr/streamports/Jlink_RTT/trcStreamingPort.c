/*
 * Trace Recorder for Tracealyzer v4.5.0b
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
	uint32_t bytesWritten;
	
	/* Some trace hooks are called via the PendSV interrupt which causes issues
	 * for RTT since mutex locks (rtt_term_mutex) are not allowed to lock/unlock
	 * in an ISR state. To avoid this issue we perform the ISR check here and if
	 * we are in an ISR we can use a RTT write with no locks. Note: there might be
	 * cases where the PendSV is interrupted but it seems that all other interrupts
	 * are disabled during this call.
	 * 
	 * While the mutex issue is a non-problem in most cases and is ignored by
	 * Zephyr, if CONFIG_ASSERT is set it will panic the kernel and stop the
	 * program for many essential trace hooks.
	 */
	if (k_is_in_isr()) {
		bytesWritten = SEGGER_RTT_WriteNoLock((TRC_CFG_RTT_UP_BUFFER_INDEX), (const char*)ptrData, size);
	} else {
		bytesWritten = SEGGER_RTT_Write((TRC_CFG_RTT_UP_BUFFER_INDEX), (const char*)ptrData, size);
	}
	
	if (ptrBytesWritten != NULL)
		*ptrBytesWritten = (int32_t)bytesWritten;

	return 0;
}

#endif
#endif
