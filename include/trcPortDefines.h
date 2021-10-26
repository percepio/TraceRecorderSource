/*
 * Trace Recorder for Tracealyzer v4.5.2
 * Copyright 2021 Percepio AB
 * www.percepio.com
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Some common defines for the trace recorder.
 */

#ifndef TRC_PORTDEFINES_H
#define TRC_PORTDEFINES_H

#define TRACE_SUCCESS (0)
#define TRACE_FAIL (1)

#define TRC_FREE_RUNNING_32BIT_INCR 1
#define TRC_FREE_RUNNING_32BIT_DECR 2
#define TRC_OS_TIMER_INCR 3
#define TRC_OS_TIMER_DECR 4
#define TRC_CUSTOM_TIMER_INCR 5
#define TRC_CUSTOM_TIMER_DECR 6

/* Start options for vTraceEnable. */
#define TRC_INIT 0
#define TRC_START 1
#define TRC_START_AWAIT_HOST 2

/* Command codes for TzCtrl task */
#define CMD_SET_ACTIVE      1 /* Start (param1 = 1) or Stop (param1 = 0) */

/* The final command code, used to validate commands. */
#define CMD_LAST_COMMAND 1

#define TRC_RECORDER_MODE_SNAPSHOT		0
#define TRC_RECORDER_MODE_STREAMING		1

#define TRC_RECORDER_BUFFER_ALLOCATION_STATIC   (0x00)
#define TRC_RECORDER_BUFFER_ALLOCATION_DYNAMIC  (0x01)
#define TRC_RECORDER_BUFFER_ALLOCATION_CUSTOM   (0x02)

/* Filter Groups */
#define FilterGroup0 (uint16_t)0x0001
#define FilterGroup1 (uint16_t)0x0002
#define FilterGroup2 (uint16_t)0x0004
#define FilterGroup3 (uint16_t)0x0008
#define FilterGroup4 (uint16_t)0x0010
#define FilterGroup5 (uint16_t)0x0020
#define FilterGroup6 (uint16_t)0x0040
#define FilterGroup7 (uint16_t)0x0080
#define FilterGroup8 (uint16_t)0x0100
#define FilterGroup9 (uint16_t)0x0200
#define FilterGroup10 (uint16_t)0x0400
#define FilterGroup11 (uint16_t)0x0800
#define FilterGroup12 (uint16_t)0x1000
#define FilterGroup13 (uint16_t)0x2000
#define FilterGroup14 (uint16_t)0x4000
#define FilterGroup15 (uint16_t)0x8000

/******************************************************************************
 * Supported ports
 *
 * TRC_HARDWARE_PORT_HWIndependent
 * A hardware independent fallback option for event timestamping. Provides low
 * resolution timestamps based on the OS tick.
 * This may be used on the Win32 port, but may also be used on embedded hardware
 * platforms. All time durations will be truncated to the OS tick frequency,
 * typically 1 KHz. This means that a task or ISR that executes in less than
 * 1 ms get an execution time of zero.
 *
 * TRC_HARDWARE_PORT_APPLICATION_DEFINED
 * Allows for defining the port macros in other source code files.
 *
 * TRC_HARDWARE_PORT_Win32
 * "Accurate" timestamping based on the Windows performance counter for Win32
 * builds. Note that this gives the host machine time, not the kernel time.
 *
 * Hardware specific ports
 * To get accurate timestamping, a hardware timer is necessary. Below are the
 * available ports. Some of these are "unofficial", meaning that
 * they have not yet been verified by Percepio but have been contributed by
 * external developers. They should work, otherwise let us know by emailing
 * support@percepio.com. Some work on any OS platform, while other are specific
 * to a certain operating system.
 *****************************************************************************/

/****** Port Name ************************************* Code ** Official ** OS Platform *********/
#define TRC_HARDWARE_PORT_APPLICATION_DEFINED			98	/*	-			-					*/
#define TRC_HARDWARE_PORT_NOT_SET						99	/*	-			-					*/
#define TRC_HARDWARE_PORT_HWIndependent					0	/*	Yes			Any					*/
#define TRC_HARDWARE_PORT_Win32							1	/*	Yes			FreeRTOS on Win32	*/
#define TRC_HARDWARE_PORT_Atmel_AT91SAM7				2	/*	No			Any					*/
#define TRC_HARDWARE_PORT_Atmel_UC3A0					3	/*	No			Any					*/
#define TRC_HARDWARE_PORT_ARM_Cortex_M					4	/*	Yes			Any					*/
#define TRC_HARDWARE_PORT_Renesas_RX600					6	/*	Yes			Any					*/
#define TRC_HARDWARE_PORT_MICROCHIP_PIC24_PIC32			7	/*	Yes			Any					*/
#define TRC_HARDWARE_PORT_TEXAS_INSTRUMENTS_TMS570_RM48	8	/*	Yes			Any					*/
#define TRC_HARDWARE_PORT_TEXAS_INSTRUMENTS_MSP430		9	/*	No			Any					*/
#define TRC_HARDWARE_PORT_XILINX_PPC405					11	/*	No			FreeRTOS			*/
#define TRC_HARDWARE_PORT_XILINX_PPC440					12	/*	No			FreeRTOS			*/
#define TRC_HARDWARE_PORT_XILINX_MICROBLAZE				13	/*	No			Any					*/
#define TRC_HARDWARE_PORT_XILINX_ZyncUltraScaleR5		14	/*  No			FreeRTOS			*/
#define TRC_HARDWARE_PORT_NXP_LPC210X					15	/*	No			Any					*/
#define TRC_HARDWARE_PORT_ARM_CORTEX_A9					16	/*	Yes			Any					*/
#define TRC_HARDWARE_PORT_POWERPC_Z4                    17  /*  No          FreeRTOS            */
#define TRC_HARDWARE_PORT_Altera_NiosII					18  /*  Yes          Any (Tested with FreeRTOS)                 */
#define TRC_HARDWARE_PORT_ZEPHYR                        19  /*  Yes         Zephyr              */
#define TRC_HARDWARE_PORT_XTensa_LX6                    20  /*  Yes         ESP-IDF FreeRTOS    */
#define TRC_HARDWARE_PORT_XTensa_LX7                    21  /*  Yes         ESP-IDF FreeRTOS    */
#endif /*TRC_PORTDEFINES_H*/
