/*
 * Trace Recorder for Tracealyzer v4.10.2
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file defines the CTI wrappers which the ThreadX recorder kernel port
 * uses to trace the outcome of events.
 */

#include <stdio.h>
#include <stdarg.h>
#include "tx_api.h"
#include "tx_trace.h"
#include "tx_semaphore.h"
#include "trcKernelPort.h"

/* External declarations */
extern UINT _txe_block_allocate_orig(TX_BLOCK_POOL *pool_ptr, VOID **block_ptr, ULONG wait_option);
extern UINT _txe_block_pool_create_orig(TX_BLOCK_POOL *pool_ptr, CHAR *name_ptr, ULONG block_size, VOID *pool_start, ULONG pool_size, UINT pool_control_block_size);
extern UINT _txe_block_pool_delete_orig(TX_BLOCK_POOL *pool_ptr);
extern UINT _txe_block_pool_info_get_orig(TX_BLOCK_POOL *pool_ptr, CHAR **name, ULONG *available_blocks, ULONG *total_blocks, TX_THREAD **first_suspended, ULONG *suspended_count, TX_BLOCK_POOL **next_pool);
extern UINT _tx_block_pool_performance_info_get_orig(TX_BLOCK_POOL *pool_ptr, ULONG *allocates, ULONG *releases, ULONG *suspensions, ULONG *timeouts);
extern UINT _tx_block_pool_performance_system_info_get_oirg(ULONG *allocates, ULONG *releases, ULONG *suspensions, ULONG *timeouts);
extern UINT _txe_block_pool_prioritize_orig(TX_BLOCK_POOL *pool_ptr);
extern UINT _txe_block_release_orig(VOID *block_ptr);

extern UINT _txe_byte_allocate_orig(TX_BYTE_POOL *pool_ptr, VOID **memory_ptr, ULONG memory_size,  ULONG wait_option);
extern UINT _txe_byte_pool_create_orig(TX_BYTE_POOL *pool_ptr, CHAR *name_ptr, VOID *pool_start, ULONG pool_size, UINT pool_control_block_size);
extern UINT _txe_byte_pool_delete_orig(TX_BYTE_POOL *pool_ptr);
extern UINT _txe_byte_pool_info_get_orig(TX_BYTE_POOL *pool_ptr, CHAR **name, ULONG *available_bytes, ULONG *fragments, TX_THREAD **first_suspended, ULONG *suspended_count, TX_BYTE_POOL **next_pool);
extern UINT _tx_byte_pool_performance_info_get_orig(TX_BYTE_POOL *pool_ptr, ULONG *allocates, ULONG *releases, ULONG *fragments_searched, ULONG *merges, ULONG *splits, ULONG *suspensions, ULONG *timeouts);
extern UINT _tx_byte_pool_performance_system_info_get_orig(ULONG *allocates, ULONG *releases, ULONG *fragments_searched, ULONG *merges,	ULONG *splits, ULONG *suspensions,ULONG *timeouts);
extern UINT _txe_byte_pool_prioritize_orig(TX_BYTE_POOL *pool_ptr);
extern UINT _txe_byte_release_orig(VOID *memory_ptr);

extern UINT _txe_event_flags_create_orig(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR *name_ptr, UINT event_control_block_size);
extern UINT _txe_event_flags_delete_orig(TX_EVENT_FLAGS_GROUP *group_ptr);
extern UINT _txe_event_flags_get_orig(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG requested_flags, UINT get_option, ULONG *actual_flags_ptr, ULONG wait_option);
extern UINT _txe_event_flags_info_get_orig(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR **name, ULONG *current_flags, TX_THREAD **first_suspended, ULONG *suspended_count, TX_EVENT_FLAGS_GROUP **next_group);
extern UINT _tx_event_flags_performance_info_get_orig(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG *sets, ULONG *gets, ULONG *suspensions, ULONG *timeouts);
extern UINT _tx_event_flags_performance_system_info_get_orig(ULONG *sets, ULONG *gets, ULONG *suspensions, ULONG *timeouts);
extern UINT _txe_event_flags_set_orig(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG flags_to_set, UINT set_option);
extern UINT _txe_event_flags_set_notify_orig(TX_EVENT_FLAGS_GROUP *group_ptr, VOID (*events_set_notify)(TX_EVENT_FLAGS_GROUP *notify_group_ptr));

extern UINT _txe_mutex_create_orig(TX_MUTEX *mutex_ptr, CHAR *name_ptr, UINT inherit, UINT mutex_control_block_size);
extern UINT _txe_mutex_delete_orig(TX_MUTEX *mutex_ptr);
extern UINT _txe_mutex_get_orig(TX_MUTEX *mutex_ptr, ULONG wait_option);
extern UINT _txe_mutex_info_get_orig(TX_MUTEX *mutex_ptr, CHAR **name, ULONG *count, TX_THREAD **owner, TX_THREAD **first_suspended, ULONG *suspended_count, TX_MUTEX **next_mutex);
extern UINT _tx_mutex_performance_info_get_orig(TX_MUTEX *mutex_ptr, ULONG *puts, ULONG *gets, ULONG *suspensions, ULONG *timeouts, ULONG *inversions, ULONG *inheritances);
extern UINT _tx_mutex_performance_system_info_get_oirg(ULONG *puts, ULONG *gets, ULONG *suspensions, ULONG *timeouts, ULONG *inversions, ULONG *inheritances);
extern UINT _txe_mutex_prioritize_orig(TX_MUTEX *mutex_ptr);
extern UINT _txe_mutex_put_orig(TX_MUTEX *mutex_ptr);

extern UINT _txe_queue_create_orig(TX_QUEUE *queue_ptr, CHAR *name_ptr, UINT message_size, VOID *queue_start, ULONG queue_size, UINT queue_control_block_size);
extern UINT _txe_queue_delete_orig(TX_QUEUE *queue_ptr);
extern UINT _txe_queue_flush_orig(TX_QUEUE *queue_ptr);
extern UINT _txe_queue_front_send_orig(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option);
extern UINT _txe_queue_info_get_orig(TX_QUEUE *queue_ptr, CHAR **name, ULONG *enqueued, ULONG *available_storage, TX_THREAD **first_suspended, ULONG *suspended_count, TX_QUEUE **next_queue);
extern UINT _tx_queue_performance_info_get_orig(TX_QUEUE *queue_ptr, ULONG *messages_sent, ULONG *messages_received, ULONG *empty_suspensions, ULONG *full_suspensions, ULONG *full_errors, ULONG *timeouts);
extern UINT _tx_queue_performance_system_info_get_orig(ULONG *messages_sent, ULONG *messages_received, ULONG *empty_suspensions, ULONG *full_suspensions, ULONG *full_errors, ULONG *timeouts);
extern UINT _txe_queue_prioritize_orig(TX_QUEUE *queue_ptr);
extern UINT _txe_queue_receive_orig(TX_QUEUE *queue_ptr, VOID *destination_ptr, ULONG wait_option);
extern UINT _txe_queue_send_notify_orig(TX_QUEUE *queue_ptr, VOID (*queue_send_notify)(TX_QUEUE *notify_queue_ptr));
extern UINT _txe_queue_send_orig(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option);

extern UINT _txe_semaphore_ceiling_put_orig(TX_SEMAPHORE *semaphore_ptr, ULONG ceiling);
extern UINT _txe_semaphore_create_orig(TX_SEMAPHORE *semaphore_ptr, CHAR *name_ptr, ULONG initial_count, UINT semaphore_control_block_size);
extern UINT _txe_semaphore_delete_orig(TX_SEMAPHORE *semaphore_ptr);
extern UINT _txe_semaphore_get_orig(TX_SEMAPHORE *semaphore_ptr, ULONG wait_option);
extern UINT _txe_semaphore_info_get_orig(TX_SEMAPHORE *semaphore_ptr, CHAR **name, ULONG *current_value, TX_THREAD **first_suspended, ULONG *suspended_count, TX_SEMAPHORE **next_semaphore);
extern UINT _tx_semaphore_performance_info_get_orig(TX_SEMAPHORE *semaphore_ptr, ULONG *puts, ULONG *gets, ULONG *suspensions, ULONG *timeouts);
extern UINT _tx_semaphore_performance_system_info_get_oirg(ULONG *puts, ULONG *gets, ULONG *suspensions, ULONG *timeouts);
extern UINT _txe_semaphore_prioritize_orig(TX_SEMAPHORE *semaphore_ptr);
extern UINT _txe_semaphore_put_notify_orig(TX_SEMAPHORE *semaphore_ptr, VOID (*semaphore_put_notify)(TX_SEMAPHORE *notify_semaphore_ptr));
extern UINT _txe_semaphore_put_orig(TX_SEMAPHORE *semaphore_ptr);

extern UINT _txe_thread_create_orig(TX_THREAD *thread_ptr, CHAR *name_ptr, VOID (*entry_function)(ULONG id), ULONG entry_input, VOID *stack_start, ULONG stack_size, UINT priority, UINT preempt_threshold, ULONG time_slice, UINT auto_start, UINT thread_control_block_size);
extern UINT _txe_thread_delete_orig(TX_THREAD *thread_ptr);
extern UINT _txe_thread_entry_exit_notify_orig(TX_THREAD *thread_ptr, VOID (*thread_entry_exit_notify)(TX_THREAD *notify_thread_ptr, UINT type));
extern UINT _txe_thread_info_get_orig(TX_THREAD *thread_ptr, CHAR **name, UINT *state, ULONG *run_count, UINT *priority, UINT *preemption_threshold, ULONG *time_slice, TX_THREAD **next_thread, TX_THREAD **next_suspended_thread);
extern UINT _tx_thread_performance_info_get_orig(TX_THREAD *thread_ptr, ULONG *resumptions, ULONG *suspensions, ULONG *solicited_preemptions, ULONG *interrupt_preemptions, ULONG *priority_inversions, ULONG *time_slices, ULONG *relinquishes, ULONG *timeouts, ULONG *wait_aborts, TX_THREAD **last_preempted_by);
extern UINT _tx_thread_performance_system_info_get_oirg(ULONG *resumptions, ULONG *suspensions, ULONG *solicited_preemptions, ULONG *interrupt_preemptions, ULONG *priority_inversions, ULONG *time_slices, ULONG *relinquishes, ULONG *timeouts, ULONG *wait_aborts, ULONG *non_idle_returns, ULONG *idle_returns);
extern UINT _txe_thread_preemption_change_orig(TX_THREAD *thread_ptr, UINT new_threshold, UINT *old_threshold);
extern UINT _txe_thread_priority_change_orig(TX_THREAD *thread_ptr, UINT new_priority, UINT *old_priority);
extern UINT _txe_thread_reset_orig(TX_THREAD *thread_ptr);
extern UINT _txe_thread_resume_orig(TX_THREAD *thread_ptr);
extern UINT _tx_thread_sleep_orig(ULONG timer_ticks);
extern UINT _tx_thread_stack_error_notify_orig(VOID (*stack_error_handler)(TX_THREAD *thread_ptr));
extern UINT _txe_thread_suspend_orig(TX_THREAD *thread_ptr);
extern UINT _txe_thread_terminate_orig(TX_THREAD *thread_ptr);
extern VOID _tx_thread_time_slice_orig(VOID);
extern UINT _txe_thread_time_slice_change_orig(TX_THREAD *thread_ptr, ULONG new_time_slice, ULONG *old_time_slice);
extern UINT _txe_thread_wait_abort_orig(TX_THREAD  *thread_ptr);

extern ULONG _tx_time_get_orig(VOID);
extern VOID _tx_time_set_orig(ULONG new_time);
extern UINT _txe_timer_activate_orig(TX_TIMER *timer_ptr);
extern UINT _txe_timer_change_orig(TX_TIMER *timer_ptr, ULONG initial_ticks, ULONG reschedule_ticks);
extern UINT _txe_timer_create_orig(TX_TIMER *timer_ptr, CHAR *name_ptr, VOID (*expiration_function)(ULONG id), ULONG expiration_input, ULONG initial_ticks, ULONG reschedule_ticks, UINT auto_activate, UINT timer_control_block_size);
extern UINT _txe_timer_deactivate_orig(TX_TIMER *timer_ptr);
extern UINT _txe_timer_delete_orig(TX_TIMER *timer_ptr);
extern UINT _txe_timer_info_get_orig(TX_TIMER *timer_ptr, CHAR **name, UINT *active, ULONG *remaining_ticks, ULONG *reschedule_ticks, TX_TIMER **next_timer);
extern UINT _tx_timer_performance_info_get_orig(TX_TIMER *timer_ptr, ULONG *activates, ULONG *reactivates, ULONG *deactivates, ULONG *expirations, ULONG *expiration_adjusts);

extern traceResult xTraceISREnd_orig(TraceBaseType_t uxIsTaskSwitchRequired);

extern traceResult xTraceEventCreate0_orig(uint32_t uiEventCode);
extern traceResult xTraceEventCreate1_orig(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1);
extern traceResult xTraceEventCreate2_orig(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2);
extern traceResult xTraceEventCreate3_orig(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3);
extern traceResult xTraceEventCreate4_orig(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4);
extern traceResult xTraceEventCreate5_orig(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, TraceUnsignedBaseType_t uxParam5);
extern traceResult xTraceEventCreate6_orig(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, TraceUnsignedBaseType_t uxParam5, TraceUnsignedBaseType_t uxParam6);

extern traceResult xTraceEventCreateData0_orig(uint32_t uiEventCode, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);
extern traceResult xTraceEventCreateData1_orig(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);
extern traceResult xTraceEventCreateData2_orig(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);
extern traceResult xTraceEventCreateData3_orig(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);
extern traceResult xTraceEventCreateData4_orig(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);
extern traceResult xTraceEventCreateData5_orig(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, TraceUnsignedBaseType_t uxParam5, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);
extern traceResult xTraceEventCreateData6_orig(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, TraceUnsignedBaseType_t uxParam5, TraceUnsignedBaseType_t uxParam6, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);

extern traceResult xTraceEventCreateRawBlocking_orig(const void* pxSource, uint32_t ulSize);
extern traceResult xTraceEventCreateDataOffline0_orig(uint32_t uiEventCode, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize);

#if !TRC_CFG_SCHEDULING_ONLY

UINT _txe_block_allocate(TX_BLOCK_POOL *pool_ptr, VOID **block_ptr, ULONG wait_option)
{
	UINT ret = _txe_block_allocate_orig(pool_ptr, block_ptr, wait_option);

	if (ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate4(PSF_EVENT_BLOCK_ALLOCATE_SUCCESS, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)wait_option, (TraceUnsignedBaseType_t)ret, (TraceUnsignedBaseType_t)pool_ptr->tx_block_pool_available);
	}
	else if (ret == TX_NO_MEMORY)
	{
		(void)xTraceEventCreate4(PSF_EVENT_BLOCK_ALLOCATE_TIMEOUT, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)wait_option, (TraceUnsignedBaseType_t)ret, (TraceUnsignedBaseType_t)pool_ptr->tx_block_pool_available);
	}
	else
	{
		(void)xTraceEventCreate3(PSF_EVENT_BLOCK_ALLOCATE_FAILED, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)wait_option, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_block_pool_create(TX_BLOCK_POOL *pool_ptr, CHAR *name_ptr, ULONG block_size, VOID *pool_start, ULONG pool_size, UINT pool_control_block_size)
{
	UINT ret = _txe_block_pool_create_orig(pool_ptr, name_ptr, block_size, pool_start, pool_size, pool_control_block_size);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate5(PSF_EVENT_BLOCK_POOL_CREATE_FAILED, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)block_size, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)pool_size, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_block_pool_delete(TX_BLOCK_POOL *pool_ptr)
{
	UINT ret = _txe_block_pool_delete_orig(pool_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_BLOCK_POOL_DELETE_FAILED, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_block_pool_info_get(TX_BLOCK_POOL *pool_ptr, CHAR **name, ULONG *available_blocks, ULONG *total_blocks, TX_THREAD **first_suspended, ULONG *suspended_count, TX_BLOCK_POOL **next_pool)
{
	UINT ret = _txe_block_pool_info_get_orig(pool_ptr, name, available_blocks, total_blocks, first_suspended, suspended_count, next_pool);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_BLOCK_POOL_INFO_GET_FAILED, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT  _tx_block_pool_performance_info_get(TX_BLOCK_POOL *pool_ptr, ULONG *allocates, ULONG *releases, ULONG *suspensions, ULONG *timeouts)
{
	UINT ret = _tx_block_pool_performance_info_get_orig(pool_ptr, allocates, releases, suspensions, timeouts);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_BLOCK_POOL_PERFORMANCE_INFO_GET_FAILED, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT  _tx_block_pool_performance_system_info_get(ULONG *allocates, ULONG *releases, ULONG *suspensions, ULONG *timeouts)
{
	UINT ret = _tx_block_pool_performance_system_info_get_oirg(allocates, releases, suspensions, timeouts);

	(void)xTraceEventCreate0(ret == TX_SUCCESS ? PSF_EVENT_BLOCK_POOL__PERFORMANCE_SYSTEM_INFO_GET_SUCCESS : PSF_EVENT_BLOCK_POOL__PERFORMANCE_SYSTEM_INFO_GET_FAILED);

	return ret;
}

UINT _txe_block_pool_prioritize(TX_BLOCK_POOL *pool_ptr)
{
	UINT ret = _txe_block_pool_prioritize_orig(pool_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_BLOCK_POOL_PRIORITIZE_FAILED, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_block_release(VOID *block_ptr)
{
	UINT ret = _txe_block_release_orig(block_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_BLOCK_POOL_RELEASE_FAILED, (TraceUnsignedBaseType_t)block_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_byte_allocate(TX_BYTE_POOL *pool_ptr, VOID **memory_ptr, ULONG memory_size,  ULONG wait_option)
{
	(void)xTraceEventCreate4(PSF_EVENT_BYTE_ALLOCATE_BLOCKED, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)memory_ptr, (TraceUnsignedBaseType_t)memory_size, (TraceUnsignedBaseType_t)wait_option);

	UINT ret = _txe_byte_allocate_orig(pool_ptr, memory_ptr, memory_size, wait_option);

	if (ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate3(PSF_EVENT_BYTE_ALLOCATE_SUCCESS, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)memory_ptr, (TraceUnsignedBaseType_t)*memory_ptr);
	}
	else if (ret == TX_NO_MEMORY)
	{
		(void)xTraceEventCreate3(PSF_EVENT_BYTE_ALLOCATE_TIMEOUT, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)memory_ptr, (TraceUnsignedBaseType_t)memory_ptr);
	}
	else
	{
		(void)xTraceEventCreate5(PSF_EVENT_BYTE_ALLOCATE_FAILED, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)memory_ptr, (TraceUnsignedBaseType_t)memory_size, (TraceUnsignedBaseType_t)wait_option, (TraceUnsignedBaseType_t)memory_ptr);
	}

	return ret;
}

UINT _txe_byte_pool_create(TX_BYTE_POOL *pool_ptr, CHAR *name_ptr, VOID *pool_start, ULONG pool_size, UINT pool_control_block_size)
{
	UINT ret = _txe_byte_pool_create_orig(pool_ptr, name_ptr, pool_start, pool_size, pool_control_block_size);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate4(PSF_EVENT_BYTE_POOL_CREATE_FAILED, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)pool_start, (TraceUnsignedBaseType_t)pool_size, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_byte_pool_delete(TX_BYTE_POOL *pool_ptr)
{
	UINT ret = _txe_byte_pool_delete_orig(pool_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_BYTE_POOL_DELETE_FAILED, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_byte_pool_info_get(TX_BYTE_POOL *pool_ptr, CHAR **name, ULONG *available_bytes, ULONG *fragments, TX_THREAD **first_suspended, ULONG *suspended_count, TX_BYTE_POOL **next_pool)
{
	UINT ret = _txe_byte_pool_info_get_orig(pool_ptr, name, available_bytes, fragments, first_suspended, suspended_count, next_pool);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_BYTE_POOL_INFO_GET_FAILED, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _tx_byte_pool_performance_info_get(TX_BYTE_POOL *pool_ptr,
	    ULONG *allocates,
	    ULONG *releases,
	    ULONG *fragments_searched,
	    ULONG *merges,
	    ULONG *splits,
	    ULONG *suspensions,
	    ULONG *timeouts)
{
	UINT ret = _tx_byte_pool_performance_info_get_orig(pool_ptr, allocates, releases, fragments_searched, merges,
			splits, suspensions, timeouts);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_BYTE_POOL_PERFORMANCE_INFO_GET_FAILED, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT  _tx_byte_pool_performance_system_info_get(ULONG *allocates,
		ULONG *releases,
		ULONG *fragments_searched,
		ULONG *merges,
		ULONG *splits,
		ULONG *suspensions,
		ULONG *timeouts)
{
	UINT ret = _tx_byte_pool_performance_system_info_get_orig(allocates, releases, fragments_searched,
			merges, splits, suspensions, timeouts);

	if(ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate0(PSF_EVENT_BYTE_POOL__PERFORMANCE_SYSTEM_INFO_GET_SUCCESS);
	}
	else
	{
		(void)xTraceEventCreate0(PSF_EVENT_BYTE_POOL__PERFORMANCE_SYSTEM_INFO_GET_FAILED);
	}

	return ret;
}

UINT _txe_byte_pool_prioritize(TX_BYTE_POOL *pool_ptr)
{
	UINT ret = _txe_byte_pool_prioritize_orig(pool_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_BYTE_POOL_PRIORITIZE_FAILED, (TraceUnsignedBaseType_t)pool_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_byte_release(VOID *memory_ptr)
{
	UINT ret = _txe_byte_release_orig(memory_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_BYTE_POOL_RELEASE_FAILED, (TraceUnsignedBaseType_t)memory_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}


UINT _txe_event_flags_create(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR *name_ptr, UINT event_control_block_size)
{
	UINT ret = _txe_event_flags_create_orig(group_ptr, name_ptr, event_control_block_size);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_FLAGS_CREATE_FAILED, (TraceUnsignedBaseType_t)group_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}


UINT _txe_event_flags_delete(TX_EVENT_FLAGS_GROUP *group_ptr)
{
	UINT ret = _txe_event_flags_delete_orig(group_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_FLAGS_DELETE_FAILED, (TraceUnsignedBaseType_t)group_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}


UINT _txe_event_flags_get(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG requested_flags, UINT get_option, ULONG *actual_flags_ptr, ULONG wait_option)
{
	UINT ret = _txe_event_flags_get_orig(group_ptr, requested_flags, get_option, actual_flags_ptr, wait_option);

	if (ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate3(PSF_EVENT_FLAGS_GET_SUCCESS, (TraceUnsignedBaseType_t)group_ptr, (TraceUnsignedBaseType_t)requested_flags, (TraceUnsignedBaseType_t)get_option);
	}
	else if (ret == TX_NO_EVENTS)
	{
		(void)xTraceEventCreate4(PSF_EVENT_FLAGS_GET_TIMEOUT, (TraceUnsignedBaseType_t)group_ptr, (TraceUnsignedBaseType_t)requested_flags, (TraceUnsignedBaseType_t)get_option, (TraceUnsignedBaseType_t)ret);
	}
	else
	{
		(void)xTraceEventCreate4(PSF_EVENT_FLAGS_GET_FAILED, (TraceUnsignedBaseType_t)group_ptr, (TraceUnsignedBaseType_t)requested_flags, (TraceUnsignedBaseType_t)get_option, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}


UINT _txe_event_flags_info_get(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR **name, ULONG *current_flags, TX_THREAD **first_suspended, ULONG *suspended_count, TX_EVENT_FLAGS_GROUP **next_group)
{
	UINT ret = _txe_event_flags_info_get_orig(group_ptr, name, current_flags, first_suspended, suspended_count, next_group);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_FLAGS_INFO_GET_FAILED, (TraceUnsignedBaseType_t)group_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}


UINT _tx_event_flags_performance_info_get(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG *sets, ULONG *gets, ULONG *suspensions, ULONG *timeouts)
{
	UINT ret = _tx_event_flags_performance_info_get_orig(group_ptr, sets, gets, suspensions, timeouts);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_FLAGS_PERFORMANCE_INFO_GET_FAILED, (TraceUnsignedBaseType_t)group_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT  _tx_event_flags_performance_system_info_get(ULONG *sets, ULONG *gets, ULONG *suspensions, ULONG *timeouts)
{
	UINT ret = _tx_event_flags_performance_system_info_get_orig(sets, gets, suspensions, timeouts);

	if(ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate0(PSF_EVENT_FLAGS__PERFORMANCE_SYSTEM_INFO_GET_SUCCESS);
	}
	else
	{
		(void)xTraceEventCreate0(PSF_EVENT_FLAGS__PERFORMANCE_SYSTEM_INFO_GET_FAILED);
	}

	return ret;
}


UINT _txe_event_flags_set(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG flags_to_set, UINT set_option)
{
	UINT ret = _txe_event_flags_set_orig(group_ptr, flags_to_set, set_option);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate4(PSF_EVENT_FLAGS_SET_FAILED, (TraceUnsignedBaseType_t)group_ptr, (TraceUnsignedBaseType_t)flags_to_set, (TraceUnsignedBaseType_t)set_option, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}


UINT _txe_event_flags_set_notify(TX_EVENT_FLAGS_GROUP *group_ptr, VOID (*events_set_notify)(TX_EVENT_FLAGS_GROUP *notify_group_ptr))
{
	UINT ret = _txe_event_flags_set_notify_orig(group_ptr, events_set_notify);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_FLAGS_SET_NOTIFY_FAILED, (TraceUnsignedBaseType_t)group_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}


UINT _txe_mutex_create(TX_MUTEX *mutex_ptr, CHAR *name_ptr, UINT inherit, UINT mutex_control_block_size) 
{
	UINT ret = _txe_mutex_create_orig(mutex_ptr, name_ptr, inherit, mutex_control_block_size);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate3(PSF_EVENT_MUTEX_CREATE_FAILED, (TraceUnsignedBaseType_t)mutex_ptr, (TraceUnsignedBaseType_t)inherit, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_mutex_delete(TX_MUTEX *mutex_ptr)
{
	UINT ret = _txe_mutex_delete_orig(mutex_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_MUTEX_DELETE_FAILED, (TraceUnsignedBaseType_t)mutex_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_mutex_get(TX_MUTEX *mutex_ptr, ULONG wait_option)
{
	(void)xTraceEventCreate2(PSF_EVENT_MUTEX_GET_BLOCKED, (TraceUnsignedBaseType_t)mutex_ptr, (TraceUnsignedBaseType_t)wait_option);

	UINT ret = _txe_mutex_get_orig(mutex_ptr, wait_option);

	if (ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_MUTEX_GET_SUCCESS, (TraceUnsignedBaseType_t)mutex_ptr, (TraceUnsignedBaseType_t)wait_option);
	}
	else if (ret == TX_NOT_AVAILABLE || ret == TX_WAIT_ABORTED)
	{
		(void)xTraceEventCreate3(PSF_EVENT_MUTEX_GET_TIMEOUT, (TraceUnsignedBaseType_t)mutex_ptr, (TraceUnsignedBaseType_t)wait_option, (TraceUnsignedBaseType_t)ret);
	}
	else
	{
		(void)xTraceEventCreate3(PSF_EVENT_MUTEX_GET_FAILED, (TraceUnsignedBaseType_t)mutex_ptr, (TraceUnsignedBaseType_t)wait_option, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_mutex_info_get(TX_MUTEX *mutex_ptr, CHAR **name, ULONG *count,
		TX_THREAD **owner, TX_THREAD **first_suspended, ULONG *suspended_count, TX_MUTEX **next_mutex)
{
	UINT ret = _txe_mutex_info_get_orig(mutex_ptr, name, count, owner,
			first_suspended, suspended_count, next_mutex);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_MUTEX_INFO_GET_FAILED, (TraceUnsignedBaseType_t)mutex_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _tx_mutex_performance_info_get(TX_MUTEX *mutex_ptr, ULONG *puts, ULONG *gets, ULONG *suspensions, ULONG *timeouts, ULONG *inversions, ULONG *inheritances)
{
	UINT ret = _tx_mutex_performance_info_get_orig(mutex_ptr, puts, gets, suspensions, timeouts, inversions, inheritances);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_MUTEX_PERFORMANCE_INFO_GET_FAILED, (TraceUnsignedBaseType_t)mutex_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT  _tx_mutex_performance_system_info_get(ULONG *puts, ULONG *gets, ULONG *suspensions,
                                ULONG *timeouts, ULONG *inversions, ULONG *inheritances)
{
	UINT ret = _tx_mutex_performance_system_info_get_oirg(puts, gets, suspensions, timeouts, inversions, inheritances);

	if(ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate0(PSF_EVENT_MUTEX_PERFORMANCE_SYSTEM_INFO_GET_SUCCESS);
	}
	else
	{
		(void)xTraceEventCreate0(PSF_EVENT_BLOCK_POOL__PERFORMANCE_SYSTEM_INFO_GET_FAILED);
	}

	return ret;
}

UINT _txe_mutex_prioritize(TX_MUTEX *mutex_ptr)
{
	UINT ret = _txe_mutex_prioritize_orig(mutex_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_MUTEX_PRIORITIZE_FAILED, (TraceUnsignedBaseType_t)mutex_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_mutex_put(TX_MUTEX *mutex_ptr)
{
	UINT ret = _txe_mutex_put_orig(mutex_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_MUTEX_PUT_FAILED, (TraceUnsignedBaseType_t)mutex_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}


UINT _txe_queue_create(TX_QUEUE *queue_ptr, CHAR *name_ptr, UINT message_size,
                        VOID *queue_start, ULONG queue_size, UINT queue_control_block_size) 
{
	UINT ret = _txe_queue_create_orig(queue_ptr, name_ptr, message_size, queue_start, queue_size, queue_control_block_size);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate5(PSF_EVENT_QUEUE_CREATE_FAILED, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)message_size, (TraceUnsignedBaseType_t)queue_start, (TraceUnsignedBaseType_t)queue_size, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_queue_delete(TX_QUEUE *queue_ptr)
{
	UINT ret = _txe_queue_delete_orig(queue_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_QUEUE_DELETE_FAILED, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}


UINT _txe_queue_flush(TX_QUEUE *queue_ptr)
{
	UINT ret = _txe_queue_flush_orig(queue_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_QUEUE_FLUSH_FAILED, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_queue_front_send(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option)
{
	(void)xTraceEventCreate3(PSF_EVENT_QUEUE_FRONT_SEND_BLOCKED, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)source_ptr, (TraceUnsignedBaseType_t)wait_option);

	UINT ret = _txe_queue_front_send_orig(queue_ptr, source_ptr, wait_option);

	if (ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate3(PSF_EVENT_QUEUE_FRONT_SEND_SUCCESS, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)source_ptr, (TraceUnsignedBaseType_t)wait_option);
	}
	else if (ret == TX_QUEUE_FULL)
	{
		(void)xTraceEventCreate4(PSF_EVENT_QUEUE_FRONT_SEND_TIMEOUT, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)source_ptr, (TraceUnsignedBaseType_t)wait_option, (TraceUnsignedBaseType_t)ret);
	}
	else
	{
		(void)xTraceEventCreate4(PSF_EVENT_QUEUE_FRONT_SEND_FAILED, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)source_ptr, (TraceUnsignedBaseType_t)wait_option, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_queue_info_get(TX_QUEUE *queue_ptr, CHAR **name, ULONG *enqueued, ULONG *available_storage, TX_THREAD **first_suspended, ULONG *suspended_count, TX_QUEUE **next_queue)
{
	UINT ret = _txe_queue_info_get_orig(queue_ptr, name, enqueued, available_storage, first_suspended, suspended_count, next_queue);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_QUEUE_INFO_GET_FAILED, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT  _tx_queue_performance_info_get(TX_QUEUE *queue_ptr, ULONG *messages_sent, ULONG *messages_received,
                    ULONG *empty_suspensions, ULONG *full_suspensions, ULONG *full_errors, ULONG *timeouts)
{
	UINT ret = _tx_queue_performance_info_get_orig(queue_ptr, messages_sent, messages_received, empty_suspensions, full_suspensions, full_errors, timeouts);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_QUEUE_PERFORMANCE_INFO_GET_FAILED, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT  _tx_queue_performance_system_info_get(ULONG *messages_sent, ULONG *messages_received,
                    ULONG *empty_suspensions, ULONG *full_suspensions, ULONG *full_errors, ULONG *timeouts)
{
	UINT ret = _tx_queue_performance_system_info_get_orig(messages_sent, messages_received, empty_suspensions, full_suspensions, full_errors, timeouts);

	if(ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate0(PSF_EVENT_QUEUE_PERFORMANCE_SYSTEM_INFO_GET_SUCCESS);
	}
	else
	{
		(void)xTraceEventCreate0(PSF_EVENT_QUEUE_PERFORMANCE_SYSTEM_INFO_GET_FAILED);
	}

	return ret;
}

UINT _txe_queue_prioritize(TX_QUEUE *queue_ptr)
{
	UINT ret = _txe_queue_prioritize_orig(queue_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_QUEUE_PRIORITIZE_FAILED, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_queue_receive(TX_QUEUE *queue_ptr, VOID *destination_ptr, ULONG wait_option)
{
	(void)xTraceEventCreate3(PSF_EVENT_QUEUE_RECEIVE_BLOCKED, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)destination_ptr, (TraceUnsignedBaseType_t)wait_option);

	UINT ret = _txe_queue_receive_orig(queue_ptr, destination_ptr, wait_option);

	if (ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate3(PSF_EVENT_QUEUE_RECEIVE_SUCCESS, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)destination_ptr, (TraceUnsignedBaseType_t)wait_option);
	}
	else if (ret == TX_QUEUE_EMPTY)
	{
		(void)xTraceEventCreate4(PSF_EVENT_QUEUE_RECEIVE_TIMEOUT, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)destination_ptr, (TraceUnsignedBaseType_t)wait_option, (TraceUnsignedBaseType_t)ret);
	}
	else
	{
		(void)xTraceEventCreate4(PSF_EVENT_QUEUE_RECEIVE_FAILED, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)destination_ptr, (TraceUnsignedBaseType_t)wait_option, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_queue_send_notify(TX_QUEUE *queue_ptr, VOID (*queue_send_notify)(TX_QUEUE *notify_queue_ptr))
{
	UINT ret = _txe_queue_send_notify_orig(queue_ptr, queue_send_notify);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_QUEUE_SEND_NOTIFY_FAILED, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_queue_send(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option)
{
	(void)xTraceEventCreate3(PSF_EVENT_QUEUE_SEND_BLOCKED, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)source_ptr, (TraceUnsignedBaseType_t)wait_option);

	UINT ret = _txe_queue_send_orig(queue_ptr, source_ptr, wait_option);

	if (ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate3(PSF_EVENT_QUEUE_SEND_SUCCESS, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)source_ptr, (TraceUnsignedBaseType_t)wait_option);
	}
	else if (ret == TX_QUEUE_FULL)
	{
		(void)xTraceEventCreate4(PSF_EVENT_QUEUE_SEND_TIMEOUT, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)source_ptr, (TraceUnsignedBaseType_t)wait_option, (TraceUnsignedBaseType_t)ret);
	}
	else
	{
		(void)xTraceEventCreate4(PSF_EVENT_QUEUE_SEND_FAILED, (TraceUnsignedBaseType_t)queue_ptr, (TraceUnsignedBaseType_t)source_ptr, (TraceUnsignedBaseType_t)wait_option, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_semaphore_ceiling_put(TX_SEMAPHORE *semaphore_ptr, ULONG ceiling)
{
	(void)xTraceEventCreate2(PSF_EVENT_SEMAPHORE_CEILING_PUT_BLOCKED, (TraceUnsignedBaseType_t)semaphore_ptr, (TraceUnsignedBaseType_t)ceiling);

	UINT ret = _txe_semaphore_ceiling_put_orig(semaphore_ptr, ceiling);

	if (ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_SEMAPHORE_CEILING_PUT_SUCCESS, (TraceUnsignedBaseType_t)semaphore_ptr, (TraceUnsignedBaseType_t)ceiling);
	}
	else
	{
		(void)xTraceEventCreate3(PSF_EVENT_SEMAPHORE_CEILING_PUT_FAILED, (TraceUnsignedBaseType_t)semaphore_ptr, (TraceUnsignedBaseType_t)ceiling, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_semaphore_create(TX_SEMAPHORE *semaphore_ptr, CHAR *name_ptr, ULONG initial_count, UINT semaphore_control_block_size) 
{
	UINT ret = _txe_semaphore_create_orig(semaphore_ptr, name_ptr, initial_count, semaphore_control_block_size);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate3(PSF_EVENT_SEMAPHORE_CREATE_FAILED, (TraceUnsignedBaseType_t)semaphore_ptr, (TraceUnsignedBaseType_t)initial_count, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_semaphore_delete(TX_SEMAPHORE *semaphore_ptr)
{
	UINT ret = _txe_semaphore_delete_orig(semaphore_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_SEMAPHORE_DELETE_FAILED, (TraceUnsignedBaseType_t)semaphore_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_semaphore_get(TX_SEMAPHORE *semaphore_ptr, ULONG wait_option)
{
		(void)xTraceEventCreate2(PSF_EVENT_SEMAPHORE_GET_BLOCKED, (TraceUnsignedBaseType_t)semaphore_ptr, (TraceUnsignedBaseType_t)wait_option);

	UINT ret = _txe_semaphore_get_orig(semaphore_ptr, wait_option);

	if (ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_SEMAPHORE_GET_SUCCESS, (TraceUnsignedBaseType_t)semaphore_ptr, (TraceUnsignedBaseType_t)wait_option);
	}
	else if (ret == TX_NO_INSTANCE)
	{
		(void)xTraceEventCreate3(PSF_EVENT_SEMAPHORE_GET_TIMEOUT, (TraceUnsignedBaseType_t)semaphore_ptr, (TraceUnsignedBaseType_t)wait_option, (TraceUnsignedBaseType_t)ret);
	}
	else
	{
		(void)xTraceEventCreate3(PSF_EVENT_SEMAPHORE_GET_FAILED, (TraceUnsignedBaseType_t)semaphore_ptr, (TraceUnsignedBaseType_t)wait_option, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_semaphore_info_get(TX_SEMAPHORE *semaphore_ptr, CHAR **name, ULONG *current_value, TX_THREAD **first_suspended, ULONG *suspended_count, TX_SEMAPHORE **next_semaphore)
{
	UINT ret = _txe_semaphore_info_get_orig(semaphore_ptr, name, current_value, first_suspended, suspended_count, next_semaphore);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_SEMAPHORE_INFO_GET_FAILED, (TraceUnsignedBaseType_t)semaphore_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _tx_semaphore_performance_info_get(TX_SEMAPHORE *semaphore_ptr, ULONG *puts, ULONG *gets, ULONG *suspensions, ULONG *timeouts)
{
	UINT ret = _tx_semaphore_performance_info_get_orig(semaphore_ptr, puts, gets, suspensions, timeouts);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_SEMAPHORE_PERFORMANCE_INFO_GET_FAILED, (TraceUnsignedBaseType_t)semaphore_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT  _tx_semaphore_performance_system_info_get(ULONG *puts, ULONG *gets, ULONG *suspensions, ULONG *timeouts)
{
	UINT ret = _tx_semaphore_performance_system_info_get_oirg(puts, gets, suspensions, timeouts);

	if(ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate0(PSF_EVENT_SEMAPHORE__PERFORMANCE_SYSTEM_INFO_GET_SUCCESS);
	}
	else
	{
		(void)xTraceEventCreate0(PSF_EVENT_SEMAPHORE__PERFORMANCE_SYSTEM_INFO_GET_FAILED);
	}

	return ret;
}


UINT _txe_semaphore_prioritize(TX_SEMAPHORE *semaphore_ptr)
{
	UINT ret = _txe_semaphore_prioritize_orig(semaphore_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_SEMAPHORE_PRIORITIZE_FAILED, (TraceUnsignedBaseType_t)semaphore_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_semaphore_put_notify(TX_SEMAPHORE *semaphore_ptr, VOID (*semaphore_put_notify)(TX_SEMAPHORE *notify_semaphore_ptr))
{
	UINT ret = _txe_semaphore_put_notify_orig(semaphore_ptr, semaphore_put_notify);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_SEMAPHORE_PUT_NOTIFY_FAILED, (TraceUnsignedBaseType_t)semaphore_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_semaphore_put(TX_SEMAPHORE *semaphore_ptr)
{
	UINT ret = _txe_semaphore_put_orig(semaphore_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_SEMAPHORE_PUT_FAILED, (TraceUnsignedBaseType_t)semaphore_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

ULONG _tx_time_get(VOID)
{
	ULONG ret = _tx_time_get_orig();

	(void)xTraceEventCreate1(PSF_EVENT_TIME_GET_SUCCESS, (TraceUnsignedBaseType_t)ret);

	return ret;
}

VOID _tx_time_set(ULONG new_time)
{
	_tx_time_set_orig(new_time);
}

UINT _txe_timer_activate(TX_TIMER *timer_ptr)
{
	UINT ret = _txe_timer_activate_orig(timer_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_TIMER_ACTIVATE_FAILED, (TraceUnsignedBaseType_t)timer_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_timer_change(TX_TIMER *timer_ptr, ULONG initial_ticks, ULONG reschedule_ticks)
{
	UINT ret = _txe_timer_change_orig(timer_ptr, initial_ticks, reschedule_ticks);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate4(PSF_EVENT_TIMER_CHANGE_FAILED, (TraceUnsignedBaseType_t)timer_ptr, (TraceUnsignedBaseType_t)initial_ticks, (TraceUnsignedBaseType_t)reschedule_ticks, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_timer_create(TX_TIMER *timer_ptr, CHAR *name_ptr,
            VOID (*expiration_function)(ULONG id), ULONG expiration_input,
            ULONG initial_ticks, ULONG reschedule_ticks, UINT auto_activate, UINT timer_control_block_size)
{
	UINT ret = _txe_timer_create_orig(timer_ptr, name_ptr, expiration_function, expiration_input, initial_ticks,
			reschedule_ticks, auto_activate, timer_control_block_size);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate5(PSF_EVENT_TIMER_CREATE_FAILED, (TraceUnsignedBaseType_t)timer_ptr, (TraceUnsignedBaseType_t)initial_ticks, (TraceUnsignedBaseType_t)reschedule_ticks, (TraceUnsignedBaseType_t)auto_activate, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_timer_deactivate(TX_TIMER *timer_ptr)
{
	UINT ret = _txe_timer_deactivate_orig(timer_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_TIMER_DEACTIVATE_FAILED, (TraceUnsignedBaseType_t)timer_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_timer_delete(TX_TIMER *timer_ptr)
{
	UINT ret = _txe_timer_delete_orig(timer_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_TIMER_DELETE_FAILED, (TraceUnsignedBaseType_t)timer_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_timer_info_get(TX_TIMER *timer_ptr, CHAR **name, UINT *active, ULONG *remaining_ticks, ULONG *reschedule_ticks, TX_TIMER **next_timer)
{
	UINT ret = _txe_timer_info_get_orig(timer_ptr, name, active, remaining_ticks, reschedule_ticks, next_timer);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_TIMER_INFO_GET_FAILED, (TraceUnsignedBaseType_t)timer_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _tx_timer_performance_info_get(TX_TIMER *timer_ptr, ULONG *activates, ULONG *reactivates, ULONG *deactivates, ULONG *expirations, ULONG *expiration_adjusts)
{
	UINT ret = _tx_timer_performance_info_get_orig(timer_ptr, activates, reactivates, deactivates, expirations, expiration_adjusts);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_TIMER_PERFORMANCE_INFO_GET_FAILED, (TraceUnsignedBaseType_t)timer_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}
#endif /* TRC_CFG_SCHEDULING_ONLY */

UINT _txe_thread_create(TX_THREAD *thread_ptr, CHAR *name_ptr, VOID (*entry_function)(ULONG id), ULONG entry_input,
        VOID *stack_start, ULONG stack_size, UINT priority, UINT preempt_threshold,
        ULONG time_slice, UINT auto_start, UINT thread_control_block_size)
{
	UINT ret = _txe_thread_create_orig(thread_ptr, name_ptr, entry_function, entry_input, stack_start,
			stack_size, priority, preempt_threshold, time_slice, auto_start, thread_control_block_size);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_THREAD_CREATE_FAILED, (TraceUnsignedBaseType_t)thread_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_thread_delete(TX_THREAD *thread_ptr)
{
	UINT ret = _txe_thread_delete_orig(thread_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_THREAD_DELETE_FAILED, (TraceUnsignedBaseType_t)thread_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_thread_entry_exit_notify(TX_THREAD *thread_ptr, VOID (*thread_entry_exit_notify)(TX_THREAD *notify_thread_ptr, UINT type))
{
	UINT ret = _txe_thread_entry_exit_notify_orig(thread_ptr, thread_entry_exit_notify);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_THREAD_ENTRY_EXIT_NOTIFY_FAILED, (TraceUnsignedBaseType_t)thread_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_thread_info_get(TX_THREAD *thread_ptr, CHAR **name, UINT *state, ULONG *run_count, UINT *priority, UINT *preemption_threshold, ULONG *time_slice, TX_THREAD **next_thread, TX_THREAD **next_suspended_thread)
{
	UINT ret = _txe_thread_info_get_orig(thread_ptr, name, state, run_count, priority, preemption_threshold, time_slice, next_thread, next_suspended_thread);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_THREAD_INFO_GET_FAILED, (TraceUnsignedBaseType_t)thread_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _tx_thread_performance_info_get(TX_THREAD *thread_ptr, ULONG *resumptions, ULONG *suspensions,
                ULONG *solicited_preemptions, ULONG *interrupt_preemptions, ULONG *priority_inversions,
                ULONG *time_slices, ULONG *relinquishes, ULONG *timeouts, ULONG *wait_aborts, TX_THREAD **last_preempted_by)
{
	UINT ret = _tx_thread_performance_info_get_orig(thread_ptr, resumptions, suspensions, solicited_preemptions, interrupt_preemptions, priority_inversions, time_slices, relinquishes, timeouts, wait_aborts, last_preempted_by);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_THREAD_PERFORMANCE_INFO_GET_FAILED, (TraceUnsignedBaseType_t)thread_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT  _tx_thread_performance_system_info_get(ULONG *resumptions, ULONG *suspensions,
                ULONG *solicited_preemptions, ULONG *interrupt_preemptions, ULONG *priority_inversions,
                ULONG *time_slices, ULONG *relinquishes, ULONG *timeouts, ULONG *wait_aborts,
                ULONG *non_idle_returns, ULONG *idle_returns)
{
	UINT ret = _tx_thread_performance_system_info_get_oirg(resumptions, suspensions, solicited_preemptions, interrupt_preemptions, priority_inversions, time_slices, relinquishes, timeouts, wait_aborts, non_idle_returns, idle_returns);

	if(ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate0(PSF_EVENT_THREAD_PERFORMANCE_SYSTEM_INFO_GET_SUCCESS);
	}
	else
	{
		(void)xTraceEventCreate0(PSF_EVENT_THREAD_PERFORMANCE_SYSTEM_INFO_GET_FAILED);
	}

	return ret;
}

UINT _txe_thread_preemption_change(TX_THREAD *thread_ptr, UINT new_threshold, UINT *old_threshold)
{
	UINT ret = _txe_thread_preemption_change_orig(thread_ptr, new_threshold, old_threshold);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate3(PSF_EVENT_THREAD_PREEMPTION_CHANGE_FAILED, (TraceUnsignedBaseType_t)thread_ptr, (TraceUnsignedBaseType_t)new_threshold, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_thread_priority_change(TX_THREAD *thread_ptr, UINT new_priority, UINT *old_priority)
{
	UINT ret = _txe_thread_priority_change_orig(thread_ptr, new_priority, old_priority);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate3(PSF_EVENT_THREAD_PRIORITY_CHANGE_FAILED, (TraceUnsignedBaseType_t)thread_ptr, (TraceUnsignedBaseType_t)new_priority, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_thread_reset(TX_THREAD *thread_ptr)
{
	UINT ret = _txe_thread_reset_orig(thread_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_THREAD_RESET_FAILED, (TraceUnsignedBaseType_t)thread_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_thread_resume(TX_THREAD *thread_ptr)
{
	UINT ret = _txe_thread_resume_orig(thread_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_THREAD_RESUME_FAILED, (TraceUnsignedBaseType_t)thread_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _tx_thread_sleep(ULONG timer_ticks)
{
	(void)xTraceEventCreate1(PSF_EVENT_THREAD_SLEEP_BLOCKED, (TraceUnsignedBaseType_t)timer_ticks);

	UINT ret = _tx_thread_sleep_orig(timer_ticks);

	if (ret == TX_SUCCESS)
	{
		(void)xTraceEventCreate1(PSF_EVENT_THREAD_SLEEP_SUCCESS, (TraceUnsignedBaseType_t)timer_ticks);
	}
	else
	{
		(void)xTraceEventCreate2(PSF_EVENT_THREAD_SLEEP_FAILED, (TraceUnsignedBaseType_t)timer_ticks, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _tx_thread_stack_error_notify(VOID (*stack_error_handler)(TX_THREAD *thread_ptr))
{
	UINT ret = _tx_thread_stack_error_notify_orig(stack_error_handler);

	(void)xTraceEventCreate1(TX_TRACE_THREAD_STACK_ERROR_NOTIFY, (TraceUnsignedBaseType_t)ret);

	return ret;
}

UINT _txe_thread_suspend(TX_THREAD *thread_ptr)
{
	UINT ret = _txe_thread_suspend_orig(thread_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_THREAD_SUSPEND_FAILED, (TraceUnsignedBaseType_t)thread_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_thread_terminate(TX_THREAD *thread_ptr)
{
	UINT ret = _txe_thread_terminate_orig(thread_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_THREAD_TERMINATE_FAILED, (TraceUnsignedBaseType_t)thread_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

VOID _tx_thread_time_slice(VOID)
{
	_tx_thread_time_slice_orig();
}

UINT _txe_thread_time_slice_change(TX_THREAD *thread_ptr, ULONG new_time_slice, ULONG *old_time_slice)
{
	UINT ret = _txe_thread_time_slice_change_orig(thread_ptr, new_time_slice, old_time_slice);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate3(PSF_EVENT_THREAD_TIME_SLICE_CHANGE_FAILED, (TraceUnsignedBaseType_t)thread_ptr, (TraceUnsignedBaseType_t)new_time_slice, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

UINT _txe_thread_wait_abort(TX_THREAD  *thread_ptr)
{
	UINT ret = _txe_thread_wait_abort_orig(thread_ptr);

	if (ret != TX_SUCCESS)
	{
		(void)xTraceEventCreate2(PSF_EVENT_THREAD_WAIT_ABORT_FAILED, (TraceUnsignedBaseType_t)thread_ptr, (TraceUnsignedBaseType_t)ret);
	}

	return ret;
}

traceResult xTraceISREnd(TraceBaseType_t uxIsTaskSwitchRequired)
{
	traceResult xResult;

	xResult = xTraceISREnd_orig(uxIsTaskSwitchRequired);

	/* Force a thread switch outcome result when exiting an ISR. Without this
	 * we would get incorrect switching if the ISR exits to the thread that
	 * it entered from.
	 */
	xTraceResetCurrentThread();

	return xResult;
}

traceResult xTraceEventCreate0(uint32_t uiEventCode)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreate0_orig(uiEventCode);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreate1(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreate1_orig(uiEventCode, uxParam1);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreate2(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreate2_orig(uiEventCode, uxParam1, uxParam2);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreate3(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreate3_orig(uiEventCode, uxParam1, uxParam2, uxParam3);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreate4(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreate4_orig(uiEventCode, uxParam1, uxParam2, uxParam3, uxParam4);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreate5(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, TraceUnsignedBaseType_t uxParam5)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreate5_orig(uiEventCode, uxParam1, uxParam2, uxParam3, uxParam4, uxParam5);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreate6(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, TraceUnsignedBaseType_t uxParam5, TraceUnsignedBaseType_t uxParam6)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreate6_orig(uiEventCode, uxParam1, uxParam2, uxParam3, uxParam4, uxParam5, uxParam6);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreateData0(uint32_t uiEventCode, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreateData0_orig(uiEventCode, puxData, uxSize);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreateData1(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreateData1_orig(uiEventCode, uxParam1, puxData, uxSize);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreateData2(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreateData2_orig(uiEventCode, uxParam1, uxParam2, puxData, uxSize);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreateData3(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreateData3_orig(uiEventCode, uxParam1, uxParam2, uxParam3, puxData, uxSize);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreateData4(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreateData4_orig(uiEventCode, uxParam1, uxParam2, uxParam3, uxParam4, puxData, uxSize);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreateData5(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, TraceUnsignedBaseType_t uxParam5, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreateData5_orig(uiEventCode, uxParam1, uxParam2, uxParam3, uxParam4, uxParam5, puxData, uxSize);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreateData6(uint32_t uiEventCode, TraceUnsignedBaseType_t uxParam1, TraceUnsignedBaseType_t uxParam2, TraceUnsignedBaseType_t uxParam3, TraceUnsignedBaseType_t uxParam4, TraceUnsignedBaseType_t uxParam5, TraceUnsignedBaseType_t uxParam6, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreateData6_orig(uiEventCode, uxParam1, uxParam2, uxParam3, uxParam4, uxParam5, uxParam6, puxData, uxSize);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreateRawBlocking(const void* pxSource, uint32_t ulSize)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreateRawBlocking_orig(pxSource, ulSize);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}

traceResult xTraceEventCreateDataOffline0(uint32_t uiEventCode, const TraceUnsignedBaseType_t* const puxData, TraceUnsignedBaseType_t uxSize)
{
	traceResult xResult;
	TRACE_ALLOC_CRITICAL_SECTION();

	/* We must use a critical section here or there is a chance that an interrupt
	 * could fire in the time between checking for a thread switch and the event
	 * being sent.
	 */
	TRACE_ENTER_CRITICAL_SECTION();

	/* Perform a thread switch check before each event to see if ThreadX has switched to a new
	 * thread without informing us.
	 */
	xTraceCheckThreadSwitch();

	xResult = xTraceEventCreateDataOffline0_orig(uiEventCode, puxData, uxSize);

	/* Exit wrapped critical section */
	TRACE_EXIT_CRITICAL_SECTION();

	return xResult;
}
