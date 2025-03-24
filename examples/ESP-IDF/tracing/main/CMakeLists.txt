idf_component_register(SRCS "tracealyzer.c"
                    PRIV_REQUIRES esp_driver_gptimer app_trace
                    INCLUDE_DIRS ".")
