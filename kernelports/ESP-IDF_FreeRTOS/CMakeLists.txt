# Only add the source/header files if the Tracerecorder is enables. By doing
# this we avoid having a lot of conditional checks for configuration
# options in the configuration files that use parameters from the KConfig,
# parameters that are missing when the Tracerecorder is disabled.
if(CONFIG_PERCEPIO_TRACERECORDER_ENABLED)
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_RINGBUFFER)
		set(src_dirs ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/../../streamports/RingBuffer" ${CMAKE_CURRENT_LIST_DIR}/../../)
		set(inc_dirs "${CMAKE_CURRENT_LIST_DIR}/../../include" "${CMAKE_CURRENT_LIST_DIR}/config" "${CMAKE_CURRENT_LIST_DIR}/include" "${CMAKE_CURRENT_LIST_DIR}/../../streamports/RingBuffer/include" "${CMAKE_CURRENT_LIST_DIR}/streamports/RingBuffer/config" ${CMAKE_CURRENT_LIST_DIR}/../../include)
	endif()
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_RTT)
		set(src_dirs ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/../../streamports/Jlink_RTT" ${CMAKE_CURRENT_LIST_DIR}/../../)
		set(inc_dirs "${CMAKE_CURRENT_LIST_DIR}/../../include" "${CMAKE_CURRENT_LIST_DIR}/config" "${CMAKE_CURRENT_LIST_DIR}/include" "${CMAKE_CURRENT_LIST_DIR}/../../streamports/Jlink_RTT/include" "${CMAKE_CURRENT_LIST_DIR}/streamports/Jlink_RTT/config" ${CMAKE_CURRENT_LIST_DIR}/../../include)
	endif()
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_ESP_IDF_APPTRACE)
		set(src_dirs ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/streamports/ESP_IDF_APPTRACE" ${CMAKE_CURRENT_LIST_DIR}/../../)
		set(inc_dirs "${CMAKE_CURRENT_LIST_DIR}/../../include" "${CMAKE_CURRENT_LIST_DIR}/config" "${CMAKE_CURRENT_LIST_DIR}/include" "${CMAKE_CURRENT_LIST_DIR}/streamports/ESP_IDF_APPTRACE/include" ${CMAKE_CURRENT_LIST_DIR}/../../include)
	endif()
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_TCPIP)
		set(src_dirs ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/../../streamports/TCPIP" ${CMAKE_CURRENT_LIST_DIR}/../../)
		set(inc_dirs "${CMAKE_CURRENT_LIST_DIR}/../../include" "${CMAKE_CURRENT_LIST_DIR}/config" "${CMAKE_CURRENT_LIST_DIR}/include" "${CMAKE_CURRENT_LIST_DIR}/../../streamports/TCPIP/include" ${CMAKE_CURRENT_LIST_DIR}/../../include)
	endif()
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_UDP)
		set(src_dirs ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/../../streamports/UDP" ${CMAKE_CURRENT_LIST_DIR}/../../)
		set(inc_dirs "${CMAKE_CURRENT_LIST_DIR}/../../include" "${CMAKE_CURRENT_LIST_DIR}/config" "${CMAKE_CURRENT_LIST_DIR}/include" "${CMAKE_CURRENT_LIST_DIR}/../../streamports/UDP/include" ${CMAKE_CURRENT_LIST_DIR}/../../include)
	endif()
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_FILE)
		set(src_dirs ${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_LIST_DIR}/../../streamports/File" ${CMAKE_CURRENT_LIST_DIR}/../../)
		set(inc_dirs "${CMAKE_CURRENT_LIST_DIR}/../../include" "${CMAKE_CURRENT_LIST_DIR}/config" "${CMAKE_CURRENT_LIST_DIR}/include" "${CMAKE_CURRENT_LIST_DIR}/../../streamports/File/include" ${CMAKE_CURRENT_LIST_DIR}/../../include)
	endif()
else()
	set(src_dirs "")
	set(inc_dirs "")
endif()

idf_component_register(
	SRC_DIRS "${src_dirs}" 
	INCLUDE_DIRS "${inc_dirs}" 
	LDFRAGMENTS "lifra.fr"	
	
	# Needed for streamport RingBuffer
	# REQUIRES esp_timer

	# Needed for streamport ESP-IDF APPTRACE
	# REQUIRES esp_timer app_trace

	# Needed for streamport TCP/IP and UDP
	# REQUIRES esp_timer esp_event esp_netif

	# REQUIRES can't depend on config, so "require" all. Note that only the actually used modules are included in the image.
	REQUIRES esp_timer esp_event esp_netif app_trace
	
	# Refers to TraceRecorder. Not sure if needed, but does not seem to affect the image size.
	WHOLE_ARCHIVE
	)

# Since we inject Trace Recorder in the ESP-IDF we must expose the includes to ESP-IDF.
if(CONFIG_PERCEPIO_TRACERECORDER_ENABLED)
	idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/config APPEND)
	idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include APPEND)
	idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/../../include APPEND)
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_RINGBUFFER)
		idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/streamports/RingBuffer/config APPEND)
		idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/../../streamports/RingBuffer/include APPEND)
	endif()
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_RTT)
		idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/streamports/Jlink_RTT/config APPEND)
		idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/../../streamports/Jlink_RTT/include APPEND)
	endif()
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_ESP_IDF_APPTRACE)
		idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/streamports/ESP_IDF_APPTRACE/include APPEND)
	endif()
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_TCPIP)
		idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/streamports/TCPIP/config APPEND)
		idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/../../streamports/TCPIP/include APPEND)
	endif()
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_UDP)
		idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/streamports/UDP/config APPEND)
		idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/../../streamports/UDP/include APPEND)
	endif()
	if(CONFIG_PERCEPIO_TRC_CFG_STREAM_PORT_FILE)
		idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/streamports/File/config APPEND)
		idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/../../streamports/File/include APPEND)
	endif()
endif()