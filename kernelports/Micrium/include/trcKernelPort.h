/*
 * Trace Recorder for Tracealyzer v4.5.1
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * uC/OS3 specific definitions needed by the trace recorder
 */

#ifndef TRC_KERNEL_PORT_H
#define TRC_KERNEL_PORT_H

#include <stdint.h>
#include <os.h>
#include <os_cfg.h>	                                        /* Defines OS_CFG_TRACE_EN (1: Enabled).               */
#include <os_cfg_app.h>	                                    /* Defines OS_CFG_TICK_RATE_HZ.                        */
#if(TRC_BSP_CLK_SOURCE == 1)
#include <bsp.h>
#endif
#include "trcPortDefines.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (OS_VERSION >= 30600u)
#if (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))
  #define TRC_USE_TRACEALYZER_RECORDER   1
#else
  #define TRC_USE_TRACEALYZER_RECORDER   0
#endif
#else /* (OS_VERSION >= 30600u) */
#if (defined(TRACE_CFG_EN) && (TRACE_CFG_EN > 0u))
  #define TRC_USE_TRACEALYZER_RECORDER   1
#else
  #define TRC_USE_TRACEALYZER_RECORDER   0
#endif
#endif /* (OS_VERSION >= 30600u) */

#if (TRC_USE_TRACEALYZER_RECORDER == 1)

#define TRC_PLATFORM_CFG ""
#define TRC_PLATFORM_CFG_MAJOR 1
#define TRC_PLATFORM_CFG_MINOR 0
#define TRC_PLATFORM_CFG_PATCH 0

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)
	/* Required for this feature */
#undef OS_CFG_STAT_TASK_STK_CHK_EN
#define OS_CFG_STAT_TASK_STK_CHK_EN DEF_ENABLED
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0) */

#define TRACE_KERNEL_VERSION 0x2AA2
#define TRACE_TICK_RATE_HZ  OS_CFG_TICK_RATE_HZ            /* OS tick rate in Hertz.           */

#if(TRC_BSP_CLK_SOURCE == 1 && OS_VERSION >= 30700u)
#define TRACE_CPU_CLOCK_HZ BSP_ClkFreqGet(CLK_ID_SYSCLK) 
#elif(TRC_BSP_CLK_SOURCE == 1 && OS_VERSION < 30700u)
#define TRACE_CPU_CLOCK_HZ  BSP_CPU_ClkFreq()              /* Peripheral clock speed in Hertz. */
#endif

/* Gives the currently executing task (wrapper for RTOS-specific function) */
void* prvTraceGetCurrentTaskHandle(void);
uint32_t prvIsNewTCB(void* pNewTCB);
char* prvTraceAppend(const char* name, const char* suffix);

#if defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1)
uint32_t prvTraceGetStackHighWaterMark(void* task);
#endif /* defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1)*/

#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT) && (defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0))
void vTraceStartStackMonitor(void);
#else /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT) && (defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)) */
#define vTraceStartStackMonitor()
#endif /* (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT) && (defined(TRC_CFG_ENABLE_STACK_MONITOR) && (TRC_CFG_ENABLE_STACK_MONITOR == 1) && (TRC_CFG_SCHEDULING_ONLY == 0)) */

#if (((TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT) && (TRC_CFG_INCLUDE_ISR_TRACING == 1)) || (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING))
/* Tells if the scheduler currently is suspended (task-switches can't occur) */
unsigned char prvTraceIsSchedulerSuspended(void);

#endif /* (((TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT) && (TRC_CFG_INCLUDE_ISR_TRACING == 1)) || (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)) */

#define TRACE_GET_CURRENT_TASK() prvTraceGetCurrentTaskHandle()

#define TRACE_GET_OS_TICKS() (uiTraceTickCount) /* Streaming only */

/* If using dynamic allocation of snapshot trace buffer... */
#define TRACE_MALLOC(size) pvPortMalloc(size)

/* Critical sections definition */
#define TRACE_ALLOC_CRITICAL_SECTION()    CPU_SR_ALLOC();
#define TRACE_ENTER_CRITICAL_SECTION()    CPU_CRITICAL_ENTER();
#define TRACE_EXIT_CRITICAL_SECTION()     CPU_CRITICAL_EXIT();

extern uint16_t CurrentFilterMask;

extern uint16_t CurrentFilterGroup;

#if (OS_VERSION < 30700u)
extern void* prvTrcTickTask;
#endif /* (OS_VERSION < 30700u) */

#if !defined(TRC_CFG_INCLUDE_TICK_TASK)
#define TRC_CFG_INCLUDE_TICK_TASK 0
#endif

#if (TRC_CFG_INCLUDE_TICK_TASK == 0) && (OS_VERSION < 30700u)
#define TRACE_IF_NOT_TICK_TASK(p_tcb) if (prvTrcTickTask != (void*)p_tcb)
#else /* (TRC_CFG_INCLUDE_TICK_TASK == 0) && (OS_VERSION < 30700u) */
#define TRACE_IF_NOT_TICK_TASK(p_tcb)
#endif /* (TRC_CFG_INCLUDE_TICK_TASK == 0) && (OS_VERSION < 30700u) */

/* Tasks */
uint16_t prvTraceGetTaskNumberLow16(void* handle);
uint16_t prvTraceGetTaskNumberHigh16(void* handle);
void prvTraceSetTaskNumberLow16(void* handle, uint16_t value);
void prvTraceSetTaskNumberHigh16(void* handle, uint16_t value);

/* Task Queues */
#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
uint16_t prvTraceGetTaskQueueNumberLow16(void* handle);
uint16_t prvTraceGetTaskQueueNumberHigh16(void* handle);
void prvTraceSetTaskQueueNumberLow16(void* handle, uint16_t value);
void prvTraceSetTaskQueueNumberHigh16(void* handle, uint16_t value);
#endif /* (OS_CFG_TASK_Q_EN == DEF_ENABLED) */

/* Task Semaphores */
uint16_t prvTraceGetTaskSemNumberLow16(void* handle);
uint16_t prvTraceGetTaskSemNumberHigh16(void* handle);
void prvTraceSetTaskSemNumberLow16(void* handle, uint16_t value);
void prvTraceSetTaskSemNumberHigh16(void* handle, uint16_t value);

/* Queues */
uint16_t prvTraceGetQueueNumberLow16(void* handle);
uint16_t prvTraceGetQueueNumberHigh16(void* handle);
void prvTraceSetQueueNumberLow16(void* handle, uint16_t value);
void prvTraceSetQueueNumberHigh16(void* handle, uint16_t value);

/* Semaphores */
uint16_t prvTraceGetSemaphoreNumberLow16(void* handle);
uint16_t prvTraceGetSemaphoreNumberHigh16(void* handle);
void prvTraceSetSemaphoreNumberLow16(void* handle, uint16_t value);
void prvTraceSetSemaphoreNumberHigh16(void* handle, uint16_t value);

/* Mutexes */
uint16_t prvTraceGetMutexNumberLow16(void* handle);
uint16_t prvTraceGetMutexNumberHigh16(void* handle);
void prvTraceSetMutexNumberLow16(void* handle, uint16_t value);
void prvTraceSetMutexNumberHigh16(void* handle, uint16_t value);

/* Flags */
uint16_t prvTraceGetFlagNumberLow16(void* handle);
uint16_t prvTraceGetFlagNumberHigh16(void* handle);
void prvTraceSetFlagNumberLow16(void* handle, uint16_t value);
void prvTraceSetFlagNumberHigh16(void* handle, uint16_t value);

/* Mems */
uint16_t prvTraceGetMemNumberLow16(void* handle);
uint16_t prvTraceGetMemNumberHigh16(void* handle);
void prvTraceSetMemNumberLow16(void* handle, uint16_t value);
void prvTraceSetMemNumberHigh16(void* handle, uint16_t value);

/* Task Suspend and Resume */
void prvTraceTaskSuspendEnter(void* p_tcb);
void prvTraceTaskSuspendExit(uint32_t p_err);
void prvTraceTaskResumeEnter(void* p_tcb);
void prvTraceTaskResumeExit(uint32_t p_err);

/* Filters */
#define TRACE_GET_TASK_FILTER(pxTask) 1
#define TRACE_SET_TASK_FILTER(pxTask, group) 

#if (OS_CFG_TASK_Q_EN == DEF_ENABLED)
#define TRACE_GET_MICRIUM_TASK_MSG_Q_FILTER(pxObject) 1
#define TRACE_SET_MICRIUM_TASK_MSG_Q_FILTER(pxObject, group) 
#endif /* (OS_CFG_TASK_Q_EN == DEF_ENABLED) */

#define TRACE_GET_MICRIUM_TASK_SEM_FILTER(pxObject) 1
#define TRACE_SET_MICRIUM_TASK_SEM_FILTER(pxObject, group) 

#define TRACE_GET_MICRIUM_Q_FILTER(pxObject) 1
#define TRACE_SET_MICRIUM_Q_FILTER(pxObject, group) 

#define TRACE_GET_MICRIUM_SEM_FILTER(pxObject) 1
#define TRACE_SET_MICRIUM_SEM_FILTER(pxObject, group) 

#define TRACE_GET_MICRIUM_MUTEX_FILTER(pxObject) 1
#define TRACE_SET_MICRIUM_MUTEX_FILTER(pxObject, group) 

#define TRACE_GET_MICRIUM_FLAG_FILTER(pxObject) 1
#define TRACE_SET_MICRIUM_FLAG_FILTER(pxObject, group) 

#define TRACE_GET_MICRIUM_MEM_FILTER(pxObject) 1
#define TRACE_SET_MICRIUM_MEM_FILTER(pxObject, group) 

#define TRACE_GET_OBJECT_FILTER(CLASS, pxObject) TRACE_GET_##CLASS##_FILTER(pxObject)
#define TRACE_SET_OBJECT_FILTER(CLASS, pxObject, group) TRACE_SET_##CLASS##_FILTER(pxObject, group)

/******************************************************************************/
/*** Definitions for Snapshot mode ********************************************/
/******************************************************************************/
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT)

#define  OS_TRACE_INIT()                                      vTraceEnable(TRC_INIT)
#define  OS_TRACE_START()                                     
#define  OS_TRACE_STOP()                                      
#define  OS_TRACE_CLEAR()                                     vTraceClear()

/*** The object classes *******************************************************/
#define TRACE_NCLASSES        9
#define TRACE_CLASS_QUEUE     ((traceObjectClass)0)
#define TRACE_CLASS_SEMAPHORE ((traceObjectClass)1)
#define TRACE_CLASS_MUTEX     ((traceObjectClass)2)
#define TRACE_CLASS_FLAG      ((traceObjectClass)3)
#define TRACE_CLASS_MEM       ((traceObjectClass)4)
#define TRACE_CLASS_TASK_SEM  ((traceObjectClass)5)
#define TRACE_CLASS_TASK_Q    ((traceObjectClass)6)
#define TRACE_CLASS_TASK      ((traceObjectClass)7)
#define TRACE_CLASS_ISR       ((traceObjectClass)8)

#define TRACE_KERNEL_OBJECT_COUNT ((TRC_CFG_NQUEUE) + (TRC_CFG_NSEMAPHORE) + (TRC_CFG_NMUTEX) + (TRC_CFG_NFLAG) + (TRC_CFG_NMEM) + (TRC_CFG_NTASK_SEM) + (TRC_CFG_NTASK_Q) + (TRC_CFG_NTASK) + (TRC_CFG_NISR))

/* The size of the Object Property Table entries, in bytes, per object */

/* Queue properties (except name):     current number of message in queue */
#define PropertyTableSizeQueue         ((TRC_CFG_NAME_LEN_QUEUE) + 1)      

/* Semaphore properties (except name): state (signaled = 1, cleared = 0) */
#define PropertyTableSizeSemaphore     ((TRC_CFG_NAME_LEN_SEMAPHORE) + 1) 

/* Mutex properties (except name):     owner (task handle, 0 = free) */
#define PropertyTableSizeMutex         ((TRC_CFG_NAME_LEN_MUTEX) + 1)         

/* Flags properties (except name): state (signaled = 1, cleared = 0) */
#define PropertyTableSizeFlag          ((TRC_CFG_NAME_LEN_FLAG) + 1)

/* Memory Partitions properties (except name): state (signaled = 1, cleared = 0) */
#define PropertyTableSizeMem           ((TRC_CFG_NAME_LEN_MEM) + 1)

/* Task Semaphore properties (except name): state (signaled = 1, cleared = 0) */
#define PropertyTableSizeTaskSem       ((TRC_CFG_NAME_LEN_TASK_SEM) + 1)

/* Task Q properties (except name): state (signaled = 1, cleared = 0) */
#define PropertyTableSizeTaskQ         ((TRC_CFG_NAME_LEN_TASK_Q) + 1)
   
/* Task properties (except name):      Byte 0: Current priority
                                       Byte 1: state (if already active) 
                                       Byte 2: InstanceFinishEvent_ServiceCode
                                       Byte 3: InstanceFinishEvent_ObjHandle */
#define PropertyTableSizeTask         ((TRC_CFG_NAME_LEN_TASK) + 4)

/* ISR properties:                     Byte 0: priority
                                       Byte 1: state (if already active) */
#define PropertyTableSizeISR          ((TRC_CFG_NAME_LEN_ISR) + 2)



/* The layout of the byte array representing the Object Property Table */
#define StartIndexQueue            (0)
#define StartIndexSemaphore        (StartIndexQueue     + (TRC_CFG_NQUEUE)     * PropertyTableSizeQueue)
#define StartIndexMutex            (StartIndexSemaphore + (TRC_CFG_NSEMAPHORE) * PropertyTableSizeSemaphore)
#define StartIndexFlag             (StartIndexMutex     + (TRC_CFG_NMUTEX)     * PropertyTableSizeMutex)
#define StartIndexMem              (StartIndexFlag      + (TRC_CFG_NFLAG)      * PropertyTableSizeFlag)
#define StartIndexTaskSem          (StartIndexMem       + (TRC_CFG_NMEM)       * PropertyTableSizeMem)
#define StartIndexTaskQ            (StartIndexTaskSem   + (TRC_CFG_NTASK_SEM)  * PropertyTableSizeTaskSem)
#define StartIndexTask             (StartIndexTaskQ     + (TRC_CFG_NTASK_Q)    * PropertyTableSizeTaskQ)
#define StartIndexISR              (StartIndexTask      + (TRC_CFG_NTASK)      * PropertyTableSizeTask)


/* Number of bytes used by the object table */
#define TRACE_OBJECT_TABLE_SIZE    (StartIndexISR       + (TRC_CFG_NISR)       * PropertyTableSizeISR)

/* Initialization of the object property table */
void vTraceInitObjectPropertyTable(void);

/* Initialization of the handle mechanism, see e.g, prvTraceGetObjectHandle */
void vTraceInitObjectHandleStack(void);

/* Returns the "Not enough handles" error message for the specified object class */
const char* pszTraceGetErrorNotEnoughHandles(traceObjectClass objectclass);

/*** Event codes for snapshot mode - must match Tracealyzer config files ******/

#define NULL_EVENT                   (0x00)  /* Ignored in the analysis*/

/*******************************************************************************
 * EVENTGROUP_DIV
 *
 * Miscellaneous events.
 ******************************************************************************/
#define EVENTGROUP_DIV                           (NULL_EVENT + 1)                                /*0x01*/
#define DIV_XPS                                  (EVENTGROUP_DIV + 0)                            /*0x01*/
#define DIV_TASK_READY                           (EVENTGROUP_DIV + 1)                            /*0x02*/
#define DIV_NEW_TIME                             (EVENTGROUP_DIV + 2)                            /*0x03*/

/*******************************************************************************
 * EVENTGROUP_TS
 *
 * Events for storing task-switches and interrupts. The RESUME events are 
 * generated if the task/interrupt is already marked active.
 ******************************************************************************/
#define EVENTGROUP_TS                            (EVENTGROUP_DIV + 3)                            /*0x04*/
#define TS_ISR_BEGIN                             (EVENTGROUP_TS + 0)                             /*0x04*/
#define TS_ISR_RESUME                            (EVENTGROUP_TS + 1)                             /*0x05*/
#define TS_TASK_BEGIN                            (EVENTGROUP_TS + 2)                             /*0x06*/
#define TS_TASK_RESUME                           (EVENTGROUP_TS + 3)                             /*0x07*/

/*******************************************************************************
 * EVENTGROUP_OBJCLOSE_NAME
 * 
 * About Close Events
 * When an object is evicted from the object property table (object close), two 
 * internal events are stored (EVENTGROUP_OBJCLOSE_NAME and 
 * EVENTGROUP_OBJCLOSE_PROP), containing the handle-name mapping and object 
 * properties valid up to this point.
 ******************************************************************************/
#define EVENTGROUP_OBJCLOSE_NAME_TRCSUCCESS      (EVENTGROUP_TS + 4)                             /*0x08*/

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
#define EVENTGROUP_OBJCLOSE_PROP_TRCSUCCESS      (EVENTGROUP_OBJCLOSE_NAME_TRCSUCCESS + 10)      /*0x12*/

/*******************************************************************************
 * EVENTGROUP_CREATE
 * 
 * The events in this group are used to log Kernel object creations.
 * The lower three bits in the event code gives the object class, i.e., type of
 * create operation (task, queue, semaphore, etc).
 ******************************************************************************/
#define EVENTGROUP_CREATE_TRCSUCCESS             (EVENTGROUP_OBJCLOSE_PROP_TRCSUCCESS + 10)      /*0x1C*/

/*******************************************************************************
 * EVENTGROUP_SEND
 * 
 * The events in this group are used to log Send/Give events on queues, 
 * semaphores and mutexes The lower three bits in the event code gives the 
 * object class, i.e., what type of object that is operated on (queue, semaphore 
 * or mutex).
 ******************************************************************************/
#define EVENTGROUP_SEND_TRCSUCCESS               (EVENTGROUP_CREATE_TRCSUCCESS + 10)             /*0x26*/

/*******************************************************************************
 * EVENTGROUP_RECEIVE
 * 
 * The events in this group are used to log Receive/Take events on queues, 
 * semaphores and mutexes. The lower three bits in the event code gives the 
 * object class, i.e., what type of object that is operated on (queue, semaphore
 * or mutex).
 ******************************************************************************/
#define EVENTGROUP_RECEIVE_TRCSUCCESS            (EVENTGROUP_SEND_TRCSUCCESS + 10)               /*0x30*/

/* Send/Give operations, to a task */
#define EVENTGROUP_SEND_TO_TASK_TRCSUCCESS       (EVENTGROUP_RECEIVE_TRCSUCCESS + 10)            /*0x3A*/

/* Receive/Take operations, from a task */
#define EVENTGROUP_RECEIVE_FROM_TASK_TRCSUCCESS  (EVENTGROUP_SEND_TO_TASK_TRCSUCCESS + 10)       /*0x44*/

/* "Failed" event type versions of above (timeout, failed allocation, etc) */
#define EVENTGROUP_KSE_TRCFAILED                 (EVENTGROUP_RECEIVE_FROM_TASK_TRCSUCCESS + 10)  /*0x4E*/

/* Failed create calls - memory allocation failed */
#define EVENTGROUP_CREATE_TRCFAILED              (EVENTGROUP_KSE_TRCFAILED)                      /*0x4E*/

/* Failed send/give - timeout! */
#define EVENTGROUP_SEND_TRCFAILED                (EVENTGROUP_CREATE_TRCFAILED + 10)              /*0x58*/

/* Failed receive/take - timeout! */
#define EVENTGROUP_RECEIVE_TRCFAILED             (EVENTGROUP_SEND_TRCFAILED + 10)                /*0x62*/

/* Failed non-blocking send/give - queue full */
#define EVENTGROUP_SEND_TO_TASK_TRCFAILED        (EVENTGROUP_RECEIVE_TRCFAILED + 10)             /*0x6C*/

/* Failed non-blocking receive/take - queue empty */
#define EVENTGROUP_RECEIVE_FROM_TASK_TRCFAILED   (EVENTGROUP_SEND_TO_TASK_TRCFAILED + 10)        /*0x76*/

/* Events when blocking on receive/take */
#define EVENTGROUP_RECEIVE_TRCBLOCK              (EVENTGROUP_RECEIVE_FROM_TASK_TRCFAILED + 10)   /*0x80*/

/* Events when blocking on send/give */
#define EVENTGROUP_SEND_TRCBLOCK                 (EVENTGROUP_RECEIVE_TRCBLOCK + 10)              /*0x8A*/

/* Events on queue peek (receive) */
#define EVENTGROUP_PEEK_TRCSUCCESS               (EVENTGROUP_SEND_TRCBLOCK + 10)                 /*0x94*/

/* Events on object delete (vTaskDelete or vQueueDelete) */
#define EVENTGROUP_DELETE_TRCSUCCESS             (EVENTGROUP_PEEK_TRCSUCCESS + 10)               /*0x9E*/

/* Other events - object class is implied: TASK */
#define EVENTGROUP_OTHERS						 (EVENTGROUP_DELETE_TRCSUCCESS + 10)             /*0xA8*/
#define TASK_DELAY_UNTIL						 (EVENTGROUP_OTHERS + 0)                         /*0xA8*/
#define TASK_DELAY								 (EVENTGROUP_OTHERS + 1)                         /*0xA9*/
#define TASK_SUSPEND							 (EVENTGROUP_OTHERS + 2)                         /*0xAA*/
#define TASK_RESUME								 (EVENTGROUP_OTHERS + 3)                         /*0xAB*/
#define TASK_PRIORITY_SET						 (EVENTGROUP_OTHERS + 5)                         /*0xAD*/
#define TASK_PRIORITY_INHERIT					 (EVENTGROUP_OTHERS + 6)                         /*0xAE*/
#define TASK_PRIORITY_DISINHERIT				 (EVENTGROUP_OTHERS + 7)                         /*0xAF*/
#define TASK_SUSPEND_FAIL						 (EVENTGROUP_OTHERS + 9)                         /*0xB1*/
#define TASK_RESUME_FAIL						 (EVENTGROUP_OTHERS + 10)                        /*0xB2*/

/* Not yet used */
#define EVENTGROUP_FTRACE_PLACEHOLDER            (EVENTGROUP_OTHERS + 8)                         /*0xB0*/

/* User events */
#define EVENTGROUP_USEREVENT                     (EVENTGROUP_FTRACE_PLACEHOLDER + 10)            /*0xBA*/
#define USER_EVENT                               (EVENTGROUP_USEREVENT + 0)

/* Allow for 0-15 arguments (the number of args is added to event code) */
#define USER_EVENT_LAST                          (EVENTGROUP_USEREVENT + 15)                     /*0xC9*/

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
 * which means a limit of 0xFF (255). The XTS16 is used when the original event 
 * has a 16 bit DTS field and thereby can handle values up to 0xFFFF (65535).
 * 
 * Using a very high frequency time base can result in many XTS events. 
 * Preferably, the time between two OS ticks should fit in 16 bits, i.e.,
 * at most 65535. If your time base has a higher frequency, you can define
 * the TRACE
 ******************************************************************************/

#define EVENTGROUP_SYS                           (EVENTGROUP_USEREVENT + 16)                      /*0xCA*/
#define XTS8                                     (EVENTGROUP_SYS + 0)                             /*0xCA*/
#define XTS16                                    (EVENTGROUP_SYS + 1)                             /*0xCB*/
#define EVENT_BEING_WRITTEN                      (EVENTGROUP_SYS + 2)                             /*0xCC*/
#define RESERVED_DUMMY_CODE                      (EVENTGROUP_SYS + 3)                             /*0xCD*/
#define LOW_POWER_BEGIN                          (EVENTGROUP_SYS + 4)                             /*0xCE*/
#define LOW_POWER_END                            (EVENTGROUP_SYS + 5)                             /*0xCF*/
#define XID                                      (EVENTGROUP_SYS + 6)                             /*0xD0*/
#define XTS16L                                   (EVENTGROUP_SYS + 7)                             /*0xD1*/
#define TASK_INSTANCE_FINISHED_NEXT_KSE          (EVENTGROUP_SYS + 8)                             /*0xD2*/
#define TASK_INSTANCE_FINISHED_DIRECT            (EVENTGROUP_SYS + 9)                             /*0xD3*/
#define TRACE_UNUSED_STACK         			     (EVENTGROUP_SYS + 10)                            /*0xD4*/

traceHandle prvTraceGetTaskNumber(void* handle);
unsigned char prvTraceIsSchedulerActive(void);
unsigned char prvTraceIsSchedulerSuspended(void);
unsigned char prvTraceIsSchedulerStarted(void);

/************************************************************************/
/* KERNEL SPECIFIC MACROS USED BY THE TRACE RECORDER                    */
/************************************************************************/

/****************************
* MACROS TO GET TRACE CLASS *
****************************/

#define TRACE_GET_TASK_TRACE_CLASS()          	   TRACE_CLASS_TASK
#define TRACE_GET_MICRIUM_TASK_SEM_TRACE_CLASS()   TRACE_CLASS_TASK_SEM
#define TRACE_GET_MICRIUM_TASK_MSG_Q_TRACE_CLASS() TRACE_CLASS_TASK_Q
#define TRACE_GET_MICRIUM_Q_TRACE_CLASS()          TRACE_CLASS_QUEUE
#define TRACE_GET_MICRIUM_SEM_TRACE_CLASS()        TRACE_CLASS_SEMAPHORE
#define TRACE_GET_MICRIUM_MUTEX_TRACE_CLASS()      TRACE_CLASS_MUTEX
#define TRACE_GET_MICRIUM_FLAG_TRACE_CLASS()       TRACE_CLASS_FLAG
#define TRACE_GET_MICRIUM_MEM_TRACE_CLASS()        TRACE_CLASS_MEM

#define TRACE_GET_OBJECT_TRACE_CLASS(CLASS, pxObject) TRACE_GET_##CLASS##_TRACE_CLASS()

/******************************
* MACROS TO GET OBJECT NUMBER *
******************************/
#define TRACE_GET_TASK_NUMBER(pxTCB) ((traceHandle)prvTraceGetTaskNumberLow16(pxTCB))
#define TRACE_SET_TASK_NUMBER(pxTCB) prvTraceSetTaskNumberLow16(pxTCB, prvTraceGetObjectHandle(TRACE_CLASS_TASK))

#define TRACE_GET_MICRIUM_TASK_MSG_Q_NUMBER(p_msg_q) ((traceHandle)prvTraceGetTaskQueueNumberLow16(p_msg_q))
#define TRACE_SET_MICRIUM_TASK_MSG_Q_NUMBER(p_msg_q) prvTraceSetTaskQueueNumberLow16(p_msg_q, prvTraceGetObjectHandle(TRACE_CLASS_TASK_Q))

#define TRACE_GET_MICRIUM_TASK_SEM_NUMBER(p_tcb) ((traceHandle)prvTraceGetTaskSemNumberLow16(p_tcb))
#define TRACE_SET_MICRIUM_TASK_SEM_NUMBER(p_tcb) prvTraceSetTaskSemNumberLow16(p_tcb, prvTraceGetObjectHandle(TRACE_CLASS_TASK_SEM))

#define TRACE_GET_MICRIUM_Q_NUMBER(p_q) ((traceHandle)prvTraceGetQueueNumberLow16(p_q))
#define TRACE_SET_MICRIUM_Q_NUMBER(p_q) prvTraceSetQueueNumberLow16(p_q, prvTraceGetObjectHandle(TRACE_CLASS_QUEUE))

#define TRACE_GET_MICRIUM_SEM_NUMBER(p_sem) ((traceHandle)prvTraceGetSemaphoreNumberLow16(p_sem))
#define TRACE_SET_MICRIUM_SEM_NUMBER(p_sem) prvTraceSetSemaphoreNumberLow16(p_sem, prvTraceGetObjectHandle(TRACE_CLASS_SEMAPHORE))

#define TRACE_GET_MICRIUM_MUTEX_NUMBER(p_mutex) ((traceHandle)prvTraceGetMutexNumberLow16(p_mutex))
#define TRACE_SET_MICRIUM_MUTEX_NUMBER(p_mutex) prvTraceSetMutexNumberLow16(p_mutex, prvTraceGetObjectHandle(TRACE_CLASS_MUTEX))

#define TRACE_GET_MICRIUM_FLAG_NUMBER(p_grp) ((traceHandle)prvTraceGetFlagNumberLow16(p_grp))
#define TRACE_SET_MICRIUM_FLAG_NUMBER(p_grp) prvTraceSetFlagNumberLow16(p_grp, prvTraceGetObjectHandle(TRACE_CLASS_FLAG))

#define TRACE_GET_MICRIUM_MEM_NUMBER(p_mem) ((traceHandle)prvTraceGetMemNumberLow16(p_mem))
#define TRACE_SET_MICRIUM_MEM_NUMBER(p_mem) prvTraceSetMemNumberLow16(p_mem, prvTraceGetObjectHandle(TRACE_CLASS_MEM))

#define TRACE_GET_OBJECT_NUMBER(CLASS, pxObject) TRACE_GET_##CLASS##_NUMBER(pxObject)
#define TRACE_SET_OBJECT_NUMBER(CLASS, pxObject) TRACE_SET_##CLASS##_NUMBER(pxObject)

/******************************
* MACROS FOR EVENT CODES      *
******************************/
#define TRACE_GET_CLASS_EVENT_CODE(SERVICE, RESULT, CLASS, kernelClass) (uint8_t)(EVENTGROUP_##SERVICE##_##RESULT + TRACE_GET_##CLASS##_TRACE_CLASS())
#define TRACE_GET_OBJECT_EVENT_CODE(SERVICE, RESULT, CLASS, pxObject) (uint8_t)(EVENTGROUP_##SERVICE##_##RESULT + TRACE_GET_##CLASS##_TRACE_CLASS())
#define TRACE_GET_TASK_EVENT_CODE(SERVICE, RESULT, CLASS, pxTCB) (EVENTGROUP_##SERVICE##_##RESULT + TRACE_CLASS_TASK)

/******************************
* SPECIAL MACROS FOR TASKS    *
******************************/
#define TRACE_GET_TASK_PRIORITY(pxTCB) ((uint8_t)pxTCB->Prio)              /* Prio is a field in OS_TCB (0: highest priority).     */
#define TRACE_GET_TASK_NAME(pxTCB) ((char*)pxTCB->NamePtr)                 /* NamePtr is a field in OS_TCB.                        */

#define TRACE_MALLOC(size) pvPortMalloc(size)

/*** The trace macros for snapshot mode **************************************/

/* Called each time a task is created */
/* Critical section needed */
#undef OS_TRACE_TASK_CREATE
#if (OS_VERSION >= 30700u)
#define OS_TRACE_TASK_CREATE(p_tcb) \
	TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_TASK_CREATE(TRACE_GET_OBJECT_EVENT_CODE(CREATE, TRCSUCCESS, TASK, p_tcb), NOT_USED, p_tcb); \
	prvAddTaskToStackMonitor(p_tcb); \
	TRACE_EXIT_CRITICAL_SECTION();
#else /* (OS_VERSION >= 30700u) */
#define OS_TRACE_TASK_CREATE(p_tcb) \
	TRACE_ENTER_CRITICAL_SECTION(); \
	if (p_task == OS_TickTask) \
	{ \
		prvTrcTickTask = (void*)p_tcb; /* This is the Tick Task */ \
	} \
	trcKERNEL_HOOKS_TASK_CREATE(TRACE_GET_OBJECT_EVENT_CODE(CREATE, TRCSUCCESS, TASK, p_tcb), NOT_USED, p_tcb); \
	prvAddTaskToStackMonitor(p_tcb); \
	TRACE_EXIT_CRITICAL_SECTION();
#endif /* (OS_VERSION >= 30700u) */

/* Called each time a task gets deleted */
/* No critical section needed */
#undef OS_TRACE_TASK_DEL
#define OS_TRACE_TASK_DEL(p_tcb) \
	trcKERNEL_HOOKS_TASK_DELETE(TRACE_GET_OBJECT_EVENT_CODE(DELETE, TRCSUCCESS, TASK, p_tcb), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_NAME, TRCSUCCESS, TASK, p_tcb), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_PROP, TRCSUCCESS, TASK, p_tcb), p_tcb); \
	prvRemoveTaskFromStackMonitor(p_tcb);

/* Called on each task-switch-in */
/* No critical section needed */
#undef OS_TRACE_TASK_SWITCHED_IN
#define OS_TRACE_TASK_SWITCHED_IN(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		trcKERNEL_HOOKS_TASK_SWITCH(p_tcb); \
	}

/* Called for each task that becomes ready */
/* No critical section needed */
#if (TRC_CFG_INCLUDE_READY_EVENTS == 1)
#undef OS_TRACE_TASK_READY
#define OS_TRACE_TASK_READY(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		trcKERNEL_HOOKS_MOVED_TASK_TO_READY_STATE(p_tcb); \
	}
#else /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */
#define OS_TRACE_TASK_READY(p_tcb) 
#endif /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */

/* Called on each OS tick. Will call uiPortGetTimestamp to make sure it is called at least once every OS tick. */
/* No critical section needed */
#if (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1)
#undef OS_TRACE_TICK_INCREMENT
#define OS_TRACE_TICK_INCREMENT(OSTickCtr) \
	if (OSRunning == 1) { trcKERNEL_HOOKS_NEW_TIME(DIV_NEW_TIME, OSTickCtr + 1); } \
    else if (OSRunning == 0) { trcKERNEL_HOOKS_INCREMENT_TICK(); }
#else /* (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1) */
#define OS_TRACE_TICK_INCREMENT(OSTickCtr) 
#endif /* (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1) */

#if (TRC_CFG_SCHEDULING_ONLY == 0)

#if (OS_VERSION < 30700u)
/* Called in OSTaskSuspend() */
/* Critical section needed. Also SR_ALLOC needed. */
#undef OS_TRACE_TASK_SUSPEND
#define OS_TRACE_TASK_SUSPEND(p_tcb) \
	{ \
		TRACE_ALLOC_CRITICAL_SECTION(); \
		TRACE_ENTER_CRITICAL_SECTION(); \
		if (p_tcb == 0) \
		{ \
			trcKERNEL_HOOKS_TASK_SUSPEND(TASK_SUSPEND, OSTCBCurPtr); \
		} \
		else \
		{ \
			trcKERNEL_HOOKS_TASK_SUSPEND(TASK_SUSPEND, p_tcb); \
		} \
		TRACE_EXIT_CRITICAL_SECTION(); \
	}

#else /* (OS_VERSION < 30700u) */
/* Called in OSTaskSuspend() */
/* Critical section needed. Also SR_ALLOC needed. */
#undef OS_TRACE_TASK_SUSPEND_ENTER
#define OS_TRACE_TASK_SUSPEND_ENTER(p_tcb) \
	prvTraceTaskSuspendEnter(p_tcb);

#undef OS_TRACE_TASK_SUSPEND_EXIT
#define OS_TRACE_TASK_SUSPEND_EXIT(p_err) \
	prvTraceTaskSuspendExit(p_err);
#endif /* (OS_VERSION < 30700u) */

/* Called on any of the task delay functions */
/* No critical section needed */
#undef OS_TRACE_TASK_DLY
#define OS_TRACE_TASK_DLY(dly_ticks) \
	trcKERNEL_HOOKS_TASK_DELAY(TASK_DELAY, OSTCBCurPtr, dly_ticks); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();
	
/* Called each time a task gets deleted */
/* No critical section needed */
#undef OS_TRACE_TASK_SEM_DEL
#define OS_TRACE_TASK_SEM_DEL(p_tcb) \
	trcKERNEL_HOOKS_OBJECT_DELETE(TRACE_GET_OBJECT_EVENT_CODE(DELETE, TRCSUCCESS, MICRIUM_TASK_SEM, p_tcb), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_NAME, TRCSUCCESS, MICRIUM_TASK_SEM, p_tcb), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_PROP, TRCSUCCESS, MICRIUM_TASK_SEM, p_tcb), MICRIUM_TASK_SEM, p_tcb);
	
/* Called each time a task gets deleted */
/* No critical section needed */
#undef traceTASK_MSG_Q_DELETE
#define traceTASK_MSG_Q_DELETE(p_msg_q) \
	trcKERNEL_HOOKS_OBJECT_DELETE(TRACE_GET_OBJECT_EVENT_CODE(DELETE, TRCSUCCESS, MICRIUM_TASK_MSG_Q, p_msg_q), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_NAME, TRCSUCCESS, MICRIUM_TASK_MSG_Q, p_msg_q), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_PROP, TRCSUCCESS, MICRIUM_TASK_MSG_Q, p_msg_q), MICRIUM_TASK_MSG_Q, p_msg_q);

/* Called on OSSemDel */
/* No critical section needed */
#undef OS_TRACE_SEM_DEL
#define OS_TRACE_SEM_DEL(p_sem) \
	trcKERNEL_HOOKS_OBJECT_DELETE(TRACE_GET_OBJECT_EVENT_CODE(DELETE, TRCSUCCESS, MICRIUM_SEM, p_sem), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_NAME, TRCSUCCESS, MICRIUM_SEM, p_sem), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_PROP, TRCSUCCESS, MICRIUM_SEM, p_sem), MICRIUM_SEM, p_sem);

/* Called on OSQDel */
/* No critical section needed */
#undef OS_TRACE_Q_DEL
#define OS_TRACE_Q_DEL(p_q) \
	trcKERNEL_HOOKS_OBJECT_DELETE(TRACE_GET_OBJECT_EVENT_CODE(DELETE, TRCSUCCESS, MICRIUM_Q, p_q), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_NAME, TRCSUCCESS, MICRIUM_Q, p_q), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_PROP, TRCSUCCESS, MICRIUM_Q, p_q), MICRIUM_Q, p_q);

/* Called on OSMutexDel */
/* No critical section needed */
#undef OS_TRACE_MUTEX_DEL
#define OS_TRACE_MUTEX_DEL(p_mutex) \
	trcKERNEL_HOOKS_OBJECT_DELETE(TRACE_GET_OBJECT_EVENT_CODE(DELETE, TRCSUCCESS, MICRIUM_MUTEX, p_mutex), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_NAME, TRCSUCCESS, MICRIUM_MUTEX, p_mutex), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_PROP, TRCSUCCESS, MICRIUM_MUTEX, p_mutex), MICRIUM_MUTEX, p_mutex);

/* Called on OSFlagDel */
/* No critical section needed */
#undef OS_TRACE_FLAG_DEL
#define OS_TRACE_FLAG_DEL(p_grp) \
	trcKERNEL_HOOKS_OBJECT_DELETE(TRACE_GET_OBJECT_EVENT_CODE(DELETE, TRCSUCCESS, MICRIUM_FLAG, p_grp), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_NAME, TRCSUCCESS, MICRIUM_FLAG, p_grp), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_PROP, TRCSUCCESS, MICRIUM_FLAG, p_grp), MICRIUM_FLAG, p_grp);

/* Called on OSTaskDel */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_DEL
#define OS_TRACE_TASK_SEM_DEL(p_tcb) \
	trcKERNEL_HOOKS_OBJECT_DELETE(TRACE_GET_OBJECT_EVENT_CODE(DELETE, TRCSUCCESS, MICRIUM_TASK_SEM, p_tcb), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_NAME, TRCSUCCESS, MICRIUM_TASK_SEM, p_tcb), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_PROP, TRCSUCCESS, MICRIUM_TASK_SEM, p_tcb), MICRIUM_TASK_SEM, p_tcb);

/* Called on OSTaskDel */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_MSG_Q_DEL
#define OS_TRACE_TASK_MSG_Q_DEL(p_msg_q) \
	trcKERNEL_HOOKS_OBJECT_DELETE(TRACE_GET_OBJECT_EVENT_CODE(DELETE, TRCSUCCESS, MICRIUM_TASK_MSG_Q, p_msg_q), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_NAME, TRCSUCCESS, MICRIUM_TASK_MSG_Q, p_msg_q), TRACE_GET_OBJECT_EVENT_CODE(OBJCLOSE_PROP, TRCSUCCESS, MICRIUM_TASK_MSG_Q, p_msg_q), MICRIUM_TASK_MSG_Q, p_msg_q);

/* Called in OSTaskCreate, if it fails (typically if the stack can not be allocated) */
/* Need to protect critical section without reallocating the SR.                     */
#undef OS_TRACE_TASK_CREATE_FAILED
#define OS_TRACE_TASK_CREATE_FAILED(p_tcb) \
	TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_KERNEL_SERVICE_WITH_NUMERIC_PARAM_ONLY(TRACE_GET_CLASS_EVENT_CODE(CREATE, TRCFAILED, TASK, NOT_USED), 0); \
	TRACE_EXIT_CRITICAL_SECTION();

/* Called in OSTaskCreate, while it creates the built-in message queue */
/* Need to protect critical section without reallocating the SR.       */
#undef OS_TRACE_TASK_MSG_Q_CREATE
#define OS_TRACE_TASK_MSG_Q_CREATE(p_msg_q, p_name )\
	TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_OBJECT_CREATE(TRACE_GET_OBJECT_EVENT_CODE(CREATE, TRCSUCCESS, MICRIUM_TASK_MSG_Q, pxNewQueue), MICRIUM_TASK_MSG_Q, p_msg_q); \
    prvTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(MICRIUM_TASK_MSG_Q, p_msg_q), TRACE_GET_OBJECT_NUMBER(MICRIUM_TASK_MSG_Q, p_msg_q), prvTraceAppend(p_name, "Q")); \
	TRACE_EXIT_CRITICAL_SECTION();

/* Called in OSTaskCreate, while it creates the built-in message queue */
/* Need to protect critical section without reallocating the SR.       */
#undef OS_TRACE_TASK_SEM_CREATE
#define OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name )\
	TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_OBJECT_CREATE(TRACE_GET_OBJECT_EVENT_CODE(CREATE, TRCSUCCESS, MICRIUM_TASK_SEM, p_tcb), MICRIUM_TASK_SEM, p_tcb); \
    prvTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(MICRIUM_TASK_SEM, p_tcb), TRACE_GET_OBJECT_NUMBER(MICRIUM_TASK_SEM, p_tcb), prvTraceAppend(p_name, "Sem")); \
	TRACE_EXIT_CRITICAL_SECTION();

/* No critical section needed */
#undef OS_TRACE_MUTEX_CREATE
#define OS_TRACE_MUTEX_CREATE(p_mutex, p_name )\
	trcKERNEL_HOOKS_OBJECT_CREATE(TRACE_GET_OBJECT_EVENT_CODE(CREATE, TRCSUCCESS, MICRIUM_MUTEX, p_mutex), MICRIUM_MUTEX, p_mutex); \
	prvTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(MICRIUM_MUTEX, p_mutex), TRACE_GET_OBJECT_NUMBER(MICRIUM_MUTEX, p_mutex), p_name);
	
/* No critical section needed */
#undef OS_TRACE_SEM_CREATE
#define OS_TRACE_SEM_CREATE(p_sem, p_name )\
	trcKERNEL_HOOKS_OBJECT_CREATE(TRACE_GET_OBJECT_EVENT_CODE(CREATE, TRCSUCCESS, MICRIUM_SEM, p_sem), MICRIUM_SEM, p_sem); \
    prvTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(MICRIUM_SEM, p_sem), TRACE_GET_OBJECT_NUMBER(MICRIUM_SEM, p_sem), p_name);

/* No critical section needed */
#undef OS_TRACE_Q_CREATE
#define OS_TRACE_Q_CREATE(p_q, p_name )\
	trcKERNEL_HOOKS_OBJECT_CREATE(TRACE_GET_OBJECT_EVENT_CODE(CREATE, TRCSUCCESS, MICRIUM_Q, p_q), MICRIUM_Q, p_q); \
	prvTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(MICRIUM_Q, p_q), TRACE_GET_OBJECT_NUMBER(MICRIUM_Q, p_q), p_name);

/* No critical section needed */
#undef OS_TRACE_FLAG_CREATE
#define OS_TRACE_FLAG_CREATE(p_grp, p_name )\
	trcKERNEL_HOOKS_OBJECT_CREATE(TRACE_GET_OBJECT_EVENT_CODE(CREATE, TRCSUCCESS, MICRIUM_FLAG, p_grp), MICRIUM_FLAG, p_grp); \
	prvTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(MICRIUM_FLAG, p_grp), TRACE_GET_OBJECT_NUMBER(MICRIUM_FLAG, p_grp), p_name);

/* No critical section needed */
#undef OS_TRACE_MEM_CREATE
#define OS_TRACE_MEM_CREATE(p_mem, p_name )\
	trcKERNEL_HOOKS_OBJECT_CREATE(TRACE_GET_OBJECT_EVENT_CODE(CREATE, TRCSUCCESS, MICRIUM_MEM, p_mem), MICRIUM_MEM, p_mem); \
	prvTraceSetObjectName(TRACE_GET_OBJECT_TRACE_CLASS(MICRIUM_MEM, p_mem), TRACE_GET_OBJECT_NUMBER(MICRIUM_MEM, p_mem), p_name);

/* Called when a message is sent to a queue */
/* Critical section needed. Also SR_ALLOC needed. */
#undef OS_TRACE_TASK_MSG_Q_POST
#define OS_TRACE_TASK_MSG_Q_POST(p_msg_q) \
	{ \
		TRACE_ALLOC_CRITICAL_SECTION(); \
		TRACE_ENTER_CRITICAL_SECTION(); \
		if ((p_msg_q)->NbrEntries >= (p_msg_q)->NbrEntriesSize || OSMsgPool.NbrFree == 0u) \
		{ \
			trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCFAILED, MICRIUM_TASK_MSG_Q, p_msg_q), MICRIUM_TASK_MSG_Q, p_msg_q); \
		} \
		else \
		{ \
			trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCSUCCESS, MICRIUM_TASK_MSG_Q, p_msg_q), MICRIUM_TASK_MSG_Q, p_msg_q); \
			trcKERNEL_HOOKS_SET_OBJECT_STATE(MICRIUM_TASK_MSG_Q, p_msg_q, (uint32_t)((p_msg_q)->NbrEntries)); \
		} \
		TRACE_EXIT_CRITICAL_SECTION(); \
	}

/* No critical section needed */
#undef OS_TRACE_TASK_MSG_Q_POST_FAILED
#define OS_TRACE_TASK_MSG_Q_POST_FAILED(p_msg_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCFAILED, MICRIUM_TASK_MSG_Q, p_msg_q), MICRIUM_TASK_MSG_Q, p_msg_q);	
	
/* Called when a signal is posted to a task semaphore (Post) */
/* Critical section needed. Also SR_ALLOC needed. */
#undef OS_TRACE_TASK_SEM_POST
#define OS_TRACE_TASK_SEM_POST(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		OS_TCB *trcPtr; \
		if (p_tcb == 0) \
		{ \
			trcPtr = OSTCBCurPtr; \
		} \
		else \
		{ \
			trcPtr = p_tcb; \
		} \
		TRACE_ALLOC_CRITICAL_SECTION(); \
		TRACE_ENTER_CRITICAL_SECTION(); \
		if (trcPtr->SemCtr == (OS_SEM_CTR)-1) \
		{ \
			trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCFAILED, MICRIUM_TASK_SEM, trcPtr), MICRIUM_TASK_SEM, trcPtr); \
		} \
		else \
		{ \
			trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCSUCCESS, MICRIUM_TASK_SEM, trcPtr), MICRIUM_TASK_SEM, trcPtr); \
			trcKERNEL_HOOKS_SET_OBJECT_STATE(MICRIUM_TASK_SEM, trcPtr, (uint8_t)(trcPtr->SemCtr)); \
		} \
		TRACE_EXIT_CRITICAL_SECTION(); \
	}

/* No critical section needed */
#undef OS_TRACE_TASK_SEM_POST_FAILED
#define OS_TRACE_TASK_SEM_POST_FAILED(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCFAILED, MICRIUM_TASK_SEM, p_tcb), MICRIUM_TASK_SEM, p_tcb); \
	}

/* Called when a signal is posted to a semaphore (Post) */
/* Critical section needed. Also SR_ALLOC needed. */
#undef OS_TRACE_SEM_POST
#define OS_TRACE_SEM_POST(p_sem) \
	{ \
		TRACE_ALLOC_CRITICAL_SECTION(); \
		TRACE_ENTER_CRITICAL_SECTION(); \
		if (p_sem->Ctr == (OS_SEM_CTR)-1) \
		{ \
			trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCFAILED, MICRIUM_SEM, p_sem), MICRIUM_SEM, p_sem); \
		} \
		else \
		{ \
			trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCSUCCESS, MICRIUM_SEM, p_sem), MICRIUM_SEM, p_sem); \
			trcKERNEL_HOOKS_SET_OBJECT_STATE(MICRIUM_SEM, p_sem, (uint8_t)(p_sem->Ctr)); \
		} \
		TRACE_EXIT_CRITICAL_SECTION(); \
	}

/* No critical section needed */
#undef OS_TRACE_SEM_POST_FAILED
#define OS_TRACE_SEM_POST_FAILED(p_sem) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCFAILED, MICRIUM_SEM, p_sem), MICRIUM_SEM, p_sem);

/* Called when a message is posted to a queue (Post) */
/* Critical section needed. Also SR_ALLOC needed. */
#undef OS_TRACE_Q_POST
#define OS_TRACE_Q_POST(p_q) \
  { \
		TRACE_ALLOC_CRITICAL_SECTION(); \
		TRACE_ENTER_CRITICAL_SECTION(); \
		if (p_q->MsgQ.NbrEntries >= p_q->MsgQ.NbrEntriesSize || OSMsgPool.NbrFree == 0u) \
		{ \
			trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCFAILED, MICRIUM_Q, p_q), MICRIUM_Q, p_q); \
		} \
		else \
		{ \
			trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCSUCCESS, MICRIUM_Q, p_q), MICRIUM_Q, p_q); \
			trcKERNEL_HOOKS_SET_OBJECT_STATE(MICRIUM_Q, p_q, (uint8_t)(p_q->PendList.NbrEntries)); \
		} \
		TRACE_EXIT_CRITICAL_SECTION(); \
	}

/* No critical section needed */
#undef OS_TRACE_Q_POST_FAILED
#define OS_TRACE_Q_POST_FAILED(p_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCFAILED, MICRIUM_Q, p_q), MICRIUM_Q, p_q);
	
/* Called when a signal is posted to a mutex (Post) */
/* No critical section needed */
#undef OS_TRACE_MUTEX_POST
#define OS_TRACE_MUTEX_POST(p_mutex) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCSUCCESS, MICRIUM_MUTEX, p_mutex), MICRIUM_MUTEX, p_mutex);
	
/* No critical section needed */
#undef OS_TRACE_MUTEX_POST_FAILED
#define OS_TRACE_MUTEX_POST_FAILED(p_mutex) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCFAILED, MICRIUM_MUTEX, p_mutex), MICRIUM_MUTEX, p_mutex);

/* Called when a signal is posted to a event flag group (Post) */
/* No critical section needed */
#undef OS_TRACE_FLAG_POST
#define OS_TRACE_FLAG_POST(p_grp) \
	switch (opt) \
	{ \
		case OS_OPT_POST_FLAG_SET: \
		case OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED: \
		case OS_OPT_POST_FLAG_CLR: \
		case OS_OPT_POST_FLAG_CLR | OS_OPT_POST_NO_SCHED: \
			trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCSUCCESS, MICRIUM_FLAG, p_grp), MICRIUM_FLAG, p_grp); \
			break; \
		default: \
			trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCFAILED, MICRIUM_FLAG, p_grp), MICRIUM_FLAG, p_grp); \
			break; \
	}

/* No critical section needed */
#undef OS_TRACE_FLAG_POST_FAILED
#define OS_TRACE_FLAG_POST_FAILED(p_grp) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCFAILED, MICRIUM_FLAG, p_grp), MICRIUM_FLAG, p_grp);

/* Called when a task or ISR returns a memory partition (put) */
/* Critical section needed */
#undef OS_TRACE_MEM_PUT
#define OS_TRACE_MEM_PUT(p_mem) \
	TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCSUCCESS, MICRIUM_MEM, p_mem), MICRIUM_MEM, p_mem); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(MICRIUM_MEM, p_mem, (uint8_t)(p_mem->NbrFree)); \
	TRACE_EXIT_CRITICAL_SECTION();

/* No critical section needed */
#undef OS_TRACE_MEM_PUT_FAILED
#define OS_TRACE_MEM_PUT_FAILED(p_mem) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(SEND, TRCFAILED, MICRIUM_MEM, p_mem), MICRIUM_MEM, p_mem);

/* Called when a message is received from a task message queue */
/* Critical section needed */
#undef OS_TRACE_TASK_MSG_Q_PEND
#define OS_TRACE_TASK_MSG_Q_PEND(p_msg_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCSUCCESS, MICRIUM_TASK_MSG_Q, p_msg_q), MICRIUM_TASK_MSG_Q, p_msg_q); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(MICRIUM_TASK_MSG_Q, p_msg_q, (uint8_t)(p_msg_q->NbrEntries));

/* No critical section needed */
#undef OS_TRACE_TASK_MSG_Q_PEND_FAILED
#define OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCFAILED, MICRIUM_TASK_MSG_Q, p_msg_q), MICRIUM_TASK_MSG_Q, p_msg_q);

/* Called when a message is received from a task message queue */
/* No critical section needed */
#undef OS_TRACE_TASK_MSG_Q_PEND_BLOCK
#define OS_TRACE_TASK_MSG_Q_PEND_BLOCK(p_msg_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCBLOCK, MICRIUM_TASK_MSG_Q, p_msg_q), MICRIUM_TASK_MSG_Q, p_msg_q); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

/* Called when a task pends on a task semaphore (Pend)*/
/* No critical section needed */
#undef OS_TRACE_TASK_SEM_PEND
#define OS_TRACE_TASK_SEM_PEND(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCSUCCESS, MICRIUM_TASK_SEM, p_tcb), MICRIUM_TASK_SEM, p_tcb); \
		trcKERNEL_HOOKS_SET_OBJECT_STATE(MICRIUM_TASK_SEM, p_tcb, (uint8_t)(p_tcb->SemCtr)); \
	}

/* No critical section needed */
#undef OS_TRACE_TASK_SEM_PEND_FAILED
#define OS_TRACE_TASK_SEM_PEND_FAILED(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCFAILED, MICRIUM_TASK_SEM, p_tcb), MICRIUM_TASK_SEM, p_tcb); \
	}

/* Called when a task pends and blocks on a task semaphore (Pend)*/
/* No critical section needed */
#undef OS_TRACE_TASK_SEM_PEND_BLOCK
#define OS_TRACE_TASK_SEM_PEND_BLOCK(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCBLOCK, MICRIUM_TASK_SEM, p_tcb), MICRIUM_TASK_SEM, p_tcb); \
		trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED(); \
	}

/* Called when a task pends on a semaphore (Pend)*/
/* No critical section needed */
#undef OS_TRACE_SEM_PEND
#define OS_TRACE_SEM_PEND(p_sem) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCSUCCESS, MICRIUM_SEM, p_sem), MICRIUM_SEM, p_sem); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(MICRIUM_SEM, p_sem, (uint8_t)(p_sem->Ctr));
	
/* No critical section needed */
#undef OS_TRACE_SEM_PEND_FAILED
#define OS_TRACE_SEM_PEND_FAILED(p_sem) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCFAILED, MICRIUM_SEM, p_sem), MICRIUM_SEM, p_sem);

/* Called when a task pends on a semaphore (Pend)*/
/* No critical section needed */
#undef OS_TRACE_SEM_PEND_BLOCK
#define OS_TRACE_SEM_PEND_BLOCK(p_sem) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCBLOCK, MICRIUM_SEM, p_sem), MICRIUM_SEM, p_sem); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();
	//trcKERNEL_HOOKS_SET_OBJECT_STATE(MICRIUM_SEM, p_sem, (uint8_t)(p_sem->Ctr)); \

/* Called when a message is received from a message queue */
/* Critical section needed */
#undef OS_TRACE_Q_PEND
#define OS_TRACE_Q_PEND(p_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCSUCCESS, MICRIUM_Q, p_q), MICRIUM_Q, p_q); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(MICRIUM_Q, p_q, (uint8_t)(p_q->PendList.NbrEntries));

/* No critical section needed */
#undef OS_TRACE_Q_PEND_FAILED
#define OS_TRACE_Q_PEND_FAILED(p_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCFAILED, MICRIUM_Q, p_q), MICRIUM_Q, p_q);

/* Called when a message is received from a message queue */
/* No critical section needed */
#undef OS_TRACE_Q_PEND_BLOCK
#define OS_TRACE_Q_PEND_BLOCK(p_q) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCBLOCK, MICRIUM_Q, p_q), MICRIUM_Q, p_q); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();
//	trcKERNEL_HOOKS_SET_OBJECT_STATE(MICRIUM_Q, p_q, (uint8_t)(p_q->PendList.NbrEntries)); \

/* Called when a task pends on a mutex (Pend)*/
/* No critical section needed */
#undef OS_TRACE_MUTEX_PEND
#define OS_TRACE_MUTEX_PEND(p_mutex) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCSUCCESS, MICRIUM_MUTEX, p_mutex), MICRIUM_MUTEX, p_mutex);

/* No critical section needed */
#undef OS_TRACE_MUTEX_PEND_FAILED
#define OS_TRACE_MUTEX_PEND_FAILED(p_mutex) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCFAILED, MICRIUM_MUTEX, p_mutex), MICRIUM_MUTEX, p_mutex);
	
/* Called when a task pends on a mutex (Pend)*/
/* No critical section needed */
#undef OS_TRACE_MUTEX_PEND_BLOCK
#define OS_TRACE_MUTEX_PEND_BLOCK(p_mutex) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCBLOCK, MICRIUM_MUTEX, p_mutex), MICRIUM_MUTEX, p_mutex);

/* Called when a task pends on an event flag group (Pend)*/
/* No critical section needed */
#undef OS_TRACE_FLAG_PEND
#define OS_TRACE_FLAG_PEND(p_grp) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCSUCCESS, MICRIUM_FLAG, p_grp), MICRIUM_FLAG, p_grp);

/* No critical section needed */
#undef OS_TRACE_FLAG_PEND_FAILED
#define OS_TRACE_FLAG_PEND_FAILED(p_grp) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCFAILED, MICRIUM_FLAG, p_grp), MICRIUM_FLAG, p_grp);

/* Called when a task pends on an event flag group (Pend)*/
/* No critical section needed */
#undef OS_TRACE_FLAG_PEND_BLOCK
#define OS_TRACE_FLAG_PEND_BLOCK(p_grp) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCBLOCK, MICRIUM_FLAG, p_grp), MICRIUM_FLAG, p_grp); \
	trcKERNEL_HOOKS_SET_TASK_INSTANCE_FINISHED();

/* Called when a task or ISR gets a memory partition */
/* Critical section needed */
#undef OS_TRACE_MEM_GET
#define OS_TRACE_MEM_GET(p_mem) \
	TRACE_ENTER_CRITICAL_SECTION(); \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCSUCCESS, MICRIUM_MEM, p_mem), MICRIUM_MEM, p_mem); \
	trcKERNEL_HOOKS_SET_OBJECT_STATE(MICRIUM_MEM, p_mem, (uint8_t)(p_mem->NbrFree)); \
	TRACE_EXIT_CRITICAL_SECTION();

/* No critical section needed */
#undef OS_TRACE_MEM_GET_FAILED
#define OS_TRACE_MEM_GET_FAILED(p_mem) \
	trcKERNEL_HOOKS_KERNEL_SERVICE(TRACE_GET_OBJECT_EVENT_CODE(RECEIVE, TRCFAILED, MICRIUM_MEM, p_mem), MICRIUM_MEM, p_mem);

/* Called in OSTaskChangePrio */
/* No critical section needed */
#undef OS_TRACE_TASK_PRIO_CHANGE
#define OS_TRACE_TASK_PRIO_CHANGE( pxTask, uxNewPriority ) \
	trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE(TASK_PRIORITY_SET, pxTask, uxNewPriority);

/* Called in by mutex operations, when the task's priority is changed to the one from the mutex's owner */
/* No critical section needed */
#undef OS_TRACE_MUTEX_TASK_PRIO_INHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_INHERIT(p_tcb, prio) \
	trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE(TASK_PRIORITY_INHERIT, p_tcb, prio);

/* Called in by mutex operations, when the task's priority is changed back to its original */
/* No critical section needed */
#undef OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb, prio) \
	trcKERNEL_HOOKS_TASK_PRIORITY_CHANGE(TASK_PRIORITY_DISINHERIT, p_tcb, prio);

#if (OS_VERSION < 30700u)
/* Called in vTaskResume */
/* No critical section needed */
#undef OS_TRACE_TASK_RESUME
#define OS_TRACE_TASK_RESUME(p_tcb) \
	trcKERNEL_HOOKS_TASK_RESUME(TASK_RESUME, p_tcb);

#else /* (OS_VERSION < 30700u) */
/* Called in vTaskResume */
/* Critical section needed */
#undef OS_TRACE_TASK_RESUME_ENTER
#define OS_TRACE_TASK_RESUME_ENTER(p_tcb) \
	prvTraceTaskResumeEnter(p_tcb);

#undef OS_TRACE_TASK_RESUME_EXIT
#define OS_TRACE_TASK_RESUME_EXIT(p_err) \
	prvTraceTaskResumeExit(p_err);
#endif /* (OS_VERSION < 30700u) */

#else /* (TRC_CFG_SCHEDULING_ONLY == 0) */

/* SCHEDULING ONLY, DEFINE ALL AS EMPTY */

#undef OS_TRACE_TASK_SUSPEND
#define OS_TRACE_TASK_SUSPEND(p_tcb) 

#undef OS_TRACE_TASK_SUSPEND_ENTER
#define OS_TRACE_TASK_SUSPEND_ENTER(p_tcb) 

#undef OS_TRACE_TASK_SUSPEND_EXIT
#define OS_TRACE_TASK_SUSPEND_EXIT(p_err) 

#undef OS_TRACE_TASK_DLY
#define OS_TRACE_TASK_DLY(dly_ticks) 

#undef OS_TRACE_TASK_SEM_DEL
#define OS_TRACE_TASK_SEM_DEL(p_tcb) 

#undef traceTASK_MSG_Q_DELETE
#define traceTASK_MSG_Q_DELETE(p_msg_q) 

#undef OS_TRACE_SEM_DEL
#define OS_TRACE_SEM_DEL(p_sem) 

#undef OS_TRACE_Q_DEL
#define OS_TRACE_Q_DEL(p_q) 

#undef OS_TRACE_MUTEX_DEL
#define OS_TRACE_MUTEX_DEL(p_mutex) 

#undef OS_TRACE_FLAG_DEL
#define OS_TRACE_FLAG_DEL(p_grp) 

#undef OS_TRACE_TASK_SEM_DEL
#define OS_TRACE_TASK_SEM_DEL(p_tcb) 

#undef OS_TRACE_TASK_MSG_Q_DEL
#define OS_TRACE_TASK_MSG_Q_DEL(p_msg_q) 

#undef OS_TRACE_TASK_CREATE_FAILED
#define OS_TRACE_TASK_CREATE_FAILED(p_tcb) 

#undef OS_TRACE_TASK_MSG_Q_CREATE
#define OS_TRACE_TASK_MSG_Q_CREATE(p_msg_q, p_name ) 

#undef OS_TRACE_TASK_SEM_CREATE
#define OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name ) 

#undef OS_TRACE_MUTEX_CREATE
#define OS_TRACE_MUTEX_CREATE(p_mutex, p_name ) 

#undef OS_TRACE_SEM_CREATE
#define OS_TRACE_SEM_CREATE(p_sem, p_name ) 

#undef OS_TRACE_Q_CREATE
#define OS_TRACE_Q_CREATE(p_q, p_name ) 

#undef OS_TRACE_FLAG_CREATE
#define OS_TRACE_FLAG_CREATE(p_grp, p_name ) 

#undef OS_TRACE_MEM_CREATE
#define OS_TRACE_MEM_CREATE(p_mem, p_name ) 

#undef OS_TRACE_TASK_MSG_Q_POST
#define OS_TRACE_TASK_MSG_Q_POST(p_msg_q) 

#undef OS_TRACE_TASK_MSG_Q_POST_FAILED
#define OS_TRACE_TASK_MSG_Q_POST_FAILED(p_msg_q) 

#undef OS_TRACE_TASK_SEM_POST
#define OS_TRACE_TASK_SEM_POST(p_tcb) 

#undef OS_TRACE_TASK_SEM_POST_FAILED
#define OS_TRACE_TASK_SEM_POST_FAILED(p_tcb) 

#undef OS_TRACE_SEM_POST
#define OS_TRACE_SEM_POST(p_sem) 

#undef OS_TRACE_SEM_POST_FAILED
#define OS_TRACE_SEM_POST_FAILED(p_sem) 

#undef OS_TRACE_Q_POST
#define OS_TRACE_Q_POST(p_q) 

#undef OS_TRACE_Q_POST_FAILED
#define OS_TRACE_Q_POST_FAILED(p_q) 

#undef OS_TRACE_MUTEX_POST
#define OS_TRACE_MUTEX_POST(p_mutex) 

#undef OS_TRACE_MUTEX_POST_FAILED
#define OS_TRACE_MUTEX_POST_FAILED(p_mutex) 

#undef OS_TRACE_FLAG_POST
#define OS_TRACE_FLAG_POST(p_grp) 

#undef OS_TRACE_FLAG_POST_FAILED
#define OS_TRACE_FLAG_POST_FAILED(p_grp) 

#undef OS_TRACE_MEM_PUT
#define OS_TRACE_MEM_PUT(p_mem) 

#undef OS_TRACE_MEM_PUT_FAILED
#define OS_TRACE_MEM_PUT_FAILED(p_mem) 

#undef OS_TRACE_TASK_MSG_Q_PEND
#define OS_TRACE_TASK_MSG_Q_PEND(p_msg_q) 

#undef OS_TRACE_TASK_MSG_Q_PEND_FAILED
#define OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q) 

#undef OS_TRACE_TASK_MSG_Q_PEND_BLOCK
#define OS_TRACE_TASK_MSG_Q_PEND_BLOCK(p_msg_q) 

#undef OS_TRACE_TASK_SEM_PEND
#define OS_TRACE_TASK_SEM_PEND(p_tcb) 

#undef OS_TRACE_TASK_SEM_PEND_FAILED
#define OS_TRACE_TASK_SEM_PEND_FAILED(p_tcb) 

#undef OS_TRACE_TASK_SEM_PEND_BLOCK
#define OS_TRACE_TASK_SEM_PEND_BLOCK(p_tcb) 

#undef OS_TRACE_SEM_PEND
#define OS_TRACE_SEM_PEND(p_sem) 

#undef OS_TRACE_SEM_PEND_FAILED
#define OS_TRACE_SEM_PEND_FAILED(p_sem) 

#undef OS_TRACE_SEM_PEND_BLOCK
#define OS_TRACE_SEM_PEND_BLOCK(p_sem) 

#undef OS_TRACE_Q_PEND
#define OS_TRACE_Q_PEND(p_q) 

#undef OS_TRACE_Q_PEND_FAILED
#define OS_TRACE_Q_PEND_FAILED(p_q) 

#undef OS_TRACE_Q_PEND_BLOCK
#define OS_TRACE_Q_PEND_BLOCK(p_q) 

#undef OS_TRACE_MUTEX_PEND
#define OS_TRACE_MUTEX_PEND(p_mutex) 

#undef OS_TRACE_MUTEX_PEND_FAILED
#define OS_TRACE_MUTEX_PEND_FAILED(p_mutex) 

#undef OS_TRACE_MUTEX_PEND_BLOCK
#define OS_TRACE_MUTEX_PEND_BLOCK(p_mutex) 

#undef OS_TRACE_FLAG_PEND
#define OS_TRACE_FLAG_PEND(p_grp) 

#undef OS_TRACE_FLAG_PEND_FAILED
#define OS_TRACE_FLAG_PEND_FAILED(p_grp) 

#undef OS_TRACE_FLAG_PEND_BLOCK
#define OS_TRACE_FLAG_PEND_BLOCK(p_grp) 

#undef OS_TRACE_MEM_GET
#define OS_TRACE_MEM_GET(p_mem) 

#undef OS_TRACE_MEM_GET_FAILED
#define OS_TRACE_MEM_GET_FAILED(p_mem) 

#undef OS_TRACE_TASK_PRIO_CHANGE
#define OS_TRACE_TASK_PRIO_CHANGE( pxTask, uxNewPriority ) 

#undef OS_TRACE_MUTEX_TASK_PRIO_INHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_INHERIT(p_tcb, prio) 

#undef OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb, prio) 

#undef OS_TRACE_TASK_RESUME
#define OS_TRACE_TASK_RESUME(p_tcb) 

#undef OS_TRACE_TASK_RESUME_ENTER
#define OS_TRACE_TASK_RESUME_ENTER(p_tcb) 

#undef OS_TRACE_TASK_RESUME_EXIT
#define OS_TRACE_TASK_RESUME_EXIT(p_err) 

#endif /* (TRC_CFG_SCHEDULING_ONLY == 0) */

#undef OS_TRACE_ISR_REGISTER
#define OS_TRACE_ISR_REGISTER(isr_id, isr_name, isr_prio) \
        vTraceSetISRProperties(isr_id, isr_name, isr_prio);

#undef OS_TRACE_ISR_BEGIN
#define OS_TRACE_ISR_BEGIN(isr_id) \
        vTraceStoreISRBegin(isr_id);

#undef OS_TRACE_ISR_END
#define OS_TRACE_ISR_END(flag) \
        vTraceStoreISREnd(flag);

#define  OS_TRACE_ISR_ENTER()
#define  OS_TRACE_ISR_EXIT()
#define  OS_TRACE_ISR_EXIT_TO_SCHEDULER()

#endif /*#if TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_SNAPSHOT */

/******************************************************************************/
/*** Definitions for Streaming mode *******************************************/
/******************************************************************************/
#if (TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING)

#define  OS_TRACE_INIT()                                      vTraceEnable(TRC_INIT)
#define  OS_TRACE_START()                                     
#define  OS_TRACE_STOP()                                      
#define  OS_TRACE_CLEAR()                                     vTraceClear()

//#define  OS_TRACE_USR_EVT_CREATE(evt_name)                    Use xTraceRegisterString(evt_name) to create a traceString that can be used with vTracePrint() and vTracePrintF()
//#define  OS_TRACE_USR_EVT_LOG(hnd)                            Use vTracePrint() instead
//#define  OS_TRACE_PRINTF(hnd, format_str, ...)                Use vTracePrintF() instead
	
/*******************************************************************************
* vTraceStoreKernelObjectName
*
* Set the name for a kernel object (defined by its address).
******************************************************************************/			
void vTraceStoreKernelObjectName(void* object, const char* name); 

/*******************************************************************************
 * prvTraceOnBegin
 *
 * Called on trace begin.
 ******************************************************************************/
void prvTraceOnBegin(void);

/*******************************************************************************
 * prvTraceOnEnd
 *
 * Called on trace end.
 ******************************************************************************/
void prvTraceOnEnd(void);

/*************************************************************************/
/* KERNEL SPECIFIC OBJECT CONFIGURATION									 */
/*************************************************************************/

/*******************************************************************************
 * The event codes - should match the offline config file.
 ******************************************************************************/

#define PSF_EVENT_NULL_EVENT								0x00

/* PSF event codes */
#define PSF_EVENT_TRACE_START								0x01
#define PSF_EVENT_TS_CONFIG									0x02
#define PSF_EVENT_OBJ_NAME									0x03
#define PSF_EVENT_TASK_PRIORITY								0x04
#define PSF_EVENT_TASK_PRIO_INHERIT							0x05
#define PSF_EVENT_TASK_PRIO_DISINHERIT						0x06
#define PSF_EVENT_DEFINE_ISR								0x07
#define PSF_EVENT_UNUSED_STACK								0x08

#define PSF_EVENT_TASK_CREATE								0x10
#define PSF_EVENT_TASK_SEM_CREATE							0x11
#define PSF_EVENT_TASK_MSG_Q_CREATE							0x12
#define PSF_EVENT_QUEUE_CREATE								0x13
#define PSF_EVENT_SEMAPHORE_CREATE					        0x14
#define PSF_EVENT_MUTEX_CREATE								0x15
#define PSF_EVENT_FLAG_CREATE							    0x16
#define PSF_EVENT_MEM_CREATE							    0x17
#define PSF_EVENT_TMR_CREATE							    0x18

#define PSF_EVENT_TASK_DELETE								0x20
#define PSF_EVENT_QUEUE_DELETE								0x21
#define PSF_EVENT_SEMAPHORE_DELETE							0x22
#define PSF_EVENT_MUTEX_DELETE								0x23
#define PSF_EVENT_FLAG_DELETE					   		    0x24
#define PSF_EVENT_MEM_DELETE								0x25
#define PSF_EVENT_TMR_DELETE								0x26
#define PSF_EVENT_TASK_SEM_DELETE							0x27
#define PSF_EVENT_TASK_MSG_Q_DELETE							0x28

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
#define PSF_EVENT_SEMAPHORE_CREATE_FAILED			        0x42
#define PSF_EVENT_MUTEX_CREATE_FAILED						0x43
#define PSF_EVENT_FLAG_CREATE_FAILED						0x44
#define PSF_EVENT_MEM_CREATE_FAILED					        0x45
#define PSF_EVENT_TMR_CREATE_FAILED			                0x46

#define PSF_EVENT_TASK_MSG_Q_SEND		    				0x50
#define PSF_EVENT_TASK_SEM_SEND		    					0x51
#define PSF_EVENT_SEM_SEND				        			0x52
#define PSF_EVENT_Q_SEND		    						0x53
#define PSF_EVENT_MUTEX_SEND								0x54
#define PSF_EVENT_FLAG_SEND						    		0x55
#define PSF_EVENT_MEM_SEND						    		0x56

#define PSF_EVENT_TASK_MSG_Q_SEND_FAILED		    		0x57
#define PSF_EVENT_TASK_SEM_SEND_FAILED		    			0x58
#define PSF_EVENT_SEM_SEND_FAILED				        	0x59
#define PSF_EVENT_Q_SEND_FAILED		    					0x5A
#define PSF_EVENT_MUTEX_SEND_FAILED					    	0x5B
#define PSF_EVENT_FLAG_SEND_FAILED					   		0x5C
#define PSF_EVENT_MEM_SEND_FAILED					   		0x5D

#define PSF_EVENT_TASK_MSG_Q_RECEIVE		   				0x60
#define PSF_EVENT_TASK_SEM_RECEIVE		    				0x61
#define PSF_EVENT_SEM_RECEIVE				       			0x62
#define PSF_EVENT_Q_RECEIVE		    						0x63
#define PSF_EVENT_MUTEX_RECEIVE								0x64
#define PSF_EVENT_FLAG_RECEIVE					    		0x65
#define PSF_EVENT_MEM_RECEIVE					    		0x66

#define PSF_EVENT_TASK_MSG_Q_RECEIVE_FAILED		   			0x70
#define PSF_EVENT_TASK_SEM_RECEIVE_FAILED		    		0x71
#define PSF_EVENT_SEM_RECEIVE_FAILED		      			0x72
#define PSF_EVENT_Q_RECEIVE_FAILED		    				0x73
#define PSF_EVENT_MUTEX_RECEIVE_FAILED						0x74
#define PSF_EVENT_FLAG_RECEIVE_FAILED			    		0x75
#define PSF_EVENT_MEM_RECEIVE_FAILED			    		0x76

#define PSF_EVENT_TASK_MSG_Q_RECEIVE_BLOCK		    		0x80
#define PSF_EVENT_TASK_SEM_RECEIVE_BLOCK		    		0x81
#define PSF_EVENT_SEM_RECEIVE_BLOCK				        	0x82
#define PSF_EVENT_Q_RECEIVE_BLOCK		    				0x83
#define PSF_EVENT_MUTEX_RECEIVE_BLOCK						0x84
#define PSF_EVENT_FLAG_RECEIVE_BLOCK					  	0x85
#define PSF_EVENT_MEM_RECEIVE_BLOCK				    		0x86

#define PSF_EVENT_TASK_DELAY								0xAA
#define PSF_EVENT_TASK_SUSPEND								0xAB
#define PSF_EVENT_TASK_RESUME								0xAC
#define PSF_EVENT_TASK_SUSPEND_FAILED						0xAD
#define PSF_EVENT_TASK_RESUME_FAILED						0xAE

#define PSF_EVENT_USER_EVENT								0xB4
#define PSF_EVENT_USER_EVENT_2								0xB5
#define PSF_EVENT_USER_EVENT_3								0xB6
#define PSF_EVENT_USER_EVENT_4								0xB7
#define PSF_EVENT_USER_EVENT_5								0xB8
#define PSF_EVENT_USER_EVENT_6								0xB9
#define PSF_EVENT_USER_EVENT_7								0xBA
#define PSF_EVENT_USER_EVENT_8								0xBB

/*** The trace macros for streaming ******************************************/

/* Called in OSTaskCreate */
/* Need to protect critical section without reallocating the SR.                     */
#undef OS_TRACE_TASK_CREATE
#if (OS_VERSION >= 30700u)
#define OS_TRACE_TASK_CREATE(p_tcb) \
	TRACE_ENTER_CRITICAL_SECTION(); \
    if (p_tcb != NULL) \
	{ \
		prvAddTaskToStackMonitor(p_tcb); \
		prvTraceSaveObjectSymbol(p_tcb, (const char*)p_tcb->NamePtr); \
		prvTraceSaveObjectData(p_tcb, p_tcb->Prio); \
		prvTraceStoreStringEvent(1, PSF_EVENT_OBJ_NAME, p_tcb->NamePtr, p_tcb); \
		prvTraceStoreEvent2(PSF_EVENT_TASK_CREATE, (uint32_t)p_tcb, p_tcb->Prio); \
	} \
	TRACE_EXIT_CRITICAL_SECTION();
#else /* (OS_VERSION >= 30700u) */
#define OS_TRACE_TASK_CREATE(p_tcb) \
	TRACE_ENTER_CRITICAL_SECTION(); \
    if (p_tcb != NULL) \
	{ \
    	if (p_task == OS_TickTask) \
    	{ \
    		prvTrcTickTask = (void*)p_tcb; /* This is the Tick Task */ \
    	} \
		prvAddTaskToStackMonitor(p_tcb); \
		prvTraceSaveObjectSymbol(p_tcb, (const char*)p_tcb->NamePtr); \
		prvTraceSaveObjectData(p_tcb, p_tcb->Prio); \
		prvTraceStoreStringEvent(1, PSF_EVENT_OBJ_NAME, p_tcb->NamePtr, p_tcb); \
		prvTraceStoreEvent2(PSF_EVENT_TASK_CREATE, (uint32_t)p_tcb, p_tcb->Prio); \
	} \
	TRACE_EXIT_CRITICAL_SECTION();
#endif /* (OS_VERSION >= 30700u) */

/* Called in OSTaskCreate, if it fails (typically if the stack can not be allocated) */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_CREATE_FAILED
#define OS_TRACE_TASK_CREATE_FAILED(p_tcb) \
	prvTraceStoreEvent0(PSF_EVENT_TASK_CREATE_FAILED);

/* Called on OSTaskDel */
/* No need to protect critical section. */
/* If this macro give you problem try increase optimization. */
#undef OS_TRACE_TASK_DEL
#define OS_TRACE_TASK_DEL(p_tcb) \
	prvTraceStoreEvent2(PSF_EVENT_TASK_DELETE, (uint32_t)p_tcb, p_tcb->Prio); \
	prvTraceDeleteSymbol(p_tcb); \
	prvTraceDeleteObjectData(p_tcb); \
	prvRemoveTaskFromStackMonitor(p_tcb);

/* Called for each task that becomes ready */
/* No need to protect critical section. */
#if (TRC_CFG_INCLUDE_READY_EVENTS == 1)
#undef OS_TRACE_TASK_READY
#define OS_TRACE_TASK_READY( p_tcb ) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		prvTraceStoreEvent1(PSF_EVENT_TASK_READY, (uint32_t)p_tcb); \
	}
#else /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */
#define OS_TRACE_TASK_READY( p_tcb ) 
#endif /* (TRC_CFG_INCLUDE_READY_EVENTS == 1) */

/* Called on each OS tick. Will call uiPortGetTimestamp to make sure it is called at least once every OS tick. */
/* No need to protect critical section. */
#if (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1)
#undef OS_TRACE_TICK_INCREMENT
#define OS_TRACE_TICK_INCREMENT( OSTickCtr ) \
	{ extern uint32_t uiTraceTickCount; uiTraceTickCount = OSTickCtr + 1; prvTraceStoreEvent1(PSF_EVENT_NEW_TIME, OSTickCtr + 1); }
#else /* (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1) */
#define OS_TRACE_TICK_INCREMENT( OSTickCtr ) 
#endif /* (TRC_CFG_INCLUDE_OSTICK_EVENTS == 1) */

extern volatile uint32_t uiTraceSystemState;

/* Called on each task-switch */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SWITCHED_IN
#define OS_TRACE_TASK_SWITCHED_IN(p_tcb) \
	uiTraceSystemState = TRC_STATE_IN_TASKSWITCH; \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		if (prvIsNewTCB(p_tcb)) \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_TASK_ACTIVATE, (uint32_t)p_tcb, p_tcb->Prio); \
		} \
	} \
	uiTraceSystemState = TRC_STATE_IN_APPLICATION;
	
#if (TRC_CFG_SCHEDULING_ONLY == 0)

#if (OS_VERSION < 30700u)
/* Called on OSTaskSuspend */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SUSPEND
#define OS_TRACE_TASK_SUSPEND( p_tcb ) \
	if (p_tcb == 0) \
	{ \
		prvTraceStoreEvent1(PSF_EVENT_TASK_SUSPEND, (uint32_t)OSTCBCurPtr); \
	} \
	else \
	{ \
		prvTraceStoreEvent1(PSF_EVENT_TASK_SUSPEND, (uint32_t)p_tcb); \
	}
#else /* (OS_VERSION < 30700u) */
/* Called on OSTaskSuspend */
/* Critical section needed to prevent variable to be overwritten. */
#undef OS_TRACE_TASK_SUSPEND_ENTER
#define OS_TRACE_TASK_SUSPEND_ENTER( p_tcb ) \
	prvTraceTaskSuspendEnter(p_tcb);

#undef OS_TRACE_TASK_SUSPEND_EXIT
#define OS_TRACE_TASK_SUSPEND_EXIT(p_err) \
	prvTraceTaskSuspendExit(p_err);
#endif /* (OS_VERSION < 30700u) */

#if (TRACE_CFG_USE_TICKLESS_IDLE != 0)

/* NOT USED */
/* No need to protect critical section. */
#undef traceLOW_POWER_IDLE_BEGIN
#define traceLOW_POWER_IDLE_BEGIN() \
	prvTraceStoreEvent0(PSF_EVENT_LOWPOWER_BEGIN);

/* NOT USED */
/* No need to protect critical section. */
#undef traceLOW_POWER_IDLE_END
#define traceLOW_POWER_IDLE_END() \
	prvTraceStoreEvent0(PSF_EVENT_LOWPOWER_END);

#endif

/* Called on OSTimeDly */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_DLY
#define OS_TRACE_TASK_DLY(dly_ticks) \
	prvTraceStoreEvent1(PSF_EVENT_TASK_DELAY, dly_ticks);

/* Called on OSTaskDel */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_MSG_Q_DEL
#define OS_TRACE_TASK_MSG_Q_DEL(p_msg_q) \
	prvTraceStoreEvent2(PSF_EVENT_TASK_MSG_Q_DELETE, (uint32_t)p_msg_q, (uint32_t)((p_msg_q)->NbrEntries)); \
	prvTraceDeleteSymbol(p_msg_q);

/* Called on OSTaskDel */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_DEL
#define OS_TRACE_TASK_SEM_DEL(p_tcb) \
	prvTraceStoreEvent2(PSF_EVENT_TASK_SEM_DELETE, (uint32_t)(&p_tcb->SemCtr), (uint32_t)(p_tcb->SemCtr)); \
	prvTraceDeleteSymbol(&p_tcb->SemCtr);

/* Called on OSQDel */
/* No need to protect critical section. */
#undef OS_TRACE_Q_DEL
#define OS_TRACE_Q_DEL( p_q ) \
	prvTraceStoreEvent2(PSF_EVENT_QUEUE_DELETE, (uint32_t)p_q, 0); \
	prvTraceDeleteSymbol(p_q);

/* Called on OSMutexDel */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_DEL
#define OS_TRACE_MUTEX_DEL( p_mutex ) \
	prvTraceStoreEvent2(PSF_EVENT_MUTEX_DELETE, (uint32_t)p_mutex, 0); \
	prvTraceDeleteSymbol(p_mutex);

/* Called on OSSemDel */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_DEL
#define OS_TRACE_SEM_DEL( p_sem ) \
	prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_DELETE, (uint32_t)p_sem, 0); \
	prvTraceDeleteSymbol(p_sem);

/* Called on OSFlagDel */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_DEL
#define OS_TRACE_FLAG_DEL( p_grp ) \
	prvTraceStoreEvent2(PSF_EVENT_FLAG_DELETE, (uint32_t)p_grp, 0); \
	prvTraceDeleteSymbol(p_grp);

/* Called in OSTaskCreate, while it creates the built-in message queue */
/* Need to protect critical section without reallocating the SR.       */
#undef OS_TRACE_TASK_MSG_Q_CREATE
#define OS_TRACE_TASK_MSG_Q_CREATE(p_msg_q, p_name) \
	TRACE_ENTER_CRITICAL_SECTION(); \
    if (p_msg_q != NULL) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_MSG_Q_CREATE, (uint32_t)p_msg_q, (uint32_t)(((OS_MSG_Q *)p_msg_q)->NbrEntries)); \
        vTraceStoreKernelObjectName(p_msg_q, prvTraceAppend((const char*)p_name, "Q")); \
	} \
	TRACE_EXIT_CRITICAL_SECTION();


/* Called in OSTaskCreate, while it creates the built-in semaphore */
/* Need to protect critical section without reallocating the SR.       */
#undef OS_TRACE_TASK_SEM_CREATE
#define OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name) \
	TRACE_ENTER_CRITICAL_SECTION(); \
    if (p_tcb) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_SEM_CREATE, (uint32_t)(&p_tcb->SemCtr), p_tcb->SemCtr); \
        vTraceStoreKernelObjectName((void *)(&p_tcb->SemCtr), prvTraceAppend((const char*)p_name, "Sem")); \
	} \
	TRACE_EXIT_CRITICAL_SECTION();

/* Called from os_q.c. */
/* No need to protect critical section. */
#undef OS_TRACE_Q_CREATE
#define OS_TRACE_Q_CREATE(p_q, p_name ) \
	prvTraceStoreEvent2(PSF_EVENT_QUEUE_CREATE, (uint32_t)p_q, p_q->MsgQ.NbrEntriesSize); \
	vTraceStoreKernelObjectName(p_q, (const char*)p_name);

/* Called from OSSemCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_CREATE
#define OS_TRACE_SEM_CREATE( p_sem, p_name) \
	prvTraceStoreEvent2(PSF_EVENT_SEMAPHORE_CREATE, (uint32_t)p_sem, p_sem->Ctr); \
    vTraceStoreKernelObjectName(p_sem, (const char*)p_name);

/* Called from OSMutexCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_CREATE
#define OS_TRACE_MUTEX_CREATE( p_mutex, p_name ) \
	prvTraceStoreEvent1(PSF_EVENT_MUTEX_CREATE, (uint32_t)p_mutex); \
    vTraceStoreKernelObjectName(p_mutex, (const char*)p_name);

/* Called from OSMemCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_CREATE
#define OS_TRACE_MEM_CREATE( p_mem, p_name ) \
	prvTraceStoreEvent1(PSF_EVENT_MEM_CREATE, (uint32_t)p_mem); \
    vTraceStoreKernelObjectName(p_mem, (const char*)p_name);

/* Called from OSFlagCreate. */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_CREATE
#define OS_TRACE_FLAG_CREATE( p_grp, p_name ) \
	prvTraceStoreEvent1(PSF_EVENT_FLAG_CREATE, (uint32_t)p_grp); \
    vTraceStoreKernelObjectName(p_grp, (const char*)p_name);

/* Called when a message is sent to a task's queue */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_MSG_Q_POST
#define OS_TRACE_TASK_MSG_Q_POST(p_msg_q) \
	if ((p_msg_q)->NbrEntries >= (p_msg_q)->NbrEntriesSize || OSMsgPool.NbrFree == 0u) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_MSG_Q_SEND_FAILED, (uint32_t)p_msg_q, (uint32_t)(((OS_MSG_Q *)p_msg_q)->NbrEntries)); \
	} \
	else \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_MSG_Q_SEND, (uint32_t)p_msg_q, (uint32_t)(((OS_MSG_Q *)p_msg_q)->NbrEntries)); \
	}

/* Called when a message failed to be sent to a task's queue (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_MSG_Q_POST_FAILED
#define OS_TRACE_TASK_MSG_Q_POST_FAILED(p_msg_q) \
	prvTraceStoreEvent2(PSF_EVENT_TASK_MSG_Q_SEND_FAILED, (uint32_t)p_msg_q, (uint32_t)(((OS_MSG_Q *)p_msg_q)->NbrEntries));

/* Called when a message is sent to a task's semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_POST
#define OS_TRACE_TASK_SEM_POST(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		OS_TCB* trcPtr; \
		if (p_tcb == 0) \
		{ \
			trcPtr = OSTCBCurPtr; \
		} \
		else \
		{ \
			trcPtr = p_tcb; \
		} \
		if (trcPtr->SemCtr == (OS_SEM_CTR)-1) \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_TASK_SEM_SEND_FAILED, (uint32_t)(&trcPtr->SemCtr), (uint8_t)(trcPtr->SemCtr)); \
		} \
		else \
		{ \
			prvTraceStoreEvent2(PSF_EVENT_TASK_SEM_SEND, (uint32_t)(&trcPtr->SemCtr), (uint8_t)(trcPtr->SemCtr)); \
		} \
	}

/* Called when a signal failed to be sent to a task's semaphore (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_POST_FAILED
#define OS_TRACE_TASK_SEM_POST_FAILED(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_SEM_SEND_FAILED, (uint32_t)(&p_tcb->SemCtr), (uint8_t)(p_tcb->SemCtr)); \
	}

/* Called when a signal is sent to a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_POST
#define OS_TRACE_Q_POST(p_q) \
	if (p_q->MsgQ.NbrEntries >= p_q->MsgQ.NbrEntriesSize || OSMsgPool.NbrFree == 0u) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_Q_SEND_FAILED, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries)); \
	} \
	else \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_Q_SEND, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries)); \
	}

/* Called when a message failed to be sent to a queue (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_Q_POST_FAILED
#define OS_TRACE_Q_POST_FAILED(p_q) \
	prvTraceStoreEvent2(PSF_EVENT_Q_SEND_FAILED, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries));

/* Called when a signal is sent to a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_POST
#define OS_TRACE_SEM_POST(p_sem) \
	if (p_sem->Ctr == (OS_SEM_CTR)-1) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_SEM_SEND_FAILED, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr)); \
	} \
	else \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_SEM_SEND, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr)); \
	}

/* Called when a signal failed to be sent to a semaphore (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_POST_FAILED
#define OS_TRACE_SEM_POST_FAILED(p_tcb) \
	prvTraceStoreEvent2(PSF_EVENT_SEM_SEND_FAILED, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr));

/* Called when a signal is sent to a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_POST
#define OS_TRACE_MUTEX_POST(p_mutex) \
	prvTraceStoreEvent1(PSF_EVENT_MUTEX_SEND, (uint32_t)p_mutex);

/* Called when a message failed to be sent to a mutex (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_POST_FAILED
#define OS_TRACE_MUTEX_POST_FAILED(p_mutex) \
	prvTraceStoreEvent1(PSF_EVENT_MUTEX_SEND_FAILED, (uint32_t)p_mutex);

/* Called when a signal is sent to a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_POST
#define OS_TRACE_FLAG_POST(p_grp) \
	switch (opt) \
	{ \
		case OS_OPT_POST_FLAG_SET: \
		case OS_OPT_POST_FLAG_SET | OS_OPT_POST_NO_SCHED: \
		case OS_OPT_POST_FLAG_CLR: \
		case OS_OPT_POST_FLAG_CLR | OS_OPT_POST_NO_SCHED: \
			prvTraceStoreEvent1(PSF_EVENT_FLAG_SEND, (uint32_t)p_grp); \
			break; \
		default: \
			prvTraceStoreEvent1(PSF_EVENT_FLAG_SEND_FAILED, (uint32_t)p_grp); \
			break; \
	}

/* Called when a message failed to be sent to a flag group (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_POST_FAILED
#define OS_TRACE_FLAG_POST_FAILED(p_grp) \
	prvTraceStoreEvent1(PSF_EVENT_FLAG_SEND_FAILED, (uint32_t)p_grp);

/* Called when a signal is sent to a memory partition */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_PUT
#define OS_TRACE_MEM_PUT(p_mem) \
	prvTraceStoreEvent2(PSF_EVENT_MEM_SEND, (uint32_t)p_mem, (uint8_t)(p_mem->NbrFree));

/* Called when a message failed to be sent to a memory partition (timeout) */
/* No need to protect critical section. */
#undef OS_TRACE_MEM_PUT_FAILED
#define OS_TRACE_MEM_PUT_FAILED(p_mem) \
	prvTraceStoreEvent1(PSF_EVENT_MEM_SEND_FAILED, (uint32_t)p_mem);


/* Called when a message is received from a task message queue */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_MSG_Q_PEND
#define OS_TRACE_TASK_MSG_Q_PEND(p_msg_q) \
	prvTraceStoreEvent2(PSF_EVENT_TASK_MSG_Q_RECEIVE, (uint32_t)p_msg_q, (uint8_t)(p_msg_q->NbrEntries));

/* Called when a message is received from a task message queue */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_MSG_Q_PEND_FAILED
#define OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q) \
	prvTraceStoreEvent2(PSF_EVENT_TASK_MSG_Q_RECEIVE_FAILED, (uint32_t)p_msg_q, (uint8_t)(p_msg_q->NbrEntries));

/* Called when a message is received from a task message queue */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_MSG_Q_PEND_BLOCK
#define OS_TRACE_TASK_MSG_Q_PEND_BLOCK(p_msg_q) \
	prvTraceStoreEvent2(PSF_EVENT_TASK_MSG_Q_RECEIVE_BLOCK, (uint32_t)p_msg_q, (uint8_t)(p_msg_q->NbrEntries));

/* Called when a message is received from a task semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_PEND
#define OS_TRACE_TASK_SEM_PEND(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_SEM_RECEIVE, (uint32_t)(&p_tcb->SemCtr), (uint8_t)(p_tcb->SemCtr)); \
	}

/* Called when a message is received from a task semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_PEND_FAILED
#define OS_TRACE_TASK_SEM_PEND_FAILED(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_SEM_RECEIVE_FAILED, (uint32_t)(&p_tcb->SemCtr), (uint8_t)(p_tcb->SemCtr)); \
	}

/* Called when a message is received from a task semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_SEM_PEND_BLOCK
#define OS_TRACE_TASK_SEM_PEND_BLOCK(p_tcb) \
	TRACE_IF_NOT_TICK_TASK(p_tcb) \
	{ \
		prvTraceStoreEvent2(PSF_EVENT_TASK_SEM_RECEIVE_BLOCK, (uint32_t)(&p_tcb->SemCtr), (uint8_t)(p_tcb->SemCtr)); \
	}

/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_PEND
#define OS_TRACE_SEM_PEND(p_sem) \
	prvTraceStoreEvent2(PSF_EVENT_SEM_RECEIVE, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr));

/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_PEND_FAILED
#define OS_TRACE_SEM_PEND_FAILED(p_sem) \
	prvTraceStoreEvent2(PSF_EVENT_SEM_RECEIVE_FAILED, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr));

/* Called when a message is received from a semaphore */
/* No need to protect critical section. */
#undef OS_TRACE_SEM_PEND_BLOCK
#define OS_TRACE_SEM_PEND_BLOCK(p_sem) \
	prvTraceStoreEvent2(PSF_EVENT_SEM_RECEIVE_BLOCK, (uint32_t)p_sem, (uint8_t)(p_sem->Ctr));


/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_PEND
#define OS_TRACE_Q_PEND(p_q) \
	prvTraceStoreEvent2(PSF_EVENT_Q_RECEIVE, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries));

/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_PEND_FAILED
#define OS_TRACE_Q_PEND_FAILED(p_q) \
	prvTraceStoreEvent2(PSF_EVENT_Q_RECEIVE_FAILED, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries));

/* Called when a message is received from a queue */
/* No need to protect critical section. */
#undef OS_TRACE_Q_PEND_BLOCK
#define OS_TRACE_Q_PEND_BLOCK(p_q) \
	prvTraceStoreEvent2(PSF_EVENT_Q_RECEIVE_BLOCK, (uint32_t)p_q, (uint8_t)(p_q->PendList.NbrEntries));


/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_PEND
#define OS_TRACE_MUTEX_PEND(p_mutex) \
	prvTraceStoreEvent1(PSF_EVENT_MUTEX_RECEIVE, (uint32_t)p_mutex);

/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_PEND_FAILED
#define OS_TRACE_MUTEX_PEND_FAILED(p_mutex) \
	prvTraceStoreEvent1(PSF_EVENT_MUTEX_RECEIVE_FAILED, (uint32_t)p_mutex);

/* Called when a signal is received from a mutex */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_PEND_BLOCK
#define OS_TRACE_MUTEX_PEND_BLOCK(p_mutex) \
	prvTraceStoreEvent1(PSF_EVENT_MUTEX_RECEIVE_BLOCK, (uint32_t)p_mutex);


/* Called when a signal is received from a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_PEND
#define OS_TRACE_FLAG_PEND(p_grp) \
	prvTraceStoreEvent1(PSF_EVENT_FLAG_RECEIVE, (uint32_t)p_grp);

/* Called when a signal is received from a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_PEND_FAILED
#define OS_TRACE_FLAG_PEND_FAILED(p_grp) \
	prvTraceStoreEvent1(PSF_EVENT_FLAG_RECEIVE_FAILED, (uint32_t)p_grp);

/* Called when a signal is received from a flag group */
/* No need to protect critical section. */
#undef OS_TRACE_FLAG_PEND_BLOCK
#define OS_TRACE_MEM_GET_FAILED(p_mem) \
	prvTraceStoreEvent2(PSF_EVENT_MEM_RECEIVE_FAILED, (uint32_t)p_mem, (uint8_t)(p_mem->NbrFree));

/* Called in OSTaskChangePrio */
#undef OS_TRACE_TASK_PRIO_CHANGE
#define OS_TRACE_TASK_PRIO_CHANGE( p_tcb, prio ) \
	prvTraceStoreEvent2(PSF_EVENT_TASK_PRIORITY, (uint32_t)p_tcb, prio);
	
/* Called in by mutex operations, when the task's priority is changed to the one from the mutex's owner */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_TASK_PRIO_INHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_INHERIT( p_tcb, prio ) \
	prvTraceStoreEvent2(PSF_EVENT_TASK_PRIO_INHERIT, (uint32_t)p_tcb, prio);

/* Called in by mutex operations, when the task's priority is changed back to its original */
/* No need to protect critical section. */
#undef OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT( p_tcb, prio ) \
	prvTraceStoreEvent2(PSF_EVENT_TASK_PRIO_DISINHERIT, (uint32_t)p_tcb, prio);

#if (OS_VERSION < 30700u)
/* Called in OSTaskResume */
/* No need to protect critical section. */
#undef OS_TRACE_TASK_RESUME
#define OS_TRACE_TASK_RESUME( p_tcb ) \
	prvTraceStoreEvent1(PSF_EVENT_TASK_RESUME, (uint32_t)p_tcb);

#else /* (OS_VERSION < 30700u) */
/* Called in OSTaskResume */
/* Critical section needed to prevent variable to be overwritten. */
#undef OS_TRACE_TASK_RESUME_ENTER
#define OS_TRACE_TASK_RESUME_ENTER( p_tcb )\
	prvTraceTaskResumeEnter(p_tcb);

#undef OS_TRACE_TASK_RESUME_EXIT
#define OS_TRACE_TASK_RESUME_EXIT( p_err )\
	prvTraceTaskResumeExit(p_err);
#endif /* (OS_VERSION < 30700u) */

#else /* (TRC_CFG_SCHEDULING_ONLY == 0) */
	
/* SCHEDULING ONLY, DEFINE ALL AS EMPTY */
	
#undef OS_TRACE_TASK_SUSPEND
#define OS_TRACE_TASK_SUSPEND( p_tcb ) 

#undef OS_TRACE_TASK_SUSPEND_ENTER
#define OS_TRACE_TASK_SUSPEND_ENTER( p_tcb ) 

#undef OS_TRACE_TASK_SUSPEND_EXIT
#define OS_TRACE_TASK_SUSPEND_EXIT(p_err) 

#undef traceLOW_POWER_IDLE_BEGIN
#define traceLOW_POWER_IDLE_BEGIN() 

#undef traceLOW_POWER_IDLE_END
#define traceLOW_POWER_IDLE_END() 

#undef OS_TRACE_TASK_DLY
#define OS_TRACE_TASK_DLY(dly_ticks) 

#undef OS_TRACE_TASK_MSG_Q_DEL
#define OS_TRACE_TASK_MSG_Q_DEL(p_msg_q) 

#undef OS_TRACE_TASK_SEM_DEL
#define OS_TRACE_TASK_SEM_DEL(p_tcb) 

#undef OS_TRACE_Q_DEL
#define OS_TRACE_Q_DEL( p_q ) 

#undef OS_TRACE_MUTEX_DEL
#define OS_TRACE_MUTEX_DEL( p_mutex ) 

#undef OS_TRACE_SEM_DEL
#define OS_TRACE_SEM_DEL( p_sem ) 

#undef OS_TRACE_FLAG_DEL
#define OS_TRACE_FLAG_DEL( p_grp ) 

#undef OS_TRACE_TASK_MSG_Q_CREATE
#define OS_TRACE_TASK_MSG_Q_CREATE(p_msg_q, p_name) 

#undef OS_TRACE_TASK_SEM_CREATE
#define OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name) 

#undef OS_TRACE_Q_CREATE
#define OS_TRACE_Q_CREATE(p_q, p_name ) 

#undef OS_TRACE_SEM_CREATE
#define OS_TRACE_SEM_CREATE( p_sem, p_name) 

#undef OS_TRACE_MUTEX_CREATE
#define OS_TRACE_MUTEX_CREATE( p_mutex, p_name ) 

#undef OS_TRACE_MEM_CREATE
#define OS_TRACE_MEM_CREATE( p_mem, p_name ) 

#undef OS_TRACE_FLAG_CREATE
#define OS_TRACE_FLAG_CREATE( p_grp, p_name ) 

#undef OS_TRACE_TASK_MSG_Q_POST
#define OS_TRACE_TASK_MSG_Q_POST(p_msg_q) 

#undef OS_TRACE_TASK_MSG_Q_POST_FAILED
#define OS_TRACE_TASK_MSG_Q_POST_FAILED(p_msg_q) 

#undef OS_TRACE_TASK_SEM_POST
#define OS_TRACE_TASK_SEM_POST(p_tcb) 

#undef OS_TRACE_TASK_SEM_POST_FAILED
#define OS_TRACE_TASK_SEM_POST_FAILED(p_tcb) 

#undef OS_TRACE_Q_POST
#define OS_TRACE_Q_POST(p_q) 

#undef OS_TRACE_Q_POST_FAILED
#define OS_TRACE_Q_POST_FAILED(p_q) 

#undef OS_TRACE_SEM_POST
#define OS_TRACE_SEM_POST(p_sem) 

#undef OS_TRACE_SEM_POST_FAILED
#define OS_TRACE_SEM_POST_FAILED(p_tcb) 

#undef OS_TRACE_MUTEX_POST
#define OS_TRACE_MUTEX_POST(p_mutex) 

#undef OS_TRACE_MUTEX_POST_FAILED
#define OS_TRACE_MUTEX_POST_FAILED(p_mutex) 

#undef OS_TRACE_FLAG_POST
#define OS_TRACE_FLAG_POST(p_grp) 

#undef OS_TRACE_FLAG_POST_FAILED
#define OS_TRACE_FLAG_POST_FAILED(p_grp) 

#undef OS_TRACE_MEM_PUT
#define OS_TRACE_MEM_PUT(p_mem) 

#undef OS_TRACE_MEM_PUT_FAILED
#define OS_TRACE_MEM_PUT_FAILED(p_mem) 

#undef OS_TRACE_TASK_MSG_Q_PEND
#define OS_TRACE_TASK_MSG_Q_PEND(p_msg_q) 

#undef OS_TRACE_TASK_MSG_Q_PEND_FAILED
#define OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q) 

#undef OS_TRACE_TASK_MSG_Q_PEND_BLOCK
#define OS_TRACE_TASK_MSG_Q_PEND_BLOCK(p_msg_q) 

#undef OS_TRACE_TASK_SEM_PEND
#define OS_TRACE_TASK_SEM_PEND(p_tcb) 

#undef OS_TRACE_TASK_SEM_PEND_FAILED
#define OS_TRACE_TASK_SEM_PEND_FAILED(p_tcb) 

#undef OS_TRACE_TASK_SEM_PEND_BLOCK
#define OS_TRACE_TASK_SEM_PEND_BLOCK(p_tcb) 

#undef OS_TRACE_SEM_PEND
#define OS_TRACE_SEM_PEND(p_sem) 

#undef OS_TRACE_SEM_PEND_FAILED
#define OS_TRACE_SEM_PEND_FAILED(p_sem) 

#undef OS_TRACE_SEM_PEND_BLOCK
#define OS_TRACE_SEM_PEND_BLOCK(p_sem) 

#undef OS_TRACE_Q_PEND
#define OS_TRACE_Q_PEND(p_q) 

#undef OS_TRACE_Q_PEND_FAILED
#define OS_TRACE_Q_PEND_FAILED(p_q) 

#undef OS_TRACE_Q_PEND_BLOCK
#define OS_TRACE_Q_PEND_BLOCK(p_q) 

#undef OS_TRACE_MUTEX_PEND
#define OS_TRACE_MUTEX_PEND(p_mutex) 

#undef OS_TRACE_MUTEX_PEND_FAILED
#define OS_TRACE_MUTEX_PEND_FAILED(p_mutex) 

#undef OS_TRACE_MUTEX_PEND_BLOCK
#define OS_TRACE_MUTEX_PEND_BLOCK(p_mutex) 

#undef OS_TRACE_FLAG_PEND
#define OS_TRACE_FLAG_PEND(p_grp) 

#undef OS_TRACE_FLAG_PEND_FAILED
#define OS_TRACE_FLAG_PEND_FAILED(p_grp) 

#undef OS_TRACE_FLAG_PEND_BLOCK
#define OS_TRACE_MEM_GET_FAILED(p_mem) 

#undef OS_TRACE_TASK_PRIO_CHANGE
#define OS_TRACE_TASK_PRIO_CHANGE( p_tcb, prio ) 

#undef OS_TRACE_MUTEX_TASK_PRIO_INHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_INHERIT( p_tcb, prio ) 

#undef OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT
#define OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT( p_tcb, prio ) 

#undef OS_TRACE_TASK_RESUME
#define OS_TRACE_TASK_RESUME( p_tcb ) 

#undef OS_TRACE_TASK_RESUME_ENTER
#define OS_TRACE_TASK_RESUME_ENTER( p_tcb )

#undef OS_TRACE_TASK_RESUME_EXIT
#define OS_TRACE_TASK_RESUME_EXIT( p_err )

#endif /* (TRC_CFG_SCHEDULING_ONLY == 0) */

#undef OS_TRACE_ISR_REGISTER
#define OS_TRACE_ISR_REGISTER(isr_name, isr_prio) \
	vTraceSetISRProperties(isr_name, isr_prio);

#undef OS_TRACE_ISR_BEGIN
#define OS_TRACE_ISR_BEGIN(isr_name) \
	vTraceStoreISRBegin(isr_name);

#undef OS_TRACE_ISR_END
#define OS_TRACE_ISR_END(flag) \
	vTraceStoreISREnd(flag);

#define  OS_TRACE_ISR_ENTER()
#define  OS_TRACE_ISR_EXIT()
#define  OS_TRACE_ISR_EXIT_TO_SCHEDULER()

#endif /*TRC_CFG_RECORDER_MODE == TRC_RECORDER_MODE_STREAMING*/

#else /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#define vTraceExcludeQueue(handle)
#define vTraceExcludeSemaphore(handle)
#define vTraceExcludeMutex(handle)
#define vTraceExcludeTimer(handle)
#define vTraceExcludeEventGroup(handle)
#define vTraceExcludeDelays()

#endif /*(TRC_USE_TRACEALYZER_RECORDER == 1)*/

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_H */
