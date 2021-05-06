# SPDX-License-Identifier: Apache-2.0

zephyr_sources_ifdef(
  CONFIG_PERCEPIO_RECORDER_TRC_RECORDER_STREAM_PORT_ITM
  trcStreamingPort.c
  )

zephyr_include_directories(./include)