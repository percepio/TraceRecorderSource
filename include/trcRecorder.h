/*
 * Trace Recorder for Tracealyzer v4.11.0
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TRC_RECORDER_H
#define TRC_RECORDER_H

/**
 * @file 
 * 
 * @brief The public API of the Percepio trace recorder.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Trace Recorder APIs
 * @defgroup trace_recorder_apis Trace Recorder APIs
 * @{
 * @}
 */

#define TRC_ACKNOWLEDGED (0xABC99123)

#include <trcDefines.h>
#include <trcConfig.h>

#ifndef TRC_CFG_ENTRY_SLOTS
#error TRC_CFG_ENTRY_SLOTS not defined. Please ensure you are using updated config files.
#endif

#include <trcKernelPortConfig.h>

#include <trcTypes.h>

#ifndef TRC_CFG_TEST_MODE
#define TRC_CFG_TEST_MODE 0
#endif

/* Unless specified in trcConfig.h we assume this is a single core target */
#ifndef TRC_CFG_CORE_COUNT
#define TRC_CFG_CORE_COUNT 1
#endif

/* Unless specified in trcConfig.h we assume this is a single core target */
#ifndef TRC_CFG_GET_CURRENT_CORE
#define TRC_CFG_GET_CURRENT_CORE() 0
#endif

/* Unless specified in trcConfig.h or trcKernelPortConfig.h we assume
 * GCC statement expressions aren't supported. */
#ifndef TRC_CFG_USE_GCC_STATEMENT_EXPR
#define TRC_CFG_USE_GCC_STATEMENT_EXPR 0
#endif

/* Backwards compatibility */
#undef traceHandle
#define traceHandle TraceISRHandle_t

/* Maximum event size */
#define TRC_MAX_BLOB_SIZE (16UL * sizeof(TraceUnsignedBaseType_t))

/* Platform name length */
#define TRC_PLATFORM_CFG_LENGTH 8UL

/* Header size */
#define TRC_HEADER_BUFFER_SIZE (sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t) + (sizeof(char) * (TRC_PLATFORM_CFG_LENGTH)))

typedef struct TraceHeaderBuffer	/* Aligned */
{
	uint8_t buffer[TRC_HEADER_BUFFER_SIZE];
} TraceHeaderBuffer_t;

#include <trcHardwarePort.h>
#include <trcKernelPort.h>
#include <trcString.h>
#include <trcStaticBuffer.h>
#include <trcError.h>
#include <trcEvent.h>
#include <trcEventBuffer.h>
#include <trcMultiCoreEventBuffer.h>
#include <trcTimestamp.h>
#include <trcEntryTable.h>
#include <trcStreamPort.h>
#include <trcISR.h>
#include <trcTask.h>
#include <trcObject.h>
#include <trcPrint.h>
#include <trcHeap.h>
#include <trcExtension.h>
#include <trcUtility.h>
#include <trcStackMonitor.h>
#include <trcInternalEventBuffer.h>
#include <trcDiagnostics.h>
#include <trcAssert.h>
#include <trcRunnable.h>
#include <trcDependency.h>
#include <trcProcess.h>
#include <trcThread.h>
#include <trcInterval.h>
#include <trcStateMachine.h>
#include <trcCounter.h>
#include <trcTaskMonitor.h>

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

/**
 * @brief
 *
 * Initializes the recorder data.
 * xTraceInitialize() or xTraceEnable(...) must be called before any attempts
 * at adding trace data/information. xTraceInitialize() can be called before
 * timestamp source is initialized since timestamps aren't used until
 * xTraceEnable(...) is called.
 *
 * See xTraceEnable(...) for more information.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceInitialize(void);

/**
 * @brief
 *
 * This function enables tracing.
 * To use the trace recorder, the startup must call xTraceInitialize() or
 * xTraceEnable(...) before any RTOS calls are made (including "create" calls).
 * This function should only be called after the timestamp source is properly
 * initialized. If kernel services that generate trace data are called before that,
 * an earlier call to xTraceInitialize() is necessary. Then call xTraceEnable(...)
 * once the timestamp source is initialized.
 *
 * Three start options are provided:
 * 
 * 	TRC_START: Starts the tracing directly. In snapshot mode (RingBuffer) this
 *  allows for starting the trace at any point in your code, assuming
 *  xTraceInitialize() has been called in the startup. Can also be used for streaming
 *  without Tracealyzer control, e.g. to a local flash file system (assuming such a
 * 	"stream port", see trcStreamPort.h).
 * 
 * 	TRC_START_AWAIT_HOST: For streaming mode only. Initializes the trace recorder
 * 	if necessary and waits for a Start command from Tracealyzer ("Start Recording"
 * 	button). This call is intentionally blocking! By calling xTraceEnable with
 * 	this option from the startup code, you start tracing at this point and capture
 * 	the early events.
 *
 * 	TRC_START_FROM_HOST: For streaming mode only. Initializes the trace recorder
 * 	if necessary and creates a task that waits for a Start command from
 * 	Tracealyzer ("Start Recording" button). This call is not blocking.
 *
 * @example Usage examples
 * 
 * 	Snapshot trace (RingBuffer), from startup:
 * 		<board init>
 * 		xTraceEnable(TRC_START); // Will call xTraceInitialize()
 * 		<RTOS init>
 *
 * 	Snapshot trace (RingBuffer), from a later point:
 * 		<board init>
 * 		xTraceInitialize();
 * 		<RTOS init>
 * 		...
 * 		xTraceEnable(TRC_START); // e.g., in task context, at some relevant event
 *
 * 	Streaming trace, from startup (can only be used with certain stream ports):
 *		<board startup>
 *		xTraceInitialize();
 *		<RTOS startup>
 * 		xTraceEnable(TRC_START);
 * 
 * 	Streaming trace, from startup:
 *		<board init>	
 *		xTraceEnable(TRC_START_AWAIT_HOST); // Blocks!
 *		<RTOS init>
 *
 * 	Streaming trace, from a later point:
 *		<board startup>
 *		xTraceInitialize();
 *		<RTOS startup>
 * 		xTraceEnable(TRC_START);
 * 
 * 	Streaming trace, system executes normally until host starts tracing:
 *		<board startup>
 *		xTraceInitialize();
 *		<RTOS startup>
 *		xTraceEnable(TRC_START_FROM_HOST)
 * 
 * @param[in] uiStartOption Start option.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult	xTraceEnable(uint32_t uiStartOption);

/**
 * @brief Disables tracing.
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceDisable(void);

#ifndef TRC_EXTERNAL_BUFFERS
#define TRC_EXTERNAL_BUFFERS 0
#endif

typedef struct TraceRecorderData	/* Aligned */
{
	uint32_t uiSessionCounter;
	uint32_t uiRecorderEnabled;
	TraceUnsignedBaseType_t uxTraceSystemStates[TRC_CFG_CORE_COUNT];
	uint32_t reserved;								/* alignment */

	TraceAssertData_t xAssertBuffer;				/* aligned */
	TraceEntryIndexTable_t xEntryIndexTableBuffer;	/* aligned */
#if (TRC_EXTERNAL_BUFFERS == 0)
	TraceHeaderBuffer_t xHeaderBuffer;				/* aligned */
	TraceEntryTable_t xEntryTable;					/* aligned */
	TraceTimestampData_t xTimestampBuffer;			/* aligned */
#endif
#if (TRC_USE_INTERNAL_BUFFER == 1)
	TraceInternalEventBufferData_t xInternalEventBuffer;	/* aligned */
#endif
	TraceStreamPortBuffer_t xStreamPortBuffer;		/* verify alignment in xTraceInitialize() */
	TraceStaticBufferTable_t xStaticBufferBuffer;	/* aligned */
	TraceEventDataTable_t xEventDataBuffer;			/* verify alignment in xTraceInitialize() */
	TracePrintData_t xPrintBuffer;					/* aligned */
	TraceErrorData_t xErrorBuffer;					/* aligned */
	TraceISRData_t xISRBuffer;						/* aligned */
	TraceKernelPortDataBuffer_t xKernelPortBuffer;	/* verify alignment in xTraceInitialize() */
	TraceTaskData_t xTaskInfoBuffer;				/* aligned */
	TraceStackMonitorData_t xStackMonitorBuffer;	/* aligned */
	TraceDiagnosticsData_t xDiagnosticsBuffer;		/* aligned */
	TraceExtensionData_t xExtensionBuffer;			/* aligned */
	TraceCounterData_t xCounterBuffer;				/* aligned */
	TraceTaskMonitorData_t xTaskMonitorBuffer;		/* aligned */
} TraceRecorderData_t;

extern TraceRecorderData_t* pxTraceRecorderData;
extern uint32_t RecorderInitialized;

/**
 * @brief Initializes the header data
 * 
 * @param[in] pxBuffer Pointer to header buffer
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceHeaderInitialize(TraceHeaderBuffer_t* pxBuffer);

/**
 * @brief Query if recorder is enabled
 *
 * @retval Non-zero Recorder enabled
 * @retval 0 Recorder not enabled
 */
#define xTraceIsRecorderEnabled() (xTraceIsRecorderInitialized() && pxTraceRecorderData->uiRecorderEnabled)

/**
 * @brief Query if recorder initialized
 *
 * @retval Non-zero Recorder initialized
 * @retval 0 Recorder not initialized
 */
#define xTraceIsRecorderInitialized() xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_CORE)

/**
 * @brief Flag component as initialized
 * 
 * @param[in] uiComponentBit Component bit
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceSetComponentInitialized(uiComponentBit) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(RecorderInitialized |= (uiComponentBit), TRC_SUCCESS)

/**
 * @brief Query if component is initialized
 * 
 * @param[in] uiComponentBit Component bit
 * 
 * @retval Non-zero Component initialized
 * @retval 0 Component not initialized
 */
#define xTraceIsComponentInitialized(uiComponentBit) (RecorderInitialized & (uiComponentBit))

/**
 * @brief Set the trace state
 * 
 * @param[in] uiState State
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceStateSet(uxState) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(pxTraceRecorderData->uxTraceSystemStates[TRC_CFG_GET_CURRENT_CORE()] = (uxState), TRC_SUCCESS)

/**
 * @brief Query the trace state
 * 
 * @param[out] puiState State
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
#define xTraceStateGet(puxState) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(puxState) = pxTraceRecorderData->uxTraceSystemStates[TRC_CFG_GET_CURRENT_CORE()], TRC_SUCCESS)

/**
 * @brief Call this function periodically
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceTzCtrl(void);

/******************************************************************************/
/*** INTERNAL STREAMING FUNCTIONS *********************************************/
/******************************************************************************/

#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM)
/**
 * @brief Set the recorder data buffer
 * 
 * @param[in] pxBuffer Pointer to the recorder data buffer
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceSetBuffer(TraceRecorderData_t *pxBuffer);
#else
#define xTraceSetBuffer(p) (TRC_SUCCESS)
#endif

/**
 * @brief Retrieve the event buffer and event buffer size
 * 
 * @param[out] ppvBuffer Pointer where event buffer pointer will be written
 * @param[out] puiSize Event buffer size
 * 
 * @retval TRC_FAIL Failure
 * @retval TRC_SUCCESS Success
 */
traceResult xTraceGetEventBuffer(void** ppvBuffer, TraceUnsignedBaseType_t * puiSize);

#else /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#define xTraceInitialize() (TRC_SUCCESS)
#define xTraceEnable(x) ((void)(x), TRC_SUCCESS)
#define xTraceDisable() (TRC_SUCCESS)

#define prvTraceSetReadyEventsEnabled(status) (void)(status)

#define vTraceExcludeTask(handle) (void)(handle)

#define vTraceConsoleChannelPrintF(fmt, ...) (void)(fmt)

#ifndef TRC_ALLOC_CUSTOM_BUFFER
#define TRC_ALLOC_CUSTOM_BUFFER(bufname)
#endif

#define xTraceIsRecorderEnabled() (0)
#define xTraceIsRecorderInitialized() (0)

#define xTraceSetBuffer(p) (TRC_SUCCESS)
#define xTraceGetEventBuffer(p) (TRC_FAIL)

#define vTraceSetStopHook(x) (void)(x)

#define TraceRecorderData_t uint32_t

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

/**
 * @deprecated Backwards compatibility. Use xTraceInitialize instead.
 */
#define vTraceInitialize (void)xTraceInitialize

/**
 * @deprecated Backwards compatibility. Use xTraceEnable instead.
 */
#define vTraceEnable (void)xTraceEnable

/**
 * @deprecated Backwards compatibility. Use xTraceDisable instead.
 */
#define vTraceStop (void)xTraceDisable

/**
 * @deprecated Backwards compatibility. Use xTraceTaskInstanceFinishedNow instead.
 */
#define vTraceInstanceFinishedNow (void)xTraceTaskInstanceFinishedNow

/**
 * @deprecated Backwards compatibility. Use xTraceTaskInstanceFinishedNext instead.
 */
#define vTraceInstanceFinishedNext (void)xTraceTaskInstanceFinishedNext

/**
 * @deprecated Backwards compatibility. Use xTracePrintF instead.
 */
#define vTracePrintF (void)xTracePrintF

/**
 * @deprecated Backwards compatibility. Use xTraceVPrintF instead.
 */
#define vTraceVPrintF (void)xTraceVPrintF

/**
 * @deprecated Backwards compatibility. Use xTracePrint instead.
 */
#define vTracePrint (void)xTracePrint

/**
 * @deprecated Backwards compatibility. Use xTraceSetBuffer instead.
 */
#define vTraceSetRecorderDataBuffer(pxBuffer) xTraceSetBuffer((TraceRecorderData_t*)(pxBuffer))

#ifdef __cplusplus
}
#endif

#endif
