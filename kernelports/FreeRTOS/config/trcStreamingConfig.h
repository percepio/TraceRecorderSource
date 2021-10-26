/*
 * Trace Recorder for Tracealyzer v4.5.2
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Configuration parameters for the trace recorder library in streaming mode.
 * Read more at http://percepio.com/2016/10/05/rtos-tracing/
 */

#ifndef TRC_STREAMING_CONFIG_H
#define TRC_STREAMING_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Configuration Macro: TRC_CFG_SYMBOL_TABLE_SLOTS
 *
 * The maximum number of symbols names that can be stored. This includes:
 * - Task names
 * - Named ISRs (vTraceSetISRProperties)
 * - Named kernel objects (vTraceStoreKernelObjectName)
 * - User event channels (xTraceRegisterString)
 *
 * If this value is too small, not all symbol names will be stored and the
 * trace display will be affected. In that case, there will be warnings
 * (as User Events) from TzCtrl task, that monitors this.
 ******************************************************************************/
#define TRC_CFG_SYMBOL_TABLE_SLOTS 40

/*******************************************************************************
 * Configuration Macro: TRC_CFG_SYMBOL_MAX_LENGTH
 *
 * The maximum length of symbol names, including:
 * - Task names
 * - Named ISRs (vTraceSetISRProperties)
 * - Named kernel objects (vTraceStoreKernelObjectName)
 * - User event channel names (xTraceRegisterString)
 *
 * If longer symbol names are used, they will be truncated by the recorder,
 * which will affect the trace display. In that case, there will be warnings
 * (as User Events) from TzCtrl task, that monitors this.
 ******************************************************************************/
#define TRC_CFG_SYMBOL_MAX_LENGTH 25

/*******************************************************************************
 * Configuration Macro: TRC_CFG_OBJECT_DATA_SLOTS
 *
 * The maximum number of object data entries (used for task priorities) that can
 * be stored at the same time. Must be sufficient for all tasks, otherwise there
 * will be warnings (as User Events) from TzCtrl task, that monitors this.
 ******************************************************************************/
#define TRC_CFG_OBJECT_DATA_SLOTS 40

/*******************************************************************************
 * Configuration Macro: TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT
 *
 * Specifies the number of pages used by the paged event buffer.
 * This may need to be increased if there are a lot of missed events.
 *
 * Note: not used by the J-Link RTT stream port (see trcStreamingPort.h instead)
 ******************************************************************************/
#define TRC_CFG_PAGED_EVENT_BUFFER_PAGE_COUNT 10

/*******************************************************************************
 * Configuration Macro: TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE
 *
 * Specifies the size of each page in the paged event buffer. This can be tuned 
 * to match any internal low-level buffers used by the streaming interface, like
 * the Ethernet MTU (Maximum Transmission Unit). However, since the currently
 * active page can't be transfered, having more but smaller pages is more
 * efficient with respect memory usage, than having a few large pages.  
 *
 * Note: not used by the J-Link RTT stream port (see trcStreamingPort.h instead)
 ******************************************************************************/
#define TRC_CFG_PAGED_EVENT_BUFFER_PAGE_SIZE 500

/*******************************************************************************
 * TRC_CFG_ISR_TAILCHAINING_THRESHOLD
 *
 * Macro which should be defined as an integer value.
 *
 * If tracing multiple ISRs, this setting allows for accurate display of the 
 * context-switching also in cases when the ISRs execute in direct sequence.
 * 
 * vTraceStoreISREnd normally assumes that the ISR returns to the previous
 * context, i.e., a task or a preempted ISR. But if another traced ISR 
 * executes in direct sequence, Tracealyzer may incorrectly display a minimal
 * fragment of the previous context in between the ISRs.
 *
 * By using TRC_CFG_ISR_TAILCHAINING_THRESHOLD you can avoid this. This is 
 * however a threshold value that must be measured for your specific setup.
 * See http://percepio.com/2014/03/21/isr_tailchaining_threshold/
 *
 * The default setting is 0, meaning "disabled" and that you may get an 
 * extra fragments of the previous context in between tail-chained ISRs.
 *
 * Note: This setting has separate definitions in trcSnapshotConfig.h and 
 * trcStreamingConfig.h, since it is affected by the recorder mode.
 ******************************************************************************/
#define TRC_CFG_ISR_TAILCHAINING_THRESHOLD 0

#ifdef __cplusplus
}
#endif

#endif /* TRC_STREAMING_CONFIG_H */
