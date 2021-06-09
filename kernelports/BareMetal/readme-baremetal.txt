"Kernel Port" for Trace Recorder - for Bare Metal software
Copyright 2021 Percepio AB
www.percepio.com
-------------------------------------------------------------------------
(pre-release version, 2020-10-21)

This module allows for using the Tracealyzer recorder library in bare metal software (i.e. without RTOS).

Currently this bare-metal port allows for tracing ISRs (using vTraceStoreISRBegin/End), as well as User Events (using e.g. vTracePrintF).
Unlike our RTOS integrations, this bare-metal variant only traces the events that you explicitly log by calling the recorder functions.

The recorder library has been designed for single-core 32-bit processors. There is no support for multi-core tracing yet.
Using the recorder from multiple cores will most likely result in data corruption.

Getting Started:

This is a pre-release version, intended for early adopters. There is no official "bare metal" support in the
currently released versions of Tracealyzer, but this kernel port works with a license for "Tracealyzer for FreeRTOS".

When integrating the trace recorder library in your build, you basically follow the normal integration guide
found in Help, but skip the FreeRTOS-specific steps.

The required steps to integrate the recorder are:

- Add the three .c files to your project (trcSnapshotRecorder.c, trcStreamingRecorder.c and trcKernelPort.c) and if necessary a suitable trcStreamPort.c.

- Copy all header files from the config and include directories to a suitable location in your project structure.

- Set TRC_CFG_HARDWARE_PORT in trcConfig.h to reflect your system.

- Add #include "trcRecorder.h" to all .c files where you like to use the trace recorder.

- Call vTraceEnable(TRC_START) in your main function to initialize and start the recording. 
  This should be called after the initial hardware setup (when the system clock frequency is configured), 
  but before any other trace recorder calls are made.

Additional considerations for bare metal:

- Open trcKernelPort.h and update the definition of TRACE_CPU_CLOCK_HZ accordingly.
  This should be set to match your processors clock frequency.

- Note that several settings in trcConfig.h are irrelevant without FreeRTOS, such as TRC_CFG_FREERTOS_VERSION, TRC_CFG_INCLUDE_READY_EVENTS, TRC_CFG_INCLUDE_OSTICK_EVENTS, etc.
  
- For snapshot mode, in trcSnapshotConfig.h you can set TRC_CFG_NTASK, TRC_CFG_NQUEUE, TRC_CFG_NSEMAPHORE etc. to 0 in order to reduce memory usage.
  However, if tracing ISRs you need to set TRC_CFG_NISR to match the number of ISRs you intend to trace. 
 
To use this bare metal port in streaming mode, you also need to call vTracePeriodicControl() periodically, e.g. in the main loop.
This function has two important jobs:
   1: It handles any start/stop commands from Tracealyzer.
   2: For some stream ports, this also writes the trace data to the streaming interface (from a temporary buffer).

The J-Link and ITM stream ports only needs this function for starting and stopping the trace, so for those stream ports it is 
sufficient to call vTracePeriodicControl every 200 ms or so. For other stream ports, that uses the recorder's internal buffer, 
vTracePeriodicControl should be executed fairly often, e.g. every 10 ms, depending on the trace buffer size and the amount of data produced. 
 
In case of questions, contact support@percepio.com 