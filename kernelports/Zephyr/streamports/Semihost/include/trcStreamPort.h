/*
 * Trace Recorder for Tracealyzer v4.11.0
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The interface definitions for trace streaming ("stream ports").
 * This "stream port" sets up the recorder to stream the trace to file.
 */

#ifndef TRC_STREAM_PORT_H
#define TRC_STREAM_PORT_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#include <stdint.h>
#include <trcTypes.h>
#include <trcStreamPortConfig.h>
#include <stdio.h>
#include <zephyr/arch/common/semihost.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TraceStreamPortBuffer
{
	long pxFileDescriptor[TRC_CFG_CORE_COUNT]; /**< File descriptor for each core */
} TraceStreamPortBuffer_t;

/**
 * @internal Stream port initialize callback.
 *
 * This function is called by the recorder as part of its initialization phase.
 *
 * @param[in] pxBuffer Buffer
 *
 * @retval TRC_FAIL Initialization failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortInitialize(TraceStreamPortBuffer_t* pxBuffer);

/**
 * @brief Writes data through the stream port interface.
 *
 * @param[in] pvData Data to write
 * @param[in] uiSize Data to write size
 * @param[in] uiChannel Channel (0 for the first core, 1 for the second core, etc.)
 * @param[out] piBytesWritten Bytes written
 *
 * @retval TRC_FAIL Write failed
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceStreamPortWriteData(void* pvData, uint32_t uiSize, uint32_t uiChannel, int32_t* piBytesWritten);

#define xTraceStreamPortReadData(pvData, uiSize, piBytesRead) ((void)(pvData), (void)(uiSize), (void)(piBytesRead), TRC_SUCCESS)

#define xTraceStreamPortOnEnable(uiStartOption) ((void)(uiStartOption), TRC_SUCCESS)

#define xTraceStreamPortOnDisable() (TRC_SUCCESS)

traceResult xTraceStreamPortOnTraceBegin(void);

traceResult xTraceStreamPortOnTraceEnd(void);

#ifdef __cplusplus
}
#endif

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#endif /* TRC_STREAM_PORT_H */
