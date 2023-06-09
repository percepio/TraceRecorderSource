/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Configuration parameters for the kernel port.
 * More settings can be found in trcKernelPortStreamingConfig.h and
 * trcKernelPortSnapshotConfig.h.
 */

#ifndef TRC_KERNEL_PORT_CONFIG_H
#define TRC_KERNEL_PORT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def TRC_CFG_CPU_CLOCK_HZ
 * @brief Board CPU clock frequency in Hz.
 */
#define TRC_CFG_CPU_CLOCK_HZ 0

#if(TRC_CFG_CPU_CLOCK_HZ == 0)
#error "TRC_CFG_CPU_CLOCK_HZ needs to be set to the CPU frequency."
#endif
/**
 * @def TRC_CFG_USE_GCC_STATEMENT_EXPR
 * @brief Enable/Disable the use of GCC statement expressions in the
 * recorder.
 */
#define TRC_CFG_USE_GCC_STATEMENT_EXPR 1

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_CONFIG_H */
