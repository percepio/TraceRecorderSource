/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _TRACE_TRACERECORDER_H
#define _TRACE_TRACERECORDER_H

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <trcRecorder.h>


/* Legacy trace defines that are pending refactoring/removal by
 * the Zephyr team.
 */
#undef sys_trace_isr_enter
#undef sys_trace_isr_exit
#undef sys_trace_isr_exit_to_scheduler
#undef sys_trace_idle


/* Thread trace mappings */
#undef sys_port_trace_k_thread_foreach_enter
#define sys_port_trace_k_thread_foreach_enter(...)                                  \
    sys_trace_k_thread_foreach_enter(user_cb, user_data)
#undef sys_port_trace_k_thread_foreach_exit
#define sys_port_trace_k_thread_foreach_exit(...)                                   \
    sys_trace_k_thread_foreach_exit(user_cb, user_data)
#undef sys_port_trace_k_thread_foreach_unlocked_enter
#define sys_port_trace_k_thread_foreach_unlocked_enter(...)                         \
    sys_trace_k_thread_foreach_unlocked_enter(user_cb, user_data)
#undef sys_port_trace_k_thread_foreach_unlocked_exit
#define sys_port_trace_k_thread_foreach_unlocked_exit(...)                          \
    sys_trace_k_thread_foreach_unlocked_exit(user_cb, user_data)
#undef sys_port_trace_k_thread_create
#define sys_port_trace_k_thread_create(new_thread, ...)                             \
    sys_trace_k_thread_create(new_thread, stack_size, prio)
#undef sys_port_trace_k_thread_user_mode_enter
#define sys_port_trace_k_thread_user_mode_enter(...)                                \
    sys_trace_k_thread_user_mode_enter(entry, p1, p2, p3)
#undef sys_port_trace_k_thread_heap_assign
#define sys_port_trace_k_thread_heap_assign(thread, heap, ...)                      \
    sys_trace_k_thread_heap_assign(thread, heap)
#undef sys_port_trace_k_thread_join_enter
#define sys_port_trace_k_thread_join_enter(thread, timeout, ...)     
#undef sys_port_trace_k_thread_join_blocking
#define sys_port_trace_k_thread_join_blocking(thread, timeout, ...)                 \
    sys_trace_k_thread_join_blocking(thread, timeout)
#undef sys_port_trace_k_thread_join_exit
#define sys_port_trace_k_thread_join_exit(thread, timeout, ret, ...)                \
    sys_trace_k_thread_join_exit(thread, timeout, ret)
#undef sys_port_trace_k_thread_sleep_enter
#define sys_port_trace_k_thread_sleep_enter(timeout, ...)                           \
    sys_trace_k_thread_sleep_enter(timeout)
#undef sys_port_trace_k_thread_sleep_exit
#define sys_port_trace_k_thread_sleep_exit(timeout, ret, ...)                       \
    sys_trace_k_thread_sleep_exit(timeout, ret)
#undef sys_port_trace_k_thread_msleep_enter
#define sys_port_trace_k_thread_msleep_enter(ms, ...)                               \
    sys_trace_k_thread_msleep_enter(ms)
#undef sys_port_trace_k_thread_msleep_exit
#define sys_port_trace_k_thread_msleep_exit(ms, ret, ...)                           \
    sys_trace_k_thread_msleep_exit(ms, ret)
#undef sys_port_trace_k_thread_usleep_enter
#define sys_port_trace_k_thread_usleep_enter(us, ...)                               \
    sys_trace_k_thread_usleep_enter(us)
#undef sys_port_trace_k_thread_usleep_exit
#define sys_port_trace_k_thread_usleep_exit(us, ret, ...)                           \
    sys_trace_k_thread_usleep_exit(us, ret)
#undef sys_port_trace_k_thread_busy_wait_enter
#define sys_port_trace_k_thread_busy_wait_enter(usec_to_wait, ...)                  \
    sys_trace_k_thread_busy_wait_enter(usec_to_wait)
#undef sys_port_trace_k_thread_busy_wait_exit
#define sys_port_trace_k_thread_busy_wait_exit(usec_to_wait, ...)                   \
    sys_trace_k_thread_busy_wait_exit(usec_to_wait)
#undef sys_port_trace_k_thread_yield
#define sys_port_trace_k_thread_yield(...)                                          \
    sys_trace_k_thread_yield()
#undef sys_port_trace_k_thread_wakeup
#define sys_port_trace_k_thread_wakeup(thread, ...)                                 \
    sys_trace_k_thread_wakeup(thread)
#undef sys_port_trace_k_thread_start
#define sys_port_trace_k_thread_start(thread, ...)                                  \
    sys_trace_k_thread_start(thread)
#undef sys_port_trace_k_thread_abort
#define sys_port_trace_k_thread_abort(thread, ...)                                  \
    sys_trace_k_thread_abort(thread)
#undef sys_port_trace_k_thread_priority_set
#define sys_port_trace_k_thread_priority_set(thread, ...)                           \
    sys_trace_k_thread_priority_set(thread)
#undef sys_port_trace_k_thread_suspend_enter
#define sys_port_trace_k_thread_suspend_enter(thread, ...)                          \
    sys_trace_k_thread_suspend(thread)
#undef sys_port_trace_k_thread_suspend_exit
#define sys_port_trace_k_thread_suspend_exit(thread, ...)    
#undef sys_port_trace_k_thread_resume_enter
#define sys_port_trace_k_thread_resume_enter(thread, ...)                           \
    sys_trace_k_thread_resume(thread)
#undef sys_port_trace_k_thread_resume_exit
#define sys_port_trace_k_thread_resume_exit(thread, ...)
#undef sys_port_trace_k_thread_name_set
#define sys_port_trace_k_thread_name_set(thread, ret, ...)                          \
    sys_trace_k_thread_name_set(thread, ret)
#undef sys_port_trace_k_thread_switched_out
#define sys_port_trace_k_thread_switched_out(...)                                   \
    sys_trace_k_thread_switched_out()
#undef sys_port_trace_k_thread_switched_in
#define sys_port_trace_k_thread_switched_in(...)                                    \
    sys_trace_k_thread_switched_in()
#undef sys_port_trace_k_thread_info
#define sys_port_trace_k_thread_info(thread, ...)                                   \
    sys_trace_k_thread_info(thread)


/* Thread scheduler trace mappings */
#undef sys_port_trace_k_thread_sched_lock
#define sys_port_trace_k_thread_sched_lock(...)                                     \
    sys_trace_k_thread_sched_lock()
#undef sys_port_trace_k_thread_sched_unlock
#define sys_port_trace_k_thread_sched_unlock(...)                                   \
    sys_trace_k_thread_sched_unlock()
#undef sys_port_trace_k_thread_sched_wakeup
#define sys_port_trace_k_thread_sched_wakeup(thread, ...)                           \
    sys_trace_k_thread_sched_wakeup(thread)
#undef sys_port_trace_k_thread_sched_abort
#define sys_port_trace_k_thread_sched_abort(thread, ...)                            \
    sys_trace_k_thread_sched_abort(thread)
#undef sys_port_trace_k_thread_sched_priority_set
#define sys_port_trace_k_thread_sched_priority_set(thread, prio, ...)               \
    sys_trace_k_thread_sched_set_priority(thread, prio)
#undef sys_port_trace_k_thread_sched_ready
#define sys_port_trace_k_thread_sched_ready(thread, ...)                            \
    sys_trace_k_thread_sched_ready(thread)
#undef sys_port_trace_k_thread_sched_pend
#define sys_port_trace_k_thread_sched_pend(thread, ...)                             \
    sys_trace_k_thread_sched_pend(thread)
#undef sys_port_trace_k_thread_sched_resume
#define sys_port_trace_k_thread_sched_resume(thread, ...)                           \
    sys_trace_k_thread_sched_resume(thread)
#undef sys_port_trace_k_thread_sched_suspend
#define sys_port_trace_k_thread_sched_suspend(thread, ...)                          \
    sys_trace_k_thread_sched_suspend(thread)


/* Work trace mappings */
#undef sys_port_trace_k_work_init
#define sys_port_trace_k_work_init(work, ...)                                       \
    sys_trace_k_work_init(work, handler)
#undef sys_port_trace_k_work_submit_to_queue_enter
#define sys_port_trace_k_work_submit_to_queue_enter(queue, work, ...)               \
    sys_trace_k_work_submit_to_queue_enter(queue, work)
#undef sys_port_trace_k_work_submit_to_queue_exit
#define sys_port_trace_k_work_submit_to_queue_exit(queue, work, ret, ...)           \
    sys_trace_k_work_submit_to_queue_exit(queue, work, ret)
#undef sys_port_trace_k_work_submit_enter
#define sys_port_trace_k_work_submit_enter(work, ...)                               \
    sys_trace_k_work_submit_enter(work)
#undef sys_port_trace_k_work_submit_exit
#define sys_port_trace_k_work_submit_exit(work, ret, ...)                           \
    sys_trace_k_work_submit_exit(work, ret)
#undef sys_port_trace_k_work_flush_enter
#define sys_port_trace_k_work_flush_enter(work, ...)                                \
    sys_trace_k_work_flush_enter(work, sync)
#undef sys_port_trace_k_work_flush_blocking
#define sys_port_trace_k_work_flush_blocking(work, timeout, ...)                    \
    sys_trace_k_work_flush_blocking(work, sync, timeout)
#undef sys_port_trace_k_work_flush_exit
#define sys_port_trace_k_work_flush_exit(work, ret, ...)                            \
    sys_trace_k_work_flush_exit(work, sync, ret)
#undef sys_port_trace_k_work_cancel_enter
#define sys_port_trace_k_work_cancel_enter(work, ...)                               \
    sys_trace_k_work_cancel_enter(work)
#undef sys_port_trace_k_work_cancel_exit
#define sys_port_trace_k_work_cancel_exit(work, ret, ...)                           \
    sys_trace_k_work_cancel_exit(work, ret)
#undef sys_port_trace_k_work_cancel_sync_enter
#define sys_port_trace_k_work_cancel_sync_enter(work, sync, ...)                    \
    sys_trace_k_work_cancel_sync_enter(work, sync)
#undef sys_port_trace_k_work_cancel_sync_blocking
#define sys_port_trace_k_work_cancel_sync_blocking(work, sync, ...)                 \
    sys_trace_k_work_cancel_sync_blocking(work, sync)
#undef sys_port_trace_k_work_cancel_sync_exit
#define sys_port_trace_k_work_cancel_sync_exit(work, sync, ret, ...)                \
    sys_trace_k_work_cancel_sync_exit(work, sync, ret)


/* Work queue trace mappings */
#undef sys_port_trace_k_work_queue_start_enter
#define sys_port_trace_k_work_queue_start_enter(queue, ...)                         \
    sys_trace_k_work_queue_start_enter(queue, stack, stack_size, prio, cfg)
#undef sys_port_trace_k_work_queue_start_exit
#define sys_port_trace_k_work_queue_start_exit(queue, ...)                          \
    sys_trace_k_work_queue_start_exit(queue, stack, stack_size, prio, cfg)
#undef sys_port_trace_k_work_queue_drain_enter
#define sys_port_trace_k_work_queue_drain_enter(queue, ...)                         \
    sys_trace_k_work_queue_drain_enter(queue, plug)
#undef sys_port_trace_k_work_queue_drain_exit
#define sys_port_trace_k_work_queue_drain_exit(queue, ret, ...)                     \
    sys_trace_k_work_queue_drain_exit(queue, plug, ret)
#undef sys_port_trace_k_work_queue_unplug_enter
#define sys_port_trace_k_work_queue_unplug_enter(queue, ...)                        \
    sys_trace_k_work_queue_unplug_enter(queue)
#undef sys_port_trace_k_work_queue_unplug_exit
#define sys_port_trace_k_work_queue_unplug_exit(queue, ret, ...)                    \
    sys_trace_k_work_queue_unplug_exit(queue, ret)


/* Work delayable trace mappings */
#undef sys_port_trace_k_work_delayable_init
#define sys_port_trace_k_work_delayable_init(dwork, ...)                            \
    sys_trace_k_work_delayable_init(dwork, handler)
#undef sys_port_trace_k_work_schedule_for_queue_enter
#define sys_port_trace_k_work_schedule_for_queue_enter(queue, dwork, delay, ...)    \
    sys_trace_k_work_schedule_for_queue_enter(queue, dwork, delay)
#undef sys_port_trace_k_work_schedule_for_queue_exit
#define sys_port_trace_k_work_schedule_for_queue_exit(queue, dwork, delay,          \
    ret, ...)                                                                       \
    sys_trace_k_work_schedule_for_queue_exit(queue, dwork, delay, ret)
#undef sys_port_trace_k_work_schedule_enter
#define sys_port_trace_k_work_schedule_enter(dwork, delay, ...)                     \
    sys_trace_k_work_schedule_enter(dwork, delay)
#undef sys_port_trace_k_work_schedule_exit
#define sys_port_trace_k_work_schedule_exit(dwork, delay, ret, ...)                 \
    sys_trace_k_work_schedule_exit(dwork, delay, ret)
#undef sys_port_trace_k_work_reschedule_for_queue_enter
#define sys_port_trace_k_work_reschedule_for_queue_enter(queue, dwork, delay, ...)  \
    sys_trace_k_work_reschedule_for_queue_enter(queue, dwork, delay)
#undef sys_port_trace_k_work_reschedule_for_queue_exit
#define sys_port_trace_k_work_reschedule_for_queue_exit(queue, dwork, delay,        \
    ret, ...)                                                                       \
    sys_trace_k_work_reschedule_for_queue_exit(queue, dwork, delay, ret)
#undef sys_port_trace_k_work_reschedule_enter
#define sys_port_trace_k_work_reschedule_enter(dwork, delay, ...)                   \
    sys_trace_k_work_reschedule_enter(dwork, delay)
#undef sys_port_trace_k_work_reschedule_exit
#define sys_port_trace_k_work_reschedule_exit(dwork, delay, ret, ...)               \
    sys_trace_k_work_reschedule_exit(dwork, delay, ret)
#undef sys_port_trace_k_work_flush_delayable_enter
#define sys_port_trace_k_work_flush_delayable_enter(dwork, sync, ...)               \
    sys_trace_k_work_flush_delayable_enter(dwork, sync)
#undef sys_port_trace_k_work_flush_delayable_exit
#define sys_port_trace_k_work_flush_delayable_exit(dwork, sync, ret, ...)           \
    sys_trace_k_work_flush_delayable_exit(dwork, sync, ret)
#undef sys_port_trace_k_work_cancel_delayable_enter
#define sys_port_trace_k_work_cancel_delayable_enter(dwork, ...)                    \
    sys_trace_k_work_cancel_delayable_enter(dwork)
#undef sys_port_trace_k_work_cancel_delayable_exit
#define sys_port_trace_k_work_cancel_delayable_exit(dwork, ret, ...)                \
    sys_trace_k_work_cancel_delayable_exit(dwork, ret)
#undef sys_port_trace_k_work_cancel_delayable_sync_enter
#define sys_port_trace_k_work_cancel_delayable_sync_enter(dwork, sync, ...)         \
    sys_trace_cancel_delayable_sync_enter(dwork, sync)
#undef sys_port_trace_k_work_cancel_delayable_sync_exit
#define sys_port_trace_k_work_cancel_delayable_sync_exit(dwork, sync, ret, ...)     \
    sys_trace_cancel_delayable_sync_exit(dwork, sync, ret)


/* Work poll trace mappings */
#undef sys_port_trace_k_work_poll_init_enter
#define sys_port_trace_k_work_poll_init_enter(work, ...)                            \
    sys_trace_k_work_poll_init_enter(work, handler)
#undef sys_port_trace_k_work_poll_init_exit
#define sys_port_trace_k_work_poll_init_exit(work, ...)                             \
    sys_trace_k_work_poll_init_exit(work, handler)
#undef sys_port_trace_k_work_poll_submit_to_queue_enter
#define sys_port_trace_k_work_poll_submit_to_queue_enter(work_q, work,              \
    timeout, ...)                                                                   \
    sys_trace_k_work_poll_submit_to_queue_enter(work_q, work, events,               \
        num_events, timeout)
#undef sys_port_trace_k_work_poll_submit_to_queue_blocking
#define sys_port_trace_k_work_poll_submit_to_queue_blocking(work_q, work,           \
    timeout, ...)                                                                   \
    sys_trace_k_work_poll_submit_to_queue_blocking(work_q, work, events,            \
        num_events, timeout)
#undef sys_port_trace_k_work_poll_submit_to_queue_exit
#define sys_port_trace_k_work_poll_submit_to_queue_exit(work_q, work, timeout,      \
    ret, ...)                                                                       \
    sys_trace_k_work_poll_submit_to_queue_exit(work_q, work, events,                \
        num_events, timeout, ret)
#undef sys_port_trace_k_work_poll_submit_enter
#define sys_port_trace_k_work_poll_submit_enter(work, timeout, ...)                 \
    sys_trace_k_work_poll_submit_enter(work, events, num_events, timeout)
#undef sys_port_trace_k_work_poll_submit_exit
#define sys_port_trace_k_work_poll_submit_exit(work, timeout, ret, ...)             \
    sys_trace_k_work_poll_submit_exit(work, events, num_events, timeout, ret)
#undef sys_port_trace_k_work_poll_cancel_enter
#define sys_port_trace_k_work_poll_cancel_enter(work, ...)                          \
    sys_trace_k_work_poll_cancel_enter(work)
#undef sys_port_trace_k_work_poll_cancel_exit
#define sys_port_trace_k_work_poll_cancel_exit(work, ret, ...)                      \
    sys_trace_k_work_poll_cancel_exit(work, ret)


/* Poll API trace mappings */
#undef sys_port_trace_k_poll_api_event_init
#define sys_port_trace_k_poll_api_event_init(event, ...)                            \
    sys_trace_k_poll_api_event_init(event, type, mode, obj)
#undef sys_port_trace_k_poll_api_poll_enter
#define sys_port_trace_k_poll_api_poll_enter(events, ...)                           \
    sys_trace_k_poll_api_event_poll_enter(events, num_events, timeout)
#undef sys_port_trace_k_poll_api_poll_exit
#define sys_port_trace_k_poll_api_poll_exit(events, ret, ...)                       \
    sys_trace_k_poll_api_event_poll_exit(events, num_events, timeout, ret)
#undef sys_port_trace_k_poll_api_signal_init
#define sys_port_trace_k_poll_api_signal_init(signal, ...)                          \
    sys_trace_k_poll_api_signal_init(signal)
#undef sys_port_trace_k_poll_api_signal_reset
#define sys_port_trace_k_poll_api_signal_reset(signal, ...)                         \
    sys_trace_k_poll_api_signal_reset(signal)
#undef sys_port_trace_k_poll_api_signal_check
#define sys_port_trace_k_poll_api_signal_check(signal, ...)                         \
    sys_trace_k_poll_api_signal_check(signal, signaled,result)
#undef sys_port_trace_k_poll_api_signal_raise
#define sys_port_trace_k_poll_api_signal_raise(signal, ret, ...)                    \
    sys_trace_k_poll_api_signal_raise(signal, result, ret)


/* Semaphore trace mappings */
#undef sys_port_trace_k_sem_init
#define sys_port_trace_k_sem_init(sem, ret, ...)                                    \
    sys_trace_k_sem_init(sem, initial_count, limit, ret)
#undef sys_port_trace_k_sem_give_enter
#define sys_port_trace_k_sem_give_enter(sem, ...)                                   \
    sys_trace_k_sem_give_enter(sem)
#undef sys_port_trace_k_sem_give_exit
#define sys_port_trace_k_sem_give_exit(sem, ...)
#undef sys_port_trace_k_sem_take_enter
#define sys_port_trace_k_sem_take_enter(sem, timeout, ...)                          \
    sys_trace_k_sem_take_enter(sem, timeout)
#undef sys_port_trace_k_sem_take_blocking
#define sys_port_trace_k_sem_take_blocking(sem, timeout, ...)                       \
    sys_trace_k_sem_take_blocking(sem, timeout)
#undef sys_port_trace_k_sem_take_exit
#define sys_port_trace_k_sem_take_exit(sem, timeout, ret, ...)                      \
    sys_trace_k_sem_take_exit(sem, timeout, ret)
#undef sys_port_trace_k_sem_reset
#define sys_port_trace_k_sem_reset(sem, ...)                                        \
    sys_trace_k_sem_reset(sem)


/* Mutex trace mappings */
#undef sys_port_trace_k_mutex_init
#define sys_port_trace_k_mutex_init(mutex, ret, ...)                                \
    sys_trace_k_mutex_init(mutex, ret)
#undef sys_port_trace_k_mutex_lock_enter
#define sys_port_trace_k_mutex_lock_enter(mutex, timeout, ...)                      \
    sys_trace_k_mutex_lock_enter(mutex, timeout)
#undef sys_port_trace_k_mutex_lock_blocking
#define sys_port_trace_k_mutex_lock_blocking(mutex, timeout, ...)                   \
    sys_trace_k_mutex_lock_blocking(mutex, timeout)
#undef sys_port_trace_k_mutex_lock_exit
#define sys_port_trace_k_mutex_lock_exit(mutex, timeout, ret, ...)                  \
    sys_trace_k_mutex_lock_exit(mutex, timeout, ret)
#undef sys_port_trace_k_mutex_unlock_enter
#define sys_port_trace_k_mutex_unlock_enter(mutex, ...)                             \
    sys_trace_k_mutex_unlock_enter(mutex)
#undef sys_port_trace_k_mutex_unlock_exit
#define sys_port_trace_k_mutex_unlock_exit(mutex, ret, ...)                         \
    sys_trace_k_mutex_unlock_exit(mutex, ret)


/* Conditional variable trace mappings */
#undef sys_port_trace_k_condvar_init
#define sys_port_trace_k_condvar_init(condvar, ret, ...)                            \
    sys_trace_k_condvar_init(condvar, ret)
#undef sys_port_trace_k_condvar_signal_enter
#define sys_port_trace_k_condvar_signal_enter(condvar, ...)                         \
    sys_trace_k_condvar_signal_enter(condvar)
#undef sys_port_trace_k_condvar_signal_blocking
#define sys_port_trace_k_condvar_signal_blocking(condvar, timeout, ...)             \
    sys_trace_k_condvar_signal_blocking(condvar)
#undef sys_port_trace_k_condvar_signal_exit
#define sys_port_trace_k_condvar_signal_exit(condvar, ret, ...)                     \
    sys_trace_k_condvar_signal_exit(condvar, ret)
#undef sys_port_trace_k_condvar_broadcast_enter
#define sys_port_trace_k_condvar_broadcast_enter(condvar, ...)                      \
    sys_trace_k_condvar_broadcast_enter(condvar)
#undef sys_port_trace_k_condvar_broadcast_exit
#define sys_port_trace_k_condvar_broadcast_exit(condvar, ret, ...)                  \
    sys_trace_k_condvar_broadcast_exit(condvar, ret)
#undef sys_port_trace_k_condvar_wait_enter
#define sys_port_trace_k_condvar_wait_enter(condvar, ...)                           \
    sys_trace_k_condvar_wait_enter(condvar, mutex, timeout)
#undef sys_port_trace_k_condvar_wait_exit
#define sys_port_trace_k_condvar_wait_exit(condvar, ret, ...)                       \
    sys_trace_k_condvar_wait_exit(condvar, mutex, timeout, ret)


/* Queue trace mappings */
#undef sys_port_trace_k_queue_init
#define sys_port_trace_k_queue_init(queue, ...)                                     \
    sys_trace_k_queue_init(queue)
#undef sys_port_trace_k_queue_cancel_wait
#define sys_port_trace_k_queue_cancel_wait(queue, ...)                              \
    sys_trace_k_queue_cancel_wait(queue)
#undef sys_port_trace_k_queue_queue_insert_enter
#define sys_port_trace_k_queue_queue_insert_enter(queue, alloc, ...)                \
    sys_trace_k_queue_queue_insert_enter(queue, alloc, data)
#undef sys_port_trace_k_queue_queue_insert_blocking
#define sys_port_trace_k_queue_queue_insert_blocking(queue, alloc, timeout, ...)    \
    sys_trace_k_queue_queue_insert_blocking(queue, alloc, data)
#undef sys_port_trace_k_queue_queue_insert_exit
#define sys_port_trace_k_queue_queue_insert_exit(queue, alloc, ret, ...)            \
    sys_trace_k_queue_queue_insert_exit(queue, alloc, data, ret);
#undef sys_port_trace_k_queue_append_enter
#define sys_port_trace_k_queue_append_enter(queue, ...)                             \
    sys_trace_k_queue_append_enter(queue, data)
#undef sys_port_trace_k_queue_append_exit
#define sys_port_trace_k_queue_append_exit(queue, ...)                              \
    sys_trace_k_queue_append_exit(queue, data)
#undef sys_port_trace_k_queue_alloc_append_enter
#define sys_port_trace_k_queue_alloc_append_enter(queue, ...)                       \
    sys_trace_k_queue_alloc_append_enter(queue, data)
#undef sys_port_trace_k_queue_alloc_append_exit
#define sys_port_trace_k_queue_alloc_append_exit(queue, ret, ...)                   \
    sys_trace_k_queue_alloc_append_exit(queue, data, ret)
#undef sys_port_trace_k_queue_prepend_enter
#define sys_port_trace_k_queue_prepend_enter(queue, ...)                            \
    sys_trace_k_queue_prepend_enter(queue, data)
#undef sys_port_trace_k_queue_prepend_exit
#define sys_port_trace_k_queue_prepend_exit(queue, ...)                             \
    sys_trace_k_queue_prepend_exit(queue, data)
#undef sys_port_trace_k_queue_alloc_prepend_enter
#define sys_port_trace_k_queue_alloc_prepend_enter(queue, ...)                      \
    sys_trace_k_queue_alloc_prepend_enter(queue, data)
#undef sys_port_trace_k_queue_alloc_prepend_exit
#define sys_port_trace_k_queue_alloc_prepend_exit(queue, ret, ...)                  \
    sys_trace_k_queue_alloc_prepend_exit(queue, data, ret)
#undef sys_port_trace_k_queue_insert_enter
#define sys_port_trace_k_queue_insert_enter(queue, ...)                             \
    sys_trace_k_queue_insert_enter(queue, prev, data)
#undef sys_port_trace_k_queue_insert_blocking
#define sys_port_trace_k_queue_insert_blocking(queue, timeout, ...)                 \
    sys_trace_k_queue_insert_blocking(queue, prev, data)
#undef sys_port_trace_k_queue_insert_exit
#define sys_port_trace_k_queue_insert_exit(queue, ...)                              \
    sys_trace_k_queue_insert_exit(queue, prev, data)
#undef sys_port_trace_k_queue_append_list_enter
#define sys_port_trace_k_queue_append_list_enter(queue, ...)                        \
    sys_trace_k_queue_append_list_enter(queue, head, tail)
#undef sys_port_trace_k_queue_append_list_exit
#define sys_port_trace_k_queue_append_list_exit(queue, ret, ...)                    \
    sys_trace_k_queue_append_list_exit(queue, ret)
#undef sys_port_trace_k_queue_merge_slist_enter
#define sys_port_trace_k_queue_merge_slist_enter(queue, ...)                        \
    sys_trace_k_queue_merge_slist_enter(queue, list)
#undef sys_port_trace_k_queue_merge_slist_exit
#define sys_port_trace_k_queue_merge_slist_exit(queue, ret, ...)                    \
    sys_trace_k_queue_merge_slist_exit(queue, list, ret)
#undef sys_port_trace_k_queue_get_enter
#define sys_port_trace_k_queue_get_enter(queue, timeout, ...)        
#undef sys_port_trace_k_queue_get_blocking
#define sys_port_trace_k_queue_get_blocking(queue, timeout, ...)                    \
    sys_trace_k_queue_get_blocking(queue, timeout)
#undef sys_port_trace_k_queue_get_exit
#define sys_port_trace_k_queue_get_exit(queue, timeout, ret, ...)                   \
    sys_trace_k_queue_get_exit(queue, timeout, ret)
#undef sys_port_trace_k_queue_remove_enter
#define sys_port_trace_k_queue_remove_enter(queue, ...)                             \
    sys_trace_k_queue_remove_enter(queue, data)
#undef sys_port_trace_k_queue_remove_exit
#define sys_port_trace_k_queue_remove_exit(queue, ret, ...)                         \
    sys_trace_k_queue_remove_exit(queue, data, ret)
#undef sys_port_trace_k_queue_unique_append_enter
#define sys_port_trace_k_queue_unique_append_enter(queue, ...)                      \
    sys_trace_k_queue_unique_append_enter(queue, data)
#undef sys_port_trace_k_queue_unique_append_exit
#define sys_port_trace_k_queue_unique_append_exit(queue, ret, ...)                  \
    sys_trace_k_queue_unique_append_exit(queue, data, ret)
#undef sys_port_trace_k_queue_peek_head
#define sys_port_trace_k_queue_peek_head(queue, ret, ...)                           \
    sys_trace_k_queue_peek_head(queue, ret)
#undef sys_port_trace_k_queue_peek_tail
#define sys_port_trace_k_queue_peek_tail(queue, ret, ...)                           \
    sys_trace_k_queue_peek_tail(queue, ret)


/* FIFO trace mappings */
#undef sys_port_trace_k_fifo_init_enter
#define sys_port_trace_k_fifo_init_enter(fifo, ...)                                 \
    sys_trace_k_fifo_init_enter(fifo)
#undef sys_port_trace_k_fifo_init_exit
#define sys_port_trace_k_fifo_init_exit(fifo, ...)                                  \
    sys_trace_k_fifo_init_exit(fifo)
#undef sys_port_trace_k_fifo_cancel_wait_enter
#define sys_port_trace_k_fifo_cancel_wait_enter(fifo, ...)                          \
    sys_trace_k_fifo_cancel_wait_enter(fifo)
#undef sys_port_trace_k_fifo_cancel_wait_exit
#define sys_port_trace_k_fifo_cancel_wait_exit(fifo, ...)                           \
    sys_trace_k_fifo_cancel_wait_exit(fifo)
#undef sys_port_trace_k_fifo_put_enter
#define sys_port_trace_k_fifo_put_enter(fifo, data, ...)                            \
    sys_trace_k_fifo_put_enter(fifo, data)
#undef sys_port_trace_k_fifo_put_exit
#define sys_port_trace_k_fifo_put_exit(fifo, data, ...)                             \
    sys_trace_k_fifo_put_exit(fifo, data)
#undef sys_port_trace_k_fifo_alloc_put_enter
#define sys_port_trace_k_fifo_alloc_put_enter(fifo, data, ...)                      \
    sys_trace_k_fifo_alloc_put_enter(fifo, data)
#undef sys_port_trace_k_fifo_alloc_put_exit
#define sys_port_trace_k_fifo_alloc_put_exit(fifo, data, ret, ...)                  \
    sys_trace_k_fifo_alloc_put_exit(fifo, data, ret)
#undef sys_port_trace_k_fifo_put_list_enter
#define sys_port_trace_k_fifo_put_list_enter(fifo, head, tail, ...)                 \
    sys_trace_k_fifo_put_list_enter(fifo, head, tail)
#undef sys_port_trace_k_fifo_put_list_exit
#define sys_port_trace_k_fifo_put_list_exit(fifo, head, tail, ...)                  \
    sys_trace_k_fifo_put_list_exit(fifo, head, tail)
#undef sys_port_trace_k_fifo_put_slist_enter
#define sys_port_trace_k_fifo_put_slist_enter(fifo, list, ...)                      \
    sys_trace_k_fifo_put_slist_enter(fifo, list)
#undef sys_port_trace_k_fifo_put_slist_exit
#define sys_port_trace_k_fifo_put_slist_exit(fifo, list, ...)                       \
    sys_trace_k_fifo_put_slist_exit(fifo, list)
#undef sys_port_trace_k_fifo_get_enter
#define sys_port_trace_k_fifo_get_enter(fifo, timeout, ...)                         \
    sys_trace_k_fifo_get_enter(fifo, timeout)
#undef sys_port_trace_k_fifo_get_exit
#define sys_port_trace_k_fifo_get_exit(fifo, timeout, ret, ...)                     \
    sys_trace_k_fifo_get_exit(fifo, timeout, ret)
#undef sys_port_trace_k_fifo_peek_head_enter
#define sys_port_trace_k_fifo_peek_head_enter(fifo, ...)                            \
    sys_trace_k_fifo_peek_head_enter(fifo)
#undef sys_port_trace_k_fifo_peek_head_exit
#define sys_port_trace_k_fifo_peek_head_exit(fifo, ret, ...)                        \
    sys_trace_k_fifo_peek_head_exit(fifo, ret)
#undef sys_port_trace_k_fifo_peek_tail_enter
#define sys_port_trace_k_fifo_peek_tail_enter(fifo, ...)                            \
    sys_trace_k_fifo_peek_tail_enter(fifo)
#undef sys_port_trace_k_fifo_peek_tail_exit
#define sys_port_trace_k_fifo_peek_tail_exit(fifo, ret, ...)                        \
    sys_trace_k_fifo_peek_tail_exit(fifo, ret)


/* LIFO trace mappings */
#undef sys_port_trace_k_lifo_init_enter
#define sys_port_trace_k_lifo_init_enter(lifo, ...)                                 \
    sys_trace_k_lifo_init_enter(lifo)
#undef sys_port_trace_k_lifo_init_exit
#define sys_port_trace_k_lifo_init_exit(lifo, ...)                                  \
    sys_trace_k_lifo_init_exit(lifo)
#undef sys_port_trace_k_lifo_put_enter
#define sys_port_trace_k_lifo_put_enter(lifo, data, ...)                            \
    sys_trace_k_lifo_put_enter(lifo, data)
#undef sys_port_trace_k_lifo_put_exit
#define sys_port_trace_k_lifo_put_exit(lifo, data, ...)                             \
    sys_trace_k_lifo_put_exit(lifo, data)
#undef sys_port_trace_k_lifo_alloc_put_enter
#define sys_port_trace_k_lifo_alloc_put_enter(lifo, data, ...)                      \
    sys_trace_k_lifo_alloc_put_enter(lifo, data)
#undef sys_port_trace_k_lifo_alloc_put_exit
#define sys_port_trace_k_lifo_alloc_put_exit(lifo, data, ret, ...)                  \
    sys_trace_k_lifo_alloc_put_exit(lifo, data, ret)
#undef sys_port_trace_k_lifo_get_enter
#define sys_port_trace_k_lifo_get_enter(lifo, timeout, ...)                         \
    sys_trace_k_lifo_get_enter(lifo, timeout)
#undef sys_port_trace_k_lifo_get_exit
#define sys_port_trace_k_lifo_get_exit(lifo, timeout, ret, ...)                     \
    sys_trace_k_lifo_get_exit(lifo, timeout, ret)


/* Stack trace mappings */
#undef sys_port_trace_k_stack_init
#define sys_port_trace_k_stack_init(stack, ...)                                     \
    sys_trace_k_stack_init(stack, buffer, num_entries)
#undef sys_port_trace_k_stack_alloc_init_enter
#define sys_port_trace_k_stack_alloc_init_enter(stack, ...)                         \
    sys_trace_k_stack_alloc_init_enter(stack, num_entries)
#undef sys_port_trace_k_stack_alloc_init_exit
#define sys_port_trace_k_stack_alloc_init_exit(stack, ret, ...)                     \
    sys_trace_k_stack_alloc_init_exit(stack, num_entries, ret)
#undef sys_port_trace_k_stack_cleanup_enter
#define sys_port_trace_k_stack_cleanup_enter(stack, ...)                            \
    sys_trace_k_stack_cleanup_enter(stack)
#undef sys_port_trace_k_stack_cleanup_exit
#define sys_port_trace_k_stack_cleanup_exit(stack, ret, ...)                        \
    sys_trace_k_stack_cleanup_exit(stack, ret)
#undef sys_port_trace_k_stack_push_enter
#define sys_port_trace_k_stack_push_enter(stack, ...)                               \
    sys_trace_k_stack_push_enter(stack, data)
#undef sys_port_trace_k_stack_push_exit
#define sys_port_trace_k_stack_push_exit(stack, ret, ...)                           \
    sys_trace_k_stack_push_exit(stack, data, ret)
#undef sys_port_trace_k_stack_pop_enter
#define sys_port_trace_k_stack_pop_enter(stack, timeout, ...)        
#undef sys_port_trace_k_stack_pop_blocking
#define sys_port_trace_k_stack_pop_blocking(stack, timeout, ...)                    \
    sys_trace_k_stack_pop_blocking(stack, data, timeout)
#undef sys_port_trace_k_stack_pop_exit
#define sys_port_trace_k_stack_pop_exit(stack, timeout, ret, ...)                   \
    sys_trace_k_stack_pop_exit(stack, data, timeout, ret)


/* Message queue trace mappings */
#undef sys_port_trace_k_msgq_init
#define sys_port_trace_k_msgq_init(msgq, ...)                                       \
    sys_trace_k_msgq_init(msgq)
#undef sys_port_trace_k_msgq_alloc_init_enter
#define sys_port_trace_k_msgq_alloc_init_enter(msgq, ...)                           \
    sys_trace_k_msgq_alloc_init_enter(msgq, msg_size, max_msgs)
#undef sys_port_trace_k_msgq_alloc_init_exit
#define sys_port_trace_k_msgq_alloc_init_exit(msgq, ret, ...)                       \
    sys_trace_k_msgq_alloc_init_exit(msgq, msg_size, max_msgs, ret)
#undef sys_port_trace_k_msgq_cleanup_enter
#define sys_port_trace_k_msgq_cleanup_enter(msgq, ...)                              \
    sys_trace_k_msgq_cleanup_enter(msgq)
#undef sys_port_trace_k_msgq_cleanup_exit
#define sys_port_trace_k_msgq_cleanup_exit(msgq, ret, ...)                          \
    sys_trace_k_msgq_cleanup_exit(msgq, ret)
#undef sys_port_trace_k_msgq_put_enter
#define sys_port_trace_k_msgq_put_enter(msgq, timeout, ...)                         \
    sys_trace_k_msgq_put_enter(msgq, data, timeout)
#undef sys_port_trace_k_msgq_put_blocking
#define sys_port_trace_k_msgq_put_blocking(msgq, timeout, ...)                      \
    sys_trace_k_msgq_put_blocking(msgq, data, timeout)
#undef sys_port_trace_k_msgq_put_exit
#define sys_port_trace_k_msgq_put_exit(msgq, timeout, ret, ...)                     \
    sys_trace_k_msgq_put_exit(msgq, data, timeout, ret)
#undef sys_port_trace_k_msgq_get_enter
#define sys_port_trace_k_msgq_get_enter(msgq, timeout, ...)                         \
    sys_trace_k_msgq_get_enter(msgq, data, timeout)
#undef sys_port_trace_k_msgq_get_blocking
#define sys_port_trace_k_msgq_get_blocking(msgq, timeout, ...)                      \
    sys_trace_k_msgq_get_blocking(msgq, data, timeout)
#undef sys_port_trace_k_msgq_get_exit
#define sys_port_trace_k_msgq_get_exit(msgq, timeout, ret, ...)                     \
    sys_trace_k_msgq_get_exit(msgq, data, timeout, ret)
#undef sys_port_trace_k_msgq_peek
#define sys_port_trace_k_msgq_peek(msgq, ret, ...)                                  \
    sys_trace_k_msgq_peek(msgq, data, ret)
#undef sys_port_trace_k_msgq_purge
#define sys_port_trace_k_msgq_purge(msgq, ...)                                      \
    sys_trace_k_msgq_purge(msgq)


/* Mailbox trace mappings */
#undef sys_port_trace_k_mbox_init
#define sys_port_trace_k_mbox_init(mbox, ...)                                       \
    sys_trace_k_mbox_init(mbox)
#undef sys_port_trace_k_mbox_message_put_enter
#define sys_port_trace_k_mbox_message_put_enter(mbox, timeout, ...)                 \
    sys_trace_k_mbox_message_put_enter(mbox, tx_msg, timeout)
#undef sys_port_trace_k_mbox_message_put_blocking
#define sys_port_trace_k_mbox_message_put_blocking(mbox, timeout, ...)              \
    sys_trace_k_mbox_message_put_blocking(mbox, tx_msg, timeout)
#undef sys_port_trace_k_mbox_message_put_exit
#define sys_port_trace_k_mbox_message_put_exit(mbox, timeout, ret, ...)             \
    sys_trace_k_mbox_message_put_exit(mbox, tx_msg, timeout, ret)
#undef sys_port_trace_k_mbox_put_enter
#define sys_port_trace_k_mbox_put_enter(mbox, timeout, ...)                         \
    sys_trace_k_mbox_put_enter(mbox, tx_msg, timeout)
#undef sys_port_trace_k_mbox_put_exit
#define sys_port_trace_k_mbox_put_exit(mbox, timeout, ret, ...)                     \
    sys_trace_k_mbox_put_exit(mbox, tx_msg, timeout, ret)
#undef sys_port_trace_k_mbox_async_put_enter
#define sys_port_trace_k_mbox_async_put_enter(mbox, sem, ...)                       \
    sys_trace_k_mbox_async_put_enter(mbox, sem)
#undef sys_port_trace_k_mbox_async_put_exit
#define sys_port_trace_k_mbox_async_put_exit(mbox, sem, ...)                        \
    sys_trace_k_mbox_async_put_exit(mbox, sem)
#undef sys_port_trace_k_mbox_get_enter
#define sys_port_trace_k_mbox_get_enter(mbox, timeout, ...)                         \
    sys_trace_k_mbox_get_enter(mbox, rx_msg, buffer, timeout)
#undef sys_port_trace_k_mbox_get_blocking
#define sys_port_trace_k_mbox_get_blocking(mbox, timeout, ...)                      \
    sys_trace_k_mbox_get_blocking(mbox, rx_msg, buffer, timeout)
#undef sys_port_trace_k_mbox_get_exit
#define sys_port_trace_k_mbox_get_exit(mbox, timeout, ret, ...)                     \
    sys_trace_k_mbox_get_exit(mbox, rx_msg, buffer, timeout, ret)
#undef sys_port_trace_k_mbox_data_get
#define sys_port_trace_k_mbox_data_get(rx_msg, buffer, ...)                         \
    sys_trace_k_mbox_data_get(rx_msg, buffer)


/* Pipe trace mappings */
#undef sys_port_trace_k_pipe_init
#define sys_port_trace_k_pipe_init(pipe, ...)                                       \
    sys_trace_k_pipe_init(pipe, buffer, size)
#undef sys_port_trace_k_pipe_cleanup_enter
#define sys_port_trace_k_pipe_cleanup_enter(pipe, ...)                              \
    sys_trace_k_pipe_cleanup_enter(pipe)
#undef sys_port_trace_k_pipe_cleanup_exit
#define sys_port_trace_k_pipe_cleanup_exit(pipe, ret, ...)                          \
    sys_trace_k_pipe_cleanup_exit(pipe, ret)
#undef sys_port_trace_k_pipe_alloc_init_enter
#define sys_port_trace_k_pipe_alloc_init_enter(pipe, ...)                           \
    sys_trace_k_pipe_alloc_init_enter(pipe, size)
#undef sys_port_trace_k_pipe_alloc_init_exit
#define sys_port_trace_k_pipe_alloc_init_exit(pipe, ret, ...)                       \
    sys_trace_k_pipe_alloc_init_exit(pipe, size, ret)
#undef sys_port_trace_k_pipe_put_enter
#define sys_port_trace_k_pipe_put_enter(pipe, timeout, ...)                         \
    sys_trace_k_pipe_put_enter(pipe, data, bytes_to_write, bytes_written, min_xfer, timeout)
#undef sys_port_trace_k_pipe_put_blocking
#define sys_port_trace_k_pipe_put_blocking(pipe, timeout, ...)                      \
    sys_trace_k_pipe_put_blocking(pipe, data, bytes_to_write, bytes_written, min_xfer, timeout)
#undef sys_port_trace_k_pipe_put_exit
#define sys_port_trace_k_pipe_put_exit(pipe, timeout, ret, ...)                     \
    sys_trace_k_pipe_put_exit(pipe, data, bytes_to_write, bytes_written, min_xfer, timeout, ret)
#undef sys_port_trace_k_pipe_get_enter
#define sys_port_trace_k_pipe_get_enter(pipe, timeout, ...)                         \
    sys_trace_k_pipe_get_enter(pipe, data, bytes_to_read, bytes_read, min_xfer, timeout)
#undef sys_port_trace_k_pipe_get_blocking
#define sys_port_trace_k_pipe_get_blocking(pipe, timeout, ...)                      \
    sys_trace_k_pipe_get_blocking(pipe, data, bytes_to_read, bytes_read, min_xfer, timeout)
#undef sys_port_trace_k_pipe_get_exit
#define sys_port_trace_k_pipe_get_exit(pipe, timeout, ret, ...)                     \
    sys_trace_k_pipe_get_exit(pipe, data, bytes_to_read, bytes_read, min_xfer, timeout, ret)
#undef sys_port_trace_k_pipe_block_put_enter
#define sys_port_trace_k_pipe_block_put_enter(pipe, sem, ...)                       \
    sys_trace_k_pipe_block_put_enter(pipe, block, bytes_to_write, sem)
#undef sys_port_trace_k_pipe_block_put_exit
#define sys_port_trace_k_pipe_block_put_exit(pipe, sem, ...)                        \
    sys_trace_k_pipe_block_put_exit(pipe, block, bytes_to_write, sem)


/* Heap trace mappings */
#undef sys_port_trace_k_heap_init
#define sys_port_trace_k_heap_init(h, ...)                                          \
    sys_trace_k_heap_init(h, mem, bytes)
#undef sys_port_trace_k_heap_aligned_alloc_enter
#define sys_port_trace_k_heap_aligned_alloc_enter(h, timeout, ...)                  \
    sys_trace_k_heap_aligned_alloc_enter(h, bytes, timeout)
#undef sys_port_trace_k_heap_aligned_alloc_blocking
#define sys_port_trace_k_heap_aligned_alloc_blocking(h, timeout, ...)               \
    sys_trace_k_heap_aligned_alloc_blocking(h, bytes, timeout)
#undef sys_port_trace_k_heap_aligned_alloc_exit
#define sys_port_trace_k_heap_aligned_alloc_exit(h, timeout, ret, ...)              \
    sys_trace_k_heap_aligned_alloc_exit(h, bytes, timeout, ret)
#undef sys_port_trace_k_heap_alloc_enter
#define sys_port_trace_k_heap_alloc_enter(h, timeout, ...)                          \
    sys_trace_k_heap_alloc_enter(h, bytes, timeout)
#undef sys_port_trace_k_heap_alloc_exit
#define sys_port_trace_k_heap_alloc_exit(h, timeout, ret, ...)                      \
    sys_trace_k_heap_alloc_exit(h, bytes, timeout, ret)
#undef sys_port_trace_k_heap_free
#define sys_port_trace_k_heap_free(h, ...)                                          \
    sys_trace_k_heap_free(h, mem)
#undef sys_port_trace_k_heap_sys_k_aligned_alloc_enter
#define sys_port_trace_k_heap_sys_k_aligned_alloc_enter(heap, ...)                  \
    sys_trace_k_heap_sys_k_aligned_alloc_enter(heap, align, size)
#undef sys_port_trace_k_heap_sys_k_aligned_alloc_exit
#define sys_port_trace_k_heap_sys_k_aligned_alloc_exit(heap, ret, ...)              \
    sys_trace_k_heap_sys_k_aligned_alloc_exit(heap, align, size, ret)
#undef sys_port_trace_k_heap_sys_k_malloc_enter
#define sys_port_trace_k_heap_sys_k_malloc_enter(heap, ...)                         \
    sys_trace_k_heap_sys_k_malloc_enter(heap, size)
#undef sys_port_trace_k_heap_sys_k_malloc_exit
#define sys_port_trace_k_heap_sys_k_malloc_exit(heap, ret, ...)                     \
    sys_trace_k_heap_sys_k_malloc_exit(heap, size, ret)
#undef sys_port_trace_k_heap_sys_k_free_enter
#define sys_port_trace_k_heap_sys_k_free_enter(heap, heap_ref, ...)                 \
    sys_trace_k_heap_sys_k_free_enter(heap)
#undef sys_port_trace_k_heap_sys_k_free_exit
#define sys_port_trace_k_heap_sys_k_free_exit(heap, heap_ref, ...)                  \
    sys_trace_k_heap_sys_k_free_exit(heap)
#undef sys_port_trace_k_heap_sys_k_calloc_enter
#define sys_port_trace_k_heap_sys_k_calloc_enter(heap, ...)                         \
    sys_trace_k_heap_sys_k_calloc_enter(heap, nmemb, size)
#undef sys_port_trace_k_heap_sys_k_calloc_exit
#define sys_port_trace_k_heap_sys_k_calloc_exit(heap, ret, ...)                     \
    sys_trace_k_heap_sys_k_calloc_exit(heap, nmemb, size, ret)


/* Memory slab trace mappings */
#undef sys_port_trace_k_mem_slab_init
#define sys_port_trace_k_mem_slab_init(slab, rc, ...)                               \
    sys_trace_k_mem_slab_init(slab, buffer, block_size, num_blocks, rc)
#undef sys_port_trace_k_mem_slab_alloc_enter
#define sys_port_trace_k_mem_slab_alloc_enter(slab, timeout, ...)                   \
    sys_trace_k_mem_slab_alloc_enter(slab, mem, timeout)
#undef sys_port_trace_k_mem_slab_alloc_blocking
#define sys_port_trace_k_mem_slab_alloc_blocking(slab, timeout, ...)                \
    sys_trace_k_mem_slab_alloc_blocking(slab, mem, timeout)
#undef sys_port_trace_k_mem_slab_alloc_exit
#define sys_port_trace_k_mem_slab_alloc_exit(slab, timeout, ret, ...)               \
    sys_trace_k_mem_slab_alloc_exit(slab, mem, timeout, ret)
#undef sys_port_trace_k_mem_slab_free_enter
#define sys_port_trace_k_mem_slab_free_enter(slab, ...)
#undef sys_port_trace_k_mem_slab_free_exit
#define sys_port_trace_k_mem_slab_free_exit(slab, ...)                              \
    sys_trace_k_mem_slab_free_exit(slab, mem)


/* Timer trace mappings */
#undef sys_port_trace_k_timer_init
#define sys_port_trace_k_timer_init(timer, ...)                                     \
    sys_trace_k_timer_init(timer, expiry_fn, stop_fn)
#undef sys_port_trace_k_timer_start
#define sys_port_trace_k_timer_start(timer, ...)                                    \
    sys_trace_k_timer_start(timer, duration, period)
#undef sys_port_trace_k_timer_stop
#define sys_port_trace_k_timer_stop(timer, ...)                                     \
    sys_trace_k_timer_stop(timer)
#undef sys_port_trace_k_timer_status_sync_enter
#define sys_port_trace_k_timer_status_sync_enter(timer, ...)
#undef sys_port_trace_k_timer_status_sync_blocking
#define sys_port_trace_k_timer_status_sync_blocking(timer, timeout, ...)            \
    sys_trace_k_timer_status_sync_blocking(timer)
#undef sys_port_trace_k_timer_status_sync_exit
#define sys_port_trace_k_timer_status_sync_exit(timer, result, ...)                 \
    sys_trace_k_timer_status_sync_exit(timer, result)



/* Syscall trace mappings */
#undef sys_port_trace_syscall_enter
#define sys_port_trace_syscall_enter(id, name, ...)                                 \
    sys_trace_syscall_enter(id, #name)
#undef sys_port_trace_syscall_exit
#define sys_port_trace_syscall_exit(id, name, ...)                                  \
    sys_trace_syscall_exit(id, #name)




/* Thread trace function declarations */
void sys_trace_k_thread_foreach_enter(k_thread_user_cb_t user_cb,
    void *user_data);
void sys_trace_k_thread_foreach_exit(k_thread_user_cb_t user_cb,
    void *user_data);
void sys_trace_k_thread_foreach_unlocked_enter(k_thread_user_cb_t user_cb,
    void *user_data);
void sys_trace_k_thread_foreach_unlocked_exit(k_thread_user_cb_t user_cb,
    void *user_data);
void sys_trace_k_thread_create(struct k_thread *new_thread, size_t stack_size,
    int prio);
void sys_trace_k_thread_user_mode_enter(k_thread_entry_t entry, void *p1,
    void *p2, void *p3);
void sys_trace_k_thread_heap_assign(struct k_thread *thread,
    struct k_heap *heap);
void sys_trace_k_thread_join_blocking(struct k_thread *thread,
    k_timeout_t timeout);
void sys_trace_k_thread_join_exit(struct k_thread *thread,
    k_timeout_t timeout, int ret);
void sys_trace_k_thread_sleep_enter(k_timeout_t timeout);
void sys_trace_k_thread_sleep_exit(k_timeout_t timeout, int ret);
void sys_trace_k_thread_msleep_enter(int32_t ms);
void sys_trace_k_thread_msleep_exit(int32_t ms, int ret);
void sys_trace_k_thread_usleep_enter(int32_t us);
void sys_trace_k_thread_usleep_exit(int32_t us, int ret);
void sys_trace_k_thread_busy_wait_enter(uint32_t usec_to_wait);
void sys_trace_k_thread_busy_wait_exit(uint32_t usec_to_wait);
void sys_trace_k_thread_yield();
void sys_trace_k_thread_wakeup(struct k_thread *thread);
void sys_trace_k_thread_abort(struct k_thread *thread);
void sys_trace_k_thread_start(struct k_thread *thread);
void sys_trace_k_thread_priority_set(struct k_thread *thread);
void sys_trace_k_thread_suspend(struct k_thread *thread);
void sys_trace_k_thread_resume(struct k_thread *thread);
void sys_trace_k_thread_sched_lock();
void sys_trace_k_thread_sched_unlock();
void sys_trace_k_thread_name_set(struct k_thread *thread, int ret);
void sys_trace_k_thread_switched_out();
void sys_trace_k_thread_switched_in();
void sys_trace_k_thread_ready(struct k_thread *thread);
void sys_trace_k_thread_pend(struct k_thread *thread);
void sys_trace_k_thread_info(struct k_thread *thread);


/* Thread scheduler trace function declarations */
void sys_trace_k_thread_sched_wakeup(struct k_thread *thread);
void sys_trace_k_thread_sched_abort(struct k_thread *thread);
void sys_trace_k_thread_sched_set_priority(struct k_thread *thread, int prio);
void sys_trace_k_thread_sched_ready(struct k_thread *thread);
void sys_trace_k_thread_sched_pend(struct k_thread *thread);
void sys_trace_k_thread_sched_resume(struct k_thread *thread);
void sys_trace_k_thread_sched_suspend(struct k_thread *thread);


/* Work trace function declarations */
void sys_trace_k_work_init(struct k_work *work, k_work_handler_t handler);
void sys_trace_k_work_submit_to_queue_enter(struct k_work_q *queue,
    struct k_work *work);
void sys_trace_k_work_submit_to_queue_exit(struct k_work_q *queue,
    struct k_work *work, int ret);
void sys_trace_k_work_submit_enter(struct k_work *work);
void sys_trace_k_work_submit_exit(struct k_work *work, int ret);
void sys_trace_k_work_flush_enter(struct k_work *work,
    struct k_work_sync *sync);
void sys_trace_k_work_flush_blocking(struct k_work *work,
    struct k_work_sync *sync, k_timeout_t timeout);
void sys_trace_k_work_flush_exit(struct k_work *work,
    struct k_work_sync *sync, bool ret);
void sys_trace_k_work_cancel_enter(struct k_work *work);
void sys_trace_k_work_cancel_exit(struct k_work *work, int ret);
void sys_trace_k_work_cancel_sync_enter(struct k_work *work,
    struct k_work_sync *sync);
void sys_trace_k_work_cancel_sync_blocking(struct k_work *work,
    struct k_work_sync *sync);
void sys_trace_k_work_cancel_sync_exit(struct k_work *work,
    struct k_work_sync *sync, bool ret);


/* Work queue trace function declarations */
void sys_trace_k_work_queue_start_enter(struct k_work_q *queue,
    k_thread_stack_t *stack, size_t stack_size, int prio,
    const struct k_work_queue_config *cfg);
void sys_trace_k_work_queue_start_exit(struct k_work_q *queue,
    k_thread_stack_t *stack, size_t stack_size, int prio,
    const struct k_work_queue_config *cfg);
void sys_trace_k_work_queue_drain_enter(struct k_work_q *queue, bool plug);
void sys_trace_k_work_queue_drain_exit(struct k_work_q *queue, bool plug,
    int ret);
void sys_trace_k_work_queue_unplug_enter(struct k_work_q *queue);
void sys_trace_k_work_queue_unplug_exit(struct k_work_q *queue,
    int ret);


/* Work delayable trace function declarations */
void sys_trace_k_work_delayable_init(struct k_work_delayable *dwork,
    k_work_handler_t handler);
void sys_trace_k_work_schedule_for_queue_enter(struct k_work_q *queue,
    struct k_work_delayable *dwork, k_timeout_t delay);
void sys_trace_k_work_schedule_for_queue_exit(struct k_work_q *queue,
    struct k_work_delayable *dwork, k_timeout_t delay, int ret);
void sys_trace_k_work_schedule_enter(struct k_work_delayable *dwork,
    k_timeout_t delay);
void sys_trace_k_work_schedule_exit(struct k_work_delayable *dwork,
    k_timeout_t delay, int ret);
void sys_trace_k_work_reschedule_for_queue_enter(struct k_work_q *queue,
    struct k_work_delayable *dwork, k_timeout_t delay);
void sys_trace_k_work_reschedule_for_queue_exit(struct k_work_q *queue,
    struct k_work_delayable *dwork, k_timeout_t delay, int ret);
void sys_trace_k_work_reschedule_enter(struct k_work_delayable *dwork,
    k_timeout_t delay);
void sys_trace_k_work_reschedule_exit(struct k_work_delayable *dwork,
    k_timeout_t delay, int ret);
void sys_trace_k_work_flush_delayable_enter(struct k_work_delayable *dwork,
    struct k_work_sync *sync);
void sys_trace_k_work_flush_delayable_exit(struct k_work_delayable *dwork,
    struct k_work_sync *sync, bool ret);
void sys_trace_k_work_cancel_delayable_enter(struct k_work_delayable *dwork);
void sys_trace_k_work_cancel_delayable_exit(struct k_work_delayable *dwork,
    int ret);
void sys_trace_cancel_delayable_sync_enter(struct k_work_delayable *dwork,
    struct k_work_sync *sync);
void sys_trace_cancel_delayable_sync_exit(struct k_work_delayable *dwork,
    struct k_work_sync *sync, bool ret);


/* Work poll trace function declarations */
void sys_trace_k_work_poll_init_enter(struct k_work_poll *work,
    k_work_handler_t handler);
void sys_trace_k_work_poll_init_exit(struct k_work_poll *work,
    k_work_handler_t handler);
void sys_trace_k_work_poll_submit_to_queue_enter(struct k_work_q *work_q,
    struct k_work_poll *work, struct k_poll_event *events, int num_events,
    k_timeout_t timeout);
void sys_trace_k_work_poll_submit_to_queue_blocking(struct k_work_q *work_q,
    struct k_work_poll *work, struct k_poll_event *events, int num_events,
    k_timeout_t timeout);
void sys_trace_k_work_poll_submit_to_queue_exit(struct k_work_q *work_q,
    struct k_work_poll *work, struct k_poll_event *events, int num_events,
    k_timeout_t timeout, int ret);
void sys_trace_k_work_poll_submit_enter(struct k_work_poll *work,
    struct k_poll_event *events, int num_events, k_timeout_t timeout);
void sys_trace_k_work_poll_submit_exit(struct k_work_poll *work,
    struct k_poll_event *events, int num_events, k_timeout_t timeout,
    int ret);
void sys_trace_k_work_poll_cancel_enter(struct k_work_poll *work);
void sys_trace_k_work_poll_cancel_exit(struct k_work_poll *work, int ret);


/* Poll API trace function declarations */
void sys_trace_k_poll_api_event_init(struct k_poll_event *event,
    uint32_t type, int mode, void *obj);
void sys_trace_k_poll_api_event_poll_enter(struct k_poll_event *events,
    int num_events, k_timeout_t timeout);
void sys_trace_k_poll_api_event_poll_exit(struct k_poll_event *events,
    int num_events, k_timeout_t timeout, int ret);
void sys_trace_k_poll_api_signal_init(struct k_poll_signal *signal);
void sys_trace_k_poll_api_signal_reset(struct k_poll_signal *signal);
void sys_trace_k_poll_api_signal_check(struct k_poll_signal *signal,
    unsigned int *signaled, int *result);
void sys_trace_k_poll_api_signal_raise(struct k_poll_signal *signal,
    int result, int ret);


/* Semaphore trace function declarations */
void sys_trace_k_sem_init(struct k_sem *sem, unsigned int initial_count,
    unsigned int limit, int ret);
void sys_trace_k_sem_give_enter(struct k_sem *sem);
void sys_trace_k_sem_take_enter(struct k_sem *sem, k_timeout_t timeout);
void sys_trace_k_sem_take_blocking(struct k_sem *sem, k_timeout_t timeout);
void sys_trace_k_sem_take_exit(struct k_sem *sem, k_timeout_t timeout,
    int ret);
void sys_trace_k_sem_reset(struct k_sem *sem);


/* Mutex trace function declarations */
void sys_trace_k_mutex_init(struct k_mutex *mutex, int ret);
void sys_trace_k_mutex_lock_enter(struct k_mutex *mutex, k_timeout_t timeout);
void sys_trace_k_mutex_lock_blocking(struct k_mutex *mutex,
    k_timeout_t timeout);
void sys_trace_k_mutex_lock_exit(struct k_mutex *mutex, k_timeout_t timeout,
    int ret);
void sys_trace_k_mutex_unlock_enter(struct k_mutex *mutex);
void sys_trace_k_mutex_unlock_exit(struct k_mutex *mutex, int ret);


/* Conditional variable trace function declarations */
void sys_trace_k_condvar_init(struct k_condvar *condvar, int ret);
void sys_trace_k_condvar_signal_enter(struct k_condvar *condvar);
void sys_trace_k_condvar_signal_blocking(struct k_condvar *condvar);
void sys_trace_k_condvar_signal_exit(struct k_condvar *condvar, int ret);
void sys_trace_k_condvar_broadcast_enter(struct k_condvar *condvar);
void sys_trace_k_condvar_broadcast_exit(struct k_condvar *condvar, int ret);
void sys_trace_k_condvar_wait_enter(struct k_condvar *condvar,
    struct k_mutex *mutex, k_timeout_t timeout);
void sys_trace_k_condvar_wait_exit(struct k_condvar *condvar,
    struct k_mutex *mutex, k_timeout_t timeout, int ret);


/* Queue trace function declarations */
void sys_trace_k_queue_init(struct k_queue *queue);
void sys_trace_k_queue_cancel_wait(struct k_queue *queue);
void sys_trace_k_queue_queue_insert_enter(struct k_queue *queue, bool alloc,
    void *data);
void sys_trace_k_queue_queue_insert_blocking(struct k_queue *queue,
    bool alloc, void *data);
void sys_trace_k_queue_queue_insert_exit(struct k_queue *queue, bool alloc,
    void *data, int ret);
void sys_trace_k_queue_append_enter(struct k_queue *queue, void *data);
void sys_trace_k_queue_append_exit(struct k_queue *queue, void *data);
void sys_trace_k_queue_alloc_append_enter(struct k_queue *queue, void *data);
void sys_trace_k_queue_alloc_append_exit(struct k_queue *queue, void *data,
    int ret);
void sys_trace_k_queue_prepend_enter(struct k_queue *queue, void *data);
void sys_trace_k_queue_prepend_exit(struct k_queue *queue, void *data);
void sys_trace_k_queue_alloc_prepend_enter(struct k_queue *queue, void *data);
void sys_trace_k_queue_alloc_prepend_exit(struct k_queue *queue, void *data,
    int ret);
void sys_trace_k_queue_insert_enter(struct k_queue *queue, void *prev,
    void *data);
void sys_trace_k_queue_insert_exit(struct k_queue *queue, void *prev,
    void *data);
void sys_trace_k_queue_append_list_enter(struct k_queue *queue, void *head,
    void *tail);
void sys_trace_k_queue_append_list_exit(struct k_queue *queue, int ret);
void sys_trace_k_queue_merge_slist_enter(struct k_queue *queue,
    sys_slist_t *list);
void sys_trace_k_queue_merge_slist_exit(struct k_queue *queue,
    sys_slist_t *list, int ret);
void sys_trace_k_queue_get_blocking(struct k_queue *queue,
    k_timeout_t timeout);
void sys_trace_k_queue_get_exit(struct k_queue *queue, k_timeout_t timeout,
    void *ret);
void sys_trace_k_queue_remove_enter(struct k_queue *queue, void *data);
void sys_trace_k_queue_remove_exit(struct k_queue *queue, void *data,
    bool ret);
void sys_trace_k_queue_unique_append_enter(struct k_queue *queue, void *data);
void sys_trace_k_queue_unique_append_exit(struct k_queue *queue, void *data,
    bool ret);
void sys_trace_k_queue_peek_head(struct k_queue *queue, void *ret);
void sys_trace_k_queue_peek_tail(struct k_queue *queue, void *ret);


/* FIFO trace function declarations */
void sys_trace_k_fifo_init_enter(struct k_fifo *fifo);
void sys_trace_k_fifo_init_exit(struct k_fifo *fifo);
void sys_trace_k_fifo_cancel_wait_enter(struct k_fifo *fifo);
void sys_trace_k_fifo_cancel_wait_exit(struct k_fifo *fifo);
void sys_trace_k_fifo_put_enter(struct k_fifo *fifo, void *data);
void sys_trace_k_fifo_put_exit(struct k_fifo *fifo, void *data);
void sys_trace_k_fifo_alloc_put_enter(struct k_fifo *fifo, void *data);
void sys_trace_k_fifo_alloc_put_exit(struct k_fifo *fifo, void *data,
    int ret);
void sys_trace_k_fifo_put_list_enter(struct k_fifo *fifo, void *head,
    void *tail);
void sys_trace_k_fifo_put_list_exit(struct k_fifo *fifo, void *head,
    void *tail);
void sys_trace_k_fifo_put_slist_enter(struct k_fifo *fifo, sys_slist_t *list);
void sys_trace_k_fifo_put_slist_exit(struct k_fifo *fifo, sys_slist_t *list);
void sys_trace_k_fifo_get_enter(struct k_fifo *fifo, k_timeout_t timeout);
void sys_trace_k_fifo_get_exit(struct k_fifo *fifo, k_timeout_t timeout,
    void *ret);
void sys_trace_k_fifo_peek_head_enter(struct k_fifo *fifo);
void sys_trace_k_fifo_peek_head_exit(struct k_fifo *fifo, void *ret);
void sys_trace_k_fifo_peek_tail_enter(struct k_fifo *fifo);
void sys_trace_k_fifo_peek_tail_exit(struct k_fifo *fifo, void *ret);


/* LIFO trace function declarations */
void sys_trace_k_lifo_init_enter(struct k_lifo *lifo);
void sys_trace_k_lifo_init_exit(struct k_lifo *lifo);
void sys_trace_k_lifo_put_enter(struct k_lifo *lifo, void *data);
void sys_trace_k_lifo_put_exit(struct k_lifo *lifo, void *data);
void sys_trace_k_lifo_alloc_put_enter(struct k_lifo *lifo, void *data);
void sys_trace_k_lifo_alloc_put_exit(struct k_lifo *lifo, void *data,
    int ret);
void sys_trace_k_lifo_get_enter(struct k_lifo *lifo, k_timeout_t timeout);
void sys_trace_k_lifo_get_exit(struct k_lifo *lifo, k_timeout_t timeout,
    void *ret);


/* Stack trace function declarations */
void sys_trace_k_stack_init(struct k_stack *stack, stack_data_t *buffer,
    uint32_t num_entries);
void sys_trace_k_stack_alloc_init_enter(struct k_stack *stack,
    uint32_t num_entries);
void sys_trace_k_stack_alloc_init_exit(struct k_stack *stack,
    uint32_t num_entries, int ret);
void sys_trace_k_stack_cleanup_enter(struct k_stack *stack);
void sys_trace_k_stack_cleanup_exit(struct k_stack *stack, int ret);
void sys_trace_k_stack_push_enter(struct k_stack *stack, stack_data_t data);
void sys_trace_k_stack_push_exit(struct k_stack *stack, stack_data_t data,
    int ret);
void sys_trace_k_stack_pop_blocking(struct k_stack *stack, stack_data_t *data,
    k_timeout_t timeout);
void sys_trace_k_stack_pop_exit(struct k_stack *stack, stack_data_t *data,
    k_timeout_t timeout, int ret);


/* Mailbox trace function declarations */
void sys_trace_k_mbox_init(struct k_mbox *mbox);
void sys_trace_k_mbox_message_put_enter(struct k_mbox *mbox,
    struct k_mbox_msg *tx_msg, k_timeout_t timeout);
void sys_trace_k_mbox_message_put_blocking(struct k_mbox *mbox,
    struct k_mbox_msg *tx_msg, k_timeout_t timeout);
void sys_trace_k_mbox_message_put_exit(struct k_mbox *mbox,
    struct k_mbox_msg *tx_msg, k_timeout_t timeout, int ret);
void sys_trace_k_mbox_put_enter(struct k_mbox *mbox,
    struct k_mbox_msg *tx_msg, k_timeout_t timeout);
void sys_trace_k_mbox_put_exit(struct k_mbox *mbox, struct k_mbox_msg *tx_msg,
    k_timeout_t timeout, int ret);
void sys_trace_k_mbox_async_put_enter(struct k_mbox *mbox, struct k_sem *sem);
void sys_trace_k_mbox_async_put_exit(struct k_mbox *mbox, struct k_sem *sem);
void sys_trace_k_mbox_get_enter(struct k_mbox *mbox,
    struct k_mbox_msg *rx_msg, void *buffer, k_timeout_t timeout);
void sys_trace_k_mbox_get_blocking(struct k_mbox *mbox, 
    struct k_mbox_msg *rx_msg, void *buffer, k_timeout_t timeout);
void sys_trace_k_mbox_get_exit(struct k_mbox *mbox, struct k_mbox_msg *rx_msg,
    void *buffer, k_timeout_t timeout, int ret);
void sys_trace_k_mbox_data_get(struct k_mbox_msg *rx_msg, void *buffer);


/* Pipe trace function declarations */
void sys_trace_k_pipe_init(struct k_pipe *pipe, unsigned char *buffer,
    size_t size);
void sys_trace_k_pipe_cleanup_enter(struct k_pipe *pipe);
void sys_trace_k_pipe_cleanup_exit(struct k_pipe *pipe, int ret);
void sys_trace_k_pipe_alloc_init_enter(struct k_pipe *pipe, size_t size);
void sys_trace_k_pipe_alloc_init_exit(struct k_pipe *pipe, size_t size,
    int ret);
void sys_trace_k_pipe_put_enter(struct k_pipe *pipe, void *data,
    size_t bytes_to_write, size_t *bytes_written, size_t min_xfer,
    k_timeout_t timeout);
void sys_trace_k_pipe_put_blocking(struct k_pipe *pipe, void *data,
    size_t bytes_to_write, size_t *bytes_written, size_t min_xfer,
    k_timeout_t timeout);
void sys_trace_k_pipe_put_exit(struct k_pipe *pipe, void *data,
    size_t bytes_to_write, size_t *bytes_written, size_t min_xfer,
    k_timeout_t timeout, int ret);
void sys_trace_k_pipe_get_enter(struct k_pipe *pipe, void *data,
    size_t bytes_to_read, size_t *bytes_read, size_t min_xfer, k_timeout_t timeout);
void sys_trace_k_pipe_get_blocking(struct k_pipe *pipe, void *data,
    size_t bytes_to_read, size_t *bytes_read, size_t min_xfer, k_timeout_t timeout);
void sys_trace_k_pipe_get_exit(struct k_pipe *pipe, void *data,
    size_t bytes_to_read, size_t *bytes_read, size_t min_xfer,
    k_timeout_t timeout, int ret);
void sys_trace_k_pipe_block_put_enter(struct k_pipe *pipe,
    struct k_mem_block *block, size_t size, struct k_sem *sem);
void sys_trace_k_pipe_block_put_exit(struct k_pipe *pipe,
    struct k_mem_block *block, size_t size, struct k_sem *sem);


/* Message queue trace function declarations */
void sys_trace_k_msgq_init(struct k_msgq *msgq);
void sys_trace_k_msgq_alloc_init_enter(struct k_msgq *msgq, size_t msg_size,
    uint32_t max_msgs);
void sys_trace_k_msgq_alloc_init_exit(struct k_msgq *msgq, size_t msg_size,
    uint32_t max_msgs, int ret);
void sys_trace_k_msgq_cleanup_enter(struct k_msgq *msgq);
void sys_trace_k_msgq_cleanup_exit(struct k_msgq *msgq, int ret);
void sys_trace_k_msgq_put_enter(struct k_msgq *msgq, const void *data,
    k_timeout_t timeout);
void sys_trace_k_msgq_put_blocking(struct k_msgq *msgq, const void *data,
    k_timeout_t timeout);
void sys_trace_k_msgq_put_exit(struct k_msgq *msgq, const void *data,
    k_timeout_t timeout, int ret);
void sys_trace_k_msgq_get_enter(struct k_msgq *msgq, const void *data,
    k_timeout_t timeout);
void sys_trace_k_msgq_get_blocking(struct k_msgq *msgq, const void *data,
    k_timeout_t timeout);
void sys_trace_k_msgq_get_exit(struct k_msgq *msgq, const void *data,
    k_timeout_t timeout, int ret);
void sys_trace_k_msgq_peek(struct k_msgq *msgq, void *data, int ret);
void sys_trace_k_msgq_purge(struct k_msgq *msgq);


/* Heap trace function declarations */
void sys_trace_k_heap_init(struct k_heap *h, void *mem, size_t bytes);
void sys_trace_k_heap_alloc_enter(struct k_heap *h, size_t bytes,
    k_timeout_t timeout);
void sys_trace_k_heap_alloc_exit(struct k_heap *h, size_t bytes,
    k_timeout_t timeout, void *ret);
void sys_trace_k_heap_aligned_alloc_enter(struct k_heap *h, size_t bytes,
    k_timeout_t timeout);
void sys_trace_k_heap_aligned_alloc_blocking(struct k_heap *h, size_t bytes,
    k_timeout_t timeout);
void sys_trace_k_heap_aligned_alloc_exit(struct k_heap *h, size_t bytes,
    k_timeout_t timeout, void *ret);
void sys_trace_k_heap_free(struct k_heap *h, void *mem);
void sys_trace_k_heap_sys_k_aligned_alloc_enter(struct k_heap *h,
    size_t align, size_t size);
void sys_trace_k_heap_sys_k_aligned_alloc_exit(struct k_heap *h, size_t align,
    size_t size, void *ret);
void sys_trace_k_heap_sys_k_malloc_enter(struct k_heap *h, size_t size);
void sys_trace_k_heap_sys_k_malloc_exit(struct k_heap *h, size_t size,
    void *ret);
void sys_trace_k_heap_sys_k_free_enter(struct k_heap *h);
void sys_trace_k_heap_sys_k_free_exit(struct k_heap *h);
void sys_trace_k_heap_sys_k_calloc_enter(struct k_heap *h, size_t nmemb,
    size_t size);
void sys_trace_k_heap_sys_k_calloc_exit(struct k_heap *h, size_t nmemb,
    size_t size, void *ret);


/* Memory slab trace function declarations */
void sys_trace_k_mem_slab_init(struct k_mem_slab *slab, void *buffer,
    size_t block_size, uint32_t num_blocks, int ret);
void sys_trace_k_mem_slab_alloc_enter(struct k_mem_slab *slab, void **mem,
    k_timeout_t timeout);
void sys_trace_k_mem_slab_alloc_blocking(struct k_mem_slab *slab, void **mem,
    k_timeout_t timeout);
void sys_trace_k_mem_slab_alloc_exit(struct k_mem_slab *slab, void **mem,
    k_timeout_t timeout, int ret);
void sys_trace_k_mem_slab_free_exit(struct k_mem_slab *slab, void **mem);


/* Timer trace function declarations */
void sys_trace_k_timer_init(struct k_timer *timer, k_timer_expiry_t expiry_fn,
    k_timer_expiry_t stop_fn);
void sys_trace_k_timer_start(struct k_timer *timer, k_timeout_t duration,
    k_timeout_t period);
void sys_trace_k_timer_stop(struct k_timer *timer);
void sys_trace_k_timer_status_sync_blocking(struct k_timer *timer);
void sys_trace_k_timer_status_sync_exit(struct k_timer *timer,
    uint32_t result);


/* Syscall trace function declarations */
void sys_trace_syscall_enter(uint32_t id, const char *name);
void sys_trace_syscall_exit(uint32_t id, const char *name);


/* Legacy trace functions that are pending refactoring/removal by
 * the Zephyr team.
 */
void sys_trace_idle(void);
void sys_trace_isr_enter(void);
void sys_trace_isr_exit(void);

#endif /*_TRACE_TRACERECORDER_H*/
