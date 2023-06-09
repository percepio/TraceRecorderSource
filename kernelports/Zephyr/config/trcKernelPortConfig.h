/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Main configuration parameters for the trace recorder library.
 * More settings can be found in trcStreamingConfig.h.
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

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_CONFIG_H */
