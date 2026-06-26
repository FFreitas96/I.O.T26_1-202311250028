#include "receptor.h"

#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "rede.h"

static const char *TAG = "RECEPTOR_ZB";

/* Endereco curto do sensor que entrou na rede; 0xFFFF = nenhum sensor ainda */
static uint16_t s_sensor_short_addr = 0xFFFF;

static void bdb_start_top_level_commissioning_cb(uint8_t mode_mask)
{
    ESP_ERROR_CHECK(esp_zb_bdb_start_top_level_commissioning(mode_mask));
}

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_struct)
{
    uint32_t *p_sg_p = signal_struct->p_app_signal;
    esp_err_t err_status = signal_struct->esp_err_status;
    esp_zb_app_signal_type_t sig_type = *p_sg_p;

    switch (sig_type) {
    case ESP_ZB_ZDO_SIGNAL_SKIP_STARTUP:
        ESP_LOGI(TAG, "Initialize Zigbee stack");
        esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_INITIALIZATION);
        break;
    case ESP_ZB_BDB_SIGNAL_DEVICE_FIRST_START:
    case ESP_ZB_BDB_SIGNAL_DEVICE_REBOOT:
        if (err_status == ESP_OK) {
            ESP_LOGI(TAG, "Deviced started up in %s factory-reset mode",
                      esp_zb_bdb_is_factory_new() ? "" : "non");
            if (esp_zb_bdb_is_factory_new()) {
                ESP_LOGI(TAG, "Start network formation");
                esp_zb_bdb_start_top_level_commissioning(ESP_ZB_BDB_MODE_NETWORK_FORMATION);
            } else {
                ESP_LOGI(TAG, "Device rebooted, reopening network for joining");
                esp_zb_bdb_open_network(180);
            }
        } else {
            ESP_LOGW(TAG, "Failed to initialize Zigbee stack (status: %s)", esp_err_to_name(err_status));
            esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb,
                                     ESP_ZB_BDB_MODE_INITIALIZATION, 1000);
        }
        break;
    case ESP_ZB_BDB_SIGNAL_FORMATION:
        if (err_status == ESP_OK) {
            ESP_LOGI(TAG, "Formed network successfully (PAN ID: 0x%04hx, Channel: %d)", esp_zb_get_pan_id(),
                      esp_zb_get_current_channel());
            esp_zb_bdb_open_network(180);
        } else {
            ESP_LOGW(TAG, "Network formation failed (status: %s), retry", esp_err_to_name(err_status));
            esp_zb_scheduler_alarm((esp_zb_callback_t)bdb_start_top_level_commissioning_cb,
                                     ESP_ZB_BDB_MODE_NETWORK_FORMATION, 1000);
        }
        break;
    case ESP_ZB_ZDO_SIGNAL_DEVICE_ANNCE: {
        esp_zb_zdo_signal_device_annce_params_t *dev_annce_params =
            (esp_zb_zdo_signal_device_annce_params_t *)esp_zb_app_signal_get_params(p_sg_p);
        ESP_LOGI(TAG, "Sensor entrou/voltou na rede (short addr: 0x%04hx)", dev_annce_params->device_short_addr);
        s_sensor_short_addr = dev_annce_params->device_short_addr;
        break;
    }
    default:
        ESP_LOGI(TAG, "ZDO signal: %s (0x%x), status: %s", esp_zb_zdo_signal_to_string(sig_type), sig_type,
                  esp_err_to_name(err_status));
        break;
    }
}

static esp_err_t zb_attribute_read_resp_handler(const esp_zb_zcl_cmd_read_attr_resp_message_t *message)
{
    if (message->info.status != ESP_ZB_ZCL_STATUS_SUCCESS) {
        ESP_LOGW(TAG, "Falha ao ler atributo do sensor (status: 0x%x)", message->info.status);
        return ESP_OK;
    }

    for (esp_zb_zcl_read_attr_resp_variable_t *variable = message->variables; variable; variable = variable->next) {
        if (variable->status == ESP_ZB_ZCL_STATUS_SUCCESS &&
            variable->attribute.id == ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID && variable->attribute.data.value) {
            int16_t valor = *(int16_t *)variable->attribute.data.value;
            float temperatura_c = valor / 100.0f;
            ESP_LOGI(TAG, "Temperatura recebida: %.2f C", temperatura_c);
            rede_publicar_temperatura(temperatura_c);
        }
    }
    return ESP_OK;
}

static esp_err_t zb_action_handler(esp_zb_core_action_callback_id_t callback_id, const void *message)
{
    switch (callback_id) {
    case ESP_ZB_CORE_CMD_READ_ATTR_RESP_CB_ID:
        return zb_attribute_read_resp_handler((const esp_zb_zcl_cmd_read_attr_resp_message_t *)message);
    default:
        ESP_LOGW(TAG, "Callback Zigbee nao tratado (0x%x)", callback_id);
        break;
    }
    return ESP_OK;
}

static void leitura_temperatura_task(void *pvParameters)
{
    for (;;) {
        if (s_sensor_short_addr != 0xFFFF) {
            uint16_t attr_id = ESP_ZB_ZCL_ATTR_TEMP_MEASUREMENT_VALUE_ID;
            esp_zb_zcl_read_attr_cmd_t read_req = {
                .zcl_basic_cmd = {
                    .dst_addr_u.addr_short = s_sensor_short_addr,
                    .dst_endpoint = HA_ESP_SENSOR_ENDPOINT,
                    .src_endpoint = HA_RECEPTOR_ENDPOINT,
                },
                .address_mode = ESP_ZB_APS_ADDR_MODE_16_ENDP_PRESENT,
                .clusterID = ESP_ZB_ZCL_CLUSTER_ID_TEMP_MEASUREMENT,
                .attr_number = 1,
                .attr_field = &attr_id,
            };

            esp_zb_lock_acquire(portMAX_DELAY);
            esp_zb_zcl_read_attr_cmd_req(&read_req);
            esp_zb_lock_release();
        }

        vTaskDelay(pdMS_TO_TICKS(LEITURA_TEMPERATURA_INTERVAL_S * 1000));
    }
}

static void esp_zb_task(void *pvParameters)
{
    esp_zb_cfg_t zb_nwk_cfg = ESP_ZB_ZC_CONFIG();
    esp_zb_init(&zb_nwk_cfg);

    esp_zb_attribute_list_t *basic_cluster = esp_zb_basic_cluster_create(NULL);
    esp_zb_basic_cluster_add_attr(basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, "\x09""ESPRESSIF");
    esp_zb_basic_cluster_add_attr(basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, "\x08""RECEPTOR");

    esp_zb_attribute_list_t *identify_cluster = esp_zb_identify_cluster_create(NULL);

    esp_zb_cluster_list_t *cluster_list = esp_zb_zcl_cluster_list_create();
    esp_zb_cluster_list_add_basic_cluster(cluster_list, basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);
    esp_zb_cluster_list_add_identify_cluster(cluster_list, identify_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE);

    esp_zb_ep_list_t *ep_list = esp_zb_ep_list_create();
    esp_zb_endpoint_config_t endpoint_config = {
        .endpoint = HA_RECEPTOR_ENDPOINT,
        .app_profile_id = ESP_ZB_AF_HA_PROFILE_ID,
        .app_device_id = ESP_ZB_HA_CONFIGURATION_TOOL_DEVICE_ID,
        .app_device_version = 0,
    };
    esp_zb_ep_list_add_ep(ep_list, cluster_list, endpoint_config);

    esp_zb_device_register(ep_list);
    esp_zb_core_action_handler_register(zb_action_handler);

    esp_zb_set_primary_network_channel_set(ESP_ZB_PRIMARY_CHANNEL_MASK);
    ESP_ERROR_CHECK(esp_zb_start(false));
    esp_zb_stack_main_loop();
}

void receptor_start(void)
{
    esp_zb_platform_config_t config = {
        .radio_config = ESP_ZB_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_ZB_DEFAULT_HOST_CONFIG(),
    };

    ESP_ERROR_CHECK(nvs_flash_init());

    rede_iniciar();

    ESP_ERROR_CHECK(esp_zb_platform_config(&config));

    xTaskCreate(esp_zb_task, "Zigbee_main", 4096, NULL, 5, NULL);
    xTaskCreate(leitura_temperatura_task, "leitura_temperatura_task", 3072, NULL, 4, NULL);
}
