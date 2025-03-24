# Only add the source/header files if the Tracerecorder is enables. By doing
# this we avoid having a lot of conditional checks for configuration
# options in the configuration files that use parameters from the KConfig,
# parameters that are missing when the Tracerecorder is disabled.
if(CONFIG_PERCEPIO_TRACERECORDER_ENABLED)
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_RINGBUFFER)
		set(src_dirs ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/../../streamports/RingBuffer" ${CMAKE_CURRENT_LIST_DIR}/../../)
		set(inc_dirs "${CMAKE_CURRENT_LIST_DIR}/config" "${CMAKE_CURRENT_LIST_DIR}/include" "${CMAKE_CURRENT_LIST_DIR}/../../streamports/RingBuffer/include" "${CMAKE_CURRENT_LIST_DIR}/streamports/RingBuffer/config" ${CMAKE_CURRENT_LIST_DIR}/../../include)
	endif()
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_RTT)
		set(src_dirs ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/../../streamports/Jlink_RTT" ${CMAKE_CURRENT_LIST_DIR}/../../)
		set(inc_dirs "${CMAKE_CURRENT_LIST_DIR}/config" "${CMAKE_CURRENT_LIST_DIR}/include" "${CMAKE_CURRENT_LIST_DIR}/../../streamports/Jlink_RTT/include" "${CMAKE_CURRENT_LIST_DIR}/streamports/Jlink_RTT/config" ${CMAKE_CURRENT_LIST_DIR}/../../include)
	endif()
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_ESP_IDF_APPTRACE)
		set(src_dirs ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/streamports/ESP_IDF_APPTRACE" ${CMAKE_CURRENT_LIST_DIR}/../../)
		set(inc_dirs "${CMAKE_CURRENT_LIST_DIR}/config" "${CMAKE_CURRENT_LIST_DIR}/include" "${CMAKE_CURRENT_LIST_DIR}/streamports/ESP_IDF_APPTRACE/include" ${CMAKE_CURRENT_LIST_DIR}/../../include)
	endif()
else()
	set(src_dirs "")
	set(inc_dirs "")
endif()

idf_component_register(SRC_DIRS "${src_dirs}"
		INCLUDE_DIRS "${inc_dirs}"
		LDFRAGMENTS "${CMAKE_CURRENT_LIST_DIR}/lifra.fr"
		PRIV_REQUIRES
			app_trace
		)
