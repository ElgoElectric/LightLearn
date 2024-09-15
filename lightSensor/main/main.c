#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "esp_system.h"
#include "esp_spiffs.h"
#include "nvs_flash.h"
#include "driver/adc.h"
#include <unistd.h>
#include <time.h>
// #include "esp_nimble_hci.h"
#include "wifi.h"
#include "ble.h"
#include "http.h"
#include "globals.h"
#include "logger.h"

#define LOG_FILE_PATH "/logs/runtime.txt"
#define ADC_CHANNEL ADC1_CHANNEL_0 // ADC channel connected to light sensor

#define LIGHT_LEVELS 3
#define SWITCH_POSITIONS 3
#define ACTIONS 3

// Define the action_matrix
float action_matrix[SWITCH_POSITIONS][LIGHT_LEVELS] = {
    // Switch position 0
    {
        0.5, // Light level 0
        0,   // Light level 0.5
        0    // Light level 1
    },
    // Switch position 0.5
    {
        0.5, // Light level 0
        0.5, // Light level 0.5
        0    // Light level 1
    },
    // Switch position 1
    {
        1,  // Light level 0
        1,  // Light level 0.5
        0.5 // Light level 1
    }};

float get_optimal_action(int analog_light_level, int occupancy, int switch_position)
{
    if (!occupancy)
    {
        return 0;
    }

    int light_level_index;
    if (analog_light_level <= 1000)
    {
        light_level_index = 0;
    }
    else if (analog_light_level <= 3000)
    {
        light_level_index = 0.5;
    }
    else
    {
        light_level_index = 1;
    }

    return action_matrix[switch_position][light_level_index];
}

// File system for ESP32
void initialize_spiffs()
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/storage",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};
    esp_err_t result = esp_vfs_spiffs_register(&conf);
}

int log_light_level()
{
    // Read light level from ADC
    uint32_t adc_reading = adc1_get_raw(ADC_CHANNEL);
    int light_level = (int)adc_reading;
    return light_level;

    // Get current time (replace with your timestamping method)
    // time_t now;
    // time(&now);
    // struct tm *timeinfo = localtime(&now);
    // char timestamp[64];
    // strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    // printf("%s - Light Level: %ld\n", timestamp, adc_reading);
    // return light_level;
}

void app_main(void)
{
    logger_initFileLogger(LOG_FILE_PATH, 1024 * 1024, 5);
    logger_setLevel(LogLevel_DEBUG);
    nvs_flash_init();
    wifi_connection();
    vTaskDelay(7000 / portTICK_PERIOD_MS);
    LOG_INFO("WIFI was initiated ...........\n\n");

    ble_connection();

    // // Initialize SPIFFS
    // // initialize_spiffs();

    adc1_config_width(ADC_WIDTH_BIT_12);                    // 12-bit resolution
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_0); // GPIO connected to LDR

    // Infinite loop to query light level, occupancy and switch position
    while (1)
    {
        // Gets the light level (0-4081)
        int light_level = log_light_level();
        LOG_INFO("%d\n", light_level);

        // insert code to query current switch position
        get_switch_position();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        LOG_INFO("PREV_SWITCH_POS: %f\n", PREV_SWITCH_POSITION);
        LOG_INFO("CURR_SWITCH_POS: %f\n", CURR_SWITCH_POSITION);

        // Occupancy boolean stored in OCCUPIED global variable
        LOG_INFO("OCCUPIED: %d\n", OCCUPIED);

        // insert code to send optimal action to lights
        float optimal_action = get_optimal_action(light_level, OCCUPIED, (int)CURR_SWITCH_POSITION * 2);
        LOG_INFO("Optimal Action: %f\n", optimal_action);

        // // insert code to send optimal action
        execute_optimal_action(CURR_SWITCH_POSITION, optimal_action);

        OCCUPIED = 0;
        sleep(10);
    }
}
