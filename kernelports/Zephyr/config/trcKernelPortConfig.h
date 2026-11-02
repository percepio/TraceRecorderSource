/*
 * Percepio TraceRecorder for Tracealyzer v4.12.0
 * Copyright 2025 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Main configuration parameters for the trace recorder library.
 */

#ifndef TRC_KERNEL_PORT_CONFIG_H
#define TRC_KERNEL_PORT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def TRC_CFG_USE_GCC_STATEMENT_EXPR
 * @brief Enable/Disable the use of GCC statement expressions in the
 * recorder.
 */
#define TRC_CFG_USE_GCC_STATEMENT_EXPR 1

/**
 * @def TRC_CFG_USE_SYSCALL_EXTENSION
 * @brief Enable/Disable the use of the syscall extension (i.e. send syscall
 * traces only by id instead of by name and id).
 */
#ifdef CONFIG_PERCEPIO_TRC_CFG_USE_SYSCALL_EXTENSION
#define TRC_CFG_USE_SYSCALL_EXTENSION 1
#else
#define TRC_CFG_USE_SYSCALL_EXTENSION 0
#endif

/**
 * @def TRC_CFG_CORE_COUNT
 * @brief CPU core count. Strictly speaking: the highest 
 * CPU core ID from which events can be emitted, plus one.
 */
#define TRC_CFG_CORE_COUNT CONFIG_MP_MAX_NUM_CPUS

/**
 * @def TRC_CFG_GET_CURRENT_CORE()
 * @brief Gets the current CPU core ID.
 */
#define TRC_CFG_GET_CURRENT_CORE() (arch_curr_cpu()->id)

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_CONFIG_H */
