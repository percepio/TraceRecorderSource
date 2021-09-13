/*
 * Trace Recorder for Tracealyzer v4.5.1
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * The SAFERTOS specific definitions of the trace recorder
 */

#ifndef TRC_KERNEL_PORT_H
#define TRC_KERNEL_PORT_H

#include "SafeRTOS.h"	/* Defines configUSE_TRACE_FACILITY */
#include "trcPortDefines.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ( configUSE_TRACE_FACILITY == 1 )
#define TRC_USE_TRACEALYZER_RECORDER 1
#else
#define TRC_USE_TRACEALYZER_RECORDER 0
#endif

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#define TRC_PLATFORM_CFG ""
#define TRC_PLATFORM_CFG_MAJOR 1
#define TRC_PLATFORM_CFG_MINOR 0
#define TRC_PLATFORM_CFG_PATCH 0

#define trcIS_TASK_VALID( pxTask ) ((pxTask != NULL) && xPortIsTaskHandleValid(pxTask))
#define trcIS_QUEUE_VALID( pxQueue ) ((pxQueue != NULL) && prvIS_QUEUE_VALID(pxQueue))
#define trcIS_TIMER_VALID( pxTimer ) ((pxTimer != NULL) && xTimerIsHandleValid(pxTimer))
#define trcIS_EVENTGROUP_VALID( pxEventGroup ) ((pxEventGroup != NULL) && prvIS_EVENT_GROUP_VALID(((eventGroupType*)pxEventGroup)))

unsigned char prvTraceIsSchedulerSuspended(void);
void* prvGetCurrentTaskHandle();
uint32_t prvGetTaskNumber(void* pxObject);
uint32_t prvGetQueueNumber(void* pxObject);
uint32_t prvGetTimerNumber(void* pxObject);
uint32_t prvGetEventGroupNumber(void* pxObject);

uint8_t prvTraceGetQueueType(void* handle);

/*******************************************************************************
 * Note: Setting names for event groups is difficult to support, this has been 
 * excluded intentionally. This since we don't know if event_groups.c is 
 * included in the build, so referencing it from the recorder may cause errors.
 ******************************************************************************/

#define TRACE_KERNEL_VERSION 0x6AA6
#define TRACE_TICK_RATE_HZ configTICK_RATE_HZ
#define TRACE_CPU_CLOCK_HZ configCPU_CLOCK_HZ
#define TRACE_GET_CURRENT_TASK() prvGetCurrentTaskHandle()

#define TRACE_GET_OS_TICKS() (uiTraceTickCount) /* Streaming only */

#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_ARM_Cortex_M)
	
	#define TRACE_ALLOC_CRITICAL_SECTION() int __irq_status;
	#define TRACE_ENTER_CRITICAL_SECTION() {__irq_status = __get_PRIMASK(); __set_PRIMASK(1);} /* PRIMASK disables ALL interrupts - allows for tracing in any ISR */
	#define TRACE_EXIT_CRITICAL_SECTION() {__set_PRIMASK(__irq_status);}
#endif

#if ((TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_ARM_CORTEX_A9) || (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Renesas_RX600) || (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_MICROCHIP_PIC24_PIC32))
	#define TRACE_ALLOC_CRITICAL_SECTION() int __irq_status;
	#define TRACE_ENTER_CRITICAL_SECTION() {__irq_status = portSET_INTERRUPT_MASK_FROM_ISR();}
	#define TRACE_EXIT_CRITICAL_SECTION() {portCLEAR_INTERRUPT_MASK_FROM_ISR(__irq_status);}
#endif

#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Win32)
    /* In the Win32 port, there are no real interrupts, so we can use the normal critical sections */
	#define TRACE_ALLOC_CRITICAL_SECTION()
	#define TRACE_ENTER_CRITICAL_SECTION() portENTER_CRITICAL()
	#define TRACE_EXIT_CRITICAL_SECTION() portEXIT_CRITICAL()
#endif

#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_TEXAS_INSTRUMENTS_TMS570_RM48)
	extern void prvTraceEnterCritical( void );
	extern void prvTraceExitCritical ( void );
	#define TRACE_ALLOC_CRITICAL_SECTION()
	#define TRACE_ENTER_CRITICAL_SECTION() prvTraceEnterCritical()
	#define TRACE_EXIT_CRITICAL_SECTION() prvTraceExitCritical()
#endif

#ifndef TRACE_ENTER_CRITICAL_SECTION
	#error "This hardware port has no definition for critical sections! See http://percepio.com/2014/10/27/how-to-define-critical-sections-for-the-recorder/"
#endif

extern uint16_t CurrentFilterMask;

extern uint16_t CurrentFilterGroup;

#define TRACE_GET_TASK_FILTER(pxObject) TRACE_GET_HIGH16(prvGetTaskNumber(pxObject))
#define TRACE_SET_TASK_FILTER(pxObject, group) ((xTCB*)pxObject)->uxTaskNumber = TRACE_SET_HIGH16(((xTCB*)pxObject)->uxTaskNumber, group)

#define TRACE_GET_QUEUE_FILTER(pxObject) TRACE_GET_HIGH16(prvGetQueueNumber(pxObject))
#define TRACE_SET_QUEUE_FILTER(pxObject, group) ((xQUEUE*)pxObject)->uxQueueNumber = TRACE_SET_HIGH16(((xQUEUE*)pxObject)->uxQueueNumber, group)

#define TRACE_GET_TIMER_FILTER(pxObject) TRACE_GET_HIGH16(prvGetTimerNumber(pxObject))
#define TRACE_SET_TIMER_FILTER(pxObject, group) ((timerControlBlockType*)pxObject)->uxTimerNumber = TRACE_SET_HIGH16(((timerControlBlockType*)pxObject)->uxTimerNumber, group)

#define TRACE_GET_EVENTGROUP_FILTER(pxObject) TRACE_GET_HIGH16(prvGetEventGroupNumber(pxObject))
#define TRACE_SET_EVENTGROUP_FILTER(pxObject, group) ((eventGroupType*)pxObject)->uxEventGroupNumber = TRACE_SET_HIGH16(((eventGroupType*)pxObject)->uxEventGroupNumber, group)

#define TRACE_GET_OBJECT_FILTER(CLASS, pxObject) TRACE_GET_##CLASS##_FILTER(pxObject)
#define TRACE_SET_OBJECT_FILTER(CLASS, pxObject, group) TRACE_SET_##CLASS##_FILTER(pxObject, group)

/******************************************************************************/
/*** Definitions for Snapshot mode ********************************************/
/******************************************************************************/
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)

/*******************************************************************************
* vTraceSetQueueName(void* object, const char* name)
*
* Parameter object: pointer to the Queue that shall be named
* Parameter name: the name to set (const string literal)
*
* Sets a name for Queue objects for display in Tracealyzer.
******************************************************************************/
#define vTraceSetQueueName(object, name) prvTraceSetObjectName(TRACE_CLASS_QUEUE, TRACE_GET_QUEUE_NUMBER(object), name)

/*******************************************************************************
* vTraceSetSemaphoreName(void* object, const char* name)
*
* Parameter object: pointer to the Semaphore that shall be named
* Parameter name: the name to set (const string literal)
*
* Sets a name for Semaphore objects for display in Tracealyzer.
******************************************************************************/
#define vTraceSetSemaphoreName(object, name) prvTraceSetObjectName(TRACE_CLASS_SEMAPHORE, TRACE_GET_QUEUE_NUMBER(object), name)

/*******************************************************************************
* vTraceSetMutexName(void* object, const char* name)
*
* Parameter object: pointer to the Mutex that shall be named
* Parameter name: the name to set (const string literal)
*
* Sets a name for Mutex objects for display in Tracealyzer.
******************************************************************************/
#define vTraceSetMutexName(object, name) prvTraceSetObjectName(TRACE_CLASS_MUTEX, TRACE_GET_QUEUE_NUMBER(object), name)

/*******************************************************************************
* vTraceSetEventGroupName(void* object, const char* name)
*
* Parameter object: pointer to the EventGroup that shall be named
* Parameter name: the name to set (const string literal)
*
* Sets a name for EventGroup objects for display in Tracealyzer.
******************************************************************************/
#define vTraceSetEventGroupName(object, name) prvTraceSetObjectName(TRACE_CLASS_EVENTGROUP, TRACE_GET_EVENTGROUP_NUMBER(object), name)

/*** The object classes *******************************************************/

#define TRACE_NCLASSES 7
#define TRACE_CLASS_QUEUE		((traceObjectClass)0)
#define TRACE_CLASS_TASK		((traceObjectClass)1)
#define TRACE_CLASS_ISR			((traceObjectClass)2)
#define TRACE_CLASS_TIMER		((traceObjectClass)3)
#define TRACE_CLASS_EVENTGROUP	((traceObjectClass)4)
#define TRACE_CLASS_SEMAPHORE	((traceObjectClass)5)
#define TRACE_CLASS_MUTEX		((traceObjectClass)6)

/*** Definitions for Object Table ********************************************/
#define TRACE_KERNEL_OBJECT_COUNT (TRC_CFG_NQUEUE + TRC_CFG_NTASK + TRC_CFG_NISR + TRC_CFG_NTIMER + TRC_CFG_NEVENTGROUP + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX)

/* Queue properties (except name):	current number of message in queue */
#define PropertyTableSizeQueue		(TRC_CFG_NAME_LEN_QUEUE + 1)

/* Semaphore properties (except name): state (signaled = 1, cleared = 0) */
#define PropertyTableSizeSemaphore	(TRC_CFG_NAME_LEN_SEMAPHORE + 1)

/* Mutex properties (except name):	owner (task handle, 0 = free) */
#define PropertyTableSizeMutex		(TRC_CFG_NAME_LEN_MUTEX + 1)

/* Task properties (except name):	Byte 0: Current priority
									Byte 1: state (if already active)
									Byte 2: legacy, not used
									Byte 3: legacy, not used */
#define PropertyTableSizeTask		(TRC_CFG_NAME_LEN_TASK + 4)

/* ISR properties:					Byte 0: priority
									Byte 1: state (if already active) */
#define PropertyTableSizeISR		(TRC_CFG_NAME_LEN_ISR + 2)

/* TRC_CFG_NTIMER properties:				Byte 0: state (unused for now) */
#define PropertyTableSizeTimer		(TRC_CFG_NAME_LEN_TIMER + 1)

/* TRC_CFG_NEVENTGROUP properties:			Byte 0-3: state (unused for now)*/
#define PropertyTableSizeEventGroup	(TRC_CFG_NAME_LEN_EVENTGROUP + 4)


/* The layout of the byte array representing the Object Property Table */
#define StartIndexQueue			0
#define StartIndexTask			(StartIndexQueue		+ (TRC_CFG_NQUEUE		* PropertyTableSizeQueue))
#define StartIndexISR			(StartIndexTask			+ (TRC_CFG_NTASK		* PropertyTableSizeTask))
#define StartIndexTimer			(StartIndexISR			+ (TRC_CFG_NISR			* PropertyTableSizeISR))
#define StartIndexEventGroup	(StartIndexTimer		+ (TRC_CFG_NTIMER		* PropertyTableSizeTimer))
#define StartIndexSemaphore		(StartIndexEventGroup	+ (TRC_CFG_NEVENTGROUP	* PropertyTableSizeEventGroup))
#define StartIndexMutex			(StartIndexSemaphore	+ (TRC_CFG_NSEMAPHORE	* PropertyTableSizeSemaphore))

/* Number of bytes used by the object table */
#define TRACE_OBJECT_TABLE_SIZE	(StartIndexMutex + TRC_CFG_NMUTEX * PropertyTableSizeMutex)

#if (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1 && configUSE_TICKLESS_IDLE != 0)
#error "OS Tick events can not be traced in combination with tickless idle!"
#endif

/* Flag to tell the context of tracePEND_FUNC_CALL_FROM_ISR */
extern int uiInEventGroupSetBitsFromISR;

/* Initialization of the object property table */
void vTraceInitObjectPropertyTable(void);

/* Initialization of the handle mechanism, see e.g, prvTraceGetObjectHandle */
void vTraceInitObjectHandleStack(void);

/* Returns the "Not enough handles" error message for the specified object class */
const char* pszTraceGetErrorNotEnoughHandles(traceObjectClass objectclass);

/******************************************************************************
 * TraceQueueClassTable
 * Translates a QueueType into trace objects classes (TRACE_CLASS_).
 * Has one entry for each QueueType, gives TRACE_CLASS ID.
 ******************************************************************************/			
 extern traceObjectClass TraceQueueClassTable[3];

/*** Event codes for snapshot mode - must match Tracealyzer config files ******/

#define NULL_EVENT					(0x00UL)								/*0x00*/

/*******************************************************************************
 * EVENTGROUP_DIV
 *
 * Miscellaneous events.
 ******************************************************************************/
#define EVENTGROUP_DIV				(NULL_EVENT + 1UL)						/*0x01*/
#define DIV_XPS						(EVENTGROUP_DIV + 0UL)					/*0x01*/
#define DIV_TASK_READY				(EVENTGROUP_DIV + 1UL)					/*0x02*/
#define DIV_NEW_TIME				(EVENTGROUP_DIV + 2UL)					/*0x03*/

/*******************************************************************************
 * EVENTGROUP_TS
 *
 * Events for storing task-switches and interrupts. The RESUME events are
 * generated if the task/interrupt is already marked active.
 ******************************************************************************/
#define EVENTGROUP_TS				(EVENTGROUP_DIV + 3UL)				/*0x04*/
#define TS_ISR_BEGIN				(EVENTGROUP_TS + 0UL)					/*0x04*/
#define TS_ISR_RESUME				(EVENTGROUP_TS + 1UL)					/*0x05*/
#define TS_TASK_BEGIN				(EVENTGROUP_TS + 2UL)					/*0x06*/
#define TS_TASK_RESUME				(EVENTGROUP_TS + 3UL)					/*0x07*/

/*******************************************************************************
 * EVENTGROUP_OBJCLOSE_NAME
 *
 * About Close Events
 * When an object is evicted from the object property table (object close), two
 * internal events are stored (EVENTGROUP_OBJCLOSE_NAME and
 * EVENTGROUP_OBJCLOSE_PROP), containing the handle-name mapping and object
 * properties valid up to this point.
 ******************************************************************************/
#define EVENTGROUP_OBJCLOSE_NAME_TRCSUCCESS	(EVENTGROUP_TS + 4UL)			/*0x08*/

/*******************************************************************************
 * EVENTGROUP_OBJCLOSE_PROP
 *
 * The internal event carrying properties of deleted objects
 * The handle and object class of the closed object is not stored in this event,
 * but is assumed to be the same as in the preceding CLOSE event. Thus, these
 * two events must be generated from within a critical section.
 * When queues are closed, arg1 is the "state" property (i.e., number of
 * buffered messages/signals).
 * When actors are closed, arg1 is priority, arg2 is handle of the "instance
 * finish" event, and arg3 is event code of the "instance finish" event.
 * In this case, the lower three bits is the object class of the instance finish
 * handle. The lower three bits are not used (always zero) when queues are
 * closed since the queue type is given in the previous OBJCLOSE_NAME event.
 ******************************************************************************/
#define EVENTGROUP_OBJCLOSE_PROP_TRCSUCCESS	(EVENTGROUP_OBJCLOSE_NAME_TRCSUCCESS + 8UL)	/*0x10*/

/*******************************************************************************
 * EVENTGROUP_CREATE
 *
 * The events in this group are used to log Kernel object creations.
 * The lower three bits in the event code gives the object class, i.e., type of
 * create operation (task, queue, semaphore, etc).
 ******************************************************************************/
#define EVENTGROUP_CREATE_OBJ_TRCSUCCESS	(EVENTGROUP_OBJCLOSE_PROP_TRCSUCCESS + 8UL)	/*0x18*/

/*******************************************************************************
 * EVENTGROUP_SEND
 *
 * The events in this group are used to log Send/Give events on queues,
 * semaphores and mutexes The lower three bits in the event code gives the
 * object class, i.e., what type of object that is operated on (queue, semaphore
 * or mutex).
 ******************************************************************************/
#define EVENTGROUP_SEND_TRCSUCCESS	(EVENTGROUP_CREATE_OBJ_TRCSUCCESS + 8UL)		/*0x20*/

/*******************************************************************************
 * EVENTGROUP_RECEIVE
 *
 * The events in this group are used to log Receive/Take events on queues,
 * semaphores and mutexes. The lower three bits in the event code gives the
 * object class, i.e., what type of object that is operated on (queue, semaphore
 * or mutex).
 ******************************************************************************/
#define EVENTGROUP_RECEIVE_TRCSUCCESS	(EVENTGROUP_SEND_TRCSUCCESS + 8UL)			/*0x28*/

/* Send/Give operations, from ISR */
#define EVENTGROUP_SEND_FROM_ISR_TRCSUCCESS \
									(EVENTGROUP_RECEIVE_TRCSUCCESS + 8UL)		/*0x30*/

/* Receive/Take operations, from ISR */
#define EVENTGROUP_RECEIVE_FROM_ISR_TRCSUCCESS \
							(EVENTGROUP_SEND_FROM_ISR_TRCSUCCESS + 8UL)		/*0x38*/

/* Failed create calls - memory allocation failed */
#define EVENTGROUP_CREATE_OBJ_TRCFAILED	(EVENTGROUP_RECEIVE_FROM_ISR_TRCSUCCESS + 8UL)	/*0x40*/

/* Failed send/give - timeout! */
#define EVENTGROUP_SEND_TRCFAILED		(EVENTGROUP_CREATE_OBJ_TRCFAILED + 8UL)	/*0x48*/

/* Failed receive/take - timeout! */
#define EVENTGROUP_RECEIVE_TRCFAILED	 (EVENTGROUP_SEND_TRCFAILED + 8UL)			/*0x50*/

/* Failed non-blocking send/give - queue full */
#define EVENTGROUP_SEND_FROM_ISR_TRCFAILED (EVENTGROUP_RECEIVE_TRCFAILED + 8UL)	/*0x58*/

/* Failed non-blocking receive/take - queue empty */
#define EVENTGROUP_RECEIVE_FROM_ISR_TRCFAILED \
								 (EVENTGROUP_SEND_FROM_ISR_TRCFAILED + 8UL)	/*0x60*/

/* Events when blocking on receive/take */
#define EVENTGROUP_RECEIVE_TRCBLOCK \
							(EVENTGROUP_RECEIVE_FROM_ISR_TRCFAILED + 8UL)		/*0x68*/

/* Events when blocking on send/give */
#define EVENTGROUP_SEND_TRCBLOCK	(EVENTGROUP_RECEIVE_TRCBLOCK + 8UL)			/*0x70*/

/* Events on queue peek (receive) */
#define EVENTGROUP_PEEK_TRCSUCCESS	(EVENTGROUP_SEND_TRCBLOCK + 8UL)				/*0x78*/

/* Events on object delete (vTaskDelete or vQueueDelete) */
#define EVENTGROUP_DELETE_OBJ_TRCSUCCESS	(EVENTGROUP_PEEK_TRCSUCCESS + 8UL)		/*0x80*/

/* Other events - object class is implied: TASK */
#define EVENTGROUP_OTHERS	(EVENTGROUP_DELETE_OBJ_TRCSUCCESS + 8UL)			/*0x88*/
#define TASK_DELAY_UNTIL	(EVENTGROUP_OTHERS + 0UL)						/*0x88*/
#define TASK_DELAY			(EVENTGROUP_OTHERS + 1UL)						/*0x89*/
#define TASK_SUSPEND		(EVENTGROUP_OTHERS + 2UL)						/*0x8A*/
#define TASK_RESUME			(EVENTGROUP_OTHERS + 3UL)						/*0x8B*/
#define TASK_RESUME_FROM_ISR	(EVENTGROUP_OTHERS + 4UL)					/*0x8C*/
#define TASK_PRIORITY_SET		(EVENTGROUP_OTHERS + 5UL)					/*0x8D*/
#define TASK_PRIORITY_INHERIT	(EVENTGROUP_OTHERS + 6UL)					/*0x8E*/
#define TASK_PRIORITY_DISINHERIT	(EVENTGROUP_OTHERS + 7UL)				/*0x8F*/

/* User events */
#define EVENTGROUP_USEREVENT (EVENTGROUP_OTHERS + 8UL)						/*0x90*/
#define USER_EVENT (EVENTGROUP_USEREVENT + 0UL)

/* Allow for 0-15 arguments (the number of args is added to event code) */
#define USER_EVENT_LAST (EVENTGROUP_USEREVENT + 15UL)						/*0x9F*/

/*******************************************************************************
 * XTS Event - eXtended TimeStamp events
 * The timestamps used in the recorder are "differential timestamps" (DTS), i.e.
 * the time since the last stored event. The DTS fields are either 1 or 2 bytes
 * in the other events, depending on the bytes available in the event struct.
 * If the time since the last event (the DTS) is larger than allowed for by
 * the DTS field of the current event, an XTS event is inserted immediately
 * before the original event. The XTS event contains up to 3 additional bytes
 * of the DTS value - the higher bytes of the true DTS value. The lower 1-2
 * bytes are stored in the normal DTS field.
 * There are two types of XTS events, XTS8 and XTS16. An XTS8 event is stored
 * when there is only room for 1 byte (8 bit) DTS data in the original event,
 * which means a limit of 0xFF (255UL). The XTS16 is used when the original event
 * has a 16 bit DTS field and thereby can handle values up to 0xFFFF (65535UL).
 *
 * Using a very high frequency time base can result in many XTS events.
 * Preferably, the time between two OS ticks should fit in 16 bits, i.e.,
 * at most 65535. If your time base has a higher frequency, you can define
 * the TRACE
 ******************************************************************************/

#define EVENTGROUP_SYS									(USER_EVENT_LAST + 1UL)						/*0xA0*/
#define XTS8											(EVENTGROUP_SYS + 0UL)						/*0xA0*/
#define XTS16											(EVENTGROUP_SYS + 1UL)						/*0xA1*/
#define EVENT_BEING_WRITTEN								(EVENTGROUP_SYS + 2UL)						/*0xA2*/
#define RESERVED_DUMMY_CODE								(EVENTGROUP_SYS + 3UL)						/*0xA3*/
#define LOW_POWER_BEGIN									(EVENTGROUP_SYS + 4UL)						/*0xA4*/
#define LOW_POWER_END									(EVENTGROUP_SYS + 5UL)						/*0xA5*/
#define XID												(EVENTGROUP_SYS + 6UL)						/*0xA6*/
#define XTS16L											(EVENTGROUP_SYS + 7UL)						/*0xA7*/

#define TASK_INSTANCE_FINISHED_NEXT_KSE					(EVENTGROUP_SYS + 8UL)						/*0xA8*/
#define TASK_INSTANCE_FINISHED_DIRECT					(EVENTGROUP_SYS + 9UL)						/*0xA9*/

#define EVENTGROUP_TIMER								(EVENTGROUP_SYS + 10UL)						/*0xAA*/
#define TIMER_CREATE									(EVENTGROUP_TIMER + 0UL)					/*0xAA*/
#define TIMER_DELETE_OBJ								(EVENTGROUP_TIMER + 1UL)					/*0xAB*/
#define TIMER_START										(EVENTGROUP_TIMER + 2UL)					/*0xAC*/
#define TIMER_STOP										(EVENTGROUP_TIMER + 3UL)					/*0xAD*/
#define TIMER_CHANGE_PERIOD								(EVENTGROUP_TIMER + 4UL)					/*0xAE*/
#define TIMER_START_FROM_ISR							(EVENTGROUP_TIMER + 5UL)					/*0xAF*/
#define TIMER_STOP_FROM_ISR								(EVENTGROUP_TIMER + 6UL)					/*0xB0*/
#define TIMER_CHANGE_PERIOD_FROM_ISR					(EVENTGROUP_TIMER + 7UL)					/*0xB1*/

#define TIMER_CREATE_TRCFAILED							(EVENTGROUP_TIMER + 8UL)					/*0xB2*/
#define TIMER_DELETE_OBJ_TRCFAILED						(EVENTGROUP_TIMER + 9UL)					/*0xB3*/
#define TIMER_START_TRCFAILED							(EVENTGROUP_TIMER + 10UL)					/*0xB4*/
#define TIMER_STOP_TRCFAILED							(EVENTGROUP_TIMER + 11UL)					/*0xB5*/
#define TIMER_CHANGE_PERIOD_TRCFAILED					(EVENTGROUP_TIMER + 12UL)					/*0xB6*/
#define TIMER_START_FROM_ISR_TRCFAILED					(EVENTGROUP_TIMER + 13UL)					/*0xB7*/
#define TIMER_STOP_FROM_ISR_TRCFAILED					(EVENTGROUP_TIMER + 14UL)					/*0xB8*/
#define TIMER_CHANGE_PERIOD_FROM_ISR_TRCFAILED			(EVENTGROUP_TIMER + 15UL)					/*0xB9*/

#define TIMER_PROCESS_START								(EVENTGROUP_TIMER + 16UL)					/*0xBA*/
#define TIMER_PROCESS_STOP								(EVENTGROUP_TIMER + 17UL)					/*0xBB*/
#define TIMER_PROCESS_CHANGE_PERIOD						(EVENTGROUP_TIMER + 18UL)					/*0xBC*/
#define TIMER_PROCESS_DELETE							(EVENTGROUP_TIMER + 19UL)					/*0xBD*/
#define TIMER_CALLBACK									(EVENTGROUP_TIMER + 20UL)					/*0xBE*/

#define EVENTGROUP_EG									(EVENTGROUP_TIMER + 21UL)					/*0xBF*/
#define EVENT_GROUP_CREATE								(EVENTGROUP_EG + 0UL)						/*0xBF*/
#define EVENT_GROUP_CREATE_TRCFAILED					(EVENTGROUP_EG + 1UL)						/*0xC0*/
#define EVENT_GROUP_DELETE_OBJ							(EVENTGROUP_EG + 2UL)						/*0xC1*/
#define EVENT_GROUP_DELETE_OBJ_TRCFAILED				(EVENTGROUP_EG + 3UL)						/*0xC2*/
#define EVENT_GROUP_SET_BITS							(EVENTGROUP_EG + 4UL)						/*0xC3*/
#define EVENT_GROUP_SET_BITS_TRCFAILED					(EVENTGROUP_EG + 5UL)						/*0xC4*/
#define EVENT_GROUP_SET_BITS_FROM_ISR					(EVENTGROUP_EG + 6UL)						/*0xC5*/
#define EVENT_GROUP_SET_BITS_FROM_ISR_TRCFAILED			(EVENTGROUP_EG + 7UL)						/*0xC6*/
#define EVENT_GROUP_WAIT_BITS							(EVENTGROUP_EG + 8UL)						/*0xC7*/
#define EVENT_GROUP_WAIT_BITS_TRCBLOCK					(EVENTGROUP_EG + 9UL)						/*0xC8*/
#define EVENT_GROUP_WAIT_BITS_TRCFAILED					(EVENTGROUP_EG + 10UL)						/*0xC9*/
#define EVENT_GROUP_CLEAR_BITS							(EVENTGROUP_EG + 11UL)						/*0xCA*/
#define EVENT_GROUP_CLEAR_BITS_TRCFAILED				(EVENTGROUP_EG + 12UL)						/*0xCB*/
#define EVENT_GROUP_CLEAR_BITS_FROM_ISR					(EVENTGROUP_EG + 13UL)						/*0xCC*/
#define EVENT_GROUP_CLEAR_BITS_FROM_ISR_TRCFAILED		(EVENTGROUP_EG + 14UL)						/*0xCD*/

#define TRACE_TASK_NOTIFY_GROUP							(EVENTGROUP_EG + 15UL)						/*0xCE*/
#define TRACE_TASK_NOTIFY_SEND_TRCSUCCESS				(TRACE_TASK_NOTIFY_GROUP + 0UL)				/*0xCE*/
#define TRACE_TASK_NOTIFY_SEND_TRCFAILED				(TRACE_TASK_NOTIFY_GROUP + 1UL)				/*0xCF*/
#define TRACE_TASK_NOTIFY_SEND_FROM_ISR_TRCSUCCESS		(TRACE_TASK_NOTIFY_GROUP + 2UL)				/*0xD0*/
#define TRACE_TASK_NOTIFY_SEND_FROM_ISR_TRCFAILED		(TRACE_TASK_NOTIFY_GROUP + 3UL)				/*0xD1*/
#define TRACE_TASK_NOTIFY_WAIT_TRCSUCCESS				(TRACE_TASK_NOTIFY_GROUP + 4UL)				/*0xD2*/
#define TRACE_TASK_NOTIFY_WAIT_TRCBLOCK					(TRACE_TASK_NOTIFY_GROUP + 5UL)				/*0xD3*/
#define TRACE_TASK_NOTIFY_WAIT_TRCFAILED				(TRACE_TASK_NOTIFY_GROUP + 6UL)				/*0xD4*/

#define EVENTGROUP_QUEUE_EXTRAS							(TRACE_TASK_NOTIFY_GROUP + 7UL)				/*0xD5*/
#define TRACE_QUEUE_PEEK_TRCBLOCK						(EVENTGROUP_QUEUE_EXTRAS + 0UL)				/*0xD5*/
#define TRACE_QUEUE_PEEK_TRCFAILED						(EVENTGROUP_QUEUE_EXTRAS + 1UL)				/*0xD6*/
#define TRACE_QUEUE_SEND_TO_FRONT_TRCSUCCESS			(EVENTGROUP_QUEUE_EXTRAS + 2UL)				/*0xD7*/
#define TRACE_QUEUE_SEND_TO_FRONT_TRCBLOCK				(EVENTGROUP_QUEUE_EXTRAS + 3UL)				/*0xD8*/
#define TRACE_QUEUE_SEND_TO_FRONT_TRCFAILED				(EVENTGROUP_QUEUE_EXTRAS + 4UL)				/*0xD9*/
#define TRACE_QUEUE_SEND_TO_FRONT_FROM_ISR_TRCSUCCESS	(EVENTGROUP_QUEUE_EXTRAS + 5UL)				/*0xDA*/
#define TRACE_QUEUE_SEND_TO_FRONT_FROM_ISR_TRCFAILED	(EVENTGROUP_QUEUE_EXTRAS + 6UL)				/*0xDB*/

 /* LAST EVENT (0xDB) */

 /****************************
 * MACROS TO GET TRACE CLASS *
 ****************************/
#define TRACE_GET_TRACE_CLASS_FROM_TASK_CLASS(kernelClass) (TRACE_CLASS_TASK)
#define TRACE_GET_TRACE_CLASS_FROM_TASK_OBJECT(pxObject) (TRACE_CLASS_TASK)

#define TRACE_GET_TRACE_CLASS_FROM_QUEUE_CLASS(kernelClass) (TraceQueueClassTable[kernelClass])
#define TRACE_GET_TRACE_CLASS_FROM_QUEUE_OBJECT(pxObject) (TRACE_GET_TRACE_CLASS_FROM_QUEUE_CLASS(prvTraceGetQueueType(pxObject)))

#define TRACE_GET_TRACE_CLASS_FROM_TIMER_CLASS(kernelClass) (TRACE_CLASS_TIMER)
#define TRACE_GET_TRACE_CLASS_FROM_TIMER_OBJECT(pxObject) (TRACE_CLASS_TIMER)

#define TRACE_GET_TRACE_CLASS_FROM_EVENTGROUP_CLASS(kernelClass) (TRACE_CLASS_EVENTGROUP)
#define TRACE_GET_TRACE_CLASS_FROM_EVENTGROUP_OBJECT(pxObject) (TRACE_CLASS_EVENTGROUP)

/* Generic versions */
#define TRACE_GET_CLASS_TRACE_CLASS(CLASS, kernelClass) TRACE_GET_TRACE_CLASS_FROM_##CLASS##_CLASS(kernelClass)
#define TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject) TRACE_GET_TRACE_CLASS_FROM_##CLASS##_OBJECT(pxObject)

/******************************
* MACROS TO GET OBJECT NUMBER *
******************************/
#define TRACE_GET_TASK_NUMBER(pxTCB) ((traceHandle)TRACE_GET_LOW16(prvGetTaskNumber(pxTCB)))
#define TRACE_SET_TASK_NUMBER(pxTCB) ((xTCB*)pxTCB)->uxTaskNumber = TRACE_SET_LOW16(((xTCB*)pxTCB)->uxTaskNumber, prvTraceGetObjectHandle(TRACE_CLASS_TASK));

#define TRACE_GET_QUEUE_NUMBER(q) ((traceHandle)TRACE_GET_LOW16(prvGetQueueNumber(q)))
#define TRACE_SET_QUEUE_NUMBER(q) ((xQUEUE*)q)->uxQueueNumber = TRACE_SET_LOW16(((xQUEUE*)q)->uxQueueNumber, prvTraceGetObjectHandle(TRACE_CLASS_QUEUE));

#define TRACE_GET_TIMER_NUMBER(tmr) ((traceHandle)TRACE_GET_LOW16(prvGetTimerNumber(tmr)))
#define TRACE_SET_TIMER_NUMBER(tmr) ((timerControlBlockType*)tmr)->uxTimerNumber = TRACE_SET_LOW16(((timerControlBlockType*)tmr)->uxTimerNumber, prvTraceGetObjectHandle(TRACE_CLASS_TIMER));

#define TRACE_GET_EVENTGROUP_NUMBER(eg) ((traceHandle)TRACE_GET_LOW16(prvGetEventGroupNumber(eg)))
#define TRACE_SET_EVENTGROUP_NUMBER(eg) ((eventGroupType*)eg)->uxEventGroupNumber = TRACE_SET_LOW16(((eventGroupType*)eg)->uxEventGroupNumber, prvTraceGetObjectHandle(TRACE_CLASS_EVENTGROUP));

/* Generic versions */
#define TRACE_GET_OBJECT_NUMBER(CLASS, pxObject) TRACE_GET_##CLASS##_NUMBER(pxObject)
#define TRACE_SET_OBJECT_NUMBER(CLASS, pxObject) TRACE_SET_##CLASS##_NUMBER(pxObject)

/******************************
* MACROS TO GET EVENT CODES   *
******************************/
#define TRACE_GET_TASK_CLASS_EVENT_CODE(SERVICE, RESULT, kernelClass) (uint8_t)(EVENTGROUP_##SERVICE##_##RESULT + TRACE_GET_CLASS_TRACE_CLASS(TASK, kernelClass))
#define TRACE_GET_QUEUE_CLASS_EVENT_CODE(SERVICE, RESULT, kernelClass) (uint8_t)(EVENTGROUP_##SERVICE##_##RESULT + TRACE_GET_CLASS_TRACE_CLASS(QUEUE, kernelClass))
#define TRACE_GET_TIMER_CLASS_EVENT_CODE(SERVICE, RESULT, kernelClass) -- THIS IS NOT USED --
#define TRACE_GET_EVENTGROUP_CLASS_EVENT_CODE(SERVICE, RESULT, kernelClass) -- THIS IS NOT USED --

#define TRACE_GET_TASK_OBJECT_EVENT_CODE(SERVICE, RESULT, pxTCB) (uint8_t)(EVENTGROUP_##SERVICE##_##RESULT + TRACE_GET_OBJECT_TRACE_CLASS(TASK, pxTCB))
#define TRACE_GET_QUEUE_OBJECT_EVENT_CODE(SERVICE, RESULT, pxObject) (uint8_t)(EVENTGROUP_##SERVICE##_##RESULT + TRACE_GET_OBJECT_TRACE_CLASS(QUEUE, pxObject))
#define TRACE_GET_TIMER_OBJECT_EVENT_CODE(SERVICE, RESULT, UNUSED) -- THIS IS NOT USED --
#define TRACE_GET_EVENTGROUP_OBJECT_EVENT_CODE(SERVICE, RESULT, UNUSED) -- THIS IS NOT USED --

/* Generic versions */
#define TRACE_GET_CLASS_EVENT_CODE(SERVICE, RESULT, CLASS, kernelClass) TRACE_GET_##CLASS##_CLASS_EVENT_CODE(SERVICE, RESULT, kernelClass)
#define TRACE_GET_OBJECT_EVENT_CODE(SERVICE, RESULT, CLASS, pxObject) TRACE_GET_##CLASS##_OBJECT_EVENT_CODE(SERVICE, RESULT, pxObject)

/******************************
* SPECIAL MACROS FOR TASKS    *
******************************/
#define TRACE_GET_TASK_PRIORITY(pxTCB) ((uint8_t)pxTCB->uxPriority)
#define TRACE_GET_TASK_NAME(pxTCB) ((char*)pxTCB->pcNameOfTask)

/*** The trace macros for snapshot mode **************************************/
			

/* A macro that will update the tick count when returning from tickless idle */
#undef traceINCREASE_TICK_COUNT
/* Note: This can handle time adjustments of max 2^32 ticks, i.e., 35 seconds at 120 MHz. Thus, tick-less idle periods longer than 2^32 ticks will appear "compressed" on the time line.*/
#define traceINCREASE_TICK_COUNT( xCount ) { TRC_DWT_CYCLES_ADDED += (xCount * (TRACE_CPU_CLOCK_HZ / TRACE_TICK_RATE_HZ)); }

/* Called for each task that becomes ready */
#if (TRC_CFG_INCLUDE_READY_EVENTS == 1)
#undef traceMOVED_TASK_TO_READY_STATE
#define traceMOVED_TASK_TO_READY_STATE( pxTCB ) \
	trcKERNEL_HOOKS_MOVED_TASK_TO_READY_STATE(pxTCB);
#else /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */
#define traceMOVED_TASK_TO_READY_STATE( pxTCB ) 
#endif /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */

/* Called on each OS tick. Will call uiPortGetTimestamp to make sure it is called at least once every OS tick. */
#undef traceTASK_INCREMENT_TICK
#define traceTASK_INCREMENT_TICK( xTickCount ) \
	if (uxSchedulerSuspended == ( portUnsignedBaseType ) pdTRUE || uxMissedTicks == 0) { trcKERNEL_HOOKS_INCREMENT_TICK(); } \
	if (uxSchedulerSuspended == ( portUnsignedBaseType ) pdFALSE) { trcKERNEL_HOOKS_NEW_TIME(DIV_NEW_TIME, xTickCount + 1); }

/* Called on each task-switch */
#undef traceTASK_SWITCHED_IN
#define traceTASK_SWITCHED_IN() \
	trcKERNEL_HOOKS_TASK_SWITCH(TRACE_GET_CURRENT_TASK());

/* Called on vTaskCreate */
#undef traceTASK_CREATE
#define traceTASK_CREATE(pxNewTCB) \
	trcKERNEL_HOOKS_TASK_CREATE(TRACE_GET_OBJECT_EVENT_CODE(CREATE_OBJ, TRCSUCCESS, TASK, pxNewTCB), TASK, pxNewTCB);

/* Called in vTaskCreate, if it fails (typically if the stack can not be allocated) */
#undef traceTASK_CREATE_FAILED
#define traceTASK_CREATE_FAILED() \
	trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(TRACE_GET_CLASS_EVENT_CODE(CREATE_OBJ, TRCFAILED, TASK, NOT_USED), TRACE_GET_CLASS_TRACE_CLASS(TASK, NOT_USED));

/* Called on vTaskDelete */
#undef traceTASK_DELETE
#define traceTASK_DELETE( pxTaskToDelete ) \
	{ TRACE_ALLOC_CRITICAL_SECTION(); \
	TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_TASK_DELETE(TRACE_GET_OBJECT_EVENT_CODE(DELETE_OBJ, TRCSUCCESS, TASK, pxTaskToDelete), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_NAME, TRCSUCCESS, TASK, pxTaskToDelete), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_PROP, TRCSUCCESS, TASK, pxTaskToDelete), pxTaskToDelete); \
	TRACE_EXIT_CRITICAL_SECTION(); }

#if (TRC_CFG_SCHEDULING_ONLY == 0)

#if defined(configUSE_TICKLESS_IDLE)
#if (configUSE_TICKLESS_IDLE != 0)

#undef traceLOW_POWER_IDLE_BEGIN
#define traceLOW_POWER_IDLE_BEGIN() \
	{ \
		extern uint32_t trace_disable_timestamp; \
		prvTraceStoreLowPower(0); \
		trace_disable_timestamp = 1; \
	}

#undef traceLOW_POWER_IDLE_END
#define traceLOW_POWER_IDLE_END() \
	{ \
		extern uint32_t trace_disable_timestamp; \
		trace_disable_timestamp = 0; \
		prvTraceStoreLowPower(1); \
	}

#endif /* (configUSE_TICKLESS_IDLE != 0) */
#endif /* defined(configUSE_TICKLESS_IDLE)  */

/* Called on vTaskSuspend */
#undef traceTASK_SUSPEND
#define traceTASK_SUSPEND( pxTaskToSuspend ) \
	trcKERNEL_HOOKS_TASK_SUSPEND(TASK_SUSPEND, pxTaskToSuspend);

/* Called from special case with timer only */
#undef traceTASK_DELAY_SUSPEND
#define traceTASK_DELAY_SUSPEND( pxTaskToSuspend ) \
	trcKERNEL_HOOKS_TASK_SUSPEND(TASK_SUSPEND, pxTaskToSuspend); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

/* Called on vTaskDelay - note the use of variable xTicksToDelay */
#undef traceTASK_DELAY
#define traceTASK_DELAY() \
	trcKERNEL_HOOKS_TASK_DELAY(TASK_DELAY, pxCurrentTCB, xTicksToDelay); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

/* Called on vTaskDelayUntil - note the use of variable xTimeToWake */
#undef traceTASK_DELAY_UNTIL
#define traceTASK_DELAY_UNTIL(xTimeToWake) \
	trcKERNEL_HOOKS_TASK_DELAY(TASK_DELAY_UNTIL, pxCurrentTCB, xTimeToWake); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

/* Called in xQueueCreate, and thereby for all other object based on queues, such as semaphores. */
#undef traceQUEUE_CREATE
#define traceQUEUE_CREATE( pxNewQueue ) \
	trcKERNEL_HOOKS_OBJECT_CREATE(TRACE_GET_OBJECT_EVENT_CODE(CREATE_OBJ, TRCSUCCESS, QUEUE, pxNewQueue), QUEUE, pxNewQueue);

/* Called in xQueueCreate, if the queue creation fails */
#undef traceQUEUE_CREATE_FAILED
#define traceQUEUE_CREATE_FAILED( pcBuffer, queueType, queueLength ) \
	switch( queueType ) \
	{ \
		case queueQUEUE_IS_QUEUE: \
			trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(TRACE_GET_CLASS_EVENT_CODE(CREATE_OBJ, TRCFAILED, QUEUE, traceQueueType), TRACE_GET_CLASS_TRACE_CLASS(QUEUE, traceQueueType)); \
			break; \
		case queueQUEUE_IS_SEMAPHORE: \
			trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(TRACE_GET_CLASS_EVENT_CODE(CREATE_OBJ, TRCFAILED, QUEUE, traceSemaphoreType), TRACE_GET_CLASS_TRACE_CLASS(QUEUE, traceSemaphoreType)); \
			break; \
		case queueQUEUE_IS_MUTEX: \
			trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(TRACE_GET_CLASS_EVENT_CODE(CREATE_OBJ, TRCFAILED, QUEUE, traceMutexType), TRACE_GET_CLASS_TRACE_CLASS(QUEUE, traceMutexType)); \
			break; \
	}

/***************************************************/

/* Called when a message is sent to a queue */
#undef traceQUEUE_SEND
#define traceQUEUE_SEND( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(xPosition == queueSEND_TO_BACK ? TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCSUCCESS, QUEUE, pxQueue) : TRACE_QUEUE_SEND_TO_FRONT_TRCSUCCESS, QUEUE, pxQueue); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(QUEUE, pxQueue, (uint8_t)(pxQueue->uxItemsWaiting + 1));

/* Called when a message failed to be sent to a queue (timeout) */
#undef traceQUEUE_SEND_FAILED
#define traceQUEUE_SEND_FAILED( pxQueue ) \
	if (trcIS_QUEUE_VALID(pxQueue)) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE(xPosition == queueSEND_TO_BACK ? TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCFAILED, QUEUE, pxQueue) : TRACE_QUEUE_SEND_TO_FRONT_TRCFAILED, QUEUE, pxQueue); \
	} \
	else \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(xPosition == queueSEND_TO_BACK ? TRACE_GET_CLASS_EVENT_CODE(SEND, TRCFAILED, QUEUE, traceQueueType) : TRACE_QUEUE_SEND_TO_FRONT_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(QUEUE, traceQueueType)); \
	}

/* Called when the task is blocked due to a send operation on a full queue */
#undef traceBLOCKING_ON_QUEUE_SEND
#define traceBLOCKING_ON_QUEUE_SEND( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(xPosition == queueSEND_TO_BACK ? TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCBLOCK, QUEUE, pxQueue) : TRACE_QUEUE_SEND_TO_FRONT_TRCBLOCK, QUEUE, pxQueue);

/* Called when a message is received from a queue */
#undef traceQUEUE_RECEIVE
#define traceQUEUE_RECEIVE( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCSUCCESS, QUEUE, pxQueue), QUEUE, pxQueue); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(QUEUE, pxQueue, (uint8_t)(pxQueue->uxItemsWaiting - 1));

/* Called when a receive operation on a queue fails (timeout) */
#undef traceQUEUE_RECEIVE_FAILED
#define traceQUEUE_RECEIVE_FAILED( pxQueue ) \
	if (trcIS_QUEUE_VALID(pxQueue)) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCFAILED, QUEUE, pxQueue), QUEUE, pxQueue); \
	} \
	else \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(TRACE_GET_CLASS_EVENT_CODE(RECEIVE, TRCFAILED, QUEUE, traceQueueType), TRACE_GET_CLASS_TRACE_CLASS(QUEUE, traceQueueType)); \
	}

/* Called when the task is blocked due to a receive operation on an empty queue */
#undef traceBLOCKING_ON_QUEUE_RECEIVE
#define traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCBLOCK, QUEUE, pxQueue), QUEUE, pxQueue); \
	if (pxQueue->uxQueueType != queueQUEUE_IS_MUTEX) \
	{ \
		trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED(); \
	}

/* Called on xQueuePeek */
#undef traceQUEUE_PEEK
#define traceQUEUE_PEEK( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(PEEK, TRCSUCCESS, QUEUE, pxQueue), QUEUE, pxQueue);

/* Called on xQueuePeek */
#undef traceBLOCKING_ON_QUEUE_PEEK
#define traceBLOCKING_ON_QUEUE_PEEK( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_QUEUE_PEEK_TRCBLOCK, QUEUE, pxQueue); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

/* Called on xQueuePeek */
#undef traceQUEUE_PEEK_FAILED
#define traceQUEUE_PEEK_FAILED( pxQueue ) \
	if (trcIS_QUEUE_VALID(pxQueue)) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_QUEUE_PEEK_TRCFAILED, QUEUE, pxQueue); \
	} \
	else \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(TRACE_QUEUE_PEEK_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(QUEUE, traceQueueType)); \
	}

/* Called when a message is sent from interrupt context, e.g., using xQueueSendFromISR */
#undef traceQUEUE_SEND_FROM_ISR
#define traceQUEUE_SEND_FROM_ISR( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR(xPosition == queueSEND_TO_BACK ? TRACE_GET_OBJECT_EVENT_CODE(SEND_FROM_ISR, TRCSUCCESS, QUEUE, pxQueue) : TRACE_QUEUE_SEND_TO_FRONT_FROM_ISR_TRCSUCCESS, QUEUE, pxQueue); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(QUEUE, pxQueue, (uint8_t)(pxQueue->uxItemsWaiting + 1));

/* Called when a message send from interrupt context fails (since the queue was full) */
#undef traceQUEUE_SEND_FROM_ISR_FAILED
#define traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue ) \
	if (trcIS_QUEUE_VALID(pxQueue)) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR(xPosition == queueSEND_TO_BACK ? TRACE_GET_OBJECT_EVENT_CODE(SEND_FROM_ISR, TRCFAILED, QUEUE, pxQueue) : TRACE_QUEUE_SEND_TO_FRONT_FROM_ISR_TRCFAILED, QUEUE, pxQueue); \
	} \
	else \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT_FROM_ISR(xPosition == queueSEND_TO_BACK ? TRACE_GET_CLASS_EVENT_CODE(SEND_FROM_ISR, TRCFAILED, QUEUE, traceQueueType) : TRACE_QUEUE_SEND_TO_FRONT_FROM_ISR_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(QUEUE, traceQueueType)); \
	}

/* Called when a message is received in interrupt context, e.g., using xQueueReceiveFromISR */
#undef traceQUEUE_RECEIVE_FROM_ISR
#define traceQUEUE_RECEIVE_FROM_ISR( pxQueue ) \
	trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE_FROM_ISR, TRCSUCCESS, QUEUE, pxQueue), QUEUE, pxQueue); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(QUEUE, pxQueue, (uint8_t)(pxQueue->uxItemsWaiting - 1));

/* Called when a message receive from interrupt context fails (since the queue was empty) */
#undef traceQUEUE_RECEIVE_FROM_ISR_FAILED
#define traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue ) \
	if (trcIS_QUEUE_VALID(pxQueue)) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE_FROM_ISR, TRCFAILED, QUEUE, pxQueue), QUEUE, pxQueue); \
	} \
	else \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT_FROM_ISR(TRACE_GET_CLASS_EVENT_CODE(RECEIVE_FROM_ISR, TRCFAILED, QUEUE, traceQueueType), TRACE_GET_CLASS_TRACE_CLASS(QUEUE, traceQueueType)); \
	}

/* Called in vTaskPrioritySet */
#undef traceTASK_PRIORITY_SET
#define traceTASK_PRIORITY_SET( pxTask, uxNewPriority ) \
	trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE(TASK_PRIORITY_SET, pxTask, uxNewPriority);

/* Called in vTaskPriorityInherit, which is called by Mutex operations */
#undef traceTASK_PRIORITY_INHERIT
#define traceTASK_PRIORITY_INHERIT( pxTask, uxNewPriority ) \
	trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE(TASK_PRIORITY_INHERIT, pxTask, uxNewPriority);

/* Called in vTaskPriorityDisinherit, which is called by Mutex operations */
#undef traceTASK_PRIORITY_DISINHERIT
#define traceTASK_PRIORITY_DISINHERIT( pxTask, uxNewPriority ) \
	trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE(TASK_PRIORITY_DISINHERIT, pxTask, uxNewPriority);

/* Called in vTaskResume */
#undef traceTASK_RESUME
#define traceTASK_RESUME( pxTaskToResume ) \
	trcKERNEL_HOOKS_TASK_RESUME(TASK_RESUME, pxTaskToResume);

/* Called in vTaskResumeFromISR */
#undef traceTASK_RESUME_FROM_ISR
#define traceTASK_RESUME_FROM_ISR( pxTaskToResume ) \
	trcKERNEL_HOOKS_TASK_RESUME(TASK_RESUME_FROM_ISR, pxTaskToResume);

/* Called in timer.c - xTimerCreate */
#undef traceTIMER_CREATE
#define traceTIMER_CREATE(tmr) \
	if (pdPASS == xReturn) \
	{ \
		trcKERNEL_HOOKS_OBJECT_CREATE(TIMER_CREATE, TIMER, tmr); \
		prvTraceSetObjectName(TRACE_CLASS_TIMER, TRACE_GET_TIMER_NUMBER(tmr), (const char*)tmr->pcTimerName); \
	} \
	else \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(TIMER_CREATE_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(TIMER, NOT_USED)); \
	}

#undef traceTIMER_COMMAND_SEND
#define traceTIMER_COMMAND_SEND(tmr, xCommandID, xOptionalValue, xReturn) \
	if (trcIS_TIMER_VALID(tmr)) \
	{ \
		switch(xCommandID) \
		{ \
			case timerCOMMAND_START: \
				trcKERNEL_HOOKS_KERNEL_SERVICE((xReturn == pdPASS) ? TIMER_START : TIMER_START_TRCFAILED, TIMER, tmr); \
				break; \
			case timerCOMMAND_STOP: \
				trcKERNEL_HOOKS_KERNEL_SERVICE((xReturn == pdPASS) ? TIMER_STOP : TIMER_STOP_TRCFAILED, TIMER, tmr); \
				break; \
			case timerCOMMAND_CHANGE_PERIOD: \
				trcKERNEL_HOOKS_KERNEL_SERVICE((xReturn == pdPASS) ? TIMER_CHANGE_PERIOD : TIMER_CHANGE_PERIOD_TRCFAILED, TIMER, tmr); \
				break; \
			case timerCOMMAND_DELETE: \
				trcKERNEL_HOOKS_KERNEL_SERVICE((xReturn == pdPASS) ? TIMER_DELETE_OBJ : TIMER_DELETE_OBJ_TRCFAILED, TIMER, tmr); \
				break; \
		} \
	} \
	else \
	{ \
		switch(xCommandID) \
		{ \
			case timerCOMMAND_START: \
				trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(TIMER_START_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(TIMER, NOT_USED)); \
				break; \
			case timerCOMMAND_STOP: \
				trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(TIMER_STOP_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(TIMER, NOT_USED)); \
				break; \
			case timerCOMMAND_CHANGE_PERIOD: \
				trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(TIMER_CHANGE_PERIOD_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(TIMER, NOT_USED)); \
				break; \
			case timerCOMMAND_DELETE: \
				trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(TIMER_DELETE_OBJ_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(TIMER, NOT_USED)); \
				break; \
		} \
	}

#undef traceTIMER_COMMAND_SEND_FROM_ISR
#define traceTIMER_COMMAND_SEND_FROM_ISR(tmr, xCommandID, xOptionalValue, xReturn) \
	if (trcIS_TIMER_VALID(tmr)) \
	{ \
		switch (xCommandID) \
		{ \
		case timerCOMMAND_START: \
			trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR((xReturn == pdPASS) ? TIMER_START_FROM_ISR : TIMER_START_FROM_ISR_TRCFAILED, TIMER, tmr); \
			break; \
		case timerCOMMAND_STOP: \
			trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR((xReturn == pdPASS) ? TIMER_STOP_FROM_ISR : TIMER_STOP_FROM_ISR_TRCFAILED, TIMER, tmr); \
			break; \
		case timerCOMMAND_CHANGE_PERIOD: \
			trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR((xReturn == pdPASS) ? TIMER_CHANGE_PERIOD_FROM_ISR : TIMER_CHANGE_PERIOD_FROM_ISR_TRCFAILED, TIMER, tmr); \
			break; \
		} \
	} \
	else \
	{ \
		switch(xCommandID) \
		{ \
			case timerCOMMAND_START: \
				trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT_FROM_ISR(TIMER_START_FROM_ISR_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(TIMER, NOT_USED)); \
				break; \
			case timerCOMMAND_STOP: \
				trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT_FROM_ISR(TIMER_STOP_FROM_ISR_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(TIMER, NOT_USED)); \
				break; \
			case timerCOMMAND_CHANGE_PERIOD: \
				trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT_FROM_ISR(TIMER_CHANGE_PERIOD_FROM_ISR_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(TIMER, NOT_USED)); \
				break; \
		} \
	}

/* We currently don't do anything with these timer command process events
case timerCOMMAND_START: \
trcKERNEL_HOOKS_KERNEL_SERVICE(TIMER_PROCESS_START, TIMER, tmr); \
break; \
case timerCOMMAND_STOP: \
trcKERNEL_HOOKS_KERNEL_SERVICE(TIMER_PROCESS_STOP, TIMER, tmr); \
break; \
case timerCOMMAND_CHANGE_PERIOD: \
trcKERNEL_HOOKS_KERNEL_SERVICE(TIMER_PROCESS_CHANGE_PERIOD, TIMER, tmr); \
break;*/
#undef traceTIMER_COMMAND_PROCESS
#define traceTIMER_COMMAND_PROCESS( tmr, xCommandID, xOptionalValue ) \
	switch(xCommandID) \
	{ \
		case timerCOMMAND_DELETE: \
			{ \
				TRACE_ALLOC_CRITICAL_SECTION(); \
				TRACE_ENTER_CRITICAL_SECTION(); \
				trcKERNEL_HOOKS_OBJECT_DELETE(TIMER_PROCESS_DELETE, EVENTGROUP_OBJCLOSE_NAME_TRCSUCCESS + TRACE_GET_OBJECT_TRACE_CLASS(TIMER, tmr), EVENTGROUP_OBJCLOSE_PROP_TRCSUCCESS + TRACE_GET_OBJECT_TRACE_CLASS(TIMER, tmr), TIMER, tmr); \
				TRACE_EXIT_CRITICAL_SECTION(); \
			} \
			break; \
	}

#undef traceTIMER_CALLBACK
#define traceTIMER_CALLBACK( tmr, callbackFunction ) \
	/*trcKERNEL_HOOKS_KERNEL_SERVICE_WITH_PARAM(TIMER_CALLBACK, TIMER, tmr, (uint32_t)callbackFunction);*/

#undef traceEVENT_GROUP_CREATE
#define traceEVENT_GROUP_CREATE(eg) \
	trcKERNEL_HOOKS_OBJECT_CREATE(EVENT_GROUP_CREATE, EVENTGROUP, eg);

#undef traceEVENT_GROUP_CREATE_FAILED
#define traceEVENT_GROUP_CREATE_FAILED() \
	trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(EVENT_GROUP_CREATE_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(EVENTGROUP, NOT_USED));

#undef traceEVENT_GROUP_DELETE
#define traceEVENT_GROUP_DELETE(eg) \
	{ \
		TRACE_ALLOC_CRITICAL_SECTION(); \
		TRACE_ENTER_CRITICAL_SECTION(); \
		trcKERNEL_HOOKS_OBJECT_DELETE(EVENT_GROUP_DELETE_OBJ, EVENTGROUP_OBJCLOSE_NAME_TRCSUCCESS + TRACE_GET_OBJECT_TRACE_CLASS(EVENTGROUP, eg), EVENTGROUP_OBJCLOSE_PROP_TRCSUCCESS + TRACE_GET_OBJECT_TRACE_CLASS(EVENTGROUP, eg), EVENTGROUP, eg); \
		TRACE_EXIT_CRITICAL_SECTION(); \
	}

#undef traceEVENT_GROUP_DELETE_FAILED
#define traceEVENT_GROUP_DELETE_FAILED(eg) \
	trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(EVENT_GROUP_DELETE_OBJ_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(EVENTGROUP, NOT_USED));

#undef traceEVENT_GROUP_SET_BITS
#define traceEVENT_GROUP_SET_BITS(eg, bitsToSet) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(EVENT_GROUP_SET_BITS, EVENTGROUP, eg);

#undef traceEVENT_GROUP_SET_BITS_FAILED
#define traceEVENT_GROUP_SET_BITS_FAILED(eg, bitsToSet) \
	if (trcIS_EVENTGROUP_VALID(eg)) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE(EVENT_GROUP_SET_BITS_TRCFAILED, EVENTGROUP, eg); \
	} \
	else \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(EVENT_GROUP_SET_BITS_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(EVENTGROUP, NOT_USED)); \
	}

#undef traceEVENT_GROUP_SET_BITS_FROM_ISR
#define traceEVENT_GROUP_SET_BITS_FROM_ISR(eg, bitsToSet) \
	trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR(EVENT_GROUP_SET_BITS_FROM_ISR, EVENTGROUP, eg);

#undef traceEVENT_GROUP_SET_BITS_FROM_ISR_FAILED
#define traceEVENT_GROUP_SET_BITS_FROM_ISR_FAILED(eg, bitsToSet) \
	if (trcIS_EVENTGROUP_VALID(eg)) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR(EVENT_GROUP_SET_BITS_FROM_ISR_TRCFAILED, EVENTGROUP, eg); \
	} \
	else \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT_FROM_ISR(EVENT_GROUP_SET_BITS_FROM_ISR_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(EVENTGROUP, NOT_USED)); \
	}

#undef traceEVENT_GROUP_CLEAR_BITS
#define traceEVENT_GROUP_CLEAR_BITS(eg, bitsToClear) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(EVENT_GROUP_CLEAR_BITS, EVENTGROUP, eg);

#undef traceEVENT_GROUP_CLEAR_BITS_FAILED
#define traceEVENT_GROUP_CLEAR_BITS_FAILED(eg, bitsToClear) \
	if (trcIS_EVENTGROUP_VALID(eg)) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE(EVENT_GROUP_CLEAR_BITS_TRCFAILED, EVENTGROUP, eg); \
	} \
	else \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(EVENT_GROUP_CLEAR_BITS_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(EVENTGROUP, NOT_USED)); \
	}

#undef traceEVENT_GROUP_CLEAR_BITS_FROM_ISR
#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR(eg, bitsToClear) \
	trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR(EVENT_GROUP_CLEAR_BITS_FROM_ISR, EVENTGROUP, eg);

#undef traceEVENT_GROUP_CLEAR_BITS_FROM_ISR_FAILED
#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR_FAILED(eg, bitsToClear) \
	if (trcIS_EVENTGROUP_VALID(eg)) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR(EVENT_GROUP_CLEAR_BITS_FROM_ISR_TRCFAILED, EVENTGROUP, eg); \
	} \
	else \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT_FROM_ISR(EVENT_GROUP_CLEAR_BITS_FROM_ISR_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(EVENTGROUP, NOT_USED)); \
	}

#undef traceEVENT_GROUP_WAIT_BITS_BLOCK
#define traceEVENT_GROUP_WAIT_BITS_BLOCK(eg, bitsToWaitFor, timeout) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(EVENT_GROUP_WAIT_BITS_TRCBLOCK, EVENTGROUP, eg); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

#undef traceEVENT_GROUP_WAIT_BITS
#define traceEVENT_GROUP_WAIT_BITS(eg, bitsToWaitFor, timeout) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(EVENT_GROUP_WAIT_BITS, EVENTGROUP, eg);

#undef traceEVENT_GROUP_WAIT_BITS_FAILED
#define traceEVENT_GROUP_WAIT_BITS_FAILED(eg, bitsToWaitFor, timeout) \
	if (trcIS_EVENTGROUP_VALID(eg)) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE(EVENT_GROUP_WAIT_BITS_TRCFAILED, EVENTGROUP, eg); \
	} \
	else \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(EVENT_GROUP_WAIT_BITS_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(EVENTGROUP, NOT_USED)); \
	}

#undef traceTASK_NOTIFY_WAIT
#define traceTASK_NOTIFY_WAIT() \
	trcKERNEL_HOOKS_KERNEL_SERVICE_WITH_PARAM(TRACE_TASK_NOTIFY_WAIT_TRCSUCCESS, TASK, pxCurrentTCB, xTicksToWait);

#undef traceTASK_NOTIFY_WAIT_FAILED
#define traceTASK_NOTIFY_WAIT_FAILED() \
	trcKERNEL_HOOKS_KERNEL_SERVICE_WITH_PARAM(TRACE_TASK_NOTIFY_WAIT_TRCFAILED, TASK, pxCurrentTCB, xTicksToWait);

#undef traceTASK_NOTIFY_WAIT_BLOCK
#define traceTASK_NOTIFY_WAIT_BLOCK() \
	trcKERNEL_HOOKS_KERNEL_SERVICE_WITH_PARAM(TRACE_TASK_NOTIFY_WAIT_TRCBLOCK, TASK, pxCurrentTCB, xTicksToWait); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

#undef traceTASK_NOTIFY_SEND
#define traceTASK_NOTIFY_SEND() \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_TASK_NOTIFY_SEND_TRCSUCCESS, TASK, xTaskToNotify);

#undef traceTASK_NOTIFY_SEND_FAILED
#define traceTASK_NOTIFY_SEND_FAILED() \
	if (trcIS_TASK_VALID(xTaskToNotify)) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_TASK_NOTIFY_SEND_TRCFAILED, TASK, xTaskToNotify); \
	} \
	else \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT(TRACE_TASK_NOTIFY_SEND_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(TASK, NOT_USED)); \
	}

#undef traceTASK_NOTIFY_SEND_FROM_ISR
#define traceTASK_NOTIFY_SEND_FROM_ISR() \
	trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR(TRACE_TASK_NOTIFY_SEND_FROM_ISR_TRCSUCCESS, TASK, xTaskToNotify);

#undef traceTASK_NOTIFY_SEND_FROM_ISR_FAILED
#define traceTASK_NOTIFY_SEND_FROM_ISR_FAILED() \
	if (trcIS_TASK_VALID(xTaskToNotify)) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_FROM_ISR(TRACE_TASK_NOTIFY_SEND_FROM_ISR_TRCFAILED, TASK, xTaskToNotify); \
	} \
	else \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE_NULL_OBJECT_FROM_ISR(TRACE_TASK_NOTIFY_SEND_FROM_ISR_TRCFAILED, TRACE_GET_CLASS_TRACE_CLASS(TASK, NOT_USED)); \
	}

#endif /* (TRC_CFG_SCHEDULING_ONLY == 0) */

#endif /*#if TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT */

/******************************************************************************/
/*** Definitions for Streaming mode *******************************************/
/******************************************************************************/
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#define vTraceSetQueueName(object, name) vTraceStoreKernelObjectName(object, name)
#define vTraceSetSemaphoreName(object, name) vTraceStoreKernelObjectName(object, name)
#define vTraceSetMutexName(object, name) vTraceStoreKernelObjectName(object, name)
#define vTraceSetEventGroupName(object, name) vTraceStoreKernelObjectName(object, name)

/*******************************************************************************
* vTraceStoreKernelObjectName
*
* Set the name for a kernel object (defined by its address).
******************************************************************************/			
void vTraceStoreKernelObjectName(void* object, const char* name); 

/*******************************************************************************
* prvIsNewTCB
*
* Tells if this task is already executing, or if there has been a task-switch.
* Assumed to be called within a trace hook in kernel context.
*******************************************************************************/
uint32_t prvIsNewTCB(void* pNewTCB);

/*************************************************************************/
/* KERNEL SPECIFIC OBJECT CONFIGURATION									 */
/*************************************************************************/

/*******************************************************************************
 * The event codes - should match the offline config file.
 ******************************************************************************/

/*** Event codes for streaming - should match the Tracealyzer config file *****/
#define PSF_EVENT_NULL_EVENT								0x00

#define PSF_EVENT_TRACE_START								0x01
#define PSF_EVENT_TS_CONFIG									0x02
#define PSF_EVENT_OBJ_NAME									0x03
#define PSF_EVENT_OBJ_DATA									0x04
#define PSF_EVENT_DEFINE_ISR								0x05

#define PSF_EVENT_TASK_READY								0x06

#define PSF_EVENT_NEW_TIME									0x07
#define PSF_EVENT_NEW_TIME_SCHEDULER_SUSPENDED				0x08
#define PSF_EVENT_ISR_BEGIN									0x09
#define PSF_EVENT_ISR_RESUME								0x0A
#define PSF_EVENT_TS_BEGIN									0x0B
#define PSF_EVENT_TS_RESUME									0x0C
#define PSF_EVENT_TASK_ACTIVATE								0x0D

#define PSF_EVENT_LOWPOWER_BEGIN							0x0E
#define PSF_EVENT_LOWPOWER_END								0x0F

#define PSF_EVENT_IFE_NEXT									0x10
#define PSF_EVENT_IFE_DIRECT								0x11

#define PSF_EVENT_TASK_CREATE								0x12
#define PSF_EVENT_TASK_CREATE_FAILED						0x13
#define PSF_EVENT_TASK_DELETE								0x14
#define PSF_EVENT_TASK_DELAY_UNTIL							0x15
#define PSF_EVENT_TASK_DELAY								0x16
#define PSF_EVENT_TASK_SUSPEND								0x17
#define PSF_EVENT_TASK_RESUME								0x18
#define PSF_EVENT_TASK_RESUME_FROMISR						0x19
#define PSF_EVENT_TASK_PRIORITY								0x1A
#define PSF_EVENT_TASK_PRIO_INHERIT							0x1B
#define PSF_EVENT_TASK_PRIO_DISINHERIT						0x1C

#define PSF_EVENT_TASK_NOTIFY_SEND							0x1D
#define PSF_EVENT_TASK_NOTIFY_SEND_FAILED					0x1E
#define PSF_EVENT_TASK_NOTIFY_SEND_FROM_ISR					0x1F
#define PSF_EVENT_TASK_NOTIFY_SEND_FROM_ISR_FAILED			0x20
#define PSF_EVENT_TASK_NOTIFY_WAIT							0x21
#define PSF_EVENT_TASK_NOTIFY_WAIT_BLOCK					0x22
#define PSF_EVENT_TASK_NOTIFY_WAIT_FAILED					0x23

#define PSF_EVENT_QUEUE_CREATE								0x24
#define PSF_EVENT_QUEUE_CREATE_FAILED						0x25
#define PSF_EVENT_QUEUE_SEND								0x26
#define PSF_EVENT_QUEUE_SEND_FAILED							0x27
#define PSF_EVENT_QUEUE_SEND_BLOCK							0x28
#define PSF_EVENT_QUEUE_SEND_TO_FRONT						0x29
#define PSF_EVENT_QUEUE_SEND_TO_FRONT_FAILED				0x2A
#define PSF_EVENT_QUEUE_SEND_TO_FRONT_BLOCK					0x2B
#define PSF_EVENT_QUEUE_SEND_FROMISR						0x2C
#define PSF_EVENT_QUEUE_SEND_FROMISR_FAILED					0x2D
#define PSF_EVENT_QUEUE_SEND_TO_FRONT_FROMISR				0x2E
#define PSF_EVENT_QUEUE_SEND_TO_FRONT_FROMISR_FAILED		0x2F
#define PSF_EVENT_QUEUE_RECEIVE								0x30
#define PSF_EVENT_QUEUE_RECEIVE_FAILED						0x31
#define PSF_EVENT_QUEUE_RECEIVE_BLOCK						0x32
#define PSF_EVENT_QUEUE_RECEIVE_FROMISR						0x33
#define PSF_EVENT_QUEUE_RECEIVE_FROMISR_FAILED				0x34
#define PSF_EVENT_QUEUE_PEEK								0x35
#define PSF_EVENT_QUEUE_PEEK_FAILED							0x36
#define PSF_EVENT_QUEUE_PEEK_BLOCK							0x37

#define PSF_EVENT_SEMAPHORE_CREATE							0x38
#define PSF_EVENT_SEMAPHORE_CREATE_FAILED					0x39
#define PSF_EVENT_SEMAPHORE_SEND							0x3A
#define PSF_EVENT_SEMAPHORE_SEND_FAILED						0x3B
#define PSF_EVENT_SEMAPHORE_SEND_FROMISR					0x3C
#define PSF_EVENT_SEMAPHORE_SEND_FROMISR_FAILED				0x3D
#define PSF_EVENT_SEMAPHORE_RECEIVE							0x3E
#define PSF_EVENT_SEMAPHORE_RECEIVE_FAILED					0x3F
#define PSF_EVENT_SEMAPHORE_RECEIVE_BLOCK					0x40
#define PSF_EVENT_SEMAPHORE_RECEIVE_FROMISR					0x41
#define PSF_EVENT_SEMAPHORE_RECEIVE_FROMISR_FAILED			0x42

#define PSF_EVENT_MUTEX_CREATE								0x43
#define PSF_EVENT_MUTEX_CREATE_FAILED						0x44
#define PSF_EVENT_MUTEX_SEND								0x45
#define PSF_EVENT_MUTEX_SEND_FAILED							0x46
#define PSF_EVENT_MUTEX_RECEIVE								0x47
#define PSF_EVENT_MUTEX_RECEIVE_FAILED						0x48
#define PSF_EVENT_MUTEX_RECEIVE_BLOCK						0x49

#define PSF_EVENT_TIMER_CREATE								0x4A
#define PSF_EVENT_TIMER_CREATE_FAILED						0x4B
#define PSF_EVENT_TIMER_DELETE								0x4C
#define PSF_EVENT_TIMER_DELETE_FAILED						0x4D
#define PSF_EVENT_TIMER_START								0x4E
#define PSF_EVENT_TIMER_START_FAILED						0x4F
#define PSF_EVENT_TIMER_STOP								0x50
#define PSF_EVENT_TIMER_STOP_FAILED							0x51
#define PSF_EVENT_TIMER_CHANGEPERIOD						0x52
#define PSF_EVENT_TIMER_CHANGEPERIOD_FAILED					0x53
#define PSF_EVENT_TIMER_START_FROMISR						0x54
#define PSF_EVENT_TIMER_START_FROMISR_FAILED				0x55
#define PSF_EVENT_TIMER_STOP_FROMISR						0x56
#define PSF_EVENT_TIMER_STOP_FROMISR_FAILED					0x57
#define PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR				0x58
#define PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR_FAILED			0x59
#define PSF_EVENT_TIMER_PROCESS_START						0x5A
#define PSF_EVENT_TIMER_PROCESS_STOP						0x5B
#define PSF_EVENT_TIMER_PROCESS_CHANGEPERIOD				0x5C
#define PSF_EVENT_TIMER_PROCESS_DELETE						0x5D
#define PSF_EVENT_TIMER_CALLBACK							0x5E

/*
#define UNUSED_SO_FAR										0x5F
#define UNUSED_SO_FAR										0x60
#define UNUSED_SO_FAR										0x61
#define UNUSED_SO_FAR										0x62
#define UNUSED_SO_FAR										0x63
*/

#define PSF_EVENT_EVENTGROUP_CREATE							0x64
#define PSF_EVENT_EVENTGROUP_CREATE_FAILED					0x65
#define PSF_EVENT_EVENTGROUP_DELETE							0x66
#define PSF_EVENT_EVENTGROUP_DELETE_FAILED					0x67
#define PSF_EVENT_EVENTGROUP_SETBITS						0x68
#define PSF_EVENT_EVENTGROUP_SETBITS_FAILED					0x69
#define PSF_EVENT_EVENTGROUP_SETBITS_FROMISR				0x6A
#define PSF_EVENT_EVENTGROUP_SETBITS_FROMISR_FAILED			0x6B
#define PSF_EVENT_EVENTGROUP_WAITBITS						0x6C
#define PSF_EVENT_EVENTGROUP_WAITBITS_FAILED				0x6D
#define PSF_EVENT_EVENTGROUP_WAITBITS_BLOCK					0x6E
#define PSF_EVENT_EVENTGROUP_CLEARBITS						0x6F
#define PSF_EVENT_EVENTGROUP_CLEARBITS_FAILED				0x70
#define PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR				0x71
#define PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR_FAILED		0x72

/* LAST EVENT 0x72 (except user events from 0xF0) */

/* Placing user events near the end */
#define PSF_EVENT_USER_EVENT								0xF0
#define PSF_EVENT_USER_EVENT_0								(PSF_EVENT_USER_EVENT + 0)
#define PSF_EVENT_USER_EVENT_1								(PSF_EVENT_USER_EVENT + 1)
#define PSF_EVENT_USER_EVENT_2								(PSF_EVENT_USER_EVENT + 2)
#define PSF_EVENT_USER_EVENT_3								(PSF_EVENT_USER_EVENT + 3)
#define PSF_EVENT_USER_EVENT_4								(PSF_EVENT_USER_EVENT + 4)
#define PSF_EVENT_USER_EVENT_5								(PSF_EVENT_USER_EVENT + 5)
#define PSF_EVENT_USER_EVENT_6								(PSF_EVENT_USER_EVENT + 6)
#define PSF_EVENT_USER_EVENT_7								(PSF_EVENT_USER_EVENT + 7)

/*** The trace macros for streaming ******************************************/

/* Called on each OS tick. Will call uiPortGetTimestamp to make sure it is called at least once every OS tick. */
#undef traceTASK_INCREMENT_TICK
#define traceTASK_INCREMENT_TICK( xTickCount ) \
	if (uxSchedulerSuspended == ( portUnsignedBaseType ) pdTRUE || uxMissedTicks == 0) { extern uint32_t uiTraceTickCount; uiTraceTickCount++; } \
	if (uxSchedulerSuspended == ( portUnsignedBaseType ) pdFALSE) { prvTraceStoreEvent1(PSF_EVENT_NEW_TIME, (xTickCount + 1)); }

/* A macro that will update the tick count when returning from tickless idle */
#undef traceINCREASE_TICK_COUNT
/* Note: This can handle time adjustments of max 2^32 ticks, i.e., 35 seconds at 120 MHz. Thus, tick-less idle periods longer than 2^32 ticks will appear "compressed" on the time line.*/
#define traceINCREASE_TICK_COUNT( xCount ) { extern uint32_t uiTraceTickCount; uiTraceTickCount += xCount; }

/* Called for each task that becomes ready */
#if (TRC_CFG_INCLUDE_READY_EVENTS == 1)
#undef traceMOVED_TASK_TO_READY_STATE
#define traceMOVED_TASK_TO_READY_STATE( pxTCB ) \
	if (TRACE_GET_TASK_FILTER(pxTCB) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent1(PSF_EVENT_TASK_READY, (uint32_t)pxTCB); \
	}
#else /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */
#define traceMOVED_TASK_TO_READY_STATE( pxTCB ) 
#endif /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */

extern volatile uint32_t uiTraceSystemState;

/* Called on each task-switch */
#undef traceTASK_SWITCHED_IN
#define traceTASK_SWITCHED_IN() \
	uiTraceSystemState = TRC_STATE_IN_TASKSWITCH; \
	if (TRACE_GET_TASK_FILTER(pxCurrentTCB) & CurrentFilterMask) \
	{ \
		if (prvIsNewTCB(pxCurrentTCB)) \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_TASK_ACTIVATE, (uint32_t)pxCurrentTCB, pxCurrentTCB->uxPriority); \
		} \
	} \
	uiTraceSystemState = TRC_STATE_IN_APPLICATION;

/* Called on vTaskCreate */
#undef traceTASK_CREATE
#define traceTASK_CREATE(pxNewTCB) \
	prvTraceSaveObjectSymbol(pxNewTCB, (const char*)pxNewTCB->pcNameOfTask); \
	prvTraceSaveObjectData(pxNewTCB, pxNewTCB->uxPriority); \
	prvTraceStoreStringEvent(1, PSF_EVENT_OBJ_NAME, pxNewTCB->pcNameOfTask, pxNewTCB); \
	TRACE_SET_TASK_FILTER(pxNewTCB, CurrentFilterGroup); \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		if (TRACE_GET_TASK_FILTER(pxNewTCB) & CurrentFilterMask) \
			prvTraceStoreEvent2(PSF_EVENT_TASK_CREATE, (uint32_t)pxNewTCB, pxNewTCB->uxPriority);

/* Called in vTaskCreate, if it fails (typically if the stack can not be allocated) */
#undef traceTASK_CREATE_FAILED
#define traceTASK_CREATE_FAILED() \
	if (TRACE_GET_TASK_FILTER(pxCurrentTCB) & CurrentFilterMask) \
		prvTraceStoreEvent0(PSF_EVENT_TASK_CREATE_FAILED);

/* Called on vTaskDelete */
#undef traceTASK_DELETE
#define traceTASK_DELETE( pxTaskToDelete ) \
	if (TRACE_GET_TASK_FILTER(pxCurrentTCB) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_TASK_FILTER(pxTaskToDelete) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_TASK_DELETE, (uint32_t)pxTaskToDelete, pxTaskToDelete->uxPriority); \
		} \
	} \
	prvTraceDeleteSymbol(pxTaskToDelete); \
	prvTraceDeleteObjectData(pxTaskToDelete);

#if (TRC_CFG_SCHEDULING_ONLY == 0)

#if (configUSE_TICKLESS_IDLE != 0)

#undef traceLOW_POWER_IDLE_BEGIN
#define traceLOW_POWER_IDLE_BEGIN() \
	{ \
		prvTraceStoreEvent1(PSF_EVENT_LOWPOWER_BEGIN, xExpectedIdleTime); \
	}

#undef traceLOW_POWER_IDLE_END
#define traceLOW_POWER_IDLE_END() \
	{ \
		prvTraceStoreEvent0(PSF_EVENT_LOWPOWER_END); \
	}

#endif

/* Called on vTaskSuspend */
#undef traceTASK_SUSPEND
#define traceTASK_SUSPEND( pxTaskToSuspend ) \
	if (TRACE_GET_TASK_FILTER(pxCurrentTCB) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_TASK_FILTER(pxTaskToSuspend) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent1(PSF_EVENT_TASK_SUSPEND, (uint32_t)pxTaskToSuspend); \
		} \
	}

/* Called on vTaskDelay - note the use of variable xTicksToDelay */
#undef traceTASK_DELAY
#define traceTASK_DELAY() \
	if (TRACE_GET_TASK_FILTER(pxCurrentTCB) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent1(PSF_EVENT_TASK_DELAY, xTicksToDelay); \
	}

/* Called on vTaskDelayUntil - note the use of variable xTimeToWake */
#undef traceTASK_DELAY_UNTIL
#define traceTASK_DELAY_UNTIL(xTimeToWake) \
	if (TRACE_GET_TASK_FILTER(pxCurrentTCB) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent1(PSF_EVENT_TASK_DELAY_UNTIL, xTimeToWake); \
	}

/* Called in xQueueCreate, and thereby for all other object based on queues, such as semaphores. */
#undef traceQUEUE_CREATE
#define traceQUEUE_CREATE( pxNewQueue )\
	TRACE_SET_OBJECT_FILTER(QUEUE, pxNewQueue, CurrentFilterGroup); \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(QUEUE, pxNewQueue) & CurrentFilterMask) \
		{ \
			switch (uxQueueType) \
			{ \
			case queueQUEUE_IS_QUEUE: \
				prvTraceStoreEvent2(PSF_EVENT_QUEUE_CREATE, (uint32_t)pxNewQueue, pxNewQueue->uxMaxNumberOfItems); \
				break; \
			case queueQUEUE_IS_SEMAPHORE: \
				prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_CREATE, (uint32_t)pxNewQueue, pxNewQueue->uxMaxNumberOfItems); \
				break; \
			case queueQUEUE_IS_MUTEX: \
				prvTraceStoreEvent1(PSF_EVENT_MUTEX_CREATE, (uint32_t)pxNewQueue); \
				break; \
			} \
		} \
	}

/* Called in xQueueCreate, if the queue creation fails */
#undef traceQUEUE_CREATE_FAILED
#define traceQUEUE_CREATE_FAILED( pcBuffer, queueType, queueLength ) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		switch (queueType) \
		{ \
		case queueQUEUE_IS_QUEUE: \
			prvTraceStoreEvent2(PSF_EVENT_QUEUE_CREATE_FAILED, (uint32_t)pcBuffer, queueLength); \
			break; \
		case queueQUEUE_IS_SEMAPHORE: \
			prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_CREATE_FAILED, (uint32_t)pcBuffer, queueLength); \
			break; \
		case queueQUEUE_IS_MUTEX: \
			prvTraceStoreEvent1(PSF_EVENT_MUTEX_CREATE_FAILED, (uint32_t)pcBuffer); \
			break; \
		} \
	}

/* Called when a message is sent to a queue */
#undef traceQUEUE_SEND
#define traceQUEUE_SEND( pxQueue ) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(QUEUE, pxQueue) & CurrentFilterMask) \
		{ \
			switch (pxQueue->uxQueueType) \
			{ \
			case queueQUEUE_IS_QUEUE: \
				prvTraceStoreEvent3(xPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND : PSF_EVENT_QUEUE_SEND_TO_FRONT, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting + 1); \
				break; \
			case queueQUEUE_IS_SEMAPHORE: \
				prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_SEND, (uint32_t)pxQueue, pxQueue->uxItemsWaiting + 1); \
				break; \
			case queueQUEUE_IS_MUTEX: \
				prvTraceStoreEvent2(PSF_EVENT_MUTEX_SEND, (uint32_t)pxQueue, pxQueue->uxItemsWaiting + 1); \
				break; \
			} \
		} \
	}

/* Called when a message failed to be sent to a queue (timeout) */
#undef traceQUEUE_SEND_FAILED
#define traceQUEUE_SEND_FAILED( pxQueue ) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (trcIS_QUEUE_VALID(pxQueue)) \
		{ \
			if (TRACE_GET_OBJECT_FILTER(QUEUE, pxQueue) & CurrentFilterMask) \
			{ \
				switch (pxQueue->uxQueueType) \
				{ \
				case queueQUEUE_IS_QUEUE: \
					prvTraceStoreEvent3(xPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_FAILED : PSF_EVENT_QUEUE_SEND_TO_FRONT_FAILED, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting + 1); \
					break; \
				case queueQUEUE_IS_SEMAPHORE: \
					prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_SEND_FAILED, (uint32_t)pxQueue, pxQueue->uxItemsWaiting + 1); \
					break; \
				case queueQUEUE_IS_MUTEX: \
					prvTraceStoreEvent2(PSF_EVENT_MUTEX_SEND_FAILED, (uint32_t)pxQueue, pxQueue->uxItemsWaiting + 1); \
					break; \
				} \
			} \
		} \
		else \
		{ \
			prvTraceStoreEvent3(PSF_EVENT_QUEUE_SEND_FAILED, (uint32_t)pxQueue, xTicksToWait, 0); \
		} \
	}

/* Called when the task is blocked due to a send operation on a full queue */
/* Semaphores and mutexes don't block on send */
#undef traceBLOCKING_ON_QUEUE_SEND
#define traceBLOCKING_ON_QUEUE_SEND( pxQueue ) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(QUEUE, pxQueue) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent3(xPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_BLOCK : PSF_EVENT_QUEUE_SEND_TO_FRONT_BLOCK, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting + 1); \
		} \
	}

/* Called when a message is sent from interrupt context, e.g., using xQueueSendFromISR */
/* Mutexes can't be given from ISR */
#undef traceQUEUE_SEND_FROM_ISR
#define traceQUEUE_SEND_FROM_ISR( pxQueue ) \
	if (TRACE_GET_OBJECT_FILTER(QUEUE, pxQueue) & CurrentFilterMask) \
	{ \
		switch (pxQueue->uxQueueType) \
		{ \
		case queueQUEUE_IS_QUEUE: \
			prvTraceStoreEvent2(xPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_FROMISR : PSF_EVENT_QUEUE_SEND_TO_FRONT_FROMISR, (uint32_t)pxQueue, pxQueue->uxItemsWaiting + 1); \
			break; \
		case queueQUEUE_IS_SEMAPHORE: \
			prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_SEND_FROMISR, (uint32_t)pxQueue, pxQueue->uxItemsWaiting + 1); \
			break; \
		} \
	}

/* Called when a message send from interrupt context fails (since the queue was full) */
/* Mutexes can't be given from ISR */
#undef traceQUEUE_SEND_FROM_ISR_FAILED
#define traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue ) \
	if (trcIS_QUEUE_VALID(pxQueue)) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(QUEUE, pxQueue) & CurrentFilterMask) \
		{ \
			switch (pxQueue->uxQueueType) \
			{ \
			case queueQUEUE_IS_QUEUE: \
				prvTraceStoreEvent2(xPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_FROMISR_FAILED : PSF_EVENT_QUEUE_SEND_TO_FRONT_FROMISR_FAILED, (uint32_t)pxQueue, pxQueue->uxItemsWaiting); \
				break; \
			case queueQUEUE_IS_SEMAPHORE: \
				prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_SEND_FROMISR_FAILED, (uint32_t)pxQueue, pxQueue->uxItemsWaiting); \
				break; \
			} \
		} \
	} \
	else \
	{ \
		prvTraceStoreEvent2(xPosition == queueSEND_TO_BACK ? PSF_EVENT_QUEUE_SEND_FROMISR_FAILED : PSF_EVENT_QUEUE_SEND_TO_FRONT_FROMISR_FAILED, (uint32_t)pxQueue, 0); \
	}

/* Called when a message is received from a queue */
#undef traceQUEUE_RECEIVE
#define traceQUEUE_RECEIVE( pxQueue ) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(QUEUE, pxQueue) & CurrentFilterMask) \
		{ \
			switch (pxQueue->uxQueueType) \
			{ \
			case queueQUEUE_IS_QUEUE: \
				prvTraceStoreEvent3(PSF_EVENT_QUEUE_RECEIVE, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting - 1); \
				break; \
			case queueQUEUE_IS_SEMAPHORE: \
				prvTraceStoreEvent3(PSF_EVENT_SEMAPHORE_RECEIVE, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting - 1); \
				break; \
			case queueQUEUE_IS_MUTEX: \
				prvTraceStoreEvent3(PSF_EVENT_MUTEX_RECEIVE, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting - 1); \
				break; \
			} \
		} \
	}

/* Called when a receive operation on a queue fails (timeout) */
#undef traceQUEUE_RECEIVE_FAILED
#define traceQUEUE_RECEIVE_FAILED( pxQueue ) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (trcIS_QUEUE_VALID(pxQueue)) \
		{ \
			if (TRACE_GET_OBJECT_FILTER(QUEUE, pxQueue) & CurrentFilterMask) \
			{ \
				switch (pxQueue->uxQueueType) \
				{ \
				case queueQUEUE_IS_QUEUE: \
					prvTraceStoreEvent3(PSF_EVENT_QUEUE_RECEIVE_FAILED, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting); \
					break; \
				case queueQUEUE_IS_SEMAPHORE: \
					prvTraceStoreEvent3(PSF_EVENT_SEMAPHORE_RECEIVE_FAILED, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting); \
					break; \
				case queueQUEUE_IS_MUTEX: \
					prvTraceStoreEvent3(PSF_EVENT_MUTEX_RECEIVE_FAILED, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting); \
					break; \
				} \
			} \
		} \
		else \
		{ \
			prvTraceStoreEvent3(PSF_EVENT_QUEUE_RECEIVE_FAILED, (uint32_t)pxQueue, xTicksToWait, 0); \
		} \
	}

/* Called when the task is blocked due to a receive operation on an empty queue */
#undef traceBLOCKING_ON_QUEUE_RECEIVE
#define traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue ) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(QUEUE, pxQueue) & CurrentFilterMask) \
		{ \
			switch (pxQueue->uxQueueType) \
			{ \
			case queueQUEUE_IS_QUEUE: \
				prvTraceStoreEvent3(PSF_EVENT_QUEUE_RECEIVE_BLOCK, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting); \
				break; \
			case queueQUEUE_IS_SEMAPHORE: \
				prvTraceStoreEvent3(PSF_EVENT_SEMAPHORE_RECEIVE_BLOCK, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting); \
				break; \
			case queueQUEUE_IS_MUTEX: \
				prvTraceStoreEvent3(PSF_EVENT_MUTEX_RECEIVE_BLOCK, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting); \
				break; \
			} \
		} \
	}
	
/* Called when a message is received in interrupt context, e.g., using xQueueReceiveFromISR */
/* Mutexes can't be taken from ISR */
#undef traceQUEUE_RECEIVE_FROM_ISR
#define traceQUEUE_RECEIVE_FROM_ISR( pxQueue ) \
	if (TRACE_GET_OBJECT_FILTER(QUEUE, pxQueue) & CurrentFilterMask) \
	{ \
		switch (pxQueue->uxQueueType) \
		{ \
		case queueQUEUE_IS_QUEUE: \
			prvTraceStoreEvent2(PSF_EVENT_QUEUE_RECEIVE_FROMISR, (uint32_t)pxQueue, pxQueue->uxItemsWaiting - 1); \
			break; \
		case queueQUEUE_IS_SEMAPHORE: \
			prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_RECEIVE_FROMISR, (uint32_t)pxQueue, pxQueue->uxItemsWaiting - 1); \
			break; \
		} \
	}
    
/* Called when a message receive from interrupt context fails (since the queue was empty) */
/* Mutexes can't be taken from ISR */
#undef traceQUEUE_RECEIVE_FROM_ISR_FAILED
#define traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue ) \
	if (trcIS_QUEUE_VALID(pxQueue)) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(QUEUE, pxQueue) & CurrentFilterMask) \
		{ \
			switch (pxQueue->uxQueueType) \
			{ \
			case queueQUEUE_IS_QUEUE: \
				prvTraceStoreEvent2(PSF_EVENT_QUEUE_RECEIVE_FROMISR_FAILED, (uint32_t)pxQueue, pxQueue->uxItemsWaiting); \
				break; \
			case queueQUEUE_IS_SEMAPHORE: \
				prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_RECEIVE_FROMISR_FAILED, (uint32_t)pxQueue, pxQueue->uxItemsWaiting); \
				break; \
			} \
		} \
	} \
	else \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_QUEUE_RECEIVE_FROMISR_FAILED, (uint32_t)pxQueue, 0); \
	}

/* Called on xQueuePeek */
/* Can't Peek Semaphores or Mutexes */
#undef traceQUEUE_PEEK
#define traceQUEUE_PEEK( pxQueue ) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(QUEUE, pxQueue) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent3(PSF_EVENT_QUEUE_PEEK, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting); \
		} \
	}

/* Called on xQueuePeek */
/* Can't Peek Semaphores or Mutexes */
#undef traceQUEUE_PEEK_FAILED
#define traceQUEUE_PEEK_FAILED( pxQueue ) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (trcIS_QUEUE_VALID(pxQueue)) \
		{ \
			if (TRACE_GET_OBJECT_FILTER(QUEUE, pxQueue) & CurrentFilterMask) \
			{ \
				prvTraceStoreEvent3(PSF_EVENT_QUEUE_PEEK_FAILED, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting); \
			} \
		} \
		else \
		{ \
			prvTraceStoreEvent3(PSF_EVENT_QUEUE_PEEK_FAILED, (uint32_t)pxQueue, xTicksToWait, 0); \
		} \
	}

/* Called on xQueuePeek */
/* Can't Peek Semaphores or Mutexes */
#undef traceBLOCKING_ON_QUEUE_PEEK
#define traceBLOCKING_ON_QUEUE_PEEK( pxQueue ) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(QUEUE, pxQueue) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent3(PSF_EVENT_QUEUE_PEEK_BLOCK, (uint32_t)pxQueue, xTicksToWait, pxQueue->uxItemsWaiting); \
		} \
	}

/* Called in xTaskPrioritySet */
#undef traceTASK_PRIORITY_SET
#define traceTASK_PRIORITY_SET( pxTask, uxNewPriority ) \
	prvTraceSaveObjectData(pxTask, uxNewPriority); \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_TASK_FILTER(pxTask) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_TASK_PRIORITY, (uint32_t)pxTask, uxNewPriority); \
		} \
	}
	
/* Called in vTaskPriorityInherit, which is called by Mutex operations */
#undef traceTASK_PRIORITY_INHERIT
#define traceTASK_PRIORITY_INHERIT( pxTask, uxNewPriority ) \
	if (TRACE_GET_TASK_FILTER(pxTask) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_PRIO_INHERIT, (uint32_t)pxTask, uxNewPriority); \
	}

/* Called in vTaskPriorityDisinherit, which is called by Mutex operations */
#undef traceTASK_PRIORITY_DISINHERIT
#define traceTASK_PRIORITY_DISINHERIT( pxTask, uxNewPriority ) \
	if (TRACE_GET_TASK_FILTER(pxTask) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_PRIO_DISINHERIT, (uint32_t)pxTask, uxNewPriority); \
	}

/* Called in vTaskResume */
#undef traceTASK_RESUME
#define traceTASK_RESUME( pxTaskToResume ) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_TASK_FILTER(pxTaskToResume) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent1(PSF_EVENT_TASK_RESUME, (uint32_t)pxTaskToResume); \
		} \
	}

/* Called in vTaskResumeFromISR */
#undef traceTASK_RESUME_FROM_ISR
#define traceTASK_RESUME_FROM_ISR( pxTaskToResume ) \
	if (TRACE_GET_TASK_FILTER(pxTaskToResume) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent1(PSF_EVENT_TASK_RESUME_FROMISR, (uint32_t)pxTaskToResume); \
	}

/* Called in timer.c - xTimerCreate */
#undef traceTIMER_CREATE
#define traceTIMER_CREATE(tmr) \
	if (pdPASS == xReturn) \
	{ \
		TRACE_SET_OBJECT_FILTER(TIMER, tmr, CurrentFilterGroup); \
		prvTraceSaveObjectSymbol(tmr, (const char*)tmr->pcTimerName); \
		prvTraceStoreStringEvent(1, PSF_EVENT_OBJ_NAME, (const char*)tmr->pcTimerName, tmr); \
		if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		{ \
			if (TRACE_GET_OBJECT_FILTER(TIMER, tmr) & CurrentFilterMask) \
			{ \
				prvTraceStoreEvent2(PSF_EVENT_TIMER_CREATE, (uint32_t)tmr, tmr->xTimerPeriodInTicks); \
			} \
		} \
	} \
	else \
	{ \
		if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_TIMER_CREATE_FAILED, 0, 0); \
		} \
	}

#undef traceTIMER_COMMAND_SEND
#define traceTIMER_COMMAND_SEND(tmr, xCommandID, xOptionalValue, xReturn) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (trcIS_TIMER_VALID(tmr)) \
		{ \
			if (TRACE_GET_OBJECT_FILTER(TIMER, tmr) & CurrentFilterMask) \
			{ \
				switch(xCommandID) \
				{ \
					case timerCOMMAND_START: \
						prvTraceStoreEvent1((xReturn == pdPASS) ? PSF_EVENT_TIMER_START : PSF_EVENT_TIMER_START_FAILED, (uint32_t)tmr); \
						break; \
					case timerCOMMAND_STOP: \
						prvTraceStoreEvent1((xReturn == pdPASS) ? PSF_EVENT_TIMER_STOP : PSF_EVENT_TIMER_STOP_FAILED, (uint32_t)tmr); \
						break; \
					case timerCOMMAND_CHANGE_PERIOD: \
						prvTraceStoreEvent2((xReturn == pdPASS) ? PSF_EVENT_TIMER_CHANGEPERIOD : PSF_EVENT_TIMER_CHANGEPERIOD_FAILED, (uint32_t)tmr, xOptionalValue); \
						break; \
					case timerCOMMAND_DELETE: \
						prvTraceStoreEvent1((xReturn == pdPASS) ? PSF_EVENT_TIMER_DELETE : PSF_EVENT_TIMER_DELETE_FAILED, (uint32_t)tmr); \
						break; \
				} \
			} \
		} \
		else \
		{ \
			switch(xCommandID) \
			{ \
				case timerCOMMAND_START: \
					prvTraceStoreEvent1((xReturn == pdPASS) ? PSF_EVENT_TIMER_START : PSF_EVENT_TIMER_START_FAILED, (uint32_t)tmr); \
					break; \
				case timerCOMMAND_STOP: \
					prvTraceStoreEvent1((xReturn == pdPASS) ? PSF_EVENT_TIMER_STOP : PSF_EVENT_TIMER_STOP_FAILED, (uint32_t)tmr); \
					break; \
				case timerCOMMAND_CHANGE_PERIOD: \
					prvTraceStoreEvent2((xReturn == pdPASS) ? PSF_EVENT_TIMER_CHANGEPERIOD : PSF_EVENT_TIMER_CHANGEPERIOD_FAILED, (uint32_t)tmr, xOptionalValue); \
					break; \
				case timerCOMMAND_DELETE: \
					prvTraceStoreEvent1((xReturn == pdPASS) ? PSF_EVENT_TIMER_DELETE : PSF_EVENT_TIMER_DELETE_FAILED, (uint32_t)tmr); \
					break; \
			} \
		} \
	}

#undef traceTIMER_COMMAND_SEND_FROM_ISR
#define traceTIMER_COMMAND_SEND_FROM_ISR(tmr, xCommandID, xOptionalValue, xReturn) \
	if (trcIS_TIMER_VALID(tmr)) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(TIMER, tmr) & CurrentFilterMask) \
		{ \
			switch(xCommandID) \
			{ \
				case timerCOMMAND_START: \
					prvTraceStoreEvent1((xReturn == pdPASS) ? PSF_EVENT_TIMER_START_FROMISR : PSF_EVENT_TIMER_START_FROMISR_FAILED, (uint32_t)tmr); \
					break; \
				case timerCOMMAND_STOP: \
					prvTraceStoreEvent1((xReturn == pdPASS) ? PSF_EVENT_TIMER_STOP_FROMISR : PSF_EVENT_TIMER_STOP_FROMISR_FAILED, (uint32_t)tmr); \
					break; \
				case timerCOMMAND_CHANGE_PERIOD: \
					prvTraceStoreEvent2((xReturn == pdPASS) ? PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR : PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR_FAILED, (uint32_t)tmr, xOptionalValue); \
					break; \
			} \
		} \
	} \
	else \
	{ \
		switch (xCommandID) \
		{ \
		case timerCOMMAND_START: \
			prvTraceStoreEvent1((xReturn == pdPASS) ? PSF_EVENT_TIMER_START_FROMISR : PSF_EVENT_TIMER_START_FROMISR_FAILED, (uint32_t)tmr); \
			break; \
		case timerCOMMAND_STOP: \
			prvTraceStoreEvent1((xReturn == pdPASS) ? PSF_EVENT_TIMER_STOP_FROMISR : PSF_EVENT_TIMER_STOP_FROMISR_FAILED, (uint32_t)tmr); \
			break; \
		case timerCOMMAND_CHANGE_PERIOD: \
			prvTraceStoreEvent2((xReturn == pdPASS) ? PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR : PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR_FAILED, (uint32_t)tmr, xOptionalValue); \
			break; \
		} \
	}

#undef traceTIMER_COMMAND_PROCESS
#define traceTIMER_COMMAND_PROCESS( tmr, xCommandID, xOptionalValue ) \
	if (TRACE_GET_OBJECT_FILTER(TIMER, tmr) & CurrentFilterMask) \
	{ \
		switch(xCommandID) \
		{ \
			case timerCOMMAND_START: \
				prvTraceStoreEvent1(PSF_EVENT_TIMER_PROCESS_START, (uint32_t)tmr); \
				break; \
			case timerCOMMAND_STOP: \
				prvTraceStoreEvent1(PSF_EVENT_TIMER_PROCESS_STOP, (uint32_t)tmr); \
				break; \
			case timerCOMMAND_CHANGE_PERIOD: \
				prvTraceStoreEvent2(PSF_EVENT_TIMER_PROCESS_CHANGEPERIOD, (uint32_t)tmr, xOptionalValue); \
				break; \
			case timerCOMMAND_DELETE: \
				prvTraceStoreEvent1(PSF_EVENT_TIMER_PROCESS_DELETE, (uint32_t)tmr); \
				prvTraceDeleteSymbol(tmr); \
				break; \
		} \
	}

#undef traceTIMER_CALLBACK
#define traceTIMER_CALLBACK( tmr, callbackFunction ) \
	if (TRACE_GET_OBJECT_FILTER(TIMER, tmr) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TIMER_CALLBACK, (uint32_t)tmr, (uint32_t)callbackFunction); \
	}

#undef traceEVENT_GROUP_CREATE
#define traceEVENT_GROUP_CREATE(eg) \
	TRACE_SET_OBJECT_FILTER(EVENTGROUP, eg, CurrentFilterGroup); \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(EVENTGROUP, eg) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent1(PSF_EVENT_EVENTGROUP_CREATE, (uint32_t)eg); \
		} \
	}

#undef traceEVENT_GROUP_CREATE_FAILED
#define traceEVENT_GROUP_CREATE_FAILED() \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent1(PSF_EVENT_EVENTGROUP_CREATE_FAILED, 0); \
	}

#undef traceEVENT_GROUP_DELETE
#define traceEVENT_GROUP_DELETE(eg) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(EVENTGROUP, eg) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent1(PSF_EVENT_EVENTGROUP_DELETE, (uint32_t)eg); \
		} \
	} \
	prvTraceDeleteSymbol(eg);

#undef traceEVENT_GROUP_DELETE_FAILED
#define traceEVENT_GROUP_DELETE_FAILED(eg) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent1(PSF_EVENT_EVENTGROUP_DELETE_FAILED, (uint32_t)eg); \
	}

#undef traceEVENT_GROUP_SET_BITS
#define traceEVENT_GROUP_SET_BITS(eg, bitsToSet) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(EVENTGROUP, eg) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_SETBITS, (uint32_t)eg, bitsToSet); \
		} \
	}

#undef traceEVENT_GROUP_SET_BITS_FAILED
#define traceEVENT_GROUP_SET_BITS_FAILED(eg, bitsToSet) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (trcIS_EVENTGROUP_VALID(eg)) \
		{ \
			if (TRACE_GET_OBJECT_FILTER(EVENTGROUP, eg) & CurrentFilterMask) \
			{ \
				prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_SETBITS_FAILED, (uint32_t)eg, bitsToSet); \
			} \
		} \
		else \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_SETBITS_FAILED, (uint32_t)eg, bitsToSet); \
		} \
	}

#undef traceEVENT_GROUP_SET_BITS_FROM_ISR
#define traceEVENT_GROUP_SET_BITS_FROM_ISR(eg, bitsToSet) \
	if (TRACE_GET_OBJECT_FILTER(EVENTGROUP, eg) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_SETBITS_FROMISR, (uint32_t)eg, bitsToSet); \
	}

#undef traceEVENT_GROUP_SET_BITS_FROM_ISR_FAILED
#define traceEVENT_GROUP_SET_BITS_FROM_ISR_FAILED(eg, bitsToSet) \
	if (trcIS_EVENTGROUP_VALID(eg)) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(EVENTGROUP, eg) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_SETBITS_FROMISR_FAILED, (uint32_t)eg, bitsToSet); \
		} \
	} \
	else \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_SETBITS_FROMISR_FAILED, (uint32_t)eg, bitsToSet); \
	}

#undef traceEVENT_GROUP_WAIT_BITS
#define traceEVENT_GROUP_WAIT_BITS(eg, bitsToWaitFor, timeout) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(EVENTGROUP, eg) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent3(PSF_EVENT_EVENTGROUP_WAITBITS, (uint32_t)eg, bitsToWaitFor, timeout); \
		} \
	}

#undef traceEVENT_GROUP_WAIT_BITS_FAILED
#define traceEVENT_GROUP_WAIT_BITS_FAILED(eg, bitsToWaitFor, timeout) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (trcIS_EVENTGROUP_VALID(eg)) \
		{ \
			if (TRACE_GET_OBJECT_FILTER(EVENTGROUP, eg) & CurrentFilterMask) \
			{ \
				prvTraceStoreEvent3(PSF_EVENT_EVENTGROUP_WAITBITS_FAILED, (uint32_t)eg, bitsToWaitFor, timeout); \
			} \
		} \
		else \
		{ \
			prvTraceStoreEvent3(PSF_EVENT_EVENTGROUP_WAITBITS_FAILED, (uint32_t)eg, bitsToWaitFor, timeout); \
		} \
	}

#undef traceEVENT_GROUP_WAIT_BITS_BLOCK
#define traceEVENT_GROUP_WAIT_BITS_BLOCK(eg, bitsToWaitFor, timeout) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(EVENTGROUP, eg) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent3(PSF_EVENT_EVENTGROUP_WAITBITS_BLOCK, (uint32_t)eg, bitsToWaitFor, timeout); \
		} \
	}

#undef traceEVENT_GROUP_CLEAR_BITS
#define traceEVENT_GROUP_CLEAR_BITS(eg, bitsToClear) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(EVENTGROUP, eg) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_CLEARBITS, (uint32_t)eg, bitsToClear); \
		} \
	}

#undef traceEVENT_GROUP_CLEAR_BITS_FAILED
#define traceEVENT_GROUP_CLEAR_BITS_FAILED(eg, bitsToClear) \
	if (TRACE_GET_TASK_FILTER(TRACE_GET_CURRENT_TASK()) & CurrentFilterMask) \
	{ \
		if (trcIS_EVENTGROUP_VALID(eg)) \
		{ \
			if (TRACE_GET_OBJECT_FILTER(EVENTGROUP, eg) & CurrentFilterMask) \
			{ \
				prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_CLEARBITS_FAILED, (uint32_t)eg, bitsToClear); \
			} \
		} \
		else \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_CLEARBITS_FAILED, (uint32_t)eg, bitsToClear); \
		} \
	}

#undef traceEVENT_GROUP_CLEAR_BITS_FROM_ISR
#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR(eg, bitsToClear) \
	if (TRACE_GET_OBJECT_FILTER(EVENTGROUP, eg) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR, (uint32_t)eg, bitsToClear); \
	}

#undef traceEVENT_GROUP_CLEAR_BITS_FROM_ISR_FAILED
#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR_FAILED(eg, bitsToClear) \
	if (trcIS_EVENTGROUP_VALID(eg)) \
	{ \
		if (TRACE_GET_OBJECT_FILTER(EVENTGROUP, eg) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR_FAILED, (uint32_t)eg, bitsToClear); \
		} \
	} \
	else \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR_FAILED, (uint32_t)eg, bitsToClear); \
	} \

#undef traceTASK_NOTIFY_WAIT
#define traceTASK_NOTIFY_WAIT() \
	if (TRACE_GET_TASK_FILTER(pxCurrentTCB) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_WAIT, (uint32_t)pxCurrentTCB, xTicksToWait); \
	}

#undef traceTASK_NOTIFY_WAIT_FAILED
#define traceTASK_NOTIFY_WAIT_FAILED() \
	if (TRACE_GET_TASK_FILTER(pxCurrentTCB) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_WAIT_FAILED, (uint32_t)pxCurrentTCB, xTicksToWait); \
	}

#undef traceTASK_NOTIFY_WAIT_BLOCK
#define traceTASK_NOTIFY_WAIT_BLOCK() \
	if (TRACE_GET_TASK_FILTER(pxCurrentTCB) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_WAIT_BLOCK, (uint32_t)pxCurrentTCB, xTicksToWait); \
	}

#undef traceTASK_NOTIFY_SEND
#define traceTASK_NOTIFY_SEND() \
	if (TRACE_GET_TASK_FILTER(pxCurrentTCB) & CurrentFilterMask) \
	{ \
		if (TRACE_GET_TASK_FILTER(xTaskToNotify) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_SEND, (uint32_t)xTaskToNotify, uxValue); \
		} \
	}

#undef traceTASK_NOTIFY_SEND_FAILED
#define traceTASK_NOTIFY_SEND_FAILED() \
	if (TRACE_GET_TASK_FILTER(pxCurrentTCB) & CurrentFilterMask) \
	{ \
		if (trcIS_TASK_VALID(xTaskToNotify)) \
		{ \
			if (TRACE_GET_TASK_FILTER(xTaskToNotify) & CurrentFilterMask) \
			{ \
				prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_SEND_FAILED, (uint32_t)xTaskToNotify, uxValue); \
			} \
		} \
		else \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_SEND_FAILED, (uint32_t)xTaskToNotify, uxValue); \
		} \
	}

#undef traceTASK_NOTIFY_SEND_FROM_ISR
#define traceTASK_NOTIFY_SEND_FROM_ISR() \
	if (TRACE_GET_TASK_FILTER(xTaskToNotify) & CurrentFilterMask) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_SEND_FROM_ISR, (uint32_t)xTaskToNotify, uxValue); \
	}

#undef traceTASK_NOTIFY_SEND_FROM_ISR_FAILED
#define traceTASK_NOTIFY_SEND_FROM_ISR_FAILED() \
	if (trcIS_TASK_VALID(xTaskToNotify)) \
	{ \
		if (TRACE_GET_TASK_FILTER(xTaskToNotify) & CurrentFilterMask) \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_SEND_FROM_ISR_FAILED, (uint32_t)xTaskToNotify, uxValue); \
		} \
	} \
	else \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_NOTIFY_SEND_FROM_ISR_FAILED, (uint32_t)xTaskToNotify, uxValue); \
	} \

#endif /* (TRC_CFG_SCHEDULING_ONLY == 0) */

#endif /*#if TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING  */

#if (TRC_CFG_SCHEDULING_ONLY == 1)
#define traceLOW_POWER_IDLE_BEGIN() 
#define traceLOW_POWER_IDLE_END() 
#define traceTASK_SUSPEND( pxTaskToSuspend ) 
#define traceTASK_DELAY() 
#define traceTASK_DELAY_UNTIL(xTimeToWake) 
#define traceQUEUE_CREATE( pxNewQueue ) 
#define traceQUEUE_CREATE_FAILED( pcBuffer, queueType, queueLength ) 
#define traceQUEUE_SEND( pxQueue ) 
#define traceQUEUE_SEND_FAILED( pxQueue ) 
#define traceBLOCKING_ON_QUEUE_SEND( pxQueue ) 
#define traceQUEUE_SEND_FROM_ISR( pxQueue ) 
#define traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue ) 
#define traceQUEUE_RECEIVE( pxQueue ) 
#define traceQUEUE_RECEIVE_FAILED( pxQueue ) 
#define traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue ) 
#define traceQUEUE_RECEIVE_FROM_ISR( pxQueue ) 
#define traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue ) 
#define traceQUEUE_PEEK( pxQueue ) 
#define traceQUEUE_PEEK_FAILED( pxQueue ) 
#define traceBLOCKING_ON_QUEUE_PEEK( pxQueue ) 
#define traceTASK_PRIORITY_SET( pxTask, uxNewPriority ) 
#define traceTASK_PRIORITY_INHERIT( pxTask, uxNewPriority ) 
#define traceTASK_PRIORITY_DISINHERIT( pxTask, uxNewPriority ) 
#define traceTASK_RESUME( pxTaskToResume ) 
#define traceTASK_RESUME_FROM_ISR( pxTaskToResume ) 
#define traceTIMER_CREATE(tmr) 
#define traceTIMER_COMMAND_SEND(tmr, xCommandID, xOptionalValue, xReturn) 
#define traceTIMER_COMMAND_SEND_FROM_ISR(tmr, xCommandID, xOptionalValue, xReturn) 
#define traceTIMER_COMMAND_PROCESS( tmr, xCommandID, xOptionalValue ) 
#define traceTIMER_CALLBACK( tmr, callbackFunction ) 
#define traceEVENT_GROUP_CREATE(eg) 
#define traceEVENT_GROUP_CREATE_FAILED() 
#define traceEVENT_GROUP_DELETE(eg) 
#define traceEVENT_GROUP_DELETE_FAILED(eg) 
#define traceEVENT_GROUP_SET_BITS(eg, bitsToSet) 
#define traceEVENT_GROUP_SET_BITS_FAILED(eg, bitsToSet) 
#define traceEVENT_GROUP_SET_BITS_FROM_ISR(eg, bitsToSet) 
#define traceEVENT_GROUP_SET_BITS_FROM_ISR_FAILED(eg, bitsToSet) 
#define traceEVENT_GROUP_WAIT_BITS(eg, bitsToWaitFor, timeout) 
#define traceEVENT_GROUP_WAIT_BITS_FAILED(eg, bitsToWaitFor, timeout) 
#define traceEVENT_GROUP_WAIT_BITS_BLOCK(eg, bitsToWaitFor, timeout) 
#define traceEVENT_GROUP_CLEAR_BITS(eg, bitsToClear) 
#define traceEVENT_GROUP_CLEAR_BITS_FAILED(eg, bitsToClear) 
#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR(eg, bitsToClear) 
#define traceEVENT_GROUP_CLEAR_BITS_FROM_ISR_FAILED(eg, bitsToClear) 
#define traceTASK_NOTIFY_WAIT() 
#define traceTASK_NOTIFY_WAIT_FAILED() 
#define traceTASK_NOTIFY_WAIT_BLOCK() 
#define traceTASK_NOTIFY_SEND() 
#define traceTASK_NOTIFY_SEND_FAILED() 
#define traceTASK_NOTIFY_SEND_FROM_ISR() 
#define traceTASK_NOTIFY_SEND_FROM_ISR_FAILED() 
#endif /* (TRC_CFG_SCHEDULING_ONLY == 1) */

#else /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/
	
	/* when recorder disabled */
	#define vTraceSetQueueName(object, name)
	#define vTraceSetSemaphoreName(object, name)
	#define vTraceSetMutexName(object, name)
	#define vTraceSetEventGroupName(object, name)

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_H */
