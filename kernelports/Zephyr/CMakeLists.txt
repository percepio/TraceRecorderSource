# SPDX-License-Identifier: Apache-2.0

if(CONFIG_PERCEPIO_TRACERECORDER)

set(TRACERECORDER_ROOT_DIR ${ZEPHYR_TRACERECORDER_MODULE_DIR})

zephyr_sources_ifdef(
  CONFIG_PERCEPIO_TRACERECORDER
  trcKernelPort.c
  "${TRACERECORDER_ROOT_DIR}/trcStreamingRecorder.c"
  "${TRACERECORDER_ROOT_DIR}/trcInternalBuffer.c"
  "${TRACERECORDER_ROOT_DIR}/extras/SDK/trcSDK.c"
  )

zephyr_include_directories_ifdef(
  CONFIG_PERCEPIO_TRACERECORDER
  "${TRACERECORDER_ROOT_DIR}/include"
  "${TRACERECORDER_ROOT_DIR}/extras/SDK/include"
  ./include
  ./config
  )

add_subdirectory_ifdef(CONFIG_PERCEPIO_RECORDER_TRC_RECORDER_STREAM_PORT_RTT streamports/Jlink_RTT)
add_subdirectory_ifdef(CONFIG_PERCEPIO_RECORDER_TRC_RECORDER_STREAM_PORT_ITM streamports/ARM_ITM)

endif() #CONFIG_PERCEPIO_TRACERECORDER
