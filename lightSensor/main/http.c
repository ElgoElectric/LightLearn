#include "esp_netif.h"
#include "esp_http_client.h"
#include "cJSON.h"

int PREV_SWITCH_POSITION = 0;
int CURR_SWITCH_POSITION = 0;

esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        // Parse JSON response to extract switch position
        cJSON *root = cJSON_Parse((char *)evt->data);
        if (root != NULL)
        {
            cJSON *current_light_level = cJSON_GetObjectItemCaseSensitive(root, "currentLightLevel");
            cJSON *previous_light_level = cJSON_GetObjectItemCaseSensitive(root, "previousLightLevel");
            if (cJSON_IsNumber(current_light_level))
            {
                CURR_SWITCH_POSITION = (float)current_light_level->valuedouble;
            }

            if (cJSON_IsNumber(previous_light_level))
            {
                PREV_SWITCH_POSITION = (float)previous_light_level->valuedouble;
            }

            cJSON_Delete(root);
        }
        break;
    case HTTP_EVENT_ERROR:
        printf("HTTP_EVENT_ERROR: %d\n", evt->event_id);
        break;

    default:
        break;
    }
    return ESP_OK;
}

void turn_on_light()
{
    esp_http_client_config_t config_get = {
        .url = "http://44.203.163.147:3000/togglePower?isPowerOn=true&plugID=shellyplusplugs-d4d4daec6c98",
        .method = HTTP_METHOD_GET,
        .is_async = false,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler,
        .timeout_ms = 10000};

    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

void turn_off_light()
{
    esp_http_client_config_t config_get = {
        .url = "http://44.203.163.147:3000/togglePower?isPowerOn=false&plugID=shellyplusplugs-d4d4daec6c98",
        .method = HTTP_METHOD_GET,
        .is_async = false,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler,
        .timeout_ms = 10000};

    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

void get_switch_position()
{
    esp_http_client_config_t config_get = {
        .url = "http://44.203.163.147:3000/loglightLevel?lightLevel_current=-1&plugID=shellyplusplugs-d4d4daec6c98&lightLevel_prev=-1",
        .method = HTTP_METHOD_GET,
        .is_async = false,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler,
        .timeout_ms = 10000};

    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}

void execute_optimal_action(float switch_position, float optimal_action)
{
    // Format switch_position and optimal_action to have two decimal places
    char switch_position_str[20]; // Assuming maximum length of float representation
    char optimal_action_str[20];  // Assuming maximum length of float representation
    snprintf(switch_position_str, sizeof(switch_position_str), "%.1f", switch_position);
    snprintf(optimal_action_str, sizeof(optimal_action_str), "%.1f", optimal_action);

    // Construct the URL string with formatted float values
    char url[200]; // Adjust size as per your requirement
    snprintf(url, sizeof(url), "http://44.203.163.147:3000/loglightLevel?lightLevel_current=%s&plugID=shellyplusplugs-d4d4daec6c98&lightLevel_prev=%s", optimal_action_str, switch_position_str);

    // Set up the HTTP client configuration with the formatted URL
    esp_http_client_config_t config_get = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .is_async = false,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler,
        .timeout_ms = 10000};

    // Initialize and perform the HTTP request
    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}