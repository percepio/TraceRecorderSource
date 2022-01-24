/*
 * Trace Recorder for Tracealyzer v4.6.0(RC1)
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The public API of the trace recorder.
 */

#ifndef TRC_RECORDER_H
#define TRC_RECORDER_H

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_ACKNOWLEDGED (0xABC99123)

#include <trcDefines.h>
#include <trcConfig.h>
#include <trcKernelPortConfig.h>
#include <trcTypes.h>

#ifndef TRC_CFG_DEBUG_EXPECT
#define TRC_CFG_DEBUG_EXPECT 0
#endif

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)
#include <trcSnapshotConfig.h>
#include <trcKernelPortSnapshotConfig.h>

/* Calls xTraceError if the _assert condition is false. For void functions,
where no return value is to be provided. */
#define TRC_ASSERT_VOID(_assert, _err) if (! (_assert)){ prvTraceError(_err); return; }

/* Calls xTraceError if the _assert condition is false. For non-void functions,
where a return value is to be provided. */
#define TRC_ASSERT_RET(_assert, _err, _return) if (! (_assert)){ prvTraceError(_err); return _return; }

typedef uint8_t traceUBChannel;
typedef uint8_t traceObjectClass;

#undef traceHandle
#if (TRC_CFG_USE_16BIT_OBJECT_HANDLES == 1)
typedef uint16_t traceHandle;
#else /* (TRC_CFG_USE_16BIT_OBJECT_HANDLES == 1) */
typedef uint8_t traceHandle;
#endif /* (TRC_CFG_USE_16BIT_OBJECT_HANDLES == 1) */
	
#include <trcHardwarePort.h>
#include <trcKernelPort.h>

/* Not yet available in snapshot mode */
#define vTraceConsoleChannelPrintF(fmt, ...) (void)
#define xTraceConsoleChannelPrintF(fmt, ...) (void)
#define prvTraceStoreEvent_None(...) 
#define prvTraceStoreEvent_Handle(...) 
#define prvTraceStoreEvent_Param(...) 
#define prvTraceStoreEvent_HandleParam(...) 
#define prvTraceStoreEvent_ParamParam(...) 
#define prvTraceStoreEvent_HandleParamParam(...) 
#define prvTraceStoreEvent_ParamParamParam(...) 

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT) */

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)
#include <trcStreamingConfig.h>
#include <trcKernelPortStreamingConfig.h>

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
typedef TraceISRHandle_t traceHandle;

/* Used to determine endian of data (big/little) */
#define TRACE_PSF_ENDIANESS_IDENTIFIER ((uint32_t)0x50534600)

#define TRC_MAX_BLOB_SIZE (16 * sizeof(uint32_t))

#define TRC_PLATFORM_CFG_LENGTH 8

#define TRC_HEADER_BUFFER_SIZE (sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + (sizeof(char) * (TRC_PLATFORM_CFG_LENGTH)) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t))

typedef struct TraceHeaderBuffer
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
#include <trcStateMachine.h>
#include <trcExtension.h>
#include <trcInterval.h>
#include <trcUtility.h>
#include <trcStackMonitor.h>
#include <trcInternalEventBuffer.h>
#include <trcDiagnostics.h>
#include <trcAssert.h>
#include <trcCounter.h>

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

/******************************************************************************/
/*** Common API - both Snapshot and Streaming mode ****************************/
/******************************************************************************/

/******************************************************************************
* xTraceInitialize
*
* Initializes the recorder data.
* See TRC_CFG_RECORDER_DATA_INIT in trcConfig.h.
******************************************************************************/
traceResult xTraceInitialize(void);

/******************************************************************************
* xTraceEnable(uint32_t uiStartOption);
*
* Initializes and optionally starts the trace, depending on the start option.
* To use the trace recorder, the startup must call xTraceEnable before any RTOS
* calls are made (including "create" calls). Three start options are provided:
* 
* TRC_START: Starts the tracing directly. In snapshot mode this allows for 
* starting the trace at any point in your code, assuming xTraceEnable(TRC_INIT)
* has been called in the startup.
* Can also be used for streaming without Tracealyzer control, e.g. to a local
* flash file system (assuming such a "stream port", see trcStreamPort.h).
* 
* TRC_START_AWAIT_HOST: For streaming mode only. Initializes the trace recorder
* if necessary and waits for a Start command from Tracealyzer ("Start Recording"
* button). This call is intentionally blocking! By calling xTraceEnable with
* this option from the startup code, you start tracing at this point and capture
* the early events.
*
* TRC_INIT: Initializes the trace recorder, but does not start the tracing.
* In snapshot mode, this must be followed by a xTraceEnable(TRC_START) sometime
* later.
*
* Usage examples:
* 
* Snapshot trace, from startup:
* 	<board init>
* 	xTraceEnable(TRC_START);
* 	<RTOS init>
*
* Snapshot trace, from a later point:
* 	<board init>
* 	xTraceEnable(TRC_INIT);
* 	<RTOS init>
* 	...
* 	xTraceEnable(TRC_START); // e.g., in task context, at some relevant event
* 
* Streaming trace, from startup:
*	<board init>	
*	xTraceEnable(TRC_START_AWAIT_HOST); // Blocks!
*	<RTOS init>
*
* Streaming trace, from a later point:
*	<board startup>
*	xTraceEnable(TRC_INIT);
*	<RTOS startup>
*	
******************************************************************************/
traceResult
	xTraceEnable(uint32_t uiStartOption);

traceResult xTraceDisable(void);

/*******************************************************************************
* vTraceSetFilterGroup
*
* Sets the "filter group" to assign when creating RTOS objects, such as tasks,
* queues, semaphores and mutexes. This together with vTraceSetFilterMask 
* allows you to control what events that are recorded, based on the 
* objects they refer to.
*
* There are 16 filter groups named FilterGroup0 .. FilterGroup15.
*
* Note: We don't recommend filtering out the Idle task, so make sure to call 
* vTraceSetFilterGroup just before initializing the RTOS, in order to assign
* such "default" objects to the right Filter Group (typically group 0).
*
* Example:
*  
*		// Assign tasks T1 to FilterGroup0 (default)
*		<Create Task T1>  
*
*		// Assign Q1 and Q2 to FilterGroup1
*		vTraceSetFilterGroup(FilterGroup1);
*		<Create Queue Q1> 
*		<Create Queue Q2>
*
*		// Assigns Q3 to FilterGroup2
*		vTraceSetFilterGroup(FilterGroup2);
*		<Create Queue Q3>
*
*		// Only include FilterGroup0 and FilterGroup2, exclude FilterGroup1 (Q1 and Q2) from the trace
*		vTraceSetFilterMask( FilterGroup0 | FilterGroup2 );
*
*		// Assign the default RTOS objects (e.g. Idle task) to FilterGroup0
*		vTraceSetFilterGroup(FilterGroup0);
*		<Start the RTOS scheduler>
*
* Note that you may define your own names for the filter groups using
* preprocessor definitions, to make the code easier to understand.
*
* Example:
*
*		#define BASE FilterGroup0
*		#define USB_EVENTS FilterGroup1
*		#define CAN_EVENTS FilterGroup2
*
* Note that filtering per event type (regardless of object) is also available
* in trcConfig.h.
******************************************************************************/
void vTraceSetFilterGroup(uint16_t filterGroup);

/******************************************************************************
* vTraceSetFilterMask
*
* Sets the "filter mask" that is used to filter the events by object. This can
* be used to reduce the trace data rate, i.e., if your streaming interface is
* a bottleneck or if you want longer snapshot traces without increasing the
* buffer size.
*
* Note: There are two kinds of filters in the recorder. The other filter type
* excludes all events of certain kinds (e.g., OS ticks). See trcConfig.h.
*
* The filtering is based on bitwise AND with the Filter Group ID, assigned
* to RTOS objects such as tasks, queues, semaphores and mutexes. 
* This together with vTraceSetFilterGroup allows you to control what
* events that are recorded, based on the objects they refer to.
*
* See example for vTraceSetFilterGroup.
******************************************************************************/
void vTraceSetFilterMask(uint16_t filterMask);

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)

#include <stdarg.h>

#define TRACE_GET_LOW16(value) ((uint16_t)((value) & 0x0000FFFF))
#define TRACE_GET_HIGH16(value) ((uint16_t)(((value) >> 16) & 0x0000FFFF))
#define TRACE_SET_LOW16(current, value)  (((current) & 0xFFFF0000) | (value))
#define TRACE_SET_HIGH16(current, value) (((current) & 0x0000FFFF) | (((uint32_t)(value)) << 16))

#if defined (TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)
void prvAddTaskToStackMonitor(void* task);
void prvRemoveTaskFromStackMonitor(void* task);
void prvReportStackUsage(void);
#else /* defined (TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */
#define prvAddTaskToStackMonitor(task) 
#define prvRemoveTaskFromStackMonitor(task) 
#define prvReportStackUsage() 
#endif /* defined (TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */

/******************************************************************************
* xTraceIsRecorderEnabled
*
* Returns true (1) if the recorder is enabled (i.e. is recording), otherwise 0.
******************************************************************************/
uint32_t xTraceIsRecorderEnabled(void);

/******************************************************************************
* xTraceIsRecorderInitialized
*
* Returns true (1) if the recorder is initialized.
******************************************************************************/
uint32_t xTraceIsRecorderInitialized(void);

/*******************************************************************************
 * xTraceTaskInstanceFinishedNow
 *
 * Creates an event that ends the current task instance at this very instant.
 * This makes the viewer to splits the current fragment at this point and begin
 * a new actor instance, even if no task-switch has occurred.
 *****************************************************************************/
traceResult xTraceTaskInstanceFinishedNow(void);

/*******************************************************************************
 * xTraceTaskInstanceFinishedNext
 *
 * Marks the current "task instance" as finished on the next kernel call.
 *
 * If that kernel call is blocking, the instance ends after the blocking event
 * and the corresponding return event is then the start of the next instance.
 * If the kernel call is not blocking, the viewer instead splits the current
 * fragment right before the kernel call, which makes this call the first event
 * of the next instance.
 *****************************************************************************/
traceResult xTraceTaskInstanceFinishedNext(void);

traceResult xTraceStringRegister(const char* label, TraceStringHandle_t* pxString);

/* DEPRECATED. Backwards compatibility */
TraceStringHandle_t xTraceRegisterString(const char* name);

/******************************************************************************
 * xTracePrintF
 *
 * Generates "User Events", with formatted text and data, similar to a "printf".
 * User Events can be used for very efficient logging from your application code.
 * It is very fast since the actual string formatting is done on the host side, 
 * when the trace is displayed. The execution time is just some microseconds on
 * a 32-bit MCU.
 *
 * User Events are shown as yellow labels in the main trace view of $PNAME.
 *
 * An advantage of User Events is that data can be plotted in the "User Event
 * Signal Plot" view, visualizing any data you log as User Events, discrete
 * states or control system signals (e.g. system inputs or outputs).
 *
 * You may group User Events into User Event Channels. The yellow User Event 
 * labels show the logged string, preceded by the channel name within brackets.
 * 
 * Example:
 *
 *  "[MyChannel] Hello World!"
 *
 * The User Event Channels are shown in the View Filter, which makes it easy to
 * select what User Events you wish to display. User Event Channels are created
 * using xTraceStringRegister().
 *
 * Example:
 *
 *	 TraceStringHandle_t adc_uechannel;
 *	 xTraceStringRegister("ADC User Events", &adc_uechannel);
 *	 ...
 *	 xTracePrintF(adc_uechannel,
 *				 "ADC channel %d: %d volts",
 *				 ch, adc_reading);
 *
 * The following format specifiers are supported in both modes:
 * %d - signed integer. 
 * %u - unsigned integer.
 * %X - hexadecimal, uppercase. 
 * %x - hexadecimal, lowercase.
 * %s - string (see comment below)
 *
 * For integer formats (%d, %u, %x, %X) you may also use width and padding.
 * If using -42 as data argument, two examples are:
 *    "%05d" -> "-0042"
 *     "%5d" -> "  -42".
 *
 * String arguments are supported in both snapshot and streaming, but in streaming
 * mode you need to use xTraceStringRegister and use the returned TraceStringHandle_t as
 * the argument. In snapshot you simply provide a char* as argument.
 *
 * Snapshot: xTracePrintF(myChn, "my string: %s", str);
 * Streaming: xTracePrintF(myChn, "my string: %s", strTraceString);
 * 
 * In snapshot mode you can specify 8-bit or 16-bit arguments to reduce RAM usage:
 *     %hd -> 16 bit (h) signed integer (d).
 *     %bu -> 8 bit (b) unsigned integer (u).
 *
 * However, in streaming mode all data arguments are assumed to be 32 bit wide. 
 * Width specifiers (e.g. %hd) are accepted but ignored (%hd treated like %d).
 *
 * The maximum event size also differs between the modes. In streaming this is
 * limited by a maximum payload size of 52 bytes, including format string and
 * data arguments. So if using one data argument, the format string is limited
 * to 48 byte, etc. If this is exceeded,  the format string is truncated and you
 * get a warning in Tracealyzer.
 *
 * In snapshot mode you are limited to maximum 15 arguments, that must not exceed
 * 32 bytes in total (not counting the format string). If exceeded, the recorder
 * logs an internal error (displayed when opening the trace) and stops recording. 
 ******************************************************************************/
#if (TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1)
traceResult xTracePrintF(TraceStringHandle_t chn, const char* fmt, ...);
#else
#define xTracePrintF(chn, fmt, ...) ((void)(chn), (void)(fmt), TRC_SUCCESS) /* Comma operator is used to avoid "unused variable" compiler warnings in a single statement */
#endif

 /******************************************************************************
  * xTraceVPrintF
  *
  * xTracePrintF variant that accepts a va_list.
  * See xTracePrintF documentation for further details.
  *
  ******************************************************************************/
#if (TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1)
traceResult xTraceVPrintF(TraceStringHandle_t eventLabel, const char* formatStr, va_list vl);
#else
#define xTraceVPrintF(chn, formatStr, vl) ((void)(chn), (void)(formatStr), (void)(vl), TRC_SUCCESS) /* Comma operator is used to avoid "unused variable" compiler warnings in a single statement */
#endif

  /******************************************************************************
  * xTracePrint
  *
  * A faster version of xTracePrintF, that only allows for logging a string.
  *
  * Example:
  *
  *	 TraceStringHandle_t chn;
  *	 xTraceStringRegister("MyChannel", &chn);
  *	 ...
  *	 xTracePrint(chn, "Hello World!");
  ******************************************************************************/
#if (TRC_CFG_SCHEDULING_ONLY == 0) && (TRC_CFG_INCLUDE_USER_EVENTS == 1)
traceResult xTracePrint(TraceStringHandle_t chn, const char* str);
#else
#define xTracePrint(chn, str) ((void)(chn), (void)(str), TRC_SUCCESS)
#endif

/******************************************************************************/
/*** Extended API for Snapshot mode *******************************************/
/******************************************************************************/

/******************************************************************************
* TRACE_STOP_HOOK - Hook Pointer Data Type
*
* Declares a data type for a call back function that will be invoked whenever
* the recorder is stopped.
*
* Snapshot mode only!
******************************************************************************/
typedef void(*TRACE_STOP_HOOK)(void);

/*******************************************************************************
* vTraceStopHookPtr
*
* Points to a call back function that is called from vTraceStop().
*
* Snapshot mode only!
******************************************************************************/
extern TRACE_STOP_HOOK vTraceStopHookPtr;

/*******************************************************************************
* vTraceSetStopHook
*
* Sets a function to be called when the recorder is stopped.
*
* Snapshot mode only!
******************************************************************************/
void vTraceSetStopHook(TRACE_STOP_HOOK stopHookFunction);

/*******************************************************************************
* vTraceClear
*
* Resets the recorder. Only necessary if a restart is desired - this is not
* needed in the startup initialization.
*
* Snapshot mode only!
******************************************************************************/
void vTraceClear(void);


/*****************************************************************************/
/*** INTERNAL SNAPSHOT FUNCTIONS *********************************************/
/*****************************************************************************/

#define TRC_UNUSED

#ifndef TRC_CFG_INCLUDE_OBJECT_DELETE
#define TRC_CFG_INCLUDE_OBJECT_DELETE 0
#endif

#ifndef TRC_CFG_INCLUDE_READY_EVENTS
#define TRC_CFG_INCLUDE_READY_EVENTS 1
#endif

#ifndef TRC_CFG_INCLUDE_OSTICK_EVENTS
#define TRC_CFG_INCLUDE_OSTICK_EVENTS 0
#endif

/* This macro will create a task in the object table */
#undef trcKERNEL_HOOKS_TASK_CREATE
#define trcKERNEL_HOOKS_TASK_CREATE(SERVICE, CLASS, pxTCB) \
	if ((pxTCB) != 0) \
	{ \
		TRACE_SET_OBJECT_NUMBER(TASK, pxTCB); \
		TRACE_SET_OBJECT_FILTER(TASK, pxTCB, CurrentFilterGroup); \
		prvTraceSetObjectName(TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB), TRACE_GET_TASK_NAME(pxTCB)); \
		prvTraceSetPriorityProperty(TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB), TRACE_GET_TASK_PRIORITY(pxTCB)); \
		if (TRACE_GET_OBJECT_FILTER(TASK, TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
			if (TRACE_GET_OBJECT_FILTER(TASK, pxTCB) & CurrentFilterMask) \
				prvTraceStoreKernelCall(SERVICE, TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB)); \
	} \
	else \
	{ \
		/* pxTCB is null */ \
		if (TRACE_GET_OBJECT_FILTER(TASK, TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		{ \
			prvTraceStoreKernelCall(SERVICE, TRACE_CLASS_TASK, 0); \
		} \
	}

/* This macro will remove the task and store it in the event buffer */
#undef trcKERNEL_HOOKS_TASK_DELETE
#define trcKERNEL_HOOKS_TASK_DELETE(SERVICE, SERVICE_NAME, SERVICE_PROP, pxTCB) \
	if (TRACE_GET_OBJECT_FILTER(TASK, TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		if (TRACE_GET_OBJECT_FILTER(TASK, pxTCB) & CurrentFilterMask) \
			prvTraceStoreKernelCall(SERVICE, TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB)); \
	prvTraceStoreObjectNameOnCloseEvent(SERVICE_NAME, TRACE_GET_TASK_NUMBER(pxTCB), TRACE_CLASS_TASK); \
	prvTraceStoreObjectPropertiesOnCloseEvent(SERVICE_PROP, TRACE_GET_TASK_NUMBER(pxTCB), TRACE_CLASS_TASK); \
	prvTraceSetPriorityProperty(TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB), TRACE_GET_TASK_PRIORITY(pxTCB)); \
	prvTraceSetObjectState(TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB), TASK_STATE_INSTANCE_NOT_ACTIVE); \
	prvTraceFreeObjectHandle(TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB));


/* This macro will setup a task in the object table */
#undef trcKERNEL_HOOKS_OBJECT_CREATE
#define trcKERNEL_HOOKS_OBJECT_CREATE(SERVICE, CLASS, pxObject) \
	TRACE_SET_OBJECT_NUMBER(CLASS, pxObject); \
	TRACE_SET_OBJECT_FILTER(CLASS, pxObject, CurrentFilterGroup); \
	prvMarkObjectAsUsed(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject),  TRACE_GET_OBJECT_NUMBER(CLASS, pxObject));\
	if (TRACE_GET_OBJECT_FILTER(TASK, TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		if (TRACE_GET_OBJECT_FILTER(CLASS, pxObject) & CurrentFilterMask) \
			prvTraceStoreKernelCall(SERVICE, TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject)); \
	prvTraceSetObjectState(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject), 0);

/* This macro will setup a task in the object table */
#undef trcKERNEL_HOOKS_OBJECT_CREATE_FAILED
#define trcKERNEL_HOOKS_OBJECT_CREATE_FAILED(SERVICE, TRACE_CLASS)\
	if (TRACE_GET_OBJECT_FILTER(TASK, TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		prvTraceStoreKernelCall(SERVICE, TRACE_CLASS, 0); \
	}

/* This macro will remove the object and store it in the event buffer */
#undef trcKERNEL_HOOKS_OBJECT_DELETE
#define trcKERNEL_HOOKS_OBJECT_DELETE(SERVICE, SERVICE_NAME, SERVICE_PROP, CLASS, pxObject) \
	if (TRACE_GET_OBJECT_FILTER(TASK, TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		if (TRACE_GET_OBJECT_FILTER(CLASS, pxObject) & CurrentFilterMask) \
			prvTraceStoreKernelCall(SERVICE, TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject)); \
	prvTraceStoreObjectNameOnCloseEvent(SERVICE_NAME, TRACE_GET_OBJECT_NUMBER(CLASS, pxObject), TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject)); \
	prvTraceStoreObjectPropertiesOnCloseEvent(SERVICE_PROP, TRACE_GET_OBJECT_NUMBER(CLASS, pxObject), TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject)); \
	prvTraceFreeObjectHandle(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject));

/* This macro will create a call to a kernel service with a certain result, with an object as parameter */
#undef trcKERNEL_HOOKS_KERNEL_SERVICE
#define trcKERNEL_HOOKS_KERNEL_SERVICE(SERVICE, CLASS, pxObject) \
	if (TRACE_GET_OBJECT_FILTER(TASK, TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		if (TRACE_GET_OBJECT_FILTER(CLASS, pxObject) & CurrentFilterMask) \
			prvTraceStoreKernelCall(SERVICE, TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject));

/* This macro will create a call to a kernel service with a certain result, with a null object as parameter */
#undef trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT
#define trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(SERVICE, TRACECLASS) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		prvTraceStoreKernelCall(SERVICE, TRACECLASS, 0);

/* This macro will create a call to a kernel service with a certain result, with an object as parameter */
#undef trcKERNEL_HOOKS_KERNEL_SERVICE_WITH_PARAM
#define trcKERNEL_HOOKS_KERNEL_SERVICE_WITH_PARAM(SERVICE, CLASS, pxObject, param) \
	if (TRACE_GET_OBJECT_FILTER(TASK, TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		if (TRACE_GET_OBJECT_FILTER(CLASS, pxObject) & CurrentFilterMask) \
			prvTraceStoreKernelCallWithParam(SERVICE, TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject), (uint32_t)param);

/* This macro will create a call to a kernel service with a certain result, with a null object and other value as parameter */
#undef trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT_WITH_PARAM
#define trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT_WITH_PARAM(SERVICE, TRACECLASS, param) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		prvTraceStoreKernelCallWithParam(SERVICE, TRACECLASS, 0, param);

/* This macro will create a call to a kernel service with a certain result, with an object as parameter */
#undef trcKERNEL_HOOKS_KERNEL_SERVICE_WITH_NUMERIC_PARAM_ONLY
#define trcKERNEL_HOOKS_KERNEL_SERVICE_WITH_NUMERIC_PARAM_ONLY(SERVICE, param) \
	if (TRACE_GET_OBJECT_FILTER(TASK, TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		prvTraceStoreKernelCallWithNumericParamOnly(SERVICE, (uint32_t)param);

/* This macro will create a call to a kernel service with a certain result, with an object as parameter */
#undef trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR
#define trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR(SERVICE, CLASS, pxObject) \
	if (TRACE_GET_OBJECT_FILTER(CLASS, pxObject) & CurrentFilterMask) \
		prvTraceStoreKernelCall(SERVICE, TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject));

/* This macro will create a call to a kernel service with a certain result, with a null object as parameter */
#undef trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT_FROM_ISR
#define trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT_FROM_ISR(SERVICE, TRACECLASS) \
	prvTraceStoreKernelCall(SERVICE, TRACECLASS, 0);

/* This macro will create a call to a kernel service with a certain result, with an object as parameter */
#undef trcKERNEL_HOOKS_KERNEL_SERVICE_WITH_PARAM_FROM_ISR
#define trcKERNEL_HOOKS_KERNEL_SERVICE_WITH_PARAM_FROM_ISR(SERVICE, CLASS, pxObject, param) \
	if (TRACE_GET_OBJECT_FILTER(CLASS, pxObject) & CurrentFilterMask) \
		prvTraceStoreKernelCallWithParam(SERVICE, TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject), (uint32_t)param);

/* This macro will create a call to a kernel service with a certain result, with a null object and other value as parameter */
#undef trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT_WITH_PARAM_FROM_ISR
#define trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT_WITH_PARAM_FROM_ISR(SERVICE, TRACECLASS, param) \
	prvTraceStoreKernelCallWithParam(SERVICE, TRACECLASS, 0, param);

/* This macro will create a call to a kernel service with a certain result, with an object as parameter */
#undef trcKERNEL_HOOKS_KERNEL_SERVICE_WITH_NUMERIC_PARAM_ONLY_FROM_ISR
#define trcKERNEL_HOOKS_KERNEL_SERVICE_WITH_NUMERIC_PARAM_ONLY_FROM_ISR(SERVICE, param) \
	prvTraceStoreKernelCallWithNumericParamOnly(SERVICE, (uint32_t)param);

/* This macro will set the state for an object */
#undef trcKERNEL_HOOKS_SET_OBJECT_STATE
#define trcKERNEL_HOOKS_SET_OBJECT_STATE(CLASS, pxObject, STATE) \
	prvTraceSetObjectState(TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject), TRACE_GET_OBJECT_NUMBER(CLASS, pxObject), (uint8_t)STATE);

/* This macro will flag a certain task as a finished instance */
#undef trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED
#define trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED() \
	if (TRACE_GET_OBJECT_FILTER(TASK, TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		prvTraceSetTaskInstanceFinished(TRACE_GET_TASK_NUMBER(TRACE_GET_CURRENT_TASK()));

#if (TRC_CFG_INCLUDE_READY_EVENTS == 1)
/* This macro will create an event to indicate that a task became Ready */
#undef trcKERNEL_HOOKS_MOVED_TASK_TO_READY_STATE
#define trcKERNEL_HOOKS_MOVED_TASK_TO_READY_STATE(pxTCB) \
	if (TRACE_GET_OBJECT_FILTER(TASK, pxTCB) & CurrentFilterMask) \
		prvTraceStoreTaskReady(TRACE_GET_TASK_NUMBER(pxTCB));
#else /*(TRC_CFG_INCLUDE_READY_EVENTS == 1)*/
#undef trcKERNEL_HOOKS_MOVED_TASK_TO_READY_STATE
#define trcKERNEL_HOOKS_MOVED_TASK_TO_READY_STATE(pxTCB)
#endif /*(TRC_CFG_INCLUDE_READY_EVENTS == 1)*/

/* This macro will update the internal tick counter and call prvTracePortGetTimeStamp(0) to update the internal counters */
#undef trcKERNEL_HOOKS_INCREMENT_TICK
#define trcKERNEL_HOOKS_INCREMENT_TICK() \
	{ \
		extern uint32_t uiTraceTickCount; \
		uiTraceTickCount++; \
		prvTracePortGetTimeStamp(0); \
	}

#if (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1)
/* This macro will create an event indicating that the OS tick count has increased */
#undef trcKERNEL_HOOKS_NEW_TIME
#define trcKERNEL_HOOKS_NEW_TIME(SERVICE, xValue) \
	prvTraceStoreKernelCallWithNumericParamOnly(SERVICE, xValue);
#else /*(TRC_CFG_INCLUDE_OSTICK_EVENTS == 1)*/
#undef trcKERNEL_HOOKS_NEW_TIME
#define trcKERNEL_HOOKS_NEW_TIME(SERVICE, xValue)
#endif /*(TRC_CFG_INCLUDE_OSTICK_EVENTS == 1)*/

/* This macro will create a task switch event to the currently executing task */
#undef trcKERNEL_HOOKS_TASK_SWITCH
#define trcKERNEL_HOOKS_TASK_SWITCH( pxTCB ) \
	if (TRACE_GET_OBJECT_FILTER(TASK, pxTCB) & CurrentFilterMask) \
		prvTraceStoreTaskswitch(TRACE_GET_TASK_NUMBER(pxTCB));

/* This macro will create an event to indicate that the task has been suspended */
#undef trcKERNEL_HOOKS_TASK_SUSPEND
#define trcKERNEL_HOOKS_TASK_SUSPEND(SERVICE, pxTCB) \
	if (TRACE_GET_OBJECT_FILTER(TASK, TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		if (TRACE_GET_OBJECT_FILTER(TASK, pxTCB) & CurrentFilterMask) \
			prvTraceStoreKernelCall(SERVICE, TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB)); \
	prvTraceSetTaskInstanceFinished((uint8_t)TRACE_GET_TASK_NUMBER(pxTCB));

/* This macro will create an event to indicate that a task has called a wait/delay function */
#undef trcKERNEL_HOOKS_TASK_DELAY
#define trcKERNEL_HOOKS_TASK_DELAY(SERVICE, pxTCB, xValue) \
	if (TRACE_GET_OBJECT_FILTER(TASK, pxTCB) & CurrentFilterMask) \
	{ \
		prvTraceStoreKernelCallWithNumericParamOnly(SERVICE, xValue); \
		prvTraceSetTaskInstanceFinished((uint8_t)TRACE_GET_TASK_NUMBER(pxTCB)); \
	}

/* This macro will create an event to indicate that a task has gotten its priority changed */
#undef trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE
#define trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE(SERVICE, pxTCB, uxNewPriority) \
	if (TRACE_GET_OBJECT_FILTER(TASK, pxTCB) & CurrentFilterMask) \
	{ \
		prvTraceStoreKernelCallWithParam(SERVICE, TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB), prvTraceGetPriorityProperty(TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB)));\
		prvTraceSetPriorityProperty(TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB), (uint8_t)uxNewPriority); \
	}

/* This macro will create an event to indicate that the task has been resumed */
#undef trcKERNEL_HOOKS_TASK_RESUME
#define trcKERNEL_HOOKS_TASK_RESUME(SERVICE, pxTCB) \
	if (TRACE_GET_OBJECT_FILTER(TASK, TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		if (TRACE_GET_OBJECT_FILTER(TASK, pxTCB) & CurrentFilterMask) \
			prvTraceStoreKernelCall(SERVICE, TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB));

#undef trcKERNEL_HOOKS_TASK_RESUME_FROM_ISR
#define trcKERNEL_HOOKS_TASK_RESUME_FROM_ISR(SERVICE, pxTCB) \
	if (TRACE_GET_OBJECT_FILTER(TASK, pxTCB) & CurrentFilterMask) \
		prvTraceStoreKernelCall(SERVICE, TRACE_CLASS_TASK, TRACE_GET_TASK_NUMBER(pxTCB));

#if !defined TRC_CFG_INCLUDE_READY_EVENTS || TRC_CFG_INCLUDE_READY_EVENTS == 1
	void prvTraceSetReadyEventsEnabled(int status);
	void prvTraceStoreTaskReady(traceHandle handle);
#else
	#define prvTraceSetReadyEventsEnabled(status)
#endif

void prvTraceStoreLowPower(uint32_t flag);

void prvTraceStoreTaskswitch(traceHandle task_handle);


#if (TRC_CFG_SCHEDULING_ONLY == 0)

void prvTraceStoreKernelCall(uint32_t eventcode, traceObjectClass objectClass, uint32_t byteParam);

void prvTraceStoreKernelCallWithNumericParamOnly(uint32_t evtcode, uint32_t param);

void prvTraceStoreKernelCallWithParam(uint32_t evtcode, traceObjectClass objectClass,
									uint32_t objectNumber, uint32_t param);
#else

#define prvTraceStoreKernelCall(eventcode, objectClass, byteParam) {}
#define prvTraceStoreKernelCallWithNumericParamOnly(evtcode, param) {}
#define prvTraceStoreKernelCallWithParam(evtcode, objectClass, objectNumber, param) {}

#endif

/*******************************************************************************
* prvTraceInitTraceData
*
* Allocates and initializes the recorder data structure, based on the constants
* in trcConfig.h. This allows for allocating the data on the heap, instead of
* using a static declaration.
******************************************************************************/
void prvTraceInitTraceData(void);

void prvTraceSetTaskInstanceFinished(traceHandle handle);

void prvTraceSetPriorityProperty(uint8_t objectclass, traceHandle id, uint8_t value);

uint8_t prvTraceGetPriorityProperty(uint8_t objectclass, traceHandle id);

void prvTraceSetObjectState(uint8_t objectclass, traceHandle id, uint8_t value);

void prvMarkObjectAsUsed(traceObjectClass objectclass, traceHandle handle);

void prvTraceStoreObjectNameOnCloseEvent(uint8_t evtcode, traceHandle handle,
										traceObjectClass objectclass);

void prvTraceStoreObjectPropertiesOnCloseEvent(uint8_t evtcode, traceHandle handle,
											 traceObjectClass objectclass);

/* Internal constants for task state */
#define TASK_STATE_INSTANCE_NOT_ACTIVE 0
#define TASK_STATE_INSTANCE_ACTIVE 1


#if (TRC_CFG_INCLUDE_ISR_TRACING == 0)

#undef vTraceSetISRProperties
#define vTraceSetISRProperties(handle, name, priority) (void)(handle), (void)(name), (void)(priority) /* Comma operator is used to avoid "unused variable" compiler warnings in a single statement */

#undef vTraceStoreISRBegin
#define vTraceStoreISRBegin(x) (void)(x)

#undef vTraceStoreISREnd
#define vTraceStoreISREnd(x) (void)(x)

#undef xTraceSetISRProperties
#define xTraceSetISRProperties(name, priority) ((void)(name), (void)(priority), (traceHandle)0) /* Comma operator in parenthesis is used to avoid "unused variable" compiler warnings and return 0 in a single statement */

#endif /*(TRC_CFG_INCLUDE_ISR_TRACING == 0)*/

/*******************************************************************************
 * xTraceGetTraceBuffer
 *
 * Returns a pointer to the recorder data structure. Use this together with
 * uiTraceGetTraceBufferSize if you wish to implement an own store/upload
 * solution, e.g., in case a debugger connection is not available for uploading
 * the data.
 ******************************************************************************/
void* xTraceGetTraceBuffer(void);

/*******************************************************************************
 * uiTraceGetTraceBufferSize
 *
 * Gets the size of the recorder data structure. For use together with
 * vTraceGetTraceBuffer if you wish to implement an own store/upload solution,
 * e.g., in case a debugger connection is not available for uploading the data.
 ******************************************************************************/
uint32_t uiTraceGetTraceBufferSize(void);

#if (TRC_CFG_SCHEDULING_ONLY == 1)
#undef TRC_CFG_INCLUDE_USER_EVENTS
#define TRC_CFG_INCLUDE_USER_EVENTS 0
#endif /*(TRC_CFG_SCHEDULING_ONLY == 1)*/

#if ((TRC_CFG_INCLUDE_USER_EVENTS == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)) && (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1)
traceUBChannel xTraceRegisterUBChannel(TraceStringHandle_t channel, TraceStringHandle_t formatStr);
void vTraceUBData(traceUBChannel channel, ...);
void vTraceUBEvent(traceUBChannel channel);
#else
#define xTraceRegisterChannelFormat(eventLabel, formatStr) ((void)(eventLabel), (void)(formatStr), 0) /* Comma operator in parenthesis is used to avoid "unused variable" compiler warnings and return 0 in a single statement */
#define vTraceUBData(label, ...) (void)(label)
#endif /*(TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1)*/

#define NEventCodes 0x100

/* Our local critical sections for the recorder */
#define trcCRITICAL_SECTION_BEGIN() {TRACE_ENTER_CRITICAL_SECTION(); recorder_busy++;}
#define trcCRITICAL_SECTION_END() {recorder_busy--; TRACE_EXIT_CRITICAL_SECTION();}

#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_ARM_Cortex_M)
	#define trcSR_ALLOC_CRITICAL_SECTION_ON_CORTEX_M_ONLY TRACE_ALLOC_CRITICAL_SECTION
	#define trcCRITICAL_SECTION_BEGIN_ON_CORTEX_M_ONLY trcCRITICAL_SECTION_BEGIN
	#define trcCRITICAL_SECTION_END_ON_CORTEX_M_ONLY trcCRITICAL_SECTION_END
#else
	#define trcSR_ALLOC_CRITICAL_SECTION_ON_CORTEX_M_ONLY() {}
	#define trcCRITICAL_SECTION_BEGIN_ON_CORTEX_M_ONLY() recorder_busy++;
	#define trcCRITICAL_SECTION_END_ON_CORTEX_M_ONLY() recorder_busy--;
#endif

/******************************************************************************
 * ObjectHandleStack
 * This data-structure is used to provide a mechanism for 1-byte trace object
 * handles. This way, only 1 byte is necessary instead of 4 bytes (a pointer)
 * when storing a reference to an object. This allows for up to 255 objects of
 * each object class active at any given moment. There can be more "historic"
 * objects, that have been deleted - that number is only limited by the size of
 * the symbol table.
 *
 * Note that handle zero (0) is not used, it is a code for an invalid handle.
 *
 * This data structure keeps track of the FREE handles, not the handles in use.
 * This data structure contains one stack per object class. When a handle is
 * allocated to an object, the next free handle is popped from the stack. When
 * a handle is released (on object delete), it is pushed back on the stack.
 * Note that there is no initialization code that pushed the free handles
 * initially, that is not necessary due to the following optimization:
 *
 * The stack of handles (objectHandles) is initially all zeros. Since zero
 * is not a valid handle, that is a signal of additional handles needed.
 * If a zero is received when popping a new handle, it is replaced by the
 * index of the popped handle instead.
 *****************************************************************************/
typedef struct
{
	/* For each object class, the index of the next handle to allocate */
	uint16_t indexOfNextAvailableHandle[ TRACE_NCLASSES ];

	/* The lowest index of this class (constant) */
	uint16_t lowestIndexOfClass[ TRACE_NCLASSES ];

	/* The highest index of this class (constant) */
	uint16_t highestIndexOfClass[ TRACE_NCLASSES ];

	/* The highest use count for this class (for statistics) */
	uint16_t handleCountWaterMarksOfClass[ TRACE_NCLASSES ];

	/* The free object handles - a set of stacks within this array */
	traceHandle objectHandles[ TRACE_KERNEL_OBJECT_COUNT ];

} objectHandleStackType;

extern objectHandleStackType objectHandleStacks;

/******************************************************************************
 * Object Property Table
 * The Object Table contains name and other properties of the objects (tasks,
 * queues, mutexes, etc). The below data structures defines the properties of
 * each object class and are used to cast the byte buffer into a cleaner format.
 *
 * The values in the object table are continuously overwritten and always
 * represent the current state. If a property is changed during runtime, the OLD
 * value should be stored in the trace buffer, not the new value (since the new
 * value is found in the Object Property Table).
 *
 * For close events this mechanism is the old names are stored in the symbol
 * table), for "priority set" (the old priority is stored in the event data)
 * and for "isActive", where the value decides if the task switch event type
 * should be "new" or "resume".
 ******************************************************************************/

typedef struct
{
	/* = NCLASSES */
	uint32_t NumberOfObjectClasses;

	uint32_t ObjectPropertyTableSizeInBytes;

	/* This is used to calculate the index in the dynamic object table
	(handle - 1 - nofStaticObjects = index)*/
#if (TRC_CFG_USE_16BIT_OBJECT_HANDLES == 1)
	traceHandle NumberOfObjectsPerClass[2*((TRACE_NCLASSES+1)/2)];
#else
	traceHandle NumberOfObjectsPerClass[4*((TRACE_NCLASSES+3)/4)];
#endif

	/* Allocation size rounded up to the closest multiple of 4 */
	uint8_t NameLengthPerClass[ 4*((TRACE_NCLASSES+3)/4) ];

	uint8_t TotalPropertyBytesPerClass[ 4*((TRACE_NCLASSES+3)/4) ];

	/* Allocation size rounded up to the closest multiple of 2 */
	uint16_t StartIndexOfClass[ 2*((TRACE_NCLASSES+1)/2) ];

	/* The actual handles issued, should be Initiated to all zeros */
	uint8_t objbytes[ 4*((TRACE_OBJECT_TABLE_SIZE+3)/4) ];
} ObjectPropertyTableType;

/* Symbol table data structure */
typedef struct
{
	/* = SYMBOL_HISTORY_TABLE_SIZE_IN_BYTES */
	uint32_t symTableSize;

	/* Entry 0 is reserved. Any reference to entry 0 implies NULL*/
	uint32_t nextFreeSymbolIndex;

	/* Size rounded up to closest multiple of 4, to avoid alignment issues*/
	uint8_t symbytes[4*(((TRC_CFG_SYMBOL_TABLE_SIZE)+3)/4)];

	/* Used for lookups - Up to 64 linked lists within the symbol table
	connecting all entries with the same 6 bit checksum.
	This field holds the current list heads. Should be initiated to zeros */
	uint16_t latestEntryOfChecksum[64];
} symbolTableType;


/*******************************************************************************
 * The data structures of the different events, all 4 bytes long
 ******************************************************************************/

typedef struct
{
	uint8_t type;
	uint8_t objHandle;
	uint16_t dts;	/* differential timestamp - time since last event */
} TSEvent, TREvent;

typedef struct
{
	uint8_t type;
	uint8_t dummy;
	uint16_t dts;	/* differential timestamp - time since last event */
} LPEvent;

typedef struct
{
	uint8_t type;
	uint8_t objHandle;
	uint16_t dts;	/* differential timestamp - time since last event */
} KernelCall;

typedef struct
{
	uint8_t type;
	uint8_t objHandle;
	uint8_t param;
	uint8_t dts;	/* differential timestamp - time since last event */
} KernelCallWithParamAndHandle;

typedef struct
{
	uint8_t type;
	uint8_t dts;	/* differential timestamp - time since last event */
	uint16_t param;
} KernelCallWithParam16;

typedef struct
{
	uint8_t type;
	uint8_t objHandle;		/* the handle of the closed object */
	uint16_t symbolIndex;	/* the name of the closed object */
} ObjCloseNameEvent;

typedef struct
{
	uint8_t type;
	uint8_t arg1;
	uint8_t arg2;
	uint8_t arg3;
} ObjClosePropEvent;

typedef struct
{
	uint8_t type;
	uint8_t unused1;
	uint8_t unused2;
	uint8_t dts;
} TaskInstanceStatusEvent;

typedef struct
{
	uint8_t type;
	uint8_t dts;
	uint16_t payload;		 /* the name of the user event */
} UserEvent;

typedef struct
{
	uint8_t type;

	/* 8 bits extra for storing DTS, if it does not fit in ordinary event
	(this one is always MSB if used) */
	uint8_t xts_8;

	/* 16 bits extra for storing DTS, if it does not fit in ordinary event. */
	uint16_t xts_16;
} XTSEvent;

typedef struct
{
	uint8_t type;

	uint8_t xps_8;
	uint16_t xps_16;
} XPSEvent;

typedef struct{
	uint8_t type;
	uint8_t dts;
	uint16_t size;
} MemEventSize;

typedef struct{
	uint8_t type;
	uint8_t addr_high;
	uint16_t addr_low;
} MemEventAddr;

/*******************************************************************************
 * The separate user event buffer structure. Can be enabled in trcConfig.h.
 ******************************************************************************/

#if (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1)
typedef struct
{
	TraceStringHandle_t name;
	TraceStringHandle_t defaultFormat;
} ChannelFormatPair;

typedef struct
{
	uint16_t bufferID;
	uint16_t version;
	uint32_t wraparoundCounter;
	uint32_t numberOfSlots;
	uint32_t nextSlotToWrite;
	uint8_t numberOfChannels;
	uint8_t padding1;
	uint8_t padding2;
	uint8_t padding3;
	ChannelFormatPair channels[(TRC_CFG_UB_CHANNELS)+1];
	uint8_t channelBuffer[((TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE) + 3) & 0xFFFFFFFC]; /* 1 byte per slot, with padding for 4 byte alignment */
	uint8_t dataBuffer[(TRC_CFG_SEPARATE_USER_EVENT_BUFFER_SIZE) * 4]; /* 4 bytes per slot */

} UserEventBuffer;
#endif /* (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1) */

/*******************************************************************************
 * The main data structure, read by Tracealyzer from the RAM dump
 ******************************************************************************/

typedef struct
{
	volatile uint8_t startmarker0; /* Volatile is important, see init code. */
	volatile uint8_t startmarker1;
	volatile uint8_t startmarker2;
	volatile uint8_t startmarker3;
	volatile uint8_t startmarker4;
	volatile uint8_t startmarker5;
	volatile uint8_t startmarker6;
	volatile uint8_t startmarker7;
	volatile uint8_t startmarker8;
	volatile uint8_t startmarker9;
	volatile uint8_t startmarker10;
	volatile uint8_t startmarker11;

	/* Used to determine Kernel and Endianess */
	uint16_t version;

	/* Currently 7 */
	uint8_t minor_version;

	/* This should be 0 if lower IRQ priority values implies higher priority
	levels, such as on ARM Cortex M. If the opposite scheme is used, i.e.,
	if higher IRQ priority values means higher priority, this should be 1. */
	uint8_t irq_priority_order;

	/* sizeof(RecorderDataType) - just for control */
	uint32_t filesize;

	/* Current number of events recorded */
	uint32_t numEvents;

	/* The buffer size, in number of event records */
	uint32_t maxEvents;

	/* The event buffer index, where to write the next event */
	uint32_t nextFreeIndex;

	/* 1 if the buffer is full, 0 otherwise */
	uint32_t bufferIsFull;

	/* The frequency of the clock/timer/counter used as time base */
	uint32_t frequency;

	/* The absolute timestamp of the last stored event, in the native
	timebase, modulo frequency! */
	uint32_t absTimeLastEvent;

	/* The number of seconds in total - lasts for 136 years */
	uint32_t absTimeLastEventSecond;

	/* 1 if the recorder has been started, 0 if not yet started or stopped.
	This is a 32 bit variable due to alignment issues. */
	uint32_t recorderActive;

	/* If > 0, tells the maximum time between two traced ISRs that execute
	back-to-back. If the time between vTraceStoreISREnd and a directly
	following vTraceISRBegin is above isrTailchainingThreshold, we assume a
	return to the previous context in between the ISRs, otherwise we assume
	the have executed back-to-back and don't show any fragment of the previous
	context in between. */ 
	uint32_t isrTailchainingThreshold;

	/* The maximum amount of heap memory that was allocated */
	uint32_t heapMemMaxUsage;

	/* The amount of heap memory used */
	uint32_t heapMemUsage;

	/* 0xF0F0F0F0 - for control only */
	int32_t debugMarker0;

	/* Set to value of TRC_CFG_USE_16BIT_OBJECT_HANDLES */
	uint32_t isUsing16bitHandles;

	/* The Object Property Table holds information about currently active
	tasks, queues, and other recorded objects. This is updated on each
	create call and includes object name and other properties. */
	ObjectPropertyTableType ObjectPropertyTable;

	/* 0xF1F1F1F1 - for control only */
	int32_t debugMarker1;

	/* The Symbol Table stores strings for User Events and is also used to
	store names of deleted objects, which still may be in the trace but no
	longer are available. */
	symbolTableType SymbolTable;

	/* For inclusion of float support, and for endian detection of floats.
	The value should be (float)1 or (uint32_t)0 */
#if (TRC_CFG_INCLUDE_FLOAT_SUPPORT == 1)
	float exampleFloatEncoding;
#else
	uint32_t exampleFloatEncoding;
#endif
	/* This is non-zero if an internal error occurred in the recorder, e.g., if
	one of the Nxxx constants was too small. The systemInfo string will then
	contain an error message that is displayed when attempting to view the
	trace file. */
	uint32_t internalErrorOccured;

	/* 0xF2F2F2F2 - for control only */
	int32_t debugMarker2;

	/* Error messages from the recorder. */
	char systemInfo[80];

	/* 0xF3F3F3F3 - for control only */
	int32_t debugMarker3;

	/* The event data, in 4-byte records */
	uint8_t eventData[ (TRC_CFG_EVENT_BUFFER_SIZE) * 4 ];

#if (TRC_CFG_USE_SEPARATE_USER_EVENT_BUFFER == 1)
	UserEventBuffer userEventBuffer;
#endif

	/* This should always be 0 */
	uint32_t endOfSecondaryBlocks;

	uint8_t endmarker0;
	uint8_t endmarker1;
	uint8_t endmarker2;
	uint8_t endmarker3;
	uint8_t endmarker4;
	uint8_t endmarker5;
	uint8_t endmarker6;
	uint8_t endmarker7;
	uint8_t endmarker8;
	uint8_t endmarker9;
	uint8_t endmarker10;
	uint8_t endmarker11;
} RecorderDataType;

extern RecorderDataType* RecorderDataPtr;

/* Internal functions */

/* Signal an error. */
void prvTraceError(const char* msg);

/*******************************************************************************
 * prvTracePortGetTimeStamp
 *
 * Returns the current time based on the HWTC macros which provide a hardware
 * isolation layer towards the hardware timer/counter.
 *
 * The HWTC macros and prvTracePortGetTimeStamp is the main porting issue
 * or the trace recorder library. Typically you should not need to change
 * the code of prvTracePortGetTimeStamp if using the HWTC macros.
 *
 ******************************************************************************/
void prvTracePortGetTimeStamp(uint32_t *puiTimestamp);

traceHandle prvTraceGetObjectHandle(traceObjectClass objectclass);

void prvTraceFreeObjectHandle(traceObjectClass objectclass,
							traceHandle handle);

/* Private function. Use the public functions in trcKernelPort.h */
void prvTraceSetObjectName(traceObjectClass objectclass,
							traceHandle handle,
							const char* name);

/* Internal macros */

#define TRACE_PROPERTY_NAME_GET(objectclass, objecthandle) \
(const char*)(& RecorderDataPtr->ObjectPropertyTable.objbytes \
[uiIndexOfObject(objecthandle, objectclass)])

#define TRACE_PROPERTY_OBJECT_STATE(objectclass, handle) \
RecorderDataPtr->ObjectPropertyTable.objbytes[uiIndexOfObject(handle, objectclass) \
+ RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[objectclass]]

#define TRACE_PROPERTY_ACTOR_PRIORITY(objectclass, handle) \
RecorderDataPtr->ObjectPropertyTable.objbytes[uiIndexOfObject(handle, objectclass) \
+ RecorderDataPtr->ObjectPropertyTable.NameLengthPerClass[objectclass] + 1]

/* DEBUG ASSERTS */
#if defined TRC_CFG_USE_TRACE_ASSERT && TRC_CFG_USE_TRACE_ASSERT != 0
#define TRACE_ASSERT(eval, msg, defRetVal) \
if (!(eval)) \
{ \
	prvTraceError("TRACE_ASSERT: " msg); \
	return defRetVal; \
}
#else
#define TRACE_ASSERT(eval, msg, defRetVal)
#endif

typedef RecorderDataType TraceRecorderDataBuffer_t;

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)*/

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#ifndef TRC_EXTERNAL_BUFFERS
#define TRC_EXTERNAL_BUFFERS 0
#endif

typedef struct TraceRecorderData
{
	uint32_t uiSessionCounter;
	uint32_t uiRecorderEnabled;
	uint32_t uiTraceSystemState;

	TraceAssertBuffer_t xAssertBuffer;
#if (TRC_EXTERNAL_BUFFERS == 0)
	TraceHeaderBuffer_t xHeaderBuffer;
	TraceEntryTableBuffer_t xEntryTableBuffer;
	TraceTimestampBuffer_t xTimestampBuffer;
#endif
	TraceStreamPortBuffer_t xStreamPortBuffer;
	TraceStaticBufferBuffer_t xStaticBufferBuffer;
	TraceEventDataBuffer_t xEventDataBuffer;
	TracePrintBuffer_t xPrintBuffer;
	TraceErrorBuffer_t xErrorBuffer;
	TraceISRInfoBuffer_t xISRInfoBuffer;
	TraceKernelPortDataBuffer_t xKernelPortBuffer;
	TraceTaskInfoBuffer_t xTaskInfoBuffer;
	TraceStackMonitorBuffer_t xStackMonitorBuffer;
	TraceDiagnosticsBuffer_t xDiagnosticsBuffer;
} TraceRecorderData_t;

extern TraceRecorderData_t* pxTraceRecorderData;
extern uint32_t RecorderInitialized;

#define TRC_RECORDER_DATA_BUFFER_SIZE (sizeof(TraceRecorderData_t))

typedef struct TraceRecorderDataBuffer
{
	uint8_t buffer[(TRC_RECORDER_DATA_BUFFER_SIZE)];
} TraceRecorderDataBuffer_t;

traceResult xTraceHeaderInitialize(TraceHeaderBuffer_t* pxBuffer);

#define xTraceIsRecorderEnabled() (xTraceIsRecorderInitialized() & pxTraceRecorderData->uiRecorderEnabled)

#define xTraceIsRecorderInitialized() xTraceIsComponentInitialized(TRC_RECORDER_COMPONENT_CORE)

#define xTraceSetComponentInitialized(uiComponentBit) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(RecorderInitialized |= (uiComponentBit), TRC_SUCCESS)

#define xTraceIsComponentInitialized(uiComponentBit) ((RecorderInitialized & (uiComponentBit)) ? 1 : 0)

#define xTraceStateSet(uiState) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(pxTraceRecorderData->uiTraceSystemState = (uiState), TRC_SUCCESS)
#define xTraceStateGet(puiState) TRC_COMMA_EXPR_TO_STATEMENT_EXPR_2(*(puiState) = pxTraceRecorderData->uiTraceSystemState, TRC_SUCCESS)

/******************************************************************************
* xTraceTzCtrl
*
* Call this function periodically.
******************************************************************************/
traceResult xTraceTzCtrl(void);

/******************************************************************************/
/*** INTERNAL STREAMING FUNCTIONS *********************************************/
/******************************************************************************/

#define prvTraceStoreEvent_None(_eventID) \
	{ \
		TraceEventHandle_t _xEventHandle = 0; \
		if (xTraceEventBegin(_eventID, 0, &_xEventHandle) == TRC_SUCCESS) \
		{ \
			xTraceEventEnd(_xEventHandle); \
		} \
	}

#define prvTraceStoreEvent_Handle(_eventID, _handle) \
	{ \
		TraceEventHandle_t _xEventHandle = 0; \
		if (xTraceEventBegin(_eventID, sizeof(void*), &_xEventHandle) == TRC_SUCCESS) \
		{ \
			xTraceEventAddPointer(_xEventHandle, (void*)(_handle)); \
			xTraceEventEnd(_xEventHandle); \
		} \
	}

#define prvTraceStoreEvent_Param(_eventID, _param1) \
	{ \
		TraceEventHandle_t _xEventHandle = 0; \
		if (xTraceEventBegin(_eventID, sizeof(uint32_t), &_xEventHandle) == TRC_SUCCESS) \
		{ \
			xTraceEventAdd32(_xEventHandle, (uint32_t)(_param1)); \
			xTraceEventEnd(_xEventHandle); \
		} \
	}

#define prvTraceStoreEvent_HandleParam(_eventID, _handle, _param1) \
	{ \
		TraceEventHandle_t _xEventHandle = 0; \
		if (xTraceEventBegin(_eventID, sizeof(void*) + sizeof(uint32_t), &_xEventHandle) == TRC_SUCCESS) \
		{ \
			xTraceEventAddPointer(_xEventHandle, (void*)(_handle)); \
			xTraceEventAdd32(_xEventHandle, (uint32_t)(_param1)); \
			xTraceEventEnd(_xEventHandle); \
		} \
	}

#define prvTraceStoreEvent_ParamParam(_eventID, _param1, _param2) \
	{ \
		TraceEventHandle_t _xEventHandle = 0; \
		if (xTraceEventBegin(_eventID, sizeof(uint32_t) + sizeof(uint32_t), &_xEventHandle) == TRC_SUCCESS) \
		{ \
			xTraceEventAdd32(_xEventHandle, (uint32_t)(_param1)); \
			xTraceEventAdd32(_xEventHandle, (uint32_t)(_param2)); \
			xTraceEventEnd(_xEventHandle); \
		} \
	}

#define prvTraceStoreEvent_HandleParamParam(_eventID, _handle, _param1, _param2) \
	{ \
		TraceEventHandle_t _xEventHandle = 0; \
		if (xTraceEventBegin(_eventID, sizeof(void*) + sizeof(uint32_t) + sizeof(uint32_t), &_xEventHandle) == TRC_SUCCESS) \
		{ \
			xTraceEventAddPointer(_xEventHandle, (void*)(_handle)); \
			xTraceEventAdd32(_xEventHandle, (uint32_t)(_param1)); \
			xTraceEventAdd32(_xEventHandle, (uint32_t)(_param2)); \
			xTraceEventEnd(_xEventHandle); \
		} \
	}

#define prvTraceStoreEvent_ParamParamParam(_eventID, _param1, _param2, _param3) \
	{ \
		TraceEventHandle_t _xEventHandle = 0; \
		if (xTraceEventBegin(_eventID, sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t), &_xEventHandle) == TRC_SUCCESS) \
		{ \
			xTraceEventAdd32(_xEventHandle, (uint32_t)(_param1)); \
			xTraceEventAdd32(_xEventHandle, (uint32_t)(_param2)); \
			xTraceEventAdd32(_xEventHandle, (uint32_t)(_param3)); \
			xTraceEventEnd(_xEventHandle); \
		} \
	}

#define vTraceSetStopHook(x) (void)(x)

#define vTraceInitTimestamps() 

#endif /*(TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)*/

#if (TRC_CFG_RECORDER_BUFFER_ALLOCATION == TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM)
traceResult xTraceSetBuffer(TraceRecorderDataBuffer_t *pxBuffer);
#else
#define xTraceSetBuffer(p) (TRC_SUCCESS)
#endif

traceResult xTraceGetEventBuffer(void** ppvBuffer, TraceUnsignedBaseType_t * puiSize);

#else /* when TRC_USE_TRACEALYZER_RECORDER == 0 */

#define xTraceInitialize() (TRC_SUCCESS)
#define xTraceEnable(x) ((void)(x), TRC_SUCCESS)
#define xTraceDisable() (TRC_SUCCESS)
#define xTraceStringRegister(x, y) ((void)(x), (void)y, TRC_SUCCESS) /* Comma operator in parenthesis is used to avoid "unused variable" compiler warnings and return 0 in a single statement */
#define xTracePrint(chn, ...) ((void)(chn), TRC_SUCCESS)
#define xTracePrintF(chn, fmt, ...) ((void)(chn), (void)(fmt), TRC_SUCCESS) /* Comma operator is used to avoid "unused variable" compiler warnings in a single statement */
#define xTraceVPrintF(chn, formatStr, vl) ((void)(chn), (void)(formatStr), (void)(vl), TRC_SUCCESS) /* Comma operator is used to avoid "unused variable" compiler warnings in a single statement */
#define xTraceTaskInstanceFinishedNow()
#define xTraceTaskInstanceFinishedNext()
#define vTraceStoreISRBegin(x) (void)(x)
#define vTraceStoreISREnd(x) (void)(x)
#define xTraceSetISRProperties(a, b) ((void)(a), (void)(b), (traceHandle)0) /* Comma operator in parenthesis is used to avoid "unused variable" compiler warnings and return 0 in a single statement */
#define xTraceRegisterChannelFormat(eventLabel, formatStr) ((void)(eventLabel), (void)(formatStr), 0) /* Comma operator in parenthesis is used to avoid "unused variable" compiler warnings and return 0 in a single statement */
#define vTraceUBData(label, ...) (void)(label)

#define vTraceSetFilterGroup(x) (void)(x)
#define vTraceSetFilterMask(x) (void)(x)

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

#define TraceRecorderDataBuffer_t uint32_t

#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

/* DEPRECATED. Backwards compatibility */
#define vTraceInitialize (void)xTraceInitialize

/* DEPRECATED. Backwards compatibility */
#define vTraceEnable (void)xTraceEnable

/* DEPRECATED. Backwards compatibility */
#define vTraceStop (void)xTraceDisable

/* DEPRECATED. Backwards compatibility */
#define vTraceInstanceFinishedNow (void)xTraceTaskInstanceFinishedNow

/* DEPRECATED. Backwards compatibility */
#define vTraceInstanceFinishedNext (void)xTraceTaskInstanceFinishedNext

  /* DEPRECATED. Backwards compatibility */
#define vTracePrintF (void)xTracePrintF

/* DEPRECATED. Backwards compatibility */
#define vTraceVPrintF (void)xTraceVPrintF

/* DEPRECATED. Backwards compatibility */
#define vTracePrint (void)xTracePrint

/* DEPRECATED. Backwards compatibility */
#define vTraceSetRecorderDataBuffer(pxBuffer) xTraceSetBuffer((TraceRecorderDataBuffer_t*)pxBuffer)

#ifdef __cplusplus
}
#endif

#endif /* TRC_RECORDER_H */
