/*
 * Trace Recorder for Tracealyzer v4.8.0.hotfix1
 * Copyright 2023 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Configuration parameters for the kernel port.
 * More settings can be found in trcKernelPortStreamingConfig.h.
 */

#ifndef TRC_KERNEL_PORT_CONFIG_H
#define TRC_KERNEL_PORT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Setting this to 0 will completely disable the recorder
 */
#define TRC_CFG_USE_TRACEALYZER_RECORDER 1

/**
 * @brief Board CPU clock frequency in Hz. Must be changed from 0.
 */
#define TRC_CFG_CPU_CLOCK_HZ 0

#ifdef __cplusplus
}
#endif

#endif /* TRC_KERNEL_PORT_CONFIG_H */
