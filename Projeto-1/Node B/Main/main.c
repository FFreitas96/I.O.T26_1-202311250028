#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "driver/gpio.h"
#include "esp_timer.h"

// CONFIG
#define WIFI_SSID  "wifi-iot-2.4g"
#define WIFI_PASS  "iot2026.1"

#define BROKER_URI "mqtt://10.1.133.82"
#define MQTT_TOPIC "ifpb/projeto/led"
#define STATUS_TOPIC "ifpb/projeto/status"

#define LED_GPIO GPIO_NUM_20

static const char *TAG = "NODE_B";

static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0

static esp_mqtt_client_handle_t mqtt_client = NULL;

// WIFI
static void wifi_event_handler(void *arg, esp_event_base_t base,
                               int32_t id, void *data)
{
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    }
    else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
    }
    else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "WiFi conectado");
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init(void)
{
    wifi_event_group = xEventGroupCreate();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_register(WIFI_EVENT,
        ESP_EVENT_ANY_ID, wifi_event_handler, NULL, NULL);

    esp_event_handler_instance_register(IP_EVENT,
        IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL, NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        }
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    xEventGroupWaitBits(wifi_event_group,
                        WIFI_CONNECTED_BIT,
                        pdFALSE,
                        pdTRUE,
                        portMAX_DELAY);
}

// MQTT
static void mqtt_event_handler(void *arg, esp_event_base_t base,
                               int32_t event_id, void *data)
{
    esp_mqtt_event_handle_t event = data;

    if (event_id == MQTT_EVENT_CONNECTED) {
        ESP_LOGI(TAG, "MQTT conectado");
        esp_mqtt_client_subscribe(event->client, MQTT_TOPIC, 1);
    }
    else if (event_id == MQTT_EVENT_DATA) {

        char msg[10];
        memcpy(msg, event->data, event->data_len);
        msg[event->data_len] = '\0';

        ESP_LOGI(TAG, "Recebido: %s", msg);

        if (strcmp(msg, "ON") == 0) {
            gpio_set_level(LED_GPIO, 1);
        } else {
            gpio_set_level(LED_GPIO, 0);
        }
    }
}

static void mqtt_init(void)
{
    esp_mqtt_client_config_t cfg = {
        .broker.address.uri = BROKER_URI,
        .credentials.client_id = "nodeB"
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&cfg);

    esp_mqtt_client_register_event(client,
        ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    esp_mqtt_client_start(client);
}

// LED
static void led_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_GPIO),
        .mode = GPIO_MODE_OUTPUT
    };

    gpio_config(&io_conf);
}

// MAIN
void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);

    nvs_flash_init();

    wifi_init();
    mqtt_init();
    led_init();
}