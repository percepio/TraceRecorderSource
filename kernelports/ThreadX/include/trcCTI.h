/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This file declares the CTI aliasing and weakening of ThreadX kernel object
 * functions so that the ThreadX recorder kernel port can trace the outcome
 * of events.
 */

#ifndef TRC_CTI_H_
#define TRC_CTI_H_

#if !TRC_CFG_SCHEDULING_ONLY

/* Block Pool tracing hooks */
#ifdef __inside_txe_block_allocate

#include "tx_api.h"
#include "tx_block_pool.h"
#include "tx_trace.h"

UINT _txe_block_allocate(TX_BLOCK_POOL *pool_ptr, VOID **block_ptr, ULONG wait_option) __attribute__((weak));
UINT _txe_block_allocate_orig(TX_BLOCK_POOL *pool_ptr, VOID **block_ptr, ULONG wait_option) __attribute__((alias("_txe_block_allocate")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_block_allocate
#pragma weak _txe_block_allocate_orig=_txe_block_allocate
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_block_allocate */

#ifdef __inside_txe_block_pool_create

#include "tx_api.h"
#include "tx_trace.h"
#include "tx_block_pool.h"

UINT _txe_block_pool_create(TX_BLOCK_POOL *pool_ptr, CHAR *name_ptr, ULONG block_size, VOID *pool_start, ULONG pool_size, UINT pool_control_block_size) __attribute__((weak));
UINT _txe_block_pool_create_orig(TX_BLOCK_POOL *pool_ptr, CHAR *name_ptr, ULONG block_size, VOID *pool_start, ULONG pool_size, UINT pool_control_block_size) __attribute__((alias("_txe_block_pool_create")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_block_pool_create
#pragma weak _txe_block_pool_create_orig=_txe_block_pool_create
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_block_pool_create */

#ifdef __inside_txe_block_pool_delete

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_timer.h"
#include "tx_block_pool.h"
#include "tx_trace.h"

UINT _txe_block_pool_delete(TX_BLOCK_POOL *pool_ptr) __attribute__((weak));
UINT _txe_block_pool_delete_orig(TX_BLOCK_POOL *pool_ptr) __attribute__((alias("_txe_block_pool_delete")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_block_pool_delete
#pragma weak _txe_block_pool_delete_orig=_txe_block_pool_delete
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_block_pool_delete */

#ifdef __inside_txe_block_pool_info_get

#include "tx_api.h"
#include "tx_block_pool.h"
#include "tx_trace.h"

UINT _txe_block_pool_info_get(TX_BLOCK_POOL *pool_ptr, CHAR **name, ULONG *available_blocks, ULONG *total_blocks, TX_THREAD **first_suspended, ULONG *suspended_count, TX_BLOCK_POOL **next_pool) __attribute__((weak));
UINT _txe_block_pool_info_get_orig(TX_BLOCK_POOL *pool_ptr, CHAR **name, ULONG *available_blocks, ULONG *total_blocks, TX_THREAD **first_suspended, ULONG *suspended_count, TX_BLOCK_POOL **next_pool) __attribute__((alias("_txe_block_pool_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_block_pool_info_get
#pragma weak _txe_block_pool_info_get_orig=_txe_block_pool_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_block_pool_info_get */

#ifdef __inside_tx_block_pool_performance_info_get

#include "tx_api.h"
#include "tx_block_pool.h"
#include "tx_trace.h"

UINT _tx_block_pool_performance_info_get(TX_BLOCK_POOL *pool_ptr, ULONG *allocates, ULONG *releases, ULONG *suspensions, ULONG *timeouts) __attribute__((weak));
UINT _tx_block_pool_performance_info_get_orig(TX_BLOCK_POOL *pool_ptr, ULONG *allocates, ULONG *releases, ULONG *suspensions, ULONG *timeouts) __attribute__((alias("_tx_block_pool_performance_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_block_pool_performance_info_get
#pragma weak _tx_block_pool_performance_info_get_orig=_tx_block_pool_performance_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_tx_block_pool_performance_info_get */

#ifdef __inside_tx_block_pool_performance_system_info_get

#include "tx_api.h"
#include "tx_block_pool.h"
#include "tx_trace.h"

UINT  _tx_block_pool_performance_system_info_get(ULONG *allocates, ULONG *releases, ULONG *suspensions, ULONG *timeouts) __attribute__((weak));
UINT  _tx_block_pool_performance_system_info_get_oirg(ULONG *allocates, ULONG *releases, ULONG *suspensions, ULONG *timeouts) __attribute__((alias("_tx_block_pool_performance_system_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_block_pool_performance_system_info_get
#pragma weak _tx_block_pool_performance_system_info_get_oirg=_tx_block_pool_performance_system_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /*__inside_tx_block_pool_performance_system_info_get */

#ifdef __inside_txe_block_pool_prioritize

#include "tx_api.h"
#include "tx_block_pool.h"
#include "tx_trace.h"

UINT _txe_block_pool_prioritize(TX_BLOCK_POOL *pool_ptr) __attribute__((weak));
UINT _txe_block_pool_prioritize_orig(TX_BLOCK_POOL *pool_ptr) __attribute__((alias("_txe_block_pool_prioritize")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_block_pool_prioritize
#pragma weak _txe_block_pool_prioritize_orig=_txe_block_pool_prioritize
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_block_pool_prioritize */

#ifdef __inside_txe_block_release

#include "tx_api.h"
#include "tx_block_pool.h"
#include "tx_trace.h"

UINT _txe_block_release(VOID *block_ptr) __attribute__((weak));
UINT _txe_block_release_orig(VOID *block_ptr) __attribute__((alias("_txe_block_release")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_block_release
#pragma weak _txe_block_release_orig=_txe_block_release
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_block_release */

/* Byte Pool tracing hooks */
#ifdef __inside_txe_byte_allocate

#include "tx_api.h"
#include "tx_initialize.h"
#include "tx_thread.h"
#include "tx_timer.h"
#include "tx_byte_pool.h"
#include "tx_trace.h"

UINT _txe_byte_allocate(TX_BYTE_POOL *pool_ptr, VOID **memory_ptr, ULONG memory_size,  ULONG wait_option) __attribute__((weak));
UINT _txe_byte_allocate_orig(TX_BYTE_POOL *pool_ptr, VOID **memory_ptr, ULONG memory_size,  ULONG wait_option) __attribute__((alias("_txe_byte_allocate")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_byte_allocate
#pragma weak _txe_byte_allocate_orig=_txe_byte_allocate
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_byte_allocate */

#ifdef __inside_txe_byte_pool_create

#include "tx_api.h"
#include "tx_initialize.h"
#include "tx_thread.h"
#include "tx_timer.h"
#include "tx_byte_pool.h"
#include "tx_trace.h"

UINT _txe_byte_pool_create(TX_BYTE_POOL *pool_ptr, CHAR *name_ptr, VOID *pool_start, ULONG pool_size, UINT pool_control_block_size) __attribute__((weak));
UINT _txe_byte_pool_create_orig(TX_BYTE_POOL *pool_ptr, CHAR *name_ptr, VOID *pool_start, ULONG pool_size, UINT pool_control_block_size)  __attribute__((alias("_txe_byte_pool_create")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_byte_pool_create
#pragma weak _txe_byte_pool_create_orig=_txe_byte_pool_create
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_byte_pool_create */

#ifdef __inside_txe_byte_pool_delete

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_timer.h"
#include "tx_byte_pool.h"
#include "tx_trace.h"

UINT _txe_byte_pool_delete(TX_BYTE_POOL *pool_ptr) __attribute__((weak));
UINT _txe_byte_pool_delete_orig(TX_BYTE_POOL *pool_ptr) __attribute__((alias("_txe_byte_pool_delete")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_byte_pool_delete
#pragma weak _txe_byte_pool_delete_orig=_txe_byte_pool_delete
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_byte_pool_delete */

#ifdef __inside_txe_byte_pool_info_get

#include "tx_api.h"
#include "tx_byte_pool.h"
#include "tx_trace.h"

UINT _txe_byte_pool_info_get(TX_BYTE_POOL *pool_ptr, CHAR **name, ULONG *available_bytes, ULONG *fragments, TX_THREAD **first_suspended, ULONG *suspended_count, TX_BYTE_POOL **next_pool) __attribute__((weak));
UINT _txe_byte_pool_info_get_orig(TX_BYTE_POOL *pool_ptr, CHAR **name, ULONG *available_bytes, ULONG *fragments, TX_THREAD **first_suspended, ULONG *suspended_count, TX_BYTE_POOL **next_pool) __attribute__((alias("_txe_byte_pool_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_byte_pool_info_get
#pragma weak _txe_byte_pool_info_get_orig=_txe_byte_pool_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_byte_pool_info_get */

#ifdef __inside_tx_byte_pool_performance_info_get

#include "tx_api.h"
#include "tx_byte_pool.h"
#include "tx_trace.h"

UINT _tx_byte_pool_performance_info_get(TX_BYTE_POOL *pool_ptr,
	    ULONG *allocates,
	    ULONG *releases,
	    ULONG *fragments_searched,
	    ULONG *merges,
	    ULONG *splits,
	    ULONG *suspensions,
	    ULONG *timeouts) __attribute__((weak));
UINT _tx_byte_pool_performance_info_get_orig(TX_BYTE_POOL *pool_ptr,
	    ULONG *allocates,
	    ULONG *releases,
	    ULONG *fragments_searched,
	    ULONG *merges,
	    ULONG *splits,
	    ULONG *suspensions,
	    ULONG *timeouts) __attribute__((alias("_tx_byte_pool_performance_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_byte_pool_performance_info_get
#pragma weak _tx_byte_pool_performance_info_get_orig=_tx_byte_pool_performance_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /*__inside_tx_byte_pool_performance_info_get */

#ifdef __inside_tx_byte_pool_performance_system_info_get

#include "tx_api.h"
#include "tx_byte_pool.h"
#include "tx_trace.h"

UINT  _tx_byte_pool_performance_system_info_get(ULONG *allocates,
		ULONG *releases,
		ULONG *fragments_searched,
		ULONG *merges,
		ULONG *splits,
		ULONG *suspensions,
		ULONG *timeouts) __attribute__((weak));
UINT  _tx_byte_pool_performance_system_info_get_orig(ULONG *allocates,
		ULONG *releases,
		ULONG *fragments_searched,
		ULONG *merges,
		ULONG *splits,
		ULONG *suspensions,
		ULONG *timeouts) __attribute__((alias("_tx_byte_pool_performance_system_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_byte_pool_performance_system_info_get
#pragma weak _tx_byte_pool_performance_system_info_get_orig=_tx_byte_pool_performance_system_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_tx_byte_pool_performance_system_info_get */

#ifdef __inside_txe_byte_pool_prioritize

#include "tx_api.h"
#include "tx_byte_pool.h"
#include "tx_trace.h"

UINT _txe_byte_pool_prioritize(TX_BYTE_POOL *pool_ptr) __attribute__((weak));
UINT _txe_byte_pool_prioritize_orig(TX_BYTE_POOL *pool_ptr) __attribute__((alias("_txe_byte_pool_prioritize")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_byte_pool_prioritize
#pragma weak _txe_byte_pool_prioritize_orig=_txe_byte_pool_prioritize
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_byte_pool_prioritize */

#ifdef __inside_txe_byte_release

#include "tx_api.h"
#include "tx_initialize.h"
#include "tx_thread.h"
#include "tx_timer.h"
#include "tx_byte_pool.h"

UINT _txe_byte_release(VOID *memory_ptr) __attribute__((weak));
UINT _txe_byte_release_orig(VOID *memory_ptr) __attribute__((alias("_txe_byte_release")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_byte_release
#pragma weak _txe_byte_release_orig=_txe_byte_release
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_byte_release */

#ifdef __inside_txe_event_flags_create

#include "tx_api.h"

UINT _txe_event_flags_create(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR *name_ptr, UINT event_control_block_size) __attribute__((weak));
UINT _txe_event_flags_create_orig(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR *name_ptr, UINT event_control_block_size)  __attribute__((alias("_txe_event_flags_create")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_event_flags_create
#pragma weak _txe_event_flags_create_orig=_txe_event_flags_create
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_event_flags_create */

#ifdef __inside_txe_event_flags_delete

#include "tx_api.h"

UINT _txe_event_flags_delete(TX_EVENT_FLAGS_GROUP *group_ptr) __attribute__((weak));
UINT _txe_event_flags_delete_orig(TX_EVENT_FLAGS_GROUP *group_ptr) __attribute__((alias("_txe_event_flags_delete")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_event_flags_delete
#pragma weak _txe_event_flags_delete_orig=_txe_event_flags_delete
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_event_flags_delete */

#ifdef __inside_txe_event_flags_get

#include "tx_api.h"

UINT _txe_event_flags_get(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG requested_flags, UINT get_option, ULONG *actual_flags_ptr, ULONG wait_option) __attribute__((weak));
UINT _txe_event_flags_get_orig(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG requested_flags, UINT get_option, ULONG *actual_flags_ptr, ULONG wait_option) __attribute__((alias("_txe_event_flags_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_event_flags_get
#pragma weak _txe_event_flags_get_orig=_txe_event_flags_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_event_flags_get */

#ifdef __inside_txe_event_flags_info_get

#include "tx_api.h"

UINT _txe_event_flags_info_get(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR **name, ULONG *current_flags, TX_THREAD **first_suspended, ULONG *suspended_count, TX_EVENT_FLAGS_GROUP **next_group) __attribute__((weak));
UINT _txe_event_flags_info_get_orig(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR **name, ULONG *current_flags, TX_THREAD **first_suspended, ULONG *suspended_count, TX_EVENT_FLAGS_GROUP **next_group) __attribute__((alias("_txe_event_flags_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_event_flags_info_get
#pragma weak _txe_event_flags_info_get_orig=_txe_event_flags_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_event_flags_performance_system_info_get */

#ifdef __inside_tx_event_flags_performance_info_get

#include "tx_api.h"

UINT _tx_event_flags_performance_info_get(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG *sets, ULONG *gets, ULONG *suspensions, ULONG *timeouts) __attribute__((weak));
UINT _tx_event_flags_performance_info_get_orig(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG *sets, ULONG *gets, ULONG *suspensions, ULONG *timeouts) __attribute__((alias("_tx_event_flags_performance_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_event_flags_performance_info_get
#pragma weak _tx_event_flags_performance_info_get_orig=_tx_event_flags_performance_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_event_flags_performance_info_get */

#ifdef __inside_tx_event_flags_performance_system_info_get

#include "tx_api.h"
#include "tx_event_flags.h"
#include "tx_trace.h"

UINT  _tx_event_flags_performance_system_info_get(ULONG *sets, ULONG *gets, ULONG *suspensions, ULONG *timeouts) __attribute__((weak));
UINT  _tx_event_flags_performance_system_info_get_orig(ULONG *sets, ULONG *gets, ULONG *suspensions, ULONG *timeouts) __attribute__((alias("_tx_event_flags_performance_system_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_event_flags_performance_system_info_get
#pragma weak _tx_event_flags_performance_system_info_get_orig=_tx_event_flags_performance_system_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_tx_event_flags_performance_ssytem_info_get */

#ifdef __inside_txe_event_flags_set

#include "tx_api.h"

UINT  _txe_event_flags_set(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG flags_to_set, UINT set_option) __attribute__((weak));
UINT  _txe_event_flags_set_orig(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG flags_to_set, UINT set_option) __attribute__((alias("_txe_event_flags_set")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_event_flags_set
#pragma weak _txe_event_flags_set_orig=_txe_event_flags_set
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_event_flags_set */

#ifdef __inside_txe_event_flags_set_notify

#include "tx_api.h"

UINT _txe_event_flags_set_notify(TX_EVENT_FLAGS_GROUP *group_ptr, VOID (*events_set_notify)(TX_EVENT_FLAGS_GROUP *notify_group_ptr)) __attribute__((weak));
UINT _txe_event_flags_set_notify_orig(TX_EVENT_FLAGS_GROUP *group_ptr, VOID (*events_set_notify)(TX_EVENT_FLAGS_GROUP *notify_group_ptr)) __attribute((alias("_txe_event_flags_set_notify")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_event_flags_set_notify
#pragma weak _txe_event_flags_set_notify_orig=_txe_event_flags_set_notify
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_event_flags_set_notify */


/* Mutex tracing hooks */
#ifdef __inside_txe_mutex_create

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_trace.h"
#include "tx_mutex.h"

UINT _txe_mutex_create(TX_MUTEX *mutex_ptr, CHAR *name_ptr, UINT inherit, UINT mutex_control_block_size) __attribute__((weak));
UINT _txe_mutex_create_orig(TX_MUTEX *mutex_ptr, CHAR *name_ptr, UINT inherit, UINT mutex_control_block_size) __attribute__((alias("_txe_mutex_create")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_mutex_create
#pragma weak _txe_mutex_create_orig=_txe_mutex_create
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __insider_txe_mutex_create */

#ifdef __inside_txe_mutex_delete

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_timer.h"
#include "tx_mutex.h"
#include "tx_trace.h"

UINT _txe_mutex_delete(TX_MUTEX *mutex_ptr) __attribute__((weak));
UINT _txe_mutex_delete_orig(TX_MUTEX *mutex_ptr) __attribute__((alias("_txe_mutex_delete")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_mutex_delete
#pragma weak _txe_mutex_delete_orig=_txe_mutex_delete
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_mutex_delete */

#ifdef __inside_txe_mutex_get

#include "tx_api.h"
#include "tx_initialize.h"
#include "tx_thread.h"
#ifndef TX_TIMER_PROCESS_IN_ISR
#include "tx_timer.h"
#endif
#include "tx_mutex.h"
#include "tx_trace.h"

UINT _txe_mutex_get(TX_MUTEX *mutex_ptr, ULONG wait_option) __attribute__((weak));
UINT _txe_mutex_get_orig(TX_MUTEX *mutex_ptr, ULONG wait_option) __attribute__((alias("_txe_mutex_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_mutex_get
#pragma weak _txe_mutex_get_orig=_txe_mutex_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_mutex_get */

#ifdef __inside_txe_mutex_info_get

#include "tx_api.h"
#include "tx_mutex.h"
#include "tx_trace.h"

UINT _txe_mutex_info_get(TX_MUTEX *mutex_ptr, CHAR **name, ULONG *count, TX_THREAD **owner, TX_THREAD **first_suspended, ULONG *suspended_count, TX_MUTEX **next_mutex) __attribute__((weak));
UINT _txe_mutex_info_get_orig(TX_MUTEX *mutex_ptr, CHAR **name, ULONG *count, TX_THREAD **owner, TX_THREAD **first_suspended, ULONG *suspended_count, TX_MUTEX **next_mutex) __attribute__((alias("_txe_mutex_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_mutex_info_get
#pragma weak _txe_mutex_info_get_orig=_txe_mutex_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_mutex_info_get */

#ifdef __inside_tx_mutex_performance_info_get

#include "tx_api.h"
#include "tx_mutex.h"
#include "tx_trace.h"

UINT _tx_mutex_performance_info_get(TX_MUTEX *mutex_ptr, ULONG *puts, ULONG *gets, ULONG *suspensions, ULONG *timeouts, ULONG *inversions, ULONG *inheritances) __attribute__((weak));
UINT _tx_mutex_performance_info_get_orig(TX_MUTEX *mutex_ptr, ULONG *puts, ULONG *gets, ULONG *suspensions, ULONG *timeouts, ULONG *inversions, ULONG *inheritances) __attribute__((alias("_tx_mutex_performance_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_mutex_performance_info_get
#pragma weak _tx_mutex_performance_info_get_orig=_tx_mutex_performance_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_tx_mutex_performance_info_get */

#ifdef __inside_tx_mutex_performance_system_info_get

#include "tx_api.h"
#include "tx_mutex.h"
#include "tx_trace.h"

UINT  _tx_mutex_performance_system_info_get(ULONG *puts, ULONG *gets, ULONG *suspensions,
                                ULONG *timeouts, ULONG *inversions, ULONG *inheritances) __attribute__((weak));
UINT  _tx_mutex_performance_system_info_get_oirg(ULONG *puts, ULONG *gets, ULONG *suspensions,
                                ULONG *timeouts, ULONG *inversions, ULONG *inheritances) __attribute__((alias("_tx_mutex_performance_system_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_mutex_performance_system_info_get
#pragma weak _tx_mutex_performance_system_info_get_oirg=_tx_mutex_performance_system_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_tx_mutex_performance_system_info_get */

#ifdef __inside_txe_mutex_prioritize

#include "tx_api.h"
#include "tx_mutex.h"
#include "tx_trace.h"

UINT _txe_mutex_prioritize(TX_MUTEX *mutex_ptr) __attribute__((weak));
UINT _txe_mutex_prioritize_orig(TX_MUTEX *mutex_ptr) __attribute__((alias("_txe_mutex_prioritize")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_mutex_prioritize
#pragma weak _txe_mutex_prioritize_orig=_txe_mutex_prioritize
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_mutex_prioritize */

#ifdef __inside_txe_mutex_put

#include "tx_api.h"
#include "tx_initialize.h"
#include "tx_thread.h"
#include "tx_mutex.h"

UINT _txe_mutex_put(TX_MUTEX *mutex_ptr) __attribute__((weak));
UINT _txe_mutex_put_orig(TX_MUTEX *mutex_ptr) __attribute__((alias("_txe_mutex_put")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_mutex_put
#pragma weak _txe_mutex_put_orig=_txe_mutex_put
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_mutex_put */


/* Queue tracing hooks */
#ifdef __inside_txe_queue_create

#include "tx_api.h"
#include "tx_trace.h"
#include "tx_queue.h"

UINT _txe_queue_create(TX_QUEUE *queue_ptr, CHAR *name_ptr, UINT message_size, VOID *queue_start, ULONG queue_size, UINT queue_control_block_size) __attribute__((weak));
UINT _txe_queue_create_orig(TX_QUEUE *queue_ptr, CHAR *name_ptr, UINT message_size, VOID *queue_start, ULONG queue_size, UINT queue_control_block_size) __attribute__((alias("_txe_queue_create")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_queue_create
#pragma weak _txe_queue_create_orig=_txe_queue_create
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_queue_create */

#ifdef __inside_txe_queue_delete

#include "tx_api.h"
#include "tx_timer.h"
#include "tx_thread.h"
#include "tx_queue.h"
#include "tx_trace.h"

UINT _txe_queue_delete(TX_QUEUE *queue_ptr) __attribute__((weak));
UINT _txe_queue_delete_orig(TX_QUEUE *queue_ptr) __attribute__((alias("_txe_queue_delete")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_queue_delete
#pragma weak _txe_queue_delete_orig=_txe_queue_delete
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_queue_delete */

#ifdef __inside_txe_queue_flush

#include "tx_api.h"
#include "tx_queue.h"

UINT _txe_queue_flush(TX_QUEUE *queue_ptr) __attribute__((weak));
UINT _txe_queue_flush_orig(TX_QUEUE *queue_ptr) __attribute__((alias("_txe_queue_flush")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_queue_flush
#pragma weak _txe_queue_flush_orig=_txe_queue_flush
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_queue_flush */

#ifdef __inside_txe_queue_front_send

#include "tx_api.h"
#include "tx_timer.h"
#include "tx_thread.h"
#include "tx_queue.h"

UINT _txe_queue_front_send(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option) __attribute__((weak));
UINT _txe_queue_front_send_orig(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option) __attribute__((alias("_txe_queue_front_send")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_queue_front_send
#pragma weak _txe_queue_front_send_orig=_txe_queue_front_send
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_queue_front_send */

#ifdef __inside_txe_queue_info_get

#include "tx_api.h"
#include "tx_queue.h"
#include "tx_trace.h"

UINT _txe_queue_info_get(TX_QUEUE *queue_ptr, CHAR **name, ULONG *enqueued, ULONG *available_storage, TX_THREAD **first_suspended, ULONG *suspended_count, TX_QUEUE **next_queue) __attribute__((weak));
UINT _txe_queue_info_get_orig(TX_QUEUE *queue_ptr, CHAR **name, ULONG *enqueued, ULONG *available_storage, TX_THREAD **first_suspended, ULONG *suspended_count, TX_QUEUE **next_queue) __attribute__((alias("_txe_queue_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_queue_info_get
#pragma weak _txe_queue_info_get_orig=_txe_queue_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_queue_info_get */

#ifdef __inside_tx_queue_performance_info_get

#include "tx_api.h"
#include "tx_queue.h"
#include "tx_trace.h"

UINT _tx_queue_performance_info_get(TX_QUEUE *queue_ptr, ULONG *messages_sent, ULONG *messages_received,
                    ULONG *empty_suspensions, ULONG *full_suspensions, ULONG *full_errors, ULONG *timeouts) __attribute__((weak));
UINT _tx_queue_performance_info_get_orig(TX_QUEUE *queue_ptr, ULONG *messages_sent, ULONG *messages_received,
                    ULONG *empty_suspensions, ULONG *full_suspensions, ULONG *full_errors, ULONG *timeouts) __attribute__((alias("_tx_queue_performance_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_queue_performance_info_get
#pragma weak _tx_queue_performance_info_get_orig=_tx_queue_performance_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_tx_queue_performance_info_get */

#ifdef __inside_tx_queue_performance_system_info_get

#include "tx_api.h"
#include "tx_queue.h"
#include "tx_trace.h"

UINT  _tx_queue_performance_system_info_get(ULONG *messages_sent, ULONG *messages_received,
                    ULONG *empty_suspensions, ULONG *full_suspensions, ULONG *full_errors, ULONG *timeouts) __attribute__((weak));
UINT  _tx_queue_performance_system_info_get_orig(ULONG *messages_sent, ULONG *messages_received,
                    ULONG *empty_suspensions, ULONG *full_suspensions, ULONG *full_errors, ULONG *timeouts) __attribute__((alias("_tx_queue_performance_system_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_queue_performance_system_info_get
#pragma weak _tx_queue_performance_system_info_get_orig=_tx_queue_performance_system_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_tx_queue_performance_system_info_get */

#ifdef __inside_txe_queue_prioritize

#include "tx_api.h"
#include "tx_queue.h"
#include "tx_trace.h"

UINT _txe_queue_prioritize(TX_QUEUE *queue_ptr) __attribute__((weak));
UINT _txe_queue_prioritize_orig(TX_QUEUE *queue_ptr) __attribute__((alias("_txe_queue_prioritize")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_queue_prioritize
#pragma weak _txe_queue_prioritize_orig=_txe_queue_prioritize
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_queue_prioritize */

#ifdef __inside_txe_queue_receive

#include "tx_api.h"
#include "tx_timer.h"
#include "tx_thread.h"
#include "tx_queue.h"

UINT _txe_queue_receive(TX_QUEUE *queue_ptr, VOID *destination_ptr, ULONG wait_option) __attribute__((weak));
UINT _txe_queue_receive_orig(TX_QUEUE *queue_ptr, VOID *destination_ptr, ULONG wait_option) __attribute__((alias("_txe_queue_receive")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_queue_receive
#pragma weak _txe_queue_receive_orig=_txe_queue_receive
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_queue_receive */

#ifdef __inside_txe_queue_send_notify

#include "tx_api.h"
#include "tx_queue.h"

UINT _txe_queue_send_notify(TX_QUEUE *queue_ptr, VOID (*queue_send_notify)(TX_QUEUE *notify_queue_ptr)) __attribute__((weak));
UINT _txe_queue_send_notify_orig(TX_QUEUE *queue_ptr, VOID (*queue_send_notify)(TX_QUEUE *notify_queue_ptr)) __attribute__((alias("_txe_queue_send_notify")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_queue_send_notify
#pragma weak _txe_queue_send_notify_orig=_txe_queue_send_notify
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_queue_send_notify */

#ifdef __inside_txe_queue_send

#include "tx_api.h"
#include "tx_timer.h"
#include "tx_thread.h"
#include "tx_queue.h"

UINT _txe_queue_send(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option) __attribute__((weak));
UINT _txe_queue_send_orig(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option) __attribute__((alias("_txe_queue_send")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_queue_send
#pragma weak _txe_queue_send_orig=_txe_queue_send
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_queue_send */

/* Semaphore tracing hooks */
#ifdef __inside_txe_semaphore_ceiling_put

#include "tx_api.h"
#include "tx_semaphore.h"

UINT _txe_semaphore_ceiling_put(TX_SEMAPHORE *semaphore_ptr, ULONG ceiling) __attribute__((weak));
UINT _txe_semaphore_ceiling_put_orig(TX_SEMAPHORE *semaphore_ptr, ULONG ceiling) __attribute__((alias("_txe_semaphore_ceiling_put")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_semaphore_ceiling_put
#pragma weak _txe_semaphore_ceiling_put_orig=_txe_semaphore_ceiling_put
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_semaphore_ceiling_put */

#ifdef __inside_txe_semaphore_create

#include "tx_api.h"
#include "tx_trace.h"
#include "tx_semaphore.h"

UINT _txe_semaphore_create(TX_SEMAPHORE *semaphore_ptr, CHAR *name_ptr, ULONG initial_count, UINT semaphore_control_block_size) __attribute__((weak));
UINT _txe_semaphore_create_orig(TX_SEMAPHORE *semaphore_ptr, CHAR *name_ptr, ULONG initial_count, UINT semaphore_control_block_size) __attribute__((alias("_txe_semaphore_create")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_semaphore_create
#pragma weak _txe_semaphore_create_orig=_txe_semaphore_create
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_semaphore_create */

#ifdef __inside_txe_semaphore_delete

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_timer.h"
#include "tx_semaphore.h"
#include "tx_trace.h"

UINT _txe_semaphore_delete(TX_SEMAPHORE *semaphore_ptr) __attribute__((weak));
UINT _txe_semaphore_delete_orig(TX_SEMAPHORE *semaphore_ptr) __attribute__((alias("_txe_semaphore_delete")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_semaphore_delete
#pragma weak _txe_semaphore_delete_orig=_txe_semaphore_delete
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_semaphore_delete */

#ifdef __inside_txe_semaphore_get

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_timer.h"
#include "tx_semaphore.h"
#include "tx_trace.h"

UINT  _txe_semaphore_get(TX_SEMAPHORE *semaphore_ptr, ULONG wait_option) __attribute__((weak));
UINT  _txe_semaphore_get_orig(TX_SEMAPHORE *semaphore_ptr, ULONG wait_option) __attribute__((alias("_txe_semaphore_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_semaphore_get
#pragma weak _txe_semaphore_get_orig=_txe_semaphore_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_semaphore_get */

#ifdef __inside_txe_semaphore_info_get

#include "tx_api.h"
#include "tx_semaphore.h"
#include "tx_trace.h"

UINT  _txe_semaphore_info_get(TX_SEMAPHORE *semaphore_ptr, CHAR **name, ULONG *current_value, TX_THREAD **first_suspended, ULONG *suspended_count, TX_SEMAPHORE **next_semaphore) __attribute__((weak));
UINT  _txe_semaphore_info_get_orig(TX_SEMAPHORE *semaphore_ptr, CHAR **name, ULONG *current_value, TX_THREAD **first_suspended, ULONG *suspended_count, TX_SEMAPHORE **next_semaphore) __attribute__((alias("_txe_semaphore_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_semaphore_info_get
#pragma weak _txe_semaphore_info_get_orig=_txe_semaphore_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_semaphore_info_get */

#ifdef __inside_tx_semaphore_performance_info_get

#include "tx_api.h"
#include "tx_semaphore.h"
#include "tx_trace.h"

UINT _tx_semaphore_performance_info_get(TX_SEMAPHORE *semaphore_ptr, ULONG *puts, ULONG *gets,
                    ULONG *suspensions, ULONG *timeouts) __attribute__((weak));
UINT _tx_semaphore_performance_info_get_orig(TX_SEMAPHORE *semaphore_ptr, ULONG *puts, ULONG *gets,
                    ULONG *suspensions, ULONG *timeouts) __attribute__((alias("_tx_semaphore_performance_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_semaphore_performance_info_get
#pragma weak _tx_semaphore_performance_info_get_orig=_tx_semaphore_performance_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_tx_semaphore_performance_info_get */

#ifdef __inside_tx_semaphore_performance_system_info_get

#include "tx_api.h"
#include "tx_semaphore.h"
#include "tx_trace.h"

UINT  _tx_semaphore_performance_system_info_get(ULONG *puts, ULONG *gets, ULONG *suspensions, ULONG *timeouts) __attribute__((weak));
UINT  _tx_semaphore_performance_system_info_get_oirg(ULONG *puts, ULONG *gets, ULONG *suspensions, ULONG *timeouts) __attribute__((alias("_tx_semaphore_performance_system_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_semaphore_performance_system_info_get
#pragma weak _tx_semaphore_performance_system_info_get_oirg=_tx_semaphore_performance_system_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_tx_semaphore_preformance_system_info_get */

#ifdef __inside_txe_semaphore_prioritize

#include "tx_api.h"
#include "tx_semaphore.h"
#include "tx_trace.h"

UINT _txe_semaphore_prioritize(TX_SEMAPHORE *semaphore_ptr) __attribute__((weak));
UINT _txe_semaphore_prioritize_orig(TX_SEMAPHORE *semaphore_ptr) __attribute__((alias("_txe_semaphore_prioritize")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_semaphore_prioritize
#pragma weak _txe_semaphore_prioritize_orig=_txe_semaphore_prioritize
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_semaphore_prioritize */

#ifdef __inside_txe_semaphore_put_notify

#include "tx_api.h"
#include "tx_semaphore.h"

UINT _txe_semaphore_put_notify(TX_SEMAPHORE *semaphore_ptr, VOID (*semaphore_put_notify)(TX_SEMAPHORE *notify_semaphore_ptr)) __attribute__((weak));
UINT _txe_semaphore_put_notify_orig(TX_SEMAPHORE *semaphore_ptr, VOID (*semaphore_put_notify)(TX_SEMAPHORE *notify_semaphore_ptr)) __attribute__((alias("_txe_semaphore_put_notify")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_semaphore_put_notify
#pragma weak _txe_semaphore_put_notify_orig=_txe_semaphore_put_notify
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_semaphore_put_notify */

#ifdef __inside_txe_semaphore_put

#include "tx_api.h"
#include "tx_semaphore.h"

UINT _txe_semaphore_put(TX_SEMAPHORE *semaphore_ptr) __attribute__((weak));
UINT _txe_semaphore_put_orig(TX_SEMAPHORE *semaphore_ptr) __attribute__((alias("_txe_semaphore_put")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_semaphore_put
#pragma weak _txe_semaphore_put_orig=_txe_semaphore_put
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_semaphore_put */


/* Timer tracing hooks */
#ifdef __inside_tx_time_get

#include "tx_api.h"
#include "tx_timer.h"

ULONG _tx_time_get(VOID) __attribute__((weak));
ULONG _tx_time_get_orig(VOID) __attribute__((alias("_tx_time_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_time_get
#pragma weak _tx_time_get_orig=_tx_time_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_tx_time_get */

#ifdef __inside_tx_time_set

#include "tx_api.h"
#include "tx_timer.h"

VOID _tx_time_set(ULONG new_time) __attribute((weak));
VOID _tx_time_set_orig(ULONG new_time) __attribute((alias("_tx_time_set")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_time_set
#pragma weak _tx_time_set_orig=_tx_time_set
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_tx_time_set */

#ifdef __inside_txe_timer_activate

#include "tx_api.h"
#include "tx_timer.h"

UINT _txe_timer_activate(TX_TIMER *timer_ptr) __attribute__((weak));
UINT _txe_timer_activate_orig(TX_TIMER *timer_ptr) __attribute__((alias("_txe_timer_activate")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_timer_activate
#pragma weak _txe_timer_activate_orig=_txe_timer_activate
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_timer_active */

#ifdef __inside_txe_timer_change

#include "tx_api.h"
#include "tx_initialize.h"
#include "tx_thread.h"
#include "tx_timer.h"

UINT _txe_timer_change(TX_TIMER *timer_ptr, ULONG initial_ticks, ULONG reschedule_ticks) __attribute__((weak));
UINT _txe_timer_change_orig(TX_TIMER *timer_ptr, ULONG initial_ticks, ULONG reschedule_ticks) __attribute__((alias("_txe_timer_change")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_timer_change
#pragma weak _txe_timer_change_orig=_txe_timer_change
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_timer_change */

#ifdef __inside_txe_timer_create

#include "tx_api.h"
#include "tx_trace.h"
#include "tx_timer.h"

UINT _txe_timer_create(TX_TIMER *timer_ptr, CHAR *name_ptr,
            VOID (*expiration_function)(ULONG id), ULONG expiration_input,
            ULONG initial_ticks, ULONG reschedule_ticks, UINT auto_activate, UINT timer_control_block_size) __attribute__((weak));
UINT _txe_timer_create_orig(TX_TIMER *timer_ptr, CHAR *name_ptr,
            VOID (*expiration_function)(ULONG id), ULONG expiration_input,
            ULONG initial_ticks, ULONG reschedule_ticks, UINT auto_activate, UINT timer_control_block_size) __attribute__((alias("_txe_timer_create")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_timer_create
#pragma weak _txe_timer_create_orig=_txe_timer_create
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_timer_create */

#ifdef __inside_txe_timer_deactivate

#include "tx_api.h"
#include "tx_timer.h"

UINT  _txe_timer_deactivate(TX_TIMER *timer_ptr) __attribute__((weak));
UINT  _txe_timer_deactivate_orig(TX_TIMER *timer_ptr) __attribute__((alias("_txe_timer_deactivate")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_timer_deactivate
#pragma weak _txe_timer_deactivate_orig=_txe_timer_deactivate
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_timer_deactivate */

#ifdef __inside_txe_timer_delete

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_timer.h"
#include "tx_trace.h"

UINT  _txe_timer_delete(TX_TIMER *timer_ptr) __attribute__((weak));
UINT  _txe_timer_delete_orig(TX_TIMER *timer_ptr) __attribute__((alias("_txe_timer_delete")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_timer_delete
#pragma weak _txe_timer_delete_orig=_txe_timer_delete
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_timer_delete */

#ifdef __inside_txe_timer_info_get

#include "tx_api.h"
#include "tx_timer.h"
#include "tx_trace.h"

UINT _txe_timer_info_get(TX_TIMER *timer_ptr, CHAR **name, UINT *active, ULONG *remaining_ticks, ULONG *reschedule_ticks, TX_TIMER **next_timer) __attribute__((weak));
UINT _txe_timer_info_get_orig(TX_TIMER *timer_ptr, CHAR **name, UINT *active, ULONG *remaining_ticks, ULONG *reschedule_ticks, TX_TIMER **next_timer) __attribute__((alias("_txe_timer_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_timer_info_get
#pragma weak _txe_timer_info_get_orig=_txe_timer_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_timer_info_get */

#ifdef __inside_tx_timer_performance_info_get

#include "tx_api.h"
#include "tx_timer.h"
#include "tx_trace.h"

UINT _tx_timer_performance_info_get(TX_TIMER *timer_ptr, ULONG *activates, ULONG *reactivates, ULONG *deactivates, ULONG *expirations, ULONG *expiration_adjusts) __attribute__((weak));
UINT _tx_timer_performance_info_get_orig(TX_TIMER *timer_ptr, ULONG *activates, ULONG *reactivates, ULONG *deactivates, ULONG *expirations, ULONG *expiration_adjusts) __attribute__((alias("_tx_timer_performance_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_timer_performance_info_get
#pragma weak _tx_timer_performance_info_get_orig=_tx_timer_performance_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_tx_performance_info_get */

#endif /* TRC_CFG_SCHEDULING_ONLY */

/* Thread tracing hooks */
#ifdef __inside_txe_thread_create

#include "tx_api.h"
#include "tx_trace.h"
#include "tx_thread.h"
#include "tx_initialize.h"

UINT _txe_thread_create(TX_THREAD *thread_ptr, CHAR *name_ptr, VOID (*entry_function)(ULONG id), ULONG entry_input,
        VOID *stack_start, ULONG stack_size, UINT priority, UINT preempt_threshold,
        ULONG time_slice, UINT auto_start, UINT thread_control_block_size) __attribute__((weak));
UINT _txe_thread_create_orig(TX_THREAD *thread_ptr, CHAR *name_ptr, VOID (*entry_function)(ULONG id), ULONG entry_input,
                            VOID *stack_start, ULONG stack_size, UINT priority, UINT preempt_threshold,
                            ULONG time_slice, UINT auto_start, UINT thread_control_block_size) __attribute__((alias("_txe_thread_create")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_thread_create
#pragma weak _txe_thread_create_orig=_txe_thread_create
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_thread_create */

#ifdef __inside_txe_thread_delete

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_trace.h"

UINT _txe_thread_delete(TX_THREAD *thread_ptr) __attribute__((weak));
UINT _txe_thread_delete_orig(TX_THREAD *thread_ptr) __attribute__((alias("_txe_thread_delete")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_thread_delete
#pragma weak _txe_thread_delete_orig=_txe_thread_delete
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_thread_delete */

#ifdef __inside_txe_thread_entry_exit_notify

#include "tx_api.h"
#include "tx_thread.h"

UINT _txe_thread_entry_exit_notify(TX_THREAD *thread_ptr, VOID (*thread_entry_exit_notify)(TX_THREAD *notify_thread_ptr, UINT type)) __attribute__((weak));
UINT _txe_thread_entry_exit_notify_orig(TX_THREAD *thread_ptr, VOID (*thread_entry_exit_notify)(TX_THREAD *notify_thread_ptr, UINT type)) __attribute__((alias("_txe_thread_entry_exit_notify")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_thread_entry_exit_notify
#pragma weak _txe_thread_entry_exit_notify_orig=_txe_thread_entry_exit_notify
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_thread_entry_exit_notify */

#ifdef __inside_txe_thread_info_get

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_trace.h"

UINT _txe_thread_info_get(TX_THREAD *thread_ptr, CHAR **name, UINT *state, ULONG *run_count, UINT *priority, UINT *preemption_threshold, ULONG *time_slice, TX_THREAD **next_thread, TX_THREAD **next_suspended_thread) __attribute__((weak));
UINT _txe_thread_info_get_orig(TX_THREAD *thread_ptr, CHAR **name, UINT *state, ULONG *run_count, UINT *priority, UINT *preemption_threshold, ULONG *time_slice, TX_THREAD **next_thread, TX_THREAD **next_suspended_thread) __attribute__((alias("_txe_thread_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_thread_info_get
#pragma weak _txe_thread_info_get_orig=_txe_thread_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_thread_info_get */

#ifdef __inside_tx_thread_performance_info_get

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_trace.h"

UINT _tx_thread_performance_info_get(TX_THREAD *thread_ptr, ULONG *resumptions, ULONG *suspensions,
                ULONG *solicited_preemptions, ULONG *interrupt_preemptions, ULONG *priority_inversions,
                ULONG *time_slices, ULONG *relinquishes, ULONG *timeouts, ULONG *wait_aborts, TX_THREAD **last_preempted_by) __attribute__((weak));
UINT _tx_thread_performance_info_get_orig(TX_THREAD *thread_ptr, ULONG *resumptions, ULONG *suspensions,
                ULONG *solicited_preemptions, ULONG *interrupt_preemptions, ULONG *priority_inversions,
                ULONG *time_slices, ULONG *relinquishes, ULONG *timeouts, ULONG *wait_aborts, TX_THREAD **last_preempted_by) __attribute__((alias("_tx_thread_performance_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_thread_performance_info_get
#pragma weak _tx_thread_performance_info_get_orig=_tx_thread_performance_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_thread_info_get */

#ifdef __inside_tx_thread_performance_system_info_get

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_trace.h"

UINT  _tx_thread_performance_system_info_get(ULONG *resumptions, ULONG *suspensions,
                ULONG *solicited_preemptions, ULONG *interrupt_preemptions, ULONG *priority_inversions,
                ULONG *time_slices, ULONG *relinquishes, ULONG *timeouts, ULONG *wait_aborts,
                ULONG *non_idle_returns, ULONG *idle_returns) __attribute__((weak));
UINT  _tx_thread_performance_system_info_get_oirg(ULONG *resumptions, ULONG *suspensions,
                ULONG *solicited_preemptions, ULONG *interrupt_preemptions, ULONG *priority_inversions,
                ULONG *time_slices, ULONG *relinquishes, ULONG *timeouts, ULONG *wait_aborts,
                ULONG *non_idle_returns, ULONG *idle_returns) __attribute__((alias("_tx_thread_performance_system_info_get")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_thread_performance_system_info_get
#pragma weak _tx_thread_performance_system_info_get_oirg=_tx_thread_performance_system_info_get
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_tx_thread_performance_system_info_get */

#ifdef __inside_txe_thread_preemption_change

#include "tx_api.h"
#include "tx_thread.h"

UINT _txe_thread_preemption_change(TX_THREAD *thread_ptr, UINT new_threshold, UINT *old_threshold) __attribute__((weak));
UINT _txe_thread_preemption_change_orig(TX_THREAD *thread_ptr, UINT new_threshold, UINT *old_threshold) __attribute__((alias("_txe_thread_preemption_change")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_thread_preemption_change
#pragma weak _txe_thread_preemption_change_orig=_txe_thread_preemption_change
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_thread_preemption_change */

#ifdef __inside_txe_thread_priority_change

#include "tx_api.h"
#include "tx_thread.h"

UINT _txe_thread_priority_change(TX_THREAD *thread_ptr, UINT new_priority, UINT *old_priority) __attribute__((weak));
UINT _txe_thread_priority_change_orig(TX_THREAD *thread_ptr, UINT new_priority, UINT *old_priority) __attribute__((alias("_txe_thread_priority_change")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_thread_priority_change
#pragma weak _txe_thread_priority_change_orig=_txe_thread_priority_change
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_thread_priority_change */

#ifdef __inside_txe_thread_reset

#include "tx_api.h"
#include "tx_thread.h"
#include "tx_timer.h"

UINT _txe_thread_reset(TX_THREAD *thread_ptr) __attribute__((weak));
UINT _txe_thread_reset_orig(TX_THREAD *thread_ptr) __attribute__((alias("_txe_thread_reset")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_thread_reset
#pragma weak _txe_thread_reset_orig=_txe_thread_reset
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_thread_reset */

#ifdef __inside_txe_thread_resume

#include "tx_api.h"
#include "tx_thread.h"

UINT _txe_thread_resume(TX_THREAD *thread_ptr) __attribute__((weak));
UINT _txe_thread_resume_orig(TX_THREAD *thread_ptr) __attribute__((alias("_txe_thread_resume")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_thread_resume
#pragma weak _txe_thread_resume_orig=_txe_thread_resume
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_thread_resume */

#ifdef __inside_tx_thread_sleep

#include "tx_api.h"
#include "tx_thread.h"

UINT _tx_thread_sleep(ULONG timer_ticks) __attribute__((weak));
UINT _tx_thread_sleep_orig(ULONG timer_ticks) __attribute__((alias("_tx_thread_sleep")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_thread_sleep
#pragma weak _tx_thread_sleep_orig=_tx_thread_sleep
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_thread_sleep */

#ifdef __inside_tx_thread_stack_error_notify

#include "tx_api.h"
#include "tx_thread.h"

UINT _tx_thread_stack_error_notify(VOID (*stack_error_handler)(TX_THREAD *thread_ptr)) __attribute__((weak));
UINT _tx_thread_stack_error_notify_orig(VOID (*stack_error_handler)(TX_THREAD *thread_ptr)) __attribute__((alias("_tx_thread_stack_error_notify")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_thread_stack_error_notify
#pragma weak _tx_thread_stack_error_notify_orig=_tx_thread_stack_error_notify
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* _tx_thread_stack_error_notify */

#ifdef __inside_txe_thread_suspend

#include "tx_api.h"
#include "tx_thread.h"

UINT _txe_thread_suspend(TX_THREAD *thread_ptr) __attribute__((weak));
UINT _txe_thread_suspend_orig(TX_THREAD *thread_ptr) __attribute__((alias("_txe_thread_suspend")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_thread_suspend
#pragma weak _txe_thread_suspend_orig=_txe_thread_suspend
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_thread_suspend */

#ifdef __inside_txe_thread_terminate

#include "tx_api.h"
#include "tx_thread.h"

UINT _txe_thread_terminate(TX_THREAD *thread_ptr) __attribute__((weak));
UINT _txe_thread_terminate_orig(TX_THREAD *thread_ptr) __attribute__((alias("_txe_thread_terminate")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_thread_terminate
#pragma weak _txe_thread_terminate_orig=_txe_thread_terminate
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_thread_terminate */

#ifdef __inside_tx_thread_time_slice

#include "tx_api.h"
#include "tx_thread.h"

VOID _tx_thread_time_slice(VOID) __attribute__((weak));
VOID _tx_thread_time_slice_orig(VOID) __attribute((alias("_tx_thread_time_slice")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _tx_thread_time_slice
#pragma weak _tx_thread_time_slice_orig=_tx_thread_time_slice
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_tx_thread_time_slice */

#ifdef __inside_txe_thread_time_slice_change

#include "tx_api.h"
#include "tx_thread.h"

UINT _txe_thread_time_slice_change(TX_THREAD *thread_ptr, ULONG new_time_slice, ULONG *old_time_slice) __attribute__((weak));
UINT _txe_thread_time_slice_change_orig(TX_THREAD *thread_ptr, ULONG new_time_slice, ULONG *old_time_slice) __attribute__((alias("_txe_thread_time_slice_change")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_thread_time_slice_change
#pragma weak _txe_thread_time_slice_change_orig=_txe_thread_time_slice_change
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_thread_time_slice_change */

#ifdef __inside_txe_thread_wait_abort

#include "tx_api.h"
#include "tx_thread.h"

UINT _txe_thread_wait_abort(TX_THREAD  *thread_ptr) __attribute__((weak));
UINT _txe_thread_wait_abort_orig(TX_THREAD  *thread_ptr) __attribute__((alias("_txe_thread_wait_abort")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak _txe_thread_wait_abort
#pragma weak _txe_thread_wait_abort_orig=_txe_thread_wait_abort
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_txe_thread_wait_abort */

#ifdef __inside_trcEvent

#include <trcRecorder.h>

traceResult xTraceEventBeginRawOffline(uint32_t uiSize, TraceEventHandle_t* pxEventHandle) __attribute__((weak));
traceResult xTraceEventBeginRawOffline_orig(uint32_t uiSize, TraceEventHandle_t* pxEventHandle) __attribute__((alias("xTraceEventBeginRawOffline")));
traceResult xTraceEventEndOffline(TraceEventHandle_t xEventHandle) __attribute__((weak));
traceResult xTraceEventEndOffline_orig(TraceEventHandle_t xEventHandle) __attribute__((alias("xTraceEventEndOffline")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak xTraceEventBeginRawOffline
#pragma weak xTraceEventBeginRawOffline_orig=xTraceEventBeginRawOffline
#pragma weak xTraceEventEndOffline
#pragma weak xTraceEventEndOffline_orig=xTraceEventEndOffline
#endif /* __IAR_SYSTEMS_ICC__ */

#endif /* __inside_trcEvent */

#ifdef __inside_trcISR

#include <trcRecorder.h>

traceResult xTraceISREnd(TraceBaseType_t uxIsTaskSwitchRequired) __attribute__((weak));
traceResult xTraceISREnd_orig(TraceBaseType_t uxIsTaskSwitchRequired) __attribute__((alias("xTraceISREnd")));

#ifdef __IAR_SYSTEMS_ICC__
#pragma weak xTraceISREnd
#pragma weak xTraceISREnd_orig=xTraceISREnd
#endif /* __IAR_SYSTEMS_ICC__ */

#endif

#endif /* TRC_CTI_H_ */
