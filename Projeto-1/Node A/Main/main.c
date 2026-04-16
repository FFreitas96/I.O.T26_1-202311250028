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

#define BUTTON_GPIO GPIO_NUM_3
#define DEBOUNCE_MS 200

static const char *TAG = "NODE_A";

static EventGroupHandle_t wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0

static esp_mqtt_client_handle_t mqtt_client = NULL;
static bool mqtt_connected = false;

static bool state = false;
static int64_t last_press = 0;
static QueueHandle_t queue;

// WIFI
static void wifi_event_handler(void *arg, esp_event_base_t base,
                               int32_t id, void *data)
{
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Conectando WiFi...");
        esp_wifi_connect();
    }
    else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG, "Reconectando...");
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
    switch (event_id) {

        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT conectado");
            mqtt_connected = true;
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT desconectado");
            mqtt_connected = false;
            break;
    }
}

static void mqtt_init(void)
{
    esp_mqtt_client_config_t cfg = {
        .broker.address.uri = BROKER_URI,
        .credentials.client_id = "nodeA",
        .session.keepalive = 20
    };

    mqtt_client = esp_mqtt_client_init(&cfg);

    esp_mqtt_client_register_event(mqtt_client,
        ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    esp_mqtt_client_start(mqtt_client);
}

// BOTÃO
static void IRAM_ATTR isr_handler(void *arg)
{
    int64_t now = esp_timer_get_time() / 1000;

    if ((now - last_press) < DEBOUNCE_MS) return;
    last_press = now;

    uint8_t evt = 1;
    xQueueSendFromISR(queue, &evt, NULL);
}

static void button_task(void *arg)
{
    uint8_t evt;

    while (1) {
        if (xQueueReceive(queue, &evt, portMAX_DELAY)) {

            if (!mqtt_connected) continue;

            state = !state;
            const char *msg = state ? "ON" : "OFF";

            ESP_LOGI(TAG, "Publicando: %s", msg);

            esp_mqtt_client_publish(mqtt_client,
                                    MQTT_TOPIC,
                                    msg,
                                    0,
                                    1,
                                    1);
        }
    }
}

static void button_init(void)
{
    queue = xQueueCreate(10, sizeof(uint8_t));

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };

    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_GPIO, isr_handler, NULL);

    xTaskCreate(button_task, "btn", 4096, NULL, 10, NULL);
}

// STATUS
static void status_task(void *arg)
{
    int uptime = 0;

    while (1) {
        if (mqtt_connected) {
            char msg[64];
            sprintf(msg, "NODE_A Uptime: %d s", uptime);

            ESP_LOGI(TAG, "Status: %s", msg);

            esp_mqtt_client_publish(mqtt_client,
                                    STATUS_TOPIC,
                                    msg,
                                    0,
                                    0,
                                    0);

            uptime += 30;
        }

        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}

// MAIN
void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_INFO);

    nvs_flash_init();

    wifi_init();
    mqtt_init();

    vTaskDelay(pdMS_TO_TICKS(2000));

    button_init();
    xTaskCreate(status_task, "status", 4096, NULL, 5, NULL);
}