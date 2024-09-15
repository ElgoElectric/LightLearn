#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "sdkconfig.h"
#include "globals.h"

int OCCUPIED = 0;
char *TAG = "BLE Client Scan";
uint8_t ble_addr_type;

// BLE event handling
static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_hs_adv_fields fields;

    switch (event->type)
    {
    // NimBLE event discovery
    case BLE_GAP_EVENT_DISC:
        ble_hs_adv_parse_fields(&fields, event->disc.data, event->disc.length_data);
        if ((fields.name_len > 0) && (strncmp((const char *)fields.name, "Sanat's S23+", fields.name_len) == 0))
        {
            ESP_LOGI("GAP", "GAP EVENT DISCOVERY");
            printf("Discovered Device: %.*s\n", fields.name_len, fields.name);
            OCCUPIED = 1;
        }
        break;
    default:
        break;
    }
    return 0;
}

void ble_app_scan(void)
{
    printf("Start scanning ...\n");

    struct ble_gap_disc_params disc_params;
    disc_params.filter_duplicates = 1;
    disc_params.passive = 0;
    disc_params.itvl = 0;
    disc_params.window = 0;
    disc_params.filter_policy = 0;
    disc_params.limited = 0;

    ble_gap_disc(ble_addr_type, BLE_HS_FOREVER, &disc_params, ble_gap_event, NULL);
}

// The application
void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type); // Determines the best address type automatically
    ble_app_scan();
}

// The infinite task
void host_task(void *param)
{
    nimble_port_run(); // This function will return only when nimble_port_stop() is executed
}

void ble_connection()
{
    nimble_port_init();                             // 3 - Initialize the controller stack
    ble_svc_gap_device_name_set("BLE-Scan-Client"); // 4 - Set device name characteristic
    ble_svc_gap_init();                             // 4 - Initialize GAP service
    ble_hs_cfg.sync_cb = ble_app_on_sync;           // 5 - Set application
    nimble_port_freertos_init(host_task);           // 6 - Set infinite task
}