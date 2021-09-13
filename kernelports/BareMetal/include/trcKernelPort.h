/*
 * Trace Recorder for Tracealyzer v4.5.1
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * For bare-metal use of Tracealyzer (no RTOS)
 */

#ifndef TRC_KERNEL_PORT_H
#define TRC_KERNEL_PORT_H

#include "trcPortDefines.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRC_USE_TRACEALYZER_RECORDER 1 /* Allows for disabling the recorder */

#define TRACE_CPU_CLOCK_HZ 100000 /* Change this, or get the below error */

#if (TRACE_CPU_CLOCK_HZ == 1)
	#error "Please set TRACE_CPU_CLOCK_HZ to match the processor clock frequency."
#endif

/* If using dynamic allocation of snapshot trace buffer, set this to your memory allocation function... */
#define TRACE_MALLOC(size) malloc(size) 	

/*** Don't change the below definitions, unless you know what you are doing! ***/

#define TRACE_KERNEL_VERSION 0x1AA1

/* Dummy definitions, since no RTOS */
#define TRACE_TICK_RATE_HZ 1 /* Must not be 0. */
#define TRACE_GET_CURRENT_TASK() 0
#define TRACE_GET_OS_TICKS() (0)
#define TRACE_GET_TASK_NUMBER(x) 0

/* Hardcoded to 0 for bare metal */
#define prvTraceIsSchedulerSuspended() 0
#define prvIsNewTCB(x) 0
#define prvTraceGetCurrentTaskHandle(x) NULL

#if (defined(TRC_USE_TRACEALYZER_RECORDER)) && (TRC_USE_TRACEALYZER_RECORDER == 1)

#define TRC_PLATFORM_CFG ""
#define TRC_PLATFORM_CFG_MAJOR 1
#define TRC_PLATFORM_CFG_MINOR 0
#define TRC_PLATFORM_CFG_PATCH 0

/* For ARM Cortex-M devices - assumes the ARM CMSIS API is available */
#if (defined (__CORTEX_M))	
	#define TRACE_ALLOC_CRITICAL_SECTION() uint32_t __irq_status;
	#define TRACE_ENTER_CRITICAL_SECTION() {__irq_status = __get_PRIMASK(); __set_PRIMASK(1);} /* PRIMASK disables ALL interrupts - allows for tracing in any ISR */
	#define TRACE_EXIT_CRITICAL_SECTION() {__set_PRIMASK(__irq_status);}
#endif

#if (TRC_CFG_HARDWARE_PORT == TRC_HARDWARE_PORT_Win32)
    /* In the Win32 port, there are no real ISRs, so we don't need critical sections in this case... */
	#define TRACE_ALLOC_CRITICAL_SECTION()
	#define TRACE_ENTER_CRITICAL_SECTION()  
	#define TRACE_EXIT_CRITICAL_SECTION()
#endif

#ifndef TRACE_ENTER_CRITICAL_SECTION
	#error "This hardware port has no definition for critical sections! See http://percepio.com/2014/10/27/how-to-define-critical-sections-for-the-recorder/"
#endif


/******************************************************************************/
/*** Definitions for Snapshot mode ********************************************/
/******************************************************************************/
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)

/*** The object classes *******************************************************/

/* Only the ISR class is needed for bare metal, so TRC_CFG_NTASK, TRC_CFG_NTIMER, etc. can be 0 to reduce memory usage. */

#define TRACE_NCLASSES 9
#define TRACE_CLASS_QUEUE ((traceObjectClass)0)
#define TRACE_CLASS_SEMAPHORE ((traceObjectClass)1)
#define TRACE_CLASS_MUTEX ((traceObjectClass)2)
#define TRACE_CLASS_TASK ((traceObjectClass)3)
#define TRACE_CLASS_ISR ((traceObjectClass)4)
#define TRACE_CLASS_TIMER ((traceObjectClass)5)
#define TRACE_CLASS_EVENTGROUP ((traceObjectClass)6)
#define TRACE_CLASS_STREAMBUFFER ((traceObjectClass)7)
#define TRACE_CLASS_MESSAGEBUFFER ((traceObjectClass)8)

/*** Definitions for Object Table ********************************************/
#define TRACE_KERNEL_OBJECT_COUNT (TRC_CFG_NQUEUE + TRC_CFG_NSEMAPHORE + TRC_CFG_NMUTEX + TRC_CFG_NTASK + TRC_CFG_NISR + TRC_CFG_NTIMER + TRC_CFG_NEVENTGROUP + TRC_CFG_NSTREAMBUFFER + TRC_CFG_NMESSAGEBUFFER)

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

/* TRC_CFG_NSTREAMBUFFER properties:			Byte 0-3: state (unused for now)*/
#define PropertyTableSizeStreamBuffer	(TRC_CFG_NAME_LEN_STREAMBUFFER + 4)

/* TRC_CFG_NMESSAGEBUFFER properties:			Byte 0-3: state (unused for now)*/
#define PropertyTableSizeMessageBuffer	(TRC_CFG_NAME_LEN_MESSAGEBUFFER + 4)


/* The layout of the byte array representing the Object Property Table */
#define StartIndexQueue			0
#define StartIndexSemaphore		StartIndexQueue			+ TRC_CFG_NQUEUE			* PropertyTableSizeQueue
#define StartIndexMutex			StartIndexSemaphore 	+ TRC_CFG_NSEMAPHORE		* PropertyTableSizeSemaphore
#define StartIndexTask			StartIndexMutex			+ TRC_CFG_NMUTEX			* PropertyTableSizeMutex
#define StartIndexISR			StartIndexTask			+ TRC_CFG_NTASK				* PropertyTableSizeTask
#define StartIndexTimer			StartIndexISR			+ TRC_CFG_NISR				* PropertyTableSizeISR
#define StartIndexEventGroup	StartIndexTimer			+ TRC_CFG_NTIMER			* PropertyTableSizeTimer
#define StartIndexStreamBuffer	StartIndexEventGroup	+ TRC_CFG_NEVENTGROUP		* PropertyTableSizeEventGroup
#define StartIndexMessageBuffer	StartIndexStreamBuffer	+ TRC_CFG_NSTREAMBUFFER		* PropertyTableSizeStreamBuffer

/* Number of bytes used by the object table */
#define TRACE_OBJECT_TABLE_SIZE	StartIndexMessageBuffer	+ TRC_CFG_NMESSAGEBUFFER	* PropertyTableSizeMessageBuffer

/* Initialization of the object property table */
void vTraceInitObjectPropertyTable(void);

/* Initialization of the handle mechanism, see e.g, prvTraceGetObjectHandle */
void vTraceInitObjectHandleStack(void);

/* Returns the "Not enough handles" error message for the specified object class */
const char* pszTraceGetErrorNotEnoughHandles(traceObjectClass objectclass);


/*** Event codes for snapshot mode - must match Tracealyzer config files ******/

#define NULL_EVENT					(0x00UL)

/*******************************************************************************
 * EVENTGROUP_DIV
 *
 * Miscellaneous events.
 ******************************************************************************/
#define EVENTGROUP_DIV				(NULL_EVENT + 1UL)					/*0x01*/
#define DIV_XPS						(EVENTGROUP_DIV + 0UL)				/*0x01*/
#define DIV_TASK_READY				(EVENTGROUP_DIV + 1UL)				/*0x02*/
#define DIV_NEW_TIME				(EVENTGROUP_DIV + 2UL)				/*0x03*/

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
#define EVENTGROUP_OBJCLOSE_NAME_SUCCESS	(EVENTGROUP_TS + 4UL)					/*0x08*/

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
#define EVENTGROUP_OBJCLOSE_PROP_SUCCESS	(EVENTGROUP_OBJCLOSE_NAME_SUCCESS + 8UL)		/*0x10*/

/*******************************************************************************
 * EVENTGROUP_CREATE
 *
 * The events in this group are used to log Kernel object creations.
 * The lower three bits in the event code gives the object class, i.e., type of
 * create operation (task, queue, semaphore, etc).
 ******************************************************************************/
#define EVENTGROUP_CREATE_OBJ_SUCCESS	(EVENTGROUP_OBJCLOSE_PROP_SUCCESS + 8UL)	/*0x18*/

/*******************************************************************************
 * EVENTGROUP_SEND
 *
 * The events in this group are used to log Send/Give events on queues,
 * semaphores and mutexes The lower three bits in the event code gives the
 * object class, i.e., what type of object that is operated on (queue, semaphore
 * or mutex).
 ******************************************************************************/
#define EVENTGROUP_SEND_SUCCESS	(EVENTGROUP_CREATE_OBJ_SUCCESS + 8UL)		/*0x20*/

/*******************************************************************************
 * EVENTGROUP_RECEIVE
 *
 * The events in this group are used to log Receive/Take events on queues,
 * semaphores and mutexes. The lower three bits in the event code gives the
 * object class, i.e., what type of object that is operated on (queue, semaphore
 * or mutex).
 ******************************************************************************/
#define EVENTGROUP_RECEIVE_SUCCESS	(EVENTGROUP_SEND_SUCCESS + 8UL)		/*0x28*/

/* Send/Give operations, from ISR */
#define EVENTGROUP_SEND_FROM_ISR_SUCCESS \
									(EVENTGROUP_RECEIVE_SUCCESS + 8UL)	/*0x30*/

/* Receive/Take operations, from ISR */
#define EVENTGROUP_RECEIVE_FROM_ISR_SUCCESS \
							(EVENTGROUP_SEND_FROM_ISR_SUCCESS + 8UL)		/*0x38*/

/* "Failed" event type versions of above (timeout, failed allocation, etc) */
#define EVENTGROUP_KSE_FAILED \
							(EVENTGROUP_RECEIVE_FROM_ISR_SUCCESS + 8UL)	/*0x40*/

/* Failed create calls - memory allocation failed */
#define EVENTGROUP_CREATE_OBJ_FAILED	(EVENTGROUP_KSE_FAILED)			/*0x40*/

/* Failed send/give - timeout! */
#define EVENTGROUP_SEND_FAILED		(EVENTGROUP_CREATE_OBJ_FAILED + 8UL)	/*0x48*/

/* Failed receive/take - timeout! */
#define EVENTGROUP_RECEIVE_FAILED	 (EVENTGROUP_SEND_FAILED + 8UL)		/*0x50*/

/* Failed non-blocking send/give - queue full */
#define EVENTGROUP_SEND_FROM_ISR_FAILED (EVENTGROUP_RECEIVE_FAILED + 8UL) /*0x58*/

/* Failed non-blocking receive/take - queue empty */
#define EVENTGROUP_RECEIVE_FROM_ISR_FAILED \
								 (EVENTGROUP_SEND_FROM_ISR_FAILED + 8UL)	/*0x60*/

/* Events when blocking on receive/take */
#define EVENTGROUP_RECEIVE_BLOCK \
							(EVENTGROUP_RECEIVE_FROM_ISR_FAILED + 8UL)	/*0x68*/

/* Events when blocking on send/give */
#define EVENTGROUP_SEND_BLOCK	(EVENTGROUP_RECEIVE_BLOCK + 8UL)			/*0x70*/

/* Events on queue peek (receive) */
#define EVENTGROUP_PEEK_SUCCESS	(EVENTGROUP_SEND_BLOCK + 8UL)				/*0x78*/

/* Events on object delete (vTaskDelete or vQueueDelete) */
#define EVENTGROUP_DELETE_OBJ_SUCCESS	(EVENTGROUP_PEEK_SUCCESS + 8UL)	/*0x80*/

/* Other events - object class is implied: TASK */
#define EVENTGROUP_OTHERS	(EVENTGROUP_DELETE_OBJ_SUCCESS + 8UL)			/*0x88*/
#define TASK_DELAY_UNTIL	(EVENTGROUP_OTHERS + 0UL)						/*0x88*/
#define TASK_DELAY			(EVENTGROUP_OTHERS + 1UL)						/*0x89*/
#define TASK_SUSPEND		(EVENTGROUP_OTHERS + 2UL)						/*0x8A*/
#define TASK_RESUME			(EVENTGROUP_OTHERS + 3UL)						/*0x8B*/
#define TASK_RESUME_FROM_ISR	(EVENTGROUP_OTHERS + 4UL)					/*0x8C*/
#define TASK_PRIORITY_SET		(EVENTGROUP_OTHERS + 5UL)					/*0x8D*/
#define TASK_PRIORITY_INHERIT	(EVENTGROUP_OTHERS + 6UL)					/*0x8E*/
#define TASK_PRIORITY_DISINHERIT	(EVENTGROUP_OTHERS + 7UL)				/*0x8F*/

#define EVENTGROUP_MISC_PLACEHOLDER	(EVENTGROUP_OTHERS + 8UL)				/*0x90*/
#define PEND_FUNC_CALL		(EVENTGROUP_MISC_PLACEHOLDER+0UL)				/*0x90*/
#define PEND_FUNC_CALL_FROM_ISR (EVENTGROUP_MISC_PLACEHOLDER+1UL)			/*0x91*/
#define PEND_FUNC_CALL_FAILED (EVENTGROUP_MISC_PLACEHOLDER+2UL)			/*0x92*/
#define PEND_FUNC_CALL_FROM_ISR_FAILED (EVENTGROUP_MISC_PLACEHOLDER+3UL)	/*0x93*/
#define MEM_MALLOC_SIZE (EVENTGROUP_MISC_PLACEHOLDER+4UL)					/*0x94*/
#define MEM_MALLOC_ADDR (EVENTGROUP_MISC_PLACEHOLDER+5UL)					/*0x95*/
#define MEM_FREE_SIZE (EVENTGROUP_MISC_PLACEHOLDER+6UL)					/*0x96*/
#define MEM_FREE_ADDR (EVENTGROUP_MISC_PLACEHOLDER+7UL)					/*0x97*/

/* User events */
#define EVENTGROUP_USEREVENT (EVENTGROUP_MISC_PLACEHOLDER + 8UL)			/*0x98*/
#define USER_EVENT (EVENTGROUP_USEREVENT + 0UL)

/* Allow for 0-15 arguments (the number of args is added to event code) */
#define USER_EVENT_LAST (EVENTGROUP_USEREVENT + 15UL)						/*0xA7*/

#define EVENTGROUP_SYS (EVENTGROUP_USEREVENT + 16UL)						/*0xA8*/
#define XTS8 (EVENTGROUP_SYS + 0UL)										/*0xA8*/
#define XTS16 (EVENTGROUP_SYS + 1UL)										/*0xA9*/
#define EVENT_BEING_WRITTEN (EVENTGROUP_SYS + 2UL)						/*0xAA*/
#define RESERVED_DUMMY_CODE (EVENTGROUP_SYS + 3UL)						/*0xAB*/
#define LOW_POWER_BEGIN (EVENTGROUP_SYS + 4UL)							/*0xAC*/
#define LOW_POWER_END (EVENTGROUP_SYS + 5UL)								/*0xAD*/
#define XID (EVENTGROUP_SYS + 6UL)										/*0xAE*/
#define XTS16L (EVENTGROUP_SYS + 7UL)										/*0xAF*/

#define EVENTGROUP_TIMER (EVENTGROUP_SYS + 8UL)							/*0xB0*/
#define TIMER_CREATE (EVENTGROUP_TIMER + 0UL)								/*0xB0*/
#define TIMER_START (EVENTGROUP_TIMER + 1UL)								/*0xB1*/
#define TIMER_RST (EVENTGROUP_TIMER + 2UL)								/*0xB2*/
#define TIMER_STOP (EVENTGROUP_TIMER + 3UL)								/*0xB3*/
#define TIMER_CHANGE_PERIOD (EVENTGROUP_TIMER + 4UL)						/*0xB4*/
#define TIMER_DELETE_OBJ (EVENTGROUP_TIMER + 5UL)								/*0xB5*/
#define TIMER_START_FROM_ISR (EVENTGROUP_TIMER + 6UL)						/*0xB6*/
#define TIMER_RESET_FROM_ISR (EVENTGROUP_TIMER + 7UL)						/*0xB7*/
#define TIMER_STOP_FROM_ISR (EVENTGROUP_TIMER + 8UL)						/*0xB8*/

#define TIMER_CREATE_FAILED (EVENTGROUP_TIMER + 9UL)						/*0xB9*/
#define TIMER_START_FAILED (EVENTGROUP_TIMER + 10UL)						/*0xBA*/
#define TIMER_RESET_FAILED (EVENTGROUP_TIMER + 11UL)						/*0xBB*/
#define TIMER_STOP_FAILED (EVENTGROUP_TIMER + 12UL)						/*0xBC*/
#define TIMER_CHANGE_PERIOD_FAILED (EVENTGROUP_TIMER + 13UL)				/*0xBD*/
#define TIMER_DELETE_FAILED (EVENTGROUP_TIMER + 14UL)						/*0xBE*/
#define TIMER_START_FROM_ISR_FAILED (EVENTGROUP_TIMER + 15UL)				/*0xBF*/
#define TIMER_RESET_FROM_ISR_FAILED (EVENTGROUP_TIMER + 16UL)				/*0xC0*/
#define TIMER_STOP_FROM_ISR_FAILED (EVENTGROUP_TIMER + 17UL)				/*0xC1*/

#define EVENTGROUP_EG (EVENTGROUP_TIMER + 18UL)							/*0xC2*/
#define EVENT_GROUP_CREATE (EVENTGROUP_EG + 0UL)							/*0xC2*/
#define EVENT_GROUP_CREATE_FAILED (EVENTGROUP_EG + 1UL)					/*0xC3*/
#define EVENT_GROUP_SYNC_BLOCK (EVENTGROUP_EG + 2UL)						/*0xC4*/
#define EVENT_GROUP_SYNC_END (EVENTGROUP_EG + 3UL)						/*0xC5*/
#define EVENT_GROUP_WAIT_BITS_BLOCK (EVENTGROUP_EG + 4UL)					/*0xC6*/
#define EVENT_GROUP_WAIT_BITS_END (EVENTGROUP_EG + 5UL)					/*0xC7*/
#define EVENT_GROUP_CLEAR_BITS (EVENTGROUP_EG + 6UL)						/*0xC8*/
#define EVENT_GROUP_CLEAR_BITS_FROM_ISR (EVENTGROUP_EG + 7UL)				/*0xC9*/
#define EVENT_GROUP_SET_BITS (EVENTGROUP_EG + 8UL)						/*0xCA*/
#define EVENT_GROUP_DELETE_OBJ (EVENTGROUP_EG + 9UL)							/*0xCB*/
#define EVENT_GROUP_SYNC_END_FAILED (EVENTGROUP_EG + 10UL)				/*0xCC*/
#define EVENT_GROUP_WAIT_BITS_END_FAILED (EVENTGROUP_EG + 11UL)			/*0xCD*/
#define EVENT_GROUP_SET_BITS_FROM_ISR (EVENTGROUP_EG + 12UL)				/*0xCE*/
#define EVENT_GROUP_SET_BITS_FROM_ISR_FAILED (EVENTGROUP_EG + 13UL)		/*0xCF*/

#define TASK_INSTANCE_FINISHED_NEXT_KSE (EVENTGROUP_EG + 14UL)			/*0xD0*/
#define TASK_INSTANCE_FINISHED_DIRECT (EVENTGROUP_EG + 15UL)				/*0xD1*/

#define TRACE_TASK_NOTIFY_GROUP (EVENTGROUP_EG + 16UL)					/*0xD2*/
#define TRACE_TASK_NOTIFY (TRACE_TASK_NOTIFY_GROUP + 0UL)					/*0xD2*/
#define TRACE_TASK_NOTIFY_TAKE (TRACE_TASK_NOTIFY_GROUP + 1UL)			/*0xD3*/
#define TRACE_TASK_NOTIFY_TAKE_BLOCK (TRACE_TASK_NOTIFY_GROUP + 2UL)		/*0xD4*/
#define TRACE_TASK_NOTIFY_TAKE_FAILED (TRACE_TASK_NOTIFY_GROUP + 3UL)		/*0xD5*/
#define TRACE_TASK_NOTIFY_WAIT (TRACE_TASK_NOTIFY_GROUP + 4UL)			/*0xD6*/
#define TRACE_TASK_NOTIFY_WAIT_BLOCK (TRACE_TASK_NOTIFY_GROUP + 5UL)		/*0xD7*/
#define TRACE_TASK_NOTIFY_WAIT_FAILED (TRACE_TASK_NOTIFY_GROUP + 6UL)		/*0xD8*/
#define TRACE_TASK_NOTIFY_FROM_ISR (TRACE_TASK_NOTIFY_GROUP + 7UL)		/*0xD9*/
#define TRACE_TASK_NOTIFY_GIVE_FROM_ISR (TRACE_TASK_NOTIFY_GROUP + 8UL)	/*0xDA*/

#define TIMER_EXPIRED (TRACE_TASK_NOTIFY_GROUP + 9UL)		/* 0xDB */

 /* Events on queue peek (receive) */
#define EVENTGROUP_PEEK_BLOCK	(TRACE_TASK_NOTIFY_GROUP + 10UL)		/*0xDC*/
/* peek block on queue:			0xDC	*/
/* peek block on semaphore:		0xDD	*/
/* peek block on mutex:			0xDE	*/

/* Events on queue peek (receive) */
#define EVENTGROUP_PEEK_FAILED	(EVENTGROUP_PEEK_BLOCK + 3UL)		/*0xDF*/
/* peek failed on queue:		0xDF	*/
/* peek failed on semaphore:	0xE0	*/
/* peek failed on mutex:		0xE1	*/

#define EVENTGROUP_STREAMBUFFER_DIV						(EVENTGROUP_PEEK_FAILED + 3UL)				/*0xE2*/
#define TRACE_STREAMBUFFER_RESET						(EVENTGROUP_STREAMBUFFER_DIV + 0)			/*0xE2*/
#define TRACE_MESSAGEBUFFER_RESET						(EVENTGROUP_STREAMBUFFER_DIV + 1UL)			/*0xE3*/
#define TRACE_STREAMBUFFER_OBJCLOSE_NAME_SUCCESS		(EVENTGROUP_STREAMBUFFER_DIV + 2UL)			/*0xE4*/
#define TRACE_MESSAGEBUFFER_OBJCLOSE_NAME_SUCCESS		(EVENTGROUP_STREAMBUFFER_DIV + 3UL)			/*0xE5*/
#define TRACE_STREAMBUFFER_OBJCLOSE_PROP_SUCCESS		(EVENTGROUP_STREAMBUFFER_DIV + 4UL)			/*0xE6*/
#define TRACE_MESSAGEBUFFER_OBJCLOSE_PROP_SUCCESS		(EVENTGROUP_STREAMBUFFER_DIV + 5UL)			/*0xE7*/

/* The following are using previously "lost" event codes */
#define TRACE_STREAMBUFFER_CREATE_OBJ_SUCCESS			(EVENTGROUP_CREATE_OBJ_SUCCESS + 4UL)		/*0x1C*/
#define TRACE_STREAMBUFFER_CREATE_OBJ_FAILED			(EVENTGROUP_CREATE_OBJ_FAILED + 4UL)		/*0x44*/
#define TRACE_STREAMBUFFER_DELETE_OBJ_SUCCESS			(EVENTGROUP_DELETE_OBJ_SUCCESS + 4UL)		/*0x84*/
#define TRACE_STREAMBUFFER_SEND_SUCCESS					(EVENTGROUP_SEND_SUCCESS + 3UL)				/*0x23*/
#define TRACE_STREAMBUFFER_SEND_BLOCK					(EVENTGROUP_SEND_BLOCK + 3UL)				/*0x73*/
#define TRACE_STREAMBUFFER_SEND_FAILED					(EVENTGROUP_SEND_FAILED + 3UL)				/*0x4B*/
#define TRACE_STREAMBUFFER_RECEIVE_SUCCESS				(EVENTGROUP_RECEIVE_SUCCESS + 3UL)			/*0x2B*/
#define TRACE_STREAMBUFFER_RECEIVE_BLOCK				(EVENTGROUP_RECEIVE_BLOCK + 3UL)			/*0x6B*/
#define TRACE_STREAMBUFFER_RECEIVE_FAILED				(EVENTGROUP_RECEIVE_FAILED + 3UL)			/*0x53*/
#define TRACE_STREAMBUFFER_SEND_FROM_ISR_SUCCESS		(EVENTGROUP_SEND_FROM_ISR_SUCCESS + 3UL)	/*0x33*/
#define TRACE_STREAMBUFFER_SEND_FROM_ISR_FAILED			(EVENTGROUP_SEND_FROM_ISR_FAILED + 3UL)		/*0x5B*/
#define TRACE_STREAMBUFFER_RECEIVE_FROM_ISR_SUCCESS		(EVENTGROUP_RECEIVE_FROM_ISR_SUCCESS + 3UL)	/*0x3B*/
#define TRACE_STREAMBUFFER_RECEIVE_FROM_ISR_FAILED		(EVENTGROUP_RECEIVE_FROM_ISR_FAILED + 3UL)	/*0x63*/

/* The following are using previously "lost" event codes. These macros aren't even directly referenced, instead we do (equivalent STREAMBUFFER code) + 1. */
#define TRACE_MESSAGEBUFFER_CREATE_OBJ_SUCCESS			(EVENTGROUP_CREATE_OBJ_SUCCESS + 5UL)		/*0x1D*/
#define TRACE_MESSAGEBUFFER_CREATE_OBJ_FAILED			(EVENTGROUP_CREATE_OBJ_FAILED + 5UL)		/*0x45*/
#define TRACE_MESSAGEBUFFER_DELETE_OBJ_SUCCESS			(EVENTGROUP_DELETE_OBJ_SUCCESS + 5UL)		/*0x85*/
#define TRACE_MESSAGEBUFFER_SEND_SUCCESS				(EVENTGROUP_SEND_SUCCESS + 4UL)				/*0x24*/
#define TRACE_MESSAGEBUFFER_SEND_BLOCK					(EVENTGROUP_SEND_BLOCK + 4UL)				/*0x74*/
#define TRACE_MESSAGEBUFFER_SEND_FAILED					(EVENTGROUP_SEND_FAILED + 4UL)				/*0x4C*/
#define TRACE_MESSAGEBUFFER_RECEIVE_SUCCESS				(EVENTGROUP_RECEIVE_SUCCESS + 4UL)			/*0x2C*/
#define TRACE_MESSAGEBUFFER_RECEIVE_BLOCK				(EVENTGROUP_RECEIVE_BLOCK + 4UL)			/*0x6C*/
#define TRACE_MESSAGEBUFFER_RECEIVE_FAILED				(EVENTGROUP_RECEIVE_FAILED + 4UL)			/*0x54*/
#define TRACE_MESSAGEBUFFER_SEND_FROM_ISR_SUCCESS		(EVENTGROUP_SEND_FROM_ISR_SUCCESS + 4UL)	/*0x34*/
#define TRACE_MESSAGEBUFFER_SEND_FROM_ISR_FAILED		(EVENTGROUP_SEND_FROM_ISR_FAILED + 4UL)		/*0x5C*/
#define TRACE_MESSAGEBUFFER_RECEIVE_FROM_ISR_SUCCESS	(EVENTGROUP_RECEIVE_FROM_ISR_SUCCESS + 4UL)	/*0x3C*/
#define TRACE_MESSAGEBUFFER_RECEIVE_FROM_ISR_FAILED		(EVENTGROUP_RECEIVE_FROM_ISR_FAILED + 4UL)	/*0x64*/

/* LAST EVENT (0xE7) */

/* Not needed in snapshot mode */
#define vTracePeriodicControl()

#endif /*#if TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT */

/******************************************************************************/
/*** Definitions for Streaming mode *******************************************/
/******************************************************************************/
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

/*******************************************************************************
* vTraceStoreKernelObjectName
*
* Set the name for a kernel object (defined by its address).
******************************************************************************/			
void vTraceStoreKernelObjectName(void* object, const char* name); 

/* For bare metal test - replaces TzCtrl task - call this periodically */
void vTracePeriodicControl(void);

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

#define PSF_EVENT_TASK_PRIORITY								0x04
#define PSF_EVENT_TASK_PRIO_INHERIT							0x05
#define PSF_EVENT_TASK_PRIO_DISINHERIT						0x06
#define PSF_EVENT_DEFINE_ISR								0x07

#define PSF_EVENT_TASK_CREATE								0x10
#define PSF_EVENT_QUEUE_CREATE								0x11
#define PSF_EVENT_SEMAPHORE_BINARY_CREATE					0x12
#define PSF_EVENT_MUTEX_CREATE								0x13
#define PSF_EVENT_TIMER_CREATE								0x14
#define PSF_EVENT_EVENTGROUP_CREATE							0x15
#define PSF_EVENT_SEMAPHORE_COUNTING_CREATE					0x16
#define PSF_EVENT_MUTEX_RECURSIVE_CREATE					0x17
#define PSF_EVENT_STREAMBUFFER_CREATE						0x18
#define PSF_EVENT_MESSAGEBUFFER_CREATE						0x19

#define PSF_EVENT_TASK_DELETE								0x20
#define PSF_EVENT_QUEUE_DELETE								0x21
#define PSF_EVENT_SEMAPHORE_DELETE							0x22
#define PSF_EVENT_MUTEX_DELETE								0x23
#define PSF_EVENT_TIMER_DELETE								0x24
#define PSF_EVENT_EVENTGROUP_DELETE							0x25
#define PSF_EVENT_STREAMBUFFER_DELETE						0x28
#define PSF_EVENT_MESSAGEBUFFER_DELETE						0x29

#define PSF_EVENT_TASK_READY								0x30
#define PSF_EVENT_NEW_TIME									0x31
#define PSF_EVENT_NEW_TIME_SCHEDULER_SUSPENDED				0x32
#define PSF_EVENT_ISR_BEGIN									0x33
#define PSF_EVENT_ISR_RESUME								0x34
#define PSF_EVENT_TS_BEGIN									0x35
#define PSF_EVENT_TS_RESUME									0x36
#define PSF_EVENT_TASK_ACTIVATE								0x37

#define PSF_EVENT_MALLOC									0x38
#define PSF_EVENT_FREE										0x39

#define PSF_EVENT_LOWPOWER_BEGIN							0x3A
#define PSF_EVENT_LOWPOWER_END								0x3B

#define PSF_EVENT_IFE_NEXT									0x3C
#define PSF_EVENT_IFE_DIRECT								0x3D

#define PSF_EVENT_TASK_CREATE_FAILED						0x40
#define PSF_EVENT_QUEUE_CREATE_FAILED						0x41
#define PSF_EVENT_SEMAPHORE_BINARY_CREATE_FAILED			0x42
#define PSF_EVENT_MUTEX_CREATE_FAILED						0x43
#define PSF_EVENT_TIMER_CREATE_FAILED						0x44
#define PSF_EVENT_EVENTGROUP_CREATE_FAILED					0x45
#define PSF_EVENT_SEMAPHORE_COUNTING_CREATE_FAILED			0x46
#define PSF_EVENT_MUTEX_RECURSIVE_CREATE_FAILED				0x47
#define PSF_EVENT_STREAMBUFFER_CREATE_FAILED				0x49
#define PSF_EVENT_MESSAGEBUFFER_CREATE_FAILED				0x4A

#define PSF_EVENT_TIMER_DELETE_FAILED						0x48

#define PSF_EVENT_QUEUE_SEND								0x50
#define PSF_EVENT_SEMAPHORE_GIVE							0x51
#define PSF_EVENT_MUTEX_GIVE								0x52

#define PSF_EVENT_QUEUE_SEND_FAILED							0x53
#define PSF_EVENT_SEMAPHORE_GIVE_FAILED						0x54
#define PSF_EVENT_MUTEX_GIVE_FAILED							0x55

#define PSF_EVENT_QUEUE_SEND_BLOCK							0x56
#define PSF_EVENT_SEMAPHORE_GIVE_BLOCK						0x57
#define PSF_EVENT_MUTEX_GIVE_BLOCK							0x58

#define PSF_EVENT_QUEUE_SEND_FROMISR						0x59
#define PSF_EVENT_SEMAPHORE_GIVE_FROMISR					0x5A

#define PSF_EVENT_QUEUE_SEND_FROMISR_FAILED					0x5C
#define PSF_EVENT_SEMAPHORE_GIVE_FROMISR_FAILED				0x5D

#define PSF_EVENT_QUEUE_RECEIVE								0x60
#define PSF_EVENT_SEMAPHORE_TAKE							0x61
#define PSF_EVENT_MUTEX_TAKE								0x62

#define PSF_EVENT_QUEUE_RECEIVE_FAILED						0x63
#define PSF_EVENT_SEMAPHORE_TAKE_FAILED						0x64
#define PSF_EVENT_MUTEX_TAKE_FAILED							0x65

#define PSF_EVENT_QUEUE_RECEIVE_BLOCK						0x66
#define PSF_EVENT_SEMAPHORE_TAKE_BLOCK						0x67
#define PSF_EVENT_MUTEX_TAKE_BLOCK							0x68

#define PSF_EVENT_QUEUE_RECEIVE_FROMISR						0x69
#define PSF_EVENT_SEMAPHORE_TAKE_FROMISR					0x6A

#define PSF_EVENT_QUEUE_RECEIVE_FROMISR_FAILED				0x6C
#define PSF_EVENT_SEMAPHORE_TAKE_FROMISR_FAILED				0x6D

#define PSF_EVENT_QUEUE_PEEK								0x70
#define PSF_EVENT_SEMAPHORE_PEEK							0x71
#define PSF_EVENT_MUTEX_PEEK								0x72

#define PSF_EVENT_QUEUE_PEEK_FAILED							0x73
#define PSF_EVENT_SEMAPHORE_PEEK_FAILED						0x74	
#define PSF_EVENT_MUTEX_PEEK_FAILED							0x75

#define PSF_EVENT_QUEUE_PEEK_BLOCK							0x76
#define PSF_EVENT_SEMAPHORE_PEEK_BLOCK						0x77
#define PSF_EVENT_MUTEX_PEEK_BLOCK							0x78

#define PSF_EVENT_TASK_DELAY_UNTIL							0x79
#define PSF_EVENT_TASK_DELAY								0x7A
#define PSF_EVENT_TASK_SUSPEND								0x7B
#define PSF_EVENT_TASK_RESUME								0x7C
#define PSF_EVENT_TASK_RESUME_FROMISR						0x7D

#define PSF_EVENT_TIMER_PENDFUNCCALL						0x80
#define PSF_EVENT_TIMER_PENDFUNCCALL_FROMISR				0x81
#define PSF_EVENT_TIMER_PENDFUNCCALL_FAILED					0x82
#define PSF_EVENT_TIMER_PENDFUNCCALL_FROMISR_FAILED			0x83

#define PSF_EVENT_USER_EVENT								0x90

#define PSF_EVENT_TIMER_START								0xA0
#define PSF_EVENT_TIMER_RESET								0xA1
#define PSF_EVENT_TIMER_STOP								0xA2
#define PSF_EVENT_TIMER_CHANGEPERIOD						0xA3
#define PSF_EVENT_TIMER_START_FROMISR						0xA4
#define PSF_EVENT_TIMER_RESET_FROMISR						0xA5
#define PSF_EVENT_TIMER_STOP_FROMISR						0xA6
#define PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR				0xA7
#define PSF_EVENT_TIMER_START_FAILED						0xA8
#define PSF_EVENT_TIMER_RESET_FAILED						0xA9
#define PSF_EVENT_TIMER_STOP_FAILED							0xAA
#define PSF_EVENT_TIMER_CHANGEPERIOD_FAILED					0xAB
#define PSF_EVENT_TIMER_START_FROMISR_FAILED				0xAC
#define PSF_EVENT_TIMER_RESET_FROMISR_FAILED				0xAD
#define PSF_EVENT_TIMER_STOP_FROMISR_FAILED					0xAE
#define PSF_EVENT_TIMER_CHANGEPERIOD_FROMISR_FAILED			0xAF

#define PSF_EVENT_EVENTGROUP_SYNC							0xB0
#define PSF_EVENT_EVENTGROUP_WAITBITS						0xB1
#define PSF_EVENT_EVENTGROUP_CLEARBITS						0xB2
#define PSF_EVENT_EVENTGROUP_CLEARBITS_FROMISR				0xB3
#define PSF_EVENT_EVENTGROUP_SETBITS						0xB4
#define PSF_EVENT_EVENTGROUP_SETBITS_FROMISR				0xB5
#define PSF_EVENT_EVENTGROUP_SYNC_BLOCK						0xB6
#define PSF_EVENT_EVENTGROUP_WAITBITS_BLOCK					0xB7
#define PSF_EVENT_EVENTGROUP_SYNC_FAILED					0xB8
#define PSF_EVENT_EVENTGROUP_WAITBITS_FAILED				0xB9

#define PSF_EVENT_QUEUE_SEND_FRONT							0xC0
#define PSF_EVENT_QUEUE_SEND_FRONT_FAILED					0xC1
#define PSF_EVENT_QUEUE_SEND_FRONT_BLOCK					0xC2
#define PSF_EVENT_QUEUE_SEND_FRONT_FROMISR					0xC3
#define PSF_EVENT_QUEUE_SEND_FRONT_FROMISR_FAILED			0xC4
#define PSF_EVENT_MUTEX_GIVE_RECURSIVE						0xC5
#define PSF_EVENT_MUTEX_GIVE_RECURSIVE_FAILED				0xC6
#define PSF_EVENT_MUTEX_TAKE_RECURSIVE						0xC7
#define PSF_EVENT_MUTEX_TAKE_RECURSIVE_FAILED				0xC8

#define PSF_EVENT_TASK_NOTIFY								0xC9
#define PSF_EVENT_TASK_NOTIFY_TAKE							0xCA
#define PSF_EVENT_TASK_NOTIFY_TAKE_BLOCK					0xCB
#define PSF_EVENT_TASK_NOTIFY_TAKE_FAILED					0xCC
#define PSF_EVENT_TASK_NOTIFY_WAIT							0xCD
#define PSF_EVENT_TASK_NOTIFY_WAIT_BLOCK					0xCE
#define PSF_EVENT_TASK_NOTIFY_WAIT_FAILED					0xCF
#define PSF_EVENT_TASK_NOTIFY_FROM_ISR						0xD0
#define PSF_EVENT_TASK_NOTIFY_GIVE_FROM_ISR					0xD1

#define PSF_EVENT_TIMER_EXPIRED								0xD2

#define PSF_EVENT_STREAMBUFFER_SEND							0xD3
#define PSF_EVENT_STREAMBUFFER_SEND_BLOCK					0xD4
#define PSF_EVENT_STREAMBUFFER_SEND_FAILED					0xD5
#define PSF_EVENT_STREAMBUFFER_RECEIVE						0xD6
#define PSF_EVENT_STREAMBUFFER_RECEIVE_BLOCK				0xD7
#define PSF_EVENT_STREAMBUFFER_RECEIVE_FAILED				0xD8
#define PSF_EVENT_STREAMBUFFER_SEND_FROM_ISR				0xD9
#define PSF_EVENT_STREAMBUFFER_SEND_FROM_ISR_FAILED			0xDA
#define PSF_EVENT_STREAMBUFFER_RECEIVE_FROM_ISR				0xDB
#define PSF_EVENT_STREAMBUFFER_RECEIVE_FROM_ISR_FAILED		0xDC
#define PSF_EVENT_STREAMBUFFER_RESET						0xDD

#define PSF_EVENT_MESSAGEBUFFER_SEND						0xDE
#define PSF_EVENT_MESSAGEBUFFER_SEND_BLOCK					0xDF
#define PSF_EVENT_MESSAGEBUFFER_SEND_FAILED					0xE0
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE						0xE1
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE_BLOCK				0xE2
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE_FAILED				0xE3
#define PSF_EVENT_MESSAGEBUFFER_SEND_FROM_ISR				0xE4
#define PSF_EVENT_MESSAGEBUFFER_SEND_FROM_ISR_FAILED		0xE5
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE_FROM_ISR			0xE6
#define PSF_EVENT_MESSAGEBUFFER_RECEIVE_FROM_ISR_FAILED		0xE7
#define PSF_EVENT_MESSAGEBUFFER_RESET						0xE8

#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING) */
	
#endif /* (TRC_USE_TRACEALYZER_RECORDER == 1) */

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_H */
