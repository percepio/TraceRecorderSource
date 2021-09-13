/*
 * Trace Recorder for Tracealyzer v4.5.1
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This stream port provides trace streaming using ESP-IDF apptrace
 * which depends on Xtensa TRAX for background memory access.
 */

#include "trcRecorder.h"
#include "esp_app_trace.h"

extern IRAM_ATTR bool spi_flash_cache_enabled(void);

portMUX_TYPE trax_mutex = portMUX_INITIALIZER_UNLOCKED;


#if (TRC_USE_TRACEALYZER_RECORDER == 1)
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#if (TRC_STREAM_PORT_USE_INTERNAL_BUFFER)
char _TzIntBuf[(TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT) * (TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE)] __attribute__((aligned (4)));
#endif

void initTrax() {
	while (!esp_apptrace_host_is_connected(ESP_APPTRACE_DEST_TRAX))  {}
}

int readFromTrax(void* ptrData, uint32_t size, int32_t* ptrBytesRead)
{
	esp_apptrace_read(ESP_APPTRACE_DEST_TRAX, ptrData, (uint32_t*)ptrBytesRead, 0);

	return 0;
}

int writeToTrax(void* ptrData, uint32_t size, int32_t* ptrBytesWritten)
{
	esp_err_t err = ESP_OK;

	portENTER_CRITICAL_SAFE(&trax_mutex);
	if (spi_flash_cache_enabled()) {
#if TRC_CFG_APPTRACE_TRAX_BLOCKING_MODE == 1
	err = esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, ptrData, size, ESP_APPTRACE_TMO_INFINITE);
#else
	err = esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, ptrData, size, 0);
#endif
	}
	portEXIT_CRITICAL_SAFE(&trax_mutex);

	if (err == ESP_OK) {
		*ptrBytesWritten = size;
	} else {
		*ptrBytesWritten = 0;
	}

	return 0;
}

#endif
#endif
