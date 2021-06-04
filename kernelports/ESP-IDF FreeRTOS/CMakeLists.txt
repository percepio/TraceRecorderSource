# Only add the source/header files if the Tracerecorder is enables. By doing
# this we avoid having a lot of conditional checks for configuration
# options in the configuration files that use parameters from the KConfig,
# parameters that are missing when the Tracerecorder is disabled.
if(CONFIG_PERCEPIO_TRACERECORDER_ENABLED)
	set(src_dirs "." "streamports/ESP_IDF_APPTRACE")
	set(inc_dirs "config" "include" "streamports/ESP_IDF_APPTRACE/include")
else()
	set(src_dirs "")
	set(inc_dirs "")
endif()

idf_component_register(SRC_DIRS "${src_dirs}"
                    INCLUDE_DIRS "${inc_dirs}"
                    LDFRAGMENTS "lifra.fr")

# Since we inject Trace Recorder in the ESP-IDF we must expose the includes to ESP-IDF.
if(CONFIG_PERCEPIO_TRACERECORDER_ENABLED)
idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/config APPEND)
idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include APPEND)
idf_build_set_property(INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/streamports/ESP_IDF_APPTRACE/include APPEND)
endif()