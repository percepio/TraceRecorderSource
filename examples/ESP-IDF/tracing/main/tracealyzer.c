/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Application Trace to Host Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gptimer.h"

static const char *TAG = "example";

typedef struct {
    gptimer_handle_t gptimer;
    int count;
    TaskHandle_t thnd;
    uint64_t period;
    char task_name[32];
} example_event_data_t;

static bool example_timer_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    example_event_data_t *tim_arg = (example_event_data_t *)user_ctx;
    bool need_yield = false;

    if (tim_arg->thnd != NULL) {
        if (tim_arg->count++ < 10000) {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            if (xTaskNotifyFromISR(tim_arg->thnd, tim_arg->count, eSetValueWithOverwrite, &xHigherPriorityTaskWoken) != pdPASS) {
                ESP_EARLY_LOGE(TAG, "Failed to notify task %p", tim_arg->thnd);
            } else {
                if (xHigherPriorityTaskWoken == pdTRUE) {
                    need_yield = true;
                }
            }
        }
    }
    return need_yield;
}

static void example_task(void *p)
{
    uint32_t event_val;
    example_event_data_t *arg = (example_event_data_t *) p;
    ESP_LOGI(TAG, "%p: run task", xTaskGetCurrentTaskHandle());
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = arg->period,
        .flags.auto_reload_on_alarm = true,
    };
    // This task is pinned to a specific core, to the interrupt will also be install to that core
    gptimer_event_callbacks_t cbs = {
        .on_alarm = example_timer_alarm_cb,
    };

    traceString MyChannel = xTraceRegisterString(xPortGetCoreID() == 0 ? "DataCh0" : "DataCh1");

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(arg->gptimer, &cbs, arg));
    ESP_ERROR_CHECK(gptimer_set_alarm_action(arg->gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_enable(arg->gptimer));
    ESP_ERROR_CHECK(gptimer_start(arg->gptimer));
    while (1) {
        xTaskNotifyWait(0, 0, &event_val, portMAX_DELAY);
        ESP_LOGI(TAG, "Task[%p]: received event %"PRIu32, xTaskGetCurrentTaskHandle(), event_val);
        vTracePrintF(MyChannel, "Sensor Data = %d", event_val);
    }
}

void app_main(void)
{
    static example_event_data_t event_data[CONFIG_FREERTOS_NUMBER_OF_CORES];

    for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
        gptimer_config_t timer_config = {
            .clk_src = GPTIMER_CLK_SRC_DEFAULT,
            .direction = GPTIMER_COUNT_UP,
            .resolution_hz = 1000000,
        };
        ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &event_data[i].gptimer));
        event_data[i].period = 100000 * (i + 1);
    }

    for (int i = 0; i < CONFIG_FREERTOS_NUMBER_OF_CORES; i++) {
        sprintf(event_data->task_name, "trace%d", i);
        xTaskCreatePinnedToCore(example_task, event_data->task_name, 4096, &event_data[i], 3, &event_data[i].thnd, i);
        ESP_LOGI(TAG, "Created task %p", event_data[i].thnd);
    }
}
