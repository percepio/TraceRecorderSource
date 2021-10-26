/*
*********************************************************************************************************
*                                             uC/Trace
*                                      The RTOS Event Analyzer
*
*                         (c) Copyright 2004-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*                                   Percepio's TraceAlyzer Port
*
* File    : trace_os.h
* Version : V3.03.02
* By      : JPB
*
* LICENSING TERMS:
* ---------------
*           uC/OS-III is provided in source form for FREE short-term evaluation, for educational use or 
*           for peaceful research.  If you plan or intend to use uC/OS-III in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-III for its use in your 
*           application/product.   We provide ALL the source code for your convenience and to help you 
*           experience uC/OS-III.  The fact that the source is provided does NOT mean that you can use 
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can contact us at www.micrium.com, or by phone at +1 (954) 217-2036.
*
* For     : Percepio's TraceAlyzer
*********************************************************************************************************
*/



/*
*          Percepio comment: THIS FILE WILL BE USED BY OLDER VERSION OF uC/OS-III (before v3.06.00) TO
*          REMAP THE TRACE POINTS SO THEY WORK WITH THE NEW RECORDER LIBRARY.
*/



/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <trcRecorder.h>

/*
*********************************************************************************************************
*                                            uC/Trace Macros
*********************************************************************************************************
*/

#if TRACE_CFG_EN > 0
#define  TRACE_INIT                                        OS_TRACE_INIT
#define  TRACE_START                                       OS_TRACE_START
#define  TRACE_STOP                                        OS_TRACE_STOP
#define  TRACE_CLEAR                                       OS_TRACE_CLEAR
#else
#define  TRACE_INIT()
#define  TRACE_START()
#define  TRACE_STOP()
#define  TRACE_CLEAR()
#endif


/*
*********************************************************************************************************
*                                    uC/Trace User-Defined Event Macros
*********************************************************************************************************
*/

#if TRACE_CFG_EN > 0
#define  TRACE_USR_EVT_CREATE                               OS_TRACE_USR_EVT_CREATE
#define  TRACE_USR_EVT_LOG                                  OS_TRACE_USR_EVT_LOG
#define  TRACE_PRINTF                                       OS_TRACE_PRINTF
#else
#define  TRACE_USR_EVT_CREATE(evt_name)
#define  TRACE_USR_EVT_LOG(hnd)
#define  TRACE_PRINTF(hnd, format_str, ...)
#endif


/*
*********************************************************************************************************
*                                       uC/Trace Kernel-Related Macros
*********************************************************************************************************
*/

#if TRACE_CFG_EN > 0
#define  TRACE_OS_TICK_INCREMENT                            OS_TRACE_TICK_INCREMENT

#define  TRACE_OS_TASK_CREATE                               OS_TRACE_TASK_CREATE
#define  TRACE_OS_TASK_CREATE_FAILED                        OS_TRACE_TASK_CREATE_FAILED
#define  TRACE_OS_TASK_DEL                                  OS_TRACE_TASK_DEL
#define  TRACE_OS_TASK_READY                                OS_TRACE_TASK_READY
#define  TRACE_OS_TASK_SWITCHED_IN                          OS_TRACE_TASK_SWITCHED_IN
#define  TRACE_OS_TASK_DLY                                  OS_TRACE_TASK_DLY
#define  TRACE_OS_TASK_SUSPEND                              OS_TRACE_TASK_SUSPEND
#define  TRACE_OS_TASK_RESUME                               OS_TRACE_TASK_RESUME

#define  TRACE_OS_ISR_REGISTER                              OS_TRACE_ISR_REGISTER
#define  TRACE_OS_ISR_BEGIN                                 OS_TRACE_ISR_BEGIN
#define  TRACE_OS_ISR_END                                   OS_TRACE_ISR_END

#define  TRACE_OS_TASK_MSG_Q_CREATE                         OS_TRACE_TASK_MSG_Q_CREATE
#define  TRACE_OS_TASK_MSG_Q_POST                           OS_TRACE_TASK_MSG_Q_POST
#define  TRACE_OS_TASK_MSG_Q_POST_FAILED                    OS_TRACE_TASK_MSG_Q_POST_FAILED
#define  TRACE_OS_TASK_MSG_Q_PEND                           OS_TRACE_TASK_MSG_Q_PEND
#define  TRACE_OS_TASK_MSG_Q_PEND_FAILED                    OS_TRACE_TASK_MSG_Q_PEND_FAILED
#define  TRACE_OS_TASK_MSG_Q_PEND_BLOCK                     OS_TRACE_TASK_MSG_Q_PEND_BLOCK

#define  TRACE_OS_TASK_SEM_CREATE                           OS_TRACE_TASK_SEM_CREATE
#define  TRACE_OS_TASK_SEM_POST                             OS_TRACE_TASK_SEM_POST
#define  TRACE_OS_TASK_SEM_POST_FAILED                      OS_TRACE_TASK_SEM_POST_FAILED
#define  TRACE_OS_TASK_SEM_PEND                             OS_TRACE_TASK_SEM_PEND
#define  TRACE_OS_TASK_SEM_PEND_FAILED                      OS_TRACE_TASK_SEM_PEND_FAILED
#define  TRACE_OS_TASK_SEM_PEND_BLOCK                       OS_TRACE_TASK_SEM_PEND_BLOCK

#define  TRACE_OS_MUTEX_CREATE                              OS_TRACE_MUTEX_CREATE
#define  TRACE_OS_MUTEX_DEL                                 OS_TRACE_MUTEX_DEL
#define  TRACE_OS_MUTEX_POST                                OS_TRACE_MUTEX_POST
#define  TRACE_OS_MUTEX_POST_FAILED                         OS_TRACE_MUTEX_POST_FAILED
#define  TRACE_OS_MUTEX_PEND                                OS_TRACE_MUTEX_PEND
#define  TRACE_OS_MUTEX_PEND_FAILED                         OS_TRACE_MUTEX_PEND_FAILED
#define  TRACE_OS_MUTEX_PEND_BLOCK                          OS_TRACE_MUTEX_PEND_BLOCK

#define  TRACE_OS_MUTEX_TASK_PRIO_INHERIT                   OS_TRACE_MUTEX_TASK_PRIO_INHERIT
#define  TRACE_OS_MUTEX_TASK_PRIO_DISINHERIT                OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT

#define  TRACE_OS_SEM_CREATE                                OS_TRACE_SEM_CREATE
#define  TRACE_OS_SEM_DEL                                   OS_TRACE_SEM_DEL
#define  TRACE_OS_SEM_POST                                  OS_TRACE_SEM_POST
#define  TRACE_OS_SEM_POST_FAILED                           OS_TRACE_SEM_POST_FAILED
#define  TRACE_OS_SEM_PEND                                  OS_TRACE_SEM_PEND
#define  TRACE_OS_SEM_PEND_FAILED                           OS_TRACE_SEM_PEND_FAILED
#define  TRACE_OS_SEM_PEND_BLOCK                            OS_TRACE_SEM_PEND_BLOCK

#define  TRACE_OS_Q_CREATE                                  OS_TRACE_Q_CREATE
#define  TRACE_OS_Q_DEL                                     OS_TRACE_Q_DEL
#define  TRACE_OS_Q_POST                                    OS_TRACE_Q_POST
#define  TRACE_OS_Q_POST_FAILED                             OS_TRACE_Q_POST_FAILED
#define  TRACE_OS_Q_PEND                                    OS_TRACE_Q_PEND
#define  TRACE_OS_Q_PEND_FAILED                             OS_TRACE_Q_PEND_FAILED
#define  TRACE_OS_Q_PEND_BLOCK                              OS_TRACE_Q_PEND_BLOCK

#define  TRACE_OS_FLAG_CREATE                               OS_TRACE_FLAG_CREATE
#define  TRACE_OS_FLAG_DEL                                  OS_TRACE_FLAG_DEL
#define  TRACE_OS_FLAG_POST                                 OS_TRACE_FLAG_POST
#define  TRACE_OS_FLAG_POST_FAILED                          OS_TRACE_FLAG_POST_FAILED
#define  TRACE_OS_FLAG_PEND                                 OS_TRACE_FLAG_PEND
#define  TRACE_OS_FLAG_PEND_FAILED                          OS_TRACE_FLAG_PEND_FAILED
#define  TRACE_OS_FLAG_PEND_BLOCK                           OS_TRACE_FLAG_PEND_BLOCK

#define  TRACE_OS_MEM_CREATE                                OS_TRACE_MEM_CREATE
#define  TRACE_OS_MEM_PUT                                   OS_TRACE_MEM_PUT
#define  TRACE_OS_MEM_PUT_FAILED                            OS_TRACE_MEM_PUT_FAILED
#define  TRACE_OS_MEM_GET                                   OS_TRACE_MEM_GET
#define  TRACE_OS_MEM_GET_FAILED                            OS_TRACE_MEM_GET_FAILED

#else

#define  TRACE_OS_TICK_INCREMENT(OSTickCtr)

#define  TRACE_OS_TASK_CREATE(p_tcb)
#define  TRACE_OS_TASK_CREATE_FAILED(p_tcb)
#define  TRACE_OS_TASK_DEL(p_tcb)
#define  TRACE_OS_TASK_READY(p_tcb)
#define  TRACE_OS_TASK_SWITCHED_IN(p_tcb)
#define  TRACE_OS_TASK_DLY(dly_ticks)
#define  TRACE_OS_TASK_SUSPEND(p_tcb)
#define  TRACE_OS_TASK_RESUME(p_tcb)

#define  TRACE_OS_ISR_REGISTER(isr_id, isr_name, isr_prio)
#define  TRACE_OS_ISR_BEGIN(isr_id)
#define  TRACE_OS_ISR_END()

#define  TRACE_OS_TASK_MSG_Q_CREATE(p_msg_q, p_name)
#define  TRACE_OS_TASK_MSG_Q_POST(p_msg_q)
#define  TRACE_OS_TASK_MSG_Q_POST_FAILED(p_msg_q)
#define  TRACE_OS_TASK_MSG_Q_PEND(p_msg_q)
#define  TRACE_OS_TASK_MSG_Q_PEND_FAILED(p_msg_q)
#define  TRACE_OS_TASK_MSG_Q_PEND_BLOCK(p_msg_q)

#define  TRACE_OS_TASK_SEM_CREATE(p_tcb, p_name)
#define  TRACE_OS_TASK_SEM_POST(p_tcb)
#define  TRACE_OS_TASK_SEM_POST_FAILED(p_tcb)
#define  TRACE_OS_TASK_SEM_PEND(p_tcb)
#define  TRACE_OS_TASK_SEM_PEND_FAILED(p_tcb)
#define  TRACE_OS_TASK_SEM_PEND_BLOCK(p_tcb)

#define  TRACE_OS_MUTEX_CREATE(p_mutex, p_name)
#define  TRACE_OS_MUTEX_DEL(p_mutex)
#define  TRACE_OS_MUTEX_POST(p_mutex)
#define  TRACE_OS_MUTEX_POST_FAILED(p_mutex)
#define  TRACE_OS_MUTEX_PEND(p_mutex)
#define  TRACE_OS_MUTEX_PEND_FAILED(p_mutex)
#define  TRACE_OS_MUTEX_PEND_BLOCK(p_mutex)

#define  TRACE_OS_MUTEX_TASK_PRIO_INHERIT(p_tcb, prio)
#define  TRACE_OS_MUTEX_TASK_PRIO_DISINHERIT(p_tcb, prio)

#define  TRACE_OS_SEM_CREATE(p_sem, p_name)
#define  TRACE_OS_SEM_DEL(p_sem)
#define  TRACE_OS_SEM_POST(p_sem)
#define  TRACE_OS_SEM_POST_FAILED(p_sem)
#define  TRACE_OS_SEM_PEND(p_sem)
#define  TRACE_OS_SEM_PEND_FAILED(p_sem)
#define  TRACE_OS_SEM_PEND_BLOCK(p_sem)

#define  TRACE_OS_Q_CREATE(p_q, p_name)
#define  TRACE_OS_Q_DEL(p_q)
#define  TRACE_OS_Q_POST(p_q)
#define  TRACE_OS_Q_POST_FAILED(p_q)
#define  TRACE_OS_Q_PEND(p_q)
#define  TRACE_OS_Q_PEND_FAILED(p_q)
#define  TRACE_OS_Q_PEND_BLOCK(p_q)

#define  TRACE_OS_FLAG_CREATE(p_grp, p_name)
#define  TRACE_OS_FLAG_DEL(p_grp)
#define  TRACE_OS_FLAG_POST(p_grp)
#define  TRACE_OS_FLAG_POST_FAILED(p_grp)
#define  TRACE_OS_FLAG_PEND(p_grp)
#define  TRACE_OS_FLAG_PEND_FAILED(p_grp)
#define  TRACE_OS_FLAG_PEND_BLOCK(p_grp)

#define  TRACE_OS_MEM_CREATE(p_mem, p_name)
#define  TRACE_OS_MEM_PUT(p_mem)
#define  TRACE_OS_MEM_PUT_FAILED(p_mem)
#define  TRACE_OS_MEM_GET(p_mem)
#define  TRACE_OS_MEM_GET_FAILED(p_mem)

#endif
