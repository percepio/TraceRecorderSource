# SPDX-License-Identifier: Apache-2.0

zephyr_sources_ifdef(
  CONFIG_PERCEPIO_TRACERECORDER
  trcKernelPort.c
  trcStreamingRecorder.c
  trcSDK.c
  )

zephyr_include_directories(./include ./config)

add_subdirectory_ifdef(CONFIG_PERCEPIO_RECORDER_TRC_RECORDER_STREAM_PORT_RTT streamports/Jlink_RTT)
add_subdirectory_ifdef(CONFIG_PERCEPIO_RECORDER_TRC_RECORDER_STREAM_PORT_ITM streamports/ARM_ITM)