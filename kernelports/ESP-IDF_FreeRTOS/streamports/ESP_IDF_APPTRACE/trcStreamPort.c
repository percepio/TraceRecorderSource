/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This stream port provides trace streaming using ESP-IDF apptrace
 * which depends on Xtensa TRAX for background memory access.
 */

#include <trcRecorder.h>
#include <esp_app_trace.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

/* The data structure for commands (a bit overkill) */
typedef struct TraceCommand
{
	unsigned char cmdCode;
	unsigned char param1;
	unsigned char param2;
	unsigned char param3;
	unsigned char param4;
	unsigned char param5;
	unsigned char checksumLSB;
	unsigned char checksumMSB;
} TraceCommand_t;

extern IRAM_ATTR bool spi_flash_cache_enabled(void);

static TraceStreamPortBuffer_t* pxStreamPortRTT TRC_CFG_RECORDER_DATA_ATTRIBUTE;

static portMUX_TYPE trax_mutex = portMUX_INITIALIZER_UNLOCKED TRC_CFG_RECORDER_DATA_ATTRIBUTE;

static uint32_t uiConnected = 0UL;

traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer)
{
	TRC_ASSERT_EQUAL_SIZE(TraceStreamPortBuffer_t, TraceStreamPortRTT_t);

	if (pxBuffer == 0)
	{
		return TRC_FAIL;
	}

	pxStreamPortRTT = (TraceStreamPortBuffer_t*)pxBuffer;

#if (TRC_USE_INTERNAL_BUFFER == 1)
	return xTraceInternalEventBufferInitialize(pxStreamPortRTT->bufferInternal, sizeof(pxStreamPortRTT->bufferInternal));
#else
	return TRC_SUCCESS;
#endif
}

traceResult xTraceStreamPortWriteData(void* pvData, uint32_t uiSize, int32_t* piBytesWritten)
{
	esp_err_t err = ESP_OK;

	*piBytesWritten = 0;
	
	if (!esp_apptrace_host_is_connected(ESP_APPTRACE_DEST_TRAX))
	{
		return TRC_FAIL;
	}

	portENTER_CRITICAL_SAFE(&trax_mutex);
	if (spi_flash_cache_enabled())
	{
#if TRC_CFG_APPTRACE_TRAX_BLOCKING_MODE == 1
		err = esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, pvData, uiSize, ESP_APPTRACE_TMO_INFINITE);
#else
		err = esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, pvData, uiSize, 0);
#endif
	}
	portEXIT_CRITICAL_SAFE(&trax_mutex);

	if (err != ESP_OK)
	{
		return TRC_FAIL;
	}

	*piBytesWritten = uiSize;

	return TRC_SUCCESS;
}

traceResult xTraceStreamPortReadData(void* pvData, uint32_t uiSize, int32_t* piBytesRead)
{
	TraceCommand_t* msg = (TraceCommand_t*)pvData;
	uint16_t checksum;
	
	if (uiConnected == 0UL)
	{
		/* Not connected yet. Check for connection here. */
		if (esp_apptrace_host_is_connected(ESP_APPTRACE_DEST_TRAX))
		{
			/* We're connected. Fake a start command. */
			msg->cmdCode = CMD_SET_ACTIVE;
			msg->param1 = 1;
			msg->param2 = 0;
			msg->param3 = 0;
			msg->param4 = 0;
			msg->param5 = 0;
			checksum = (uint16_t)((uint16_t)0xFFFFUL - (uint16_t)(msg->cmdCode + msg->param1 + msg->param2 + msg->param3 + msg->param4 + msg->param5));
			msg->checksumLSB = (unsigned char)(checksum & 0xFF);
			msg->checksumMSB = (unsigned char)(checksum >> 8);
			*piBytesRead = sizeof(TraceCommand_t);
			uiConnected = 1UL;
		}
	}
	else
	{
		/* Already connected. Check for lost connection here. */
		if (!esp_apptrace_host_is_connected(ESP_APPTRACE_DEST_TRAX))
		{
			/* We're no longer connected. Fake a stop command. */
			msg->cmdCode = CMD_SET_ACTIVE;
			msg->param1 = 0;
			msg->param2 = 0;
			msg->param3 = 0;
			msg->param4 = 0;
			msg->param5 = 0;
			checksum = (uint16_t)((uint16_t)0xFFFFUL - (uint16_t)(msg->cmdCode + msg->param1 + msg->param2 + msg->param3 + msg->param4 + msg->param5));
			msg->checksumLSB = (unsigned char)(checksum & 0xFF);
			msg->checksumMSB = (unsigned char)(checksum >> 8);
			*piBytesRead = sizeof(TraceCommand_t);
			uiConnected = 0UL;
		}
	}
	/* This is never used, instead we monitor the connected state and fake a receive */
	/*esp_apptrace_read(ESP_APPTRACE_DEST_TRAX, ptrData, (uint32_t*)ptrBytesRead, 0);*/

	return TRC_SUCCESS;
}

traceResult xTraceStreamPortOnEnable(uint32_t uiStartOption)
{
	if (uiStartOption == TRC_START)
	{
		uiConnected = 1UL;
	}

	return TRC_SUCCESS;
}

#endif

#endif
