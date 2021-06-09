/*
*********************************************************************************************************
*                                          uC/OS-III Trace
*                                      The RTOS Event Analyzer
*
*                         (c) Copyright 2004-2016; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*                                   Percepio's TraceAlyzer Port
*
* File    : os_trace.h
* Version : V3.06.00
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
* For     : Percepio's Tracealyzer
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "os_trace_events.h"
#include <os.h>


/*
*********************************************************************************************************
*                                         uC/OS-III Trace Macros
*********************************************************************************************************
*/
#if (OS_VERSION >= 30700u)
#if (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))

/* Do nothing. The trace macros are defined by the trace library (trcKernelPort.h). */

#else

/* Define empty trace macros since tracing isn't enabled. */

#define  OS_TRACE_INIT()
#define  OS_TRACE_START()
#define  OS_TRACE_STOP()
#define  OS_TRACE_CLEAR()

/*
*********************************************************************************************************
*                                  uC/OS-III Trace User-Defined Event Macros
*********************************************************************************************************
*/
#define  OS_TRACE_USR_EVT_CREATE(evt_name)
#define  OS_TRACE_USR_EVT_LOG(hnd)
#define  OS_TRACE_PRINTF(hnd, format_str, ...)

/*
*********************************************************************************************************
*                                    uC/OS-III Trace Kernel-Related Macros
*********************************************************************************************************
*/
#define  OS_TRACE_TICK_INCREMENT(OSTickCtr)

#define  OS_TRACE_TASK_CREATE(p_tcb)
#define  OS_TRACE_TASK_CREATE_FAILED(p_tcb)
#define  OS_TRACE_TASK_DEL(p_tcb)
#define  OS_TRACE_TASK_READY(p_tcb)
#define  OS_TRACE_TASK_SWITCHED_IN(p_tcb)
#define  OS_TRACE_TASK_DLY(dly_ticks)
#define  OS_TRACE_TASK_SUSPEND(p_tcb)
#define  OS_TRACE_TASK_RESUME(p_tcb)
#define  OS_TRACE_TASK_PRIO_CHANGE(p_tcb, prio)

#define  OS_TRACE_ISR_REGISTER(isr_id, isr_name, isr_prio)
#define  OS_TRACE_ISR_BEGIN(isr_id)
#define  OS_TRACE_ISR_END(flag)

#define  OS_TRACE_ISR_ENTER()
#define  OS_TRACE_ISR_EXIT()
#define  OS_TRACE_ISR_EXIT_TO_SCHEDULER()

#define  OS_TRACE_TASK_MSG_Q_CREATE(p_msg_q, p_name)
#define  OS_TRACE_TASK_MSG_Q_DEL(p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_POST(p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_POST_FAILED(p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_PEND(p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_PEND_BLOCK(p_msg_q)

#define  OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name)
#define  OS_TRACE_TASK_SEM_DEL(p_tcb)
#define  OS_TRACE_TASK_SEM_POST(p_tcb)
#define  OS_TRACE_TASK_SEM_POST_FAILED(p_tcb)
#define  OS_TRACE_TASK_SEM_PEND(p_tcb)
#define  OS_TRACE_TASK_SEM_PEND_FAILED(p_tcb)
#define  OS_TRACE_TASK_SEM_PEND_BLOCK(p_tcb)

#define  OS_TRACE_MUTEX_CREATE(p_mutex, p_name)
#define  OS_TRACE_MUTEX_DEL(p_mutex)
#define  OS_TRACE_MUTEX_POST(p_mutex)
#define  OS_TRACE_MUTEX_POST_FAILED(p_mutex)
#define  OS_TRACE_MUTEX_PEND(p_mutex)
#define  OS_TRACE_MUTEX_PEND_FAILED(p_mutex)
#define  OS_TRACE_MUTEX_PEND_BLOCK(p_mutex)

#define  OS_TRACE_MUTEX_TASK_PRIO_INHERIT(p_tcb, prio)
#define  OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb, prio)

#define  OS_TRACE_SEM_CREATE(p_sem, p_name)
#define  OS_TRACE_SEM_DEL(p_sem)
#define  OS_TRACE_SEM_POST(p_sem)
#define  OS_TRACE_SEM_POST_FAILED(p_sem)
#define  OS_TRACE_SEM_PEND(p_sem)
#define  OS_TRACE_SEM_PEND_FAILED(p_sem)
#define  OS_TRACE_SEM_PEND_BLOCK(p_sem)

#define  OS_TRACE_Q_CREATE(p_q, p_name)
#define  OS_TRACE_Q_DEL(p_q)
#define  OS_TRACE_Q_POST(p_q)
#define  OS_TRACE_Q_POST_FAILED(p_q)
#define  OS_TRACE_Q_PEND(p_q)
#define  OS_TRACE_Q_PEND_FAILED(p_q)
#define  OS_TRACE_Q_PEND_BLOCK(p_q)

#define  OS_TRACE_FLAG_CREATE(p_grp, p_name)
#define  OS_TRACE_FLAG_DEL(p_grp)
#define  OS_TRACE_FLAG_POST(p_grp)
#define  OS_TRACE_FLAG_POST_FAILED(p_grp)
#define  OS_TRACE_FLAG_PEND(p_grp)
#define  OS_TRACE_FLAG_PEND_FAILED(p_grp)
#define  OS_TRACE_FLAG_PEND_BLOCK(p_grp)

#define  OS_TRACE_MEM_CREATE(p_mem, p_name)
#define  OS_TRACE_MEM_PUT(p_mem)
#define  OS_TRACE_MEM_PUT_FAILED(p_mem)
#define  OS_TRACE_MEM_GET(p_mem)
#define  OS_TRACE_MEM_GET_FAILED(p_mem)

#endif /*(defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))*/

/*
*********************************************************************************************************
*                            uC/OS-III Trace Unused Macros (for other recorders)
*********************************************************************************************************
*/

#define  OS_TRACE_MUTEX_DEL_ENTER(p_mutex, opt)
#define  OS_TRACE_MUTEX_POST_ENTER(p_mutex, opt)
#define  OS_TRACE_MUTEX_PEND_ENTER(p_mutex, timeout, opt, p_ts)
#define  OS_TRACE_TASK_MSG_Q_POST_ENTER(p_msg_q, p_void, msg_size, opt)
#define  OS_TRACE_TASK_MSG_Q_PEND_ENTER(p_msg_q, timeout, opt, p_msg_size, p_ts)
#define  OS_TRACE_TASK_SEM_POST_ENTER(p_tcb, opt)
#define  OS_TRACE_TASK_SEM_PEND_ENTER(p_tcb, timeout, opt, p_ts)
#define  OS_TRACE_SEM_DEL_ENTER(p_sem, opt)
#define  OS_TRACE_SEM_POST_ENTER(p_sem, opt)
#define  OS_TRACE_SEM_PEND_ENTER(p_sem, timeout, opt, p_ts)
#define  OS_TRACE_Q_DEL_ENTER(p_q, opt)
#define  OS_TRACE_Q_POST_ENTER(p_q, p_void, msg_size, opt)
#define  OS_TRACE_Q_PEND_ENTER(p_q, timeout, opt, p_msg_size, p_ts)
#define  OS_TRACE_FLAG_DEL_ENTER(p_grp, opt)
#define  OS_TRACE_FLAG_POST_ENTER(p_grp, flags, opt)
#define  OS_TRACE_FLAG_PEND_ENTER(p_grp, flags, timeout, opt, p_ts)
#define  OS_TRACE_MEM_PUT_ENTER(p_mem, p_blk)
#define  OS_TRACE_MEM_GET_ENTER(p_mem)

#define  OS_TRACE_MUTEX_DEL_EXIT(RetVal)
#define  OS_TRACE_MUTEX_POST_EXIT(RetVal)
#define  OS_TRACE_MUTEX_PEND_EXIT(RetVal)
#define  OS_TRACE_TASK_MSG_Q_POST_EXIT(RetVal)
#define  OS_TRACE_TASK_MSG_Q_PEND_EXIT(RetVal)
#define  OS_TRACE_TASK_SEM_POST_EXIT(RetVal)
#define  OS_TRACE_TASK_SEM_PEND_EXIT(RetVal)
#define  OS_TRACE_SEM_DEL_EXIT(RetVal)
#define  OS_TRACE_SEM_POST_EXIT(RetVal)
#define  OS_TRACE_SEM_PEND_EXIT(RetVal)
#define  OS_TRACE_Q_DEL_EXIT(RetVal)
#define  OS_TRACE_Q_POST_EXIT(RetVal)
#define  OS_TRACE_Q_PEND_EXIT(RetVal)
#define  OS_TRACE_FLAG_DEL_EXIT(RetVal)
#define  OS_TRACE_FLAG_POST_EXIT(RetVal)
#define  OS_TRACE_FLAG_PEND_EXIT(RetVal)
#define  OS_TRACE_MEM_PUT_EXIT(RetVal)
#define  OS_TRACE_MEM_GET_EXIT(RetVal)

#define  OS_TRACE_TASK_PREEMPT(OSTCBCurPtr)
#define  OS_TRACE_TASK_SUSPENDED(p_tcb)

#define OS_TRACE_TASK_RESUME(p_tcb)
#define OS_TRACE_TASK_SUSPEND(p_tcb);

#else

#if (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))

/* Do nothing. The trace macros are defined by the trace library (trcKernelPort.h). */

#else

/* Define empty trace macros since tracing isn't enabled. */

#define  OS_TRACE_INIT()
#define  OS_TRACE_START()
#define  OS_TRACE_STOP()
#define  OS_TRACE_CLEAR()

/*
*********************************************************************************************************
*                                  uC/OS-III Trace User-Defined Event Macros
*********************************************************************************************************
*/
#define  OS_TRACE_USR_EVT_CREATE(evt_name)
#define  OS_TRACE_USR_EVT_LOG(hnd)
#define  OS_TRACE_PRINTF(hnd, format_str, ...)

/*
*********************************************************************************************************
*                                    uC/OS-III Trace Kernel-Related Macros
*********************************************************************************************************
*/
#define  OS_TRACE_TICK_INCREMENT(OSTickCtr)

#define  OS_TRACE_TASK_CREATE(p_tcb)
#define  OS_TRACE_TASK_CREATE_FAILED(p_tcb)
#define  OS_TRACE_TASK_DEL(p_tcb)
#define  OS_TRACE_TASK_READY(p_tcb)
#define  OS_TRACE_TASK_SWITCHED_IN(p_tcb)
#define  OS_TRACE_TASK_DLY(dly_ticks)
#define  OS_TRACE_TASK_SUSPEND(p_tcb)
#define  OS_TRACE_TASK_RESUME(p_tcb)
#define  OS_TRACE_TASK_PRIO_CHANGE(p_tcb, prio)

#define  OS_TRACE_ISR_REGISTER(isr_id, isr_name, isr_prio)
#define  OS_TRACE_ISR_BEGIN(isr_id)
#define  OS_TRACE_ISR_END(flag)

#define  OS_TRACE_ISR_ENTER()
#define  OS_TRACE_ISR_EXIT()
#define  OS_TRACE_ISR_EXIT_TO_SCHEDULER()

#define  OS_TRACE_TASK_MSG_Q_CREATE(p_msg_q, p_name)
#define  OS_TRACE_TASK_MSG_Q_DEL(p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_POST(p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_POST_FAILED(p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_PEND(p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_PEND_FAILED(p_msg_q)
#define  OS_TRACE_TASK_MSG_Q_PEND_BLOCK(p_msg_q)

#define  OS_TRACE_TASK_SEM_CREATE(p_tcb, p_name)
#define  OS_TRACE_TASK_SEM_DEL(p_tcb)
#define  OS_TRACE_TASK_SEM_POST(p_tcb)
#define  OS_TRACE_TASK_SEM_POST_FAILED(p_tcb)
#define  OS_TRACE_TASK_SEM_PEND(p_tcb)
#define  OS_TRACE_TASK_SEM_PEND_FAILED(p_tcb)
#define  OS_TRACE_TASK_SEM_PEND_BLOCK(p_tcb)

#define  OS_TRACE_MUTEX_CREATE(p_mutex, p_name)
#define  OS_TRACE_MUTEX_DEL(p_mutex)
#define  OS_TRACE_MUTEX_POST(p_mutex)
#define  OS_TRACE_MUTEX_POST_FAILED(p_mutex)
#define  OS_TRACE_MUTEX_PEND(p_mutex)
#define  OS_TRACE_MUTEX_PEND_FAILED(p_mutex)
#define  OS_TRACE_MUTEX_PEND_BLOCK(p_mutex)

#define  OS_TRACE_MUTEX_TASK_PRIO_INHERIT(p_tcb, prio)
#define  OS_TRACE_MUTEX_TASK_PRIO_DISINHERIT(p_tcb, prio)

#define  OS_TRACE_SEM_CREATE(p_sem, p_name)
#define  OS_TRACE_SEM_DEL(p_sem)
#define  OS_TRACE_SEM_POST(p_sem)
#define  OS_TRACE_SEM_POST_FAILED(p_sem)
#define  OS_TRACE_SEM_PEND(p_sem)
#define  OS_TRACE_SEM_PEND_FAILED(p_sem)
#define  OS_TRACE_SEM_PEND_BLOCK(p_sem)

#define  OS_TRACE_Q_CREATE(p_q, p_name)
#define  OS_TRACE_Q_DEL(p_q)
#define  OS_TRACE_Q_POST(p_q)
#define  OS_TRACE_Q_POST_FAILED(p_q)
#define  OS_TRACE_Q_PEND(p_q)
#define  OS_TRACE_Q_PEND_FAILED(p_q)
#define  OS_TRACE_Q_PEND_BLOCK(p_q)

#define  OS_TRACE_FLAG_CREATE(p_grp, p_name)
#define  OS_TRACE_FLAG_DEL(p_grp)
#define  OS_TRACE_FLAG_POST(p_grp)
#define  OS_TRACE_FLAG_POST_FAILED(p_grp)
#define  OS_TRACE_FLAG_PEND(p_grp)
#define  OS_TRACE_FLAG_PEND_FAILED(p_grp)
#define  OS_TRACE_FLAG_PEND_BLOCK(p_grp)

#define  OS_TRACE_MEM_CREATE(p_mem, p_name)
#define  OS_TRACE_MEM_PUT(p_mem)
#define  OS_TRACE_MEM_PUT_FAILED(p_mem)
#define  OS_TRACE_MEM_GET(p_mem)
#define  OS_TRACE_MEM_GET_FAILED(p_mem)

#endif /* (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u)) */

/*
*********************************************************************************************************
*                            uC/OS-III Trace Unused Macros (for other recorders)
*********************************************************************************************************
*/

#define  OS_TRACE_MUTEX_DEL_ENTER(p_mutex, opt, p_err)
#define  OS_TRACE_MUTEX_POST_ENTER(p_mutex, opt, p_err)
#define  OS_TRACE_MUTEX_PEND_ENTER(p_mutex, timeout, opt, p_ts, p_err)
#define  OS_TRACE_TASK_MSG_Q_POST_ENTER(p_msg_q, p_void, msg_size, opt, p_err)
#define  OS_TRACE_TASK_MSG_Q_PEND_ENTER(p_msg_q, timeout, opt, p_msg_size, p_ts, p_err)
#define  OS_TRACE_TASK_SEM_POST_ENTER(p_tcb, opt, p_err)
#define  OS_TRACE_TASK_SEM_PEND_ENTER(p_tcb, timeout, opt, p_ts, p_err)
#define  OS_TRACE_SEM_DEL_ENTER(p_sem, opt, p_err)
#define  OS_TRACE_SEM_POST_ENTER(p_sem, opt, p_err)
#define  OS_TRACE_SEM_PEND_ENTER(p_sem, timeout, opt, p_ts, p_err)
#define  OS_TRACE_Q_DEL_ENTER(p_q, opt, p_err)
#define  OS_TRACE_Q_POST_ENTER(p_q, p_void, msg_size, opt, p_err)
#define  OS_TRACE_Q_PEND_ENTER(p_q, timeout, opt, p_msg_size, p_ts, p_err)
#define  OS_TRACE_FLAG_DEL_ENTER(p_grp, opt, p_err)
#define  OS_TRACE_FLAG_POST_ENTER(p_grp, flags, opt, p_err)
#define  OS_TRACE_FLAG_PEND_ENTER(p_grp, flags, timeout, opt, p_ts, p_err)
#define  OS_TRACE_MEM_PUT_ENTER(p_mem, p_blk, p_err)
#define  OS_TRACE_MEM_GET_ENTER(p_mem, p_err)

#define  OS_TRACE_MUTEX_DEL_EXIT(RetVal)
#define  OS_TRACE_MUTEX_POST_EXIT(RetVal)
#define  OS_TRACE_MUTEX_PEND_EXIT(RetVal)
#define  OS_TRACE_TASK_MSG_Q_POST_EXIT(RetVal)
#define  OS_TRACE_TASK_MSG_Q_PEND_EXIT(RetVal)
#define  OS_TRACE_TASK_SEM_POST_EXIT(RetVal)
#define  OS_TRACE_TASK_SEM_PEND_EXIT(RetVal)
#define  OS_TRACE_SEM_DEL_EXIT(RetVal)
#define  OS_TRACE_SEM_POST_EXIT(RetVal)
#define  OS_TRACE_SEM_PEND_EXIT(RetVal)
#define  OS_TRACE_Q_DEL_EXIT(RetVal)
#define  OS_TRACE_Q_POST_EXIT(RetVal)
#define  OS_TRACE_Q_PEND_EXIT(RetVal)
#define  OS_TRACE_FLAG_DEL_EXIT(RetVal)
#define  OS_TRACE_FLAG_POST_EXIT(RetVal)
#define  OS_TRACE_FLAG_PEND_EXIT(RetVal)
#define  OS_TRACE_MEM_PUT_EXIT(RetVal)
#define  OS_TRACE_MEM_GET_EXIT(RetVal)

#endif /* (OS_VERISON >= 30700u)*/
