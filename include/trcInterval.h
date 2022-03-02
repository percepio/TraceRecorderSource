/*
* Percepio Trace Recorder for Tracealyzer v4.6.2
* Copyright 2021 Percepio AB
* www.percepio.com
*
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * @file 
 * 
 * @brief Public trace interval APIs.
 */

#ifndef TRC_INTERVAL_H
#define TRC_INTERVAL_H

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#ifdef __cplusplus
extern "C" {
#endif

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#include <trcTypes.h>

#define TRC_INTERVAL_CHANNEL_SET_INDEX 0

/**
 * @defgroup trace_interval_apis Trace Interval APIs
 * @ingroup trace_recorder_apis
 * @{
 */

/**
 * @brief Creates trace interval channel set.
 * 
 * @param[in] szName Name.
 * @param[out] pxIntervalChannelSetHandle Pointer to uninitialized trace interval channel set.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
	traceResult xTraceIntervalChannelSetCreate(const char* szName, TraceIntervalChannelSetHandle_t* pxIntervalChannelSetHandle);

/**
 * @brief Creates trace interval channel.
 * 
 * @param[in] szName Name.
 * @param[in] xIntervalChannelSetHandle Interval set that this channel belongs to.
 * @param[out] pxIntervalChannelHandle Pointer to uninitialized trace interval channel.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceIntervalChannelCreate(const char *szName, TraceIntervalChannelSetHandle_t xIntervalChannelSetHandle, TraceIntervalChannelHandle_t *pxIntervalChannelHandle);

/**
 * @brief Starts trace interval instance.
 * 
 * @param[in] xIntervalChannelHandle Interval handle.
 * @param[in] uxValue Value that can be used to tell instances apart.
 * @param[out] pxIntervalInstanceHandle Pointer to interval instance variable.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceIntervalStart(TraceIntervalChannelHandle_t xIntervalChannelHandle, TraceUnsignedBaseType_t uxValue, TraceIntervalInstanceHandle_t* pxIntervalInstanceHandle);

/**
 * @brief Stops trace interval instance.
 * 
 * @param[in] xIntervalChannelHandle Interval handle.
 * @param[in] xIntervalInstanceHandle Interval instance.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceIntervalStop(TraceIntervalChannelHandle_t xIntervalChannelHandle, TraceIntervalInstanceHandle_t xIntervalInstanceHandle);

/**
 * @brief Gets trace interval channel state.
 * 
 * @param[in] xIntervalChannelHandle Pointer to initialized trace interval.
 * @param[out] puxState State.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceIntervalGetState(xIntervalChannelHandle, puxState) xTraceEntryGetState((TraceEntryHandle_t)(xIntervalChannelHandle), TRC_INTERVAL_CHANNEL_SET_INDEX, puxState)

/** @} */

#else

/**
 * @brief Disabled by TRC_CFG_RECORDER_MODE
 */
#define xTraceIntervalCreate(szName, pxIntervalHandle) ((void)(szName), *(pxIntervalHandle) = 0)

/**
 * @brief Disabled by TRC_CFG_RECORDER_MODE
 */
#define xTraceIntervalStart(xIntervalHandle, uxValue, pxIntervalInstanceHandle) ((void)(xIntervalHandle), (void)(uxValue), *(pxIntervalInstanceHandle) = 0)

 /**
  * @brief Disabled by TRC_CFG_RECORDER_MODE
  */
#define xTraceIntervalStop(xIntervalHandle, xIntervalInstanceHandle) ((void)(xIntervalHandle), (void)(xIntervalInstanceHandle))

/**
 * @brief Disabled by TRC_CFG_RECORDER_MODE
 */
#define xTraceIntervalGetState(xIntervalHandle, puxState) ((void)(xIntervalHandle), *(puxState) = 0)

#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
