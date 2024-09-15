idf_component_register(SRCS "cJSON.c" "http.c" "main.c" "wifi.c" "ble.c"
                    INCLUDE_DIRS "."
                    "C:/Users/sanat/esp/v5.2/esp-idf/components/bt/include/esp32c6/include"
                    "C:/Users/sanat/esp/v5.2/esp-idf/components/bt/host/bluedroid/api/include/api"
                    "C:/Users/sanat/esp/v5.2/esp-idf/components/bt/host/nimble/esp-hci/include"
                    "C:/Users/sanat/Downloads/curl-8.7.1_6-win64-mingw/curl-8.7.1_6-win64-mingw/include/curl"
                    PRIV_REQUIRES bt spiffs nvs_flash driver esp_http_client
                    )
spiffs_create_partition_image(storage ../partition FLASH_IN_PROJECT)